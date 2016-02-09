#include <asiodnp3/DNP3Manager.h>
#include <asiodnp3/PrintingSOEHandler.h>
#include <asiodnp3/ConsoleLogger.h>
#include <asiopal/UTCTimeSource.h>
#include <asiodnp3/MeasUpdate.h>
#include <opendnp3/outstation/SimpleCommandHandler.h>
#include <opendnp3/LogLevels.h>
#include <string>
#include <thread>
#include <iostream>
#include <iomanip>
#include <stdlib.h>     /* srand, rand */
#include <time.h>
#include <condition_variable>
#include <iterator>
#include <math.h>
#include <string.h>
#include <stdio.h>

using namespace opendnp3;
using namespace openpal;
using namespace asiopal;
using namespace asiodnp3;
using namespace std;

const int year = 2016;
const int versionNumber = 0;
const int versionRevision = 2;

#define TCP_CLIENT	1
#define TCP_SERVER	2
#define SERIAL		3

int numeroDeEsclavos;
int numeroDeCanales;
int DNPFirstAddress;
bool mute = true;
bool logOn;
bool quit;

ChannelState* currentState;
IChannel** pChannel;

#define MAX_BINARY_INPUT 12
#define MAX_ANALOG_INPUT 12

IOutstation** pOutstation;
class conn{
public:
	string ip;
	int port;
	conn(string ip, int port){
		this->ip = ip;
		this->port = port;
	}
};
std::vector<conn> connectionsVector;

// Tipos de datos
#define BINARY_INPUT	1
#define ANALOG_INPUT	2
#define COUNTER			3
#define DOUBLEB_BINARY	4

bool partyTimeOn = false;
bool randomDisconnectOn = false;

std::mutex m1;
std::condition_variable cv1;

std::mutex m2;
std::condition_variable cv2;

void ConfigureDatabase(DatabaseConfigView view)
{
	// example of configuring analog index 0 for Class2 with floating point variations by default
	view.analogs[0].variation = StaticAnalogVariation::Group30Var5;
	view.analogs[0].metadata.clazz = PointClass::Class2;
	view.analogs[0].metadata.variation = EventAnalogVariation::Group32Var7;


	for (int m = 0; m < view.analogs.Size(); m++){

		view.analogs[m].value.quality = 1;

	}
	for (int m = 0; m < view.binaries.Size(); m++){

		view.binaries[m].value.quality = 1;

	}
}

void printHelp(){
	std::cout << "Escriba <comando> <parametros> <enter>" << std::endl;
	std::cout << "Donde comando es uno de la siguiente lista:" << endl;
	std::cout << \
		"c = Muestra el estado de conexion de los canales" << endl <<
		"b = modifica un punto digital" << endl <<
		"d = modifica un punto doublebit" << endl <<
		"o = modifica un contador" << endl <<
		"a = modifica un valor analogico" << endl <<
		"m = habilitar/deshabilitar log" << endl <<
		"s = muestra estadistica de los canales" << endl <<
		"c = muestra el cuadro de conexion" << endl <<
		"r = muestra estdisticas de las RTUs " << endl <<
		"p = inicia/detiene el modo fiesta - todos los puntos de la RTU cambian una vez por segundo" << endl <<
		"y = inicia/detiene el modo bipolar - todas las RTUs se habilitan/deshabilitan una vez cada 5 segundos" << endl <<
		"x/q = para salir" << endl <<
		std::endl;
}

/**
* Verifica si la direccion es valida.
*/
bool destinationValid(int destination){
	return (destination >= DNPFirstAddress && destination < (DNPFirstAddress + numeroDeEsclavos));
}

/**
* destination no se usa pero en el caso de que se hagan RTUs de distinto tamanio
* seria solamente cambiar esto.
*/
bool binaryInputValid(int destination, int pointnumber){
	return (pointnumber < MAX_BINARY_INPUT);
}

/**
* destination no se usa pero en el caso de que se hagan RTUs de distinto tamanio
* seria solamente cambiar esto.
*/
bool analogInputValid(int destination, int pointnumber){
	return (pointnumber < MAX_ANALOG_INPUT);
}

int outstationAddressToIndex(int address){

	return (address - DNPFirstAddress);
}

int indexToOutstationIndex(int index){

	return (index + DNPFirstAddress);
}

/**
* Message should be something like:
* b 101 0 0
* b 101 12 1
*/
void handleMeasurement(int type, string& message){
	istringstream iss(message);
	vector<string> tokens{ istream_iterator<string>{iss}, istream_iterator<string>{} };

	if (tokens.size() == 4){
		int address, pointnumber, value;
		try{
			address = std::stoi(tokens.at(1));
			pointnumber = std::stoi(tokens.at(2));
			value = std::stoi(tokens.at(3));
		}
		catch (invalid_argument& iae){
			cout << "Error: " << iae.what() << endl;
			return;
		}
		catch (out_of_range& oor){
			cout << "Error: " << oor.what() << endl;
			return;
		}
		bool val;

		if (!destinationValid(address)){
			cout << "Error: La direccion " << address << " es invalida" << endl;
			return;
		}

		if (!binaryInputValid(address, pointnumber)){
			cout << "Error: El numero de punto " << pointnumber << " es invalido" << endl;
			return;
		}

		if (tokens.at(2).compare("all") == 0){
			cout << "TBD" << endl;
		}
		else{
			MeasUpdate tx(pOutstation[outstationAddressToIndex(address)]);
			cout << "Enviando valor " << value << " a la direccion " << address << ", pointnumber " << pointnumber << endl;
			if (type == BINARY_INPUT){
				if (value == 0){
					val = false;
				}
				else if (value == 1){
					val = true;
				}
				else {
					cout << "Error: el valor " << value << " es invalido" << endl;
					return;
				}
				tx.Update(Binary(val), pointnumber);
			}
			else if (type == ANALOG_INPUT)
				tx.Update(Analog(value), pointnumber);
			else if (type == COUNTER){
				tx.Update(Counter(value), pointnumber);
			}
			else if (type == DOUBLEB_BINARY){
				tx.Update(DoubleBitBinary(value), pointnumber);
			}
			else
				cout << "Error de tipo" << endl;
		}
	}
	else{
		cout << "Error en el numero de parametros: Deberia ser:" << endl;
		cout << "b 101 2 1" << endl;
		cout << "donde envia a la direccion 101, entrada=2, valor=1" << endl;

	}
}


void handleBinaryInput(string message){
	handleMeasurement(BINARY_INPUT, message);
}

void handleAnalogInput(string message){
	handleMeasurement(ANALOG_INPUT, message);
}

void handleCounter(string message){
	handleMeasurement(COUNTER, message);
}

void handleDoubleBitBinary(string message){
	handleMeasurement(DOUBLEB_BINARY, message);
}


bool channelValid(int index){

	return (index < numeroDeCanales);

}

void muteChannel(int channelNum, int mute){

	if (mute == 0){
		cout << "Mute channel " << channelNum << endl;
		pChannel[channelNum]->SetLogFilters(levels::NOTHING);
	}
	else if (mute == 1){
		cout << "Unmute channel " << channelNum << endl;
		pChannel[channelNum]->SetLogFilters(levels::ALL_COMMS);
	}
	else
	{
		cout << "value debe ser 0 o 1" << endl;
	}
}

void handleMute(string message){

	istringstream iss(message);
	vector<string> tokens{ istream_iterator<string>{iss}, istream_iterator<string>{} };

	if (tokens.size() == 3){
		int channelNum, value;
		try{
			value = std::stoi(tokens.at(2));

			if (tokens.at(1).compare("all") == 0){
				for (int i = 0; i < numeroDeCanales; i++){
					muteChannel(i, value);
				}
				return;
			}

			channelNum = std::stoi(tokens.at(1));

		}
		catch (invalid_argument& iae){
			cout << "Error: " << iae.what() << endl;
			return;
		}
		catch (out_of_range& oor){
			cout << "Error: " << oor.what() << endl;
			return;
		}

		if (!channelValid(channelNum)){
			cout << "Error: El canal " << channelNum << " es invalido" << endl;
			return;
		}

		muteChannel(channelNum, value);

	}
	else{
		cout << "Error en el numero de parametros." << endl;
		cout << "Ejemplos:" << endl;
		cout << "\tm 101 1" << endl;
		cout << "\tpara habilitar el log la RTU 101" << endl;
		cout << "\tm 101 0" << endl;
		cout << "\tpara silenciar a la RTU 101" << endl;
		cout << "\tm all 0" << endl;
		cout << "\tpara silenciar a todas las RTUs" << endl;

	}
}

void handleConnectionChart(string message){
	int max_ch_per_col = 25;

	cout << "--------------------------------------------------------------------------------" << endl;
	cout << "\t\t\tCuadro de conexion" << endl;
	cout << "--------------------------------------------------------------------------------" << endl;
	cout << "         ";
	for (int k = 0; k < max_ch_per_col; k++){
		cout << std::setfill(' ') << std::setw(3) << k;
	}

	cout << endl << "--------------------------------------------------------------------------------";
	for (int k = 0; k < numeroDeCanales; k++){

		if (k%max_ch_per_col == 0){
			cout << endl;
			cout << std::setfill('0') << std::setw(4) << k << "-" <<
				std::setfill('0') << std::setw(4) << k + max_ch_per_col - 1 << " ";
		}

		if (currentState[k] == ChannelState::OPEN){
			cout << " . ";
		}
		else{
			cout << " # ";
		}
	}

	cout << endl << "--------------------------------------------------------------------------------" << endl;
	cout << " \".\" = CONECTADO		\"#\" = DESCONECTADO " << endl;
	cout << "--------------------------------------------------------------------------------" << endl;

}

void handleChannelStats(string message){
	cout << "Resumen de los canales" << endl;
	cout << "------------------------------------------------------------------------" << endl;
	cout << "#\t#open\topnFail\tRx\tTx\tIP\tport" << endl;
	cout << "------------------------------------------------------------------------" << endl;

	for (int k = 0; k < numeroDeCanales; k++){

		opendnp3::LinkChannelStatistics lcs = pChannel[k]->GetChannelStatistics();
		cout << k << "\t" <<
			lcs.numOpen << "\t" <<
			lcs.numOpenFail << "\t" <<
			lcs.numBytesRx << "\t" <<
			lcs.numBytesTx << "\t" <<
			connectionsVector.at(k).ip << "\t" <<
			connectionsVector.at(k).port <<
			endl;
	}
}

void handleRTUStatistics(string message){

	cout << "Resumen de RTUs (Estadistica de transporte)" << endl;
	cout << "------------------------------------------------------------------------" << endl;
	cout << "k\tT Err\tT Rx\tTRx" << endl;
	cout << "------------------------------------------------------------------------" << endl;

	for (int k = 0; k < numeroDeEsclavos; k++){
		StackStatistics s = pOutstation[k]->GetStackStatistics();
		cout << k << "\t" << s.numTransportErrorRx << "\t" << s.numTransportRx << "\t" << s.numTransportTx << endl;

	}

}

void enableDisable(int index, int value){
	if (value == 0){
		cout << "Deshabilitando RTU " << indexToOutstationIndex(index) << endl;
		pOutstation[index]->Disable();
	}
	else if (value == 1){
		cout << "Habilitando RTU" << indexToOutstationIndex(index) << endl;
		pOutstation[index]->Enable();
	}
	else
	{
		cout << "El valor debe ser 0 o 1" << endl;
	}
}

/**
* Message should be something like:
* e 101 1
* e 101 0
*/
void handleEnableDisable(string& message){
	istringstream iss(message);
	vector<string> tokens{ istream_iterator<string>{iss}, istream_iterator<string>{} };

	if (tokens.size() == 3){
		int address, value;
		try{
			value = std::stoi(tokens.at(2));

			if (tokens.at(1).compare("all") == 0){
				for (int i = 0; i < numeroDeEsclavos; i++){
					enableDisable(i, value);
				}
				return;
			}

			address = std::stoi(tokens.at(1));

		}
		catch (invalid_argument& iae){
			cout << "Error: " << iae.what() << endl;
			return;
		}
		catch (out_of_range& oor){
			cout << "Error: " << oor.what() << endl;
			return;
		}

		if (!destinationValid(address)){
			cout << "Error: La direccion " << address << " es invalida" << endl;
			return;
		}

		enableDisable(outstationAddressToIndex(address), value);

	}
	else{
		cout << "Error en el numero de parametros." << endl;
		cout << "Ejemplos:" << endl;
		cout << "\te 101 1" << endl;
		cout << "\tpara habilitar la RTU 101" << endl;
		cout << "\te 101 0" << endl;
		cout << "\tpara deshabilitar la RTU 101" << endl;

	}
}

#define TOGGLE_PROB (4)
/**
* Every 5 seconds it will randonmly enable or disable each outstation.
*/
void randomDisconnect(){

	int binary;

	while (!quit){
		{
			// It's important to surround this block by braces. Otherwise, the 
			// unique_lock will be locked for the duration of the whole while
			// block
			std::unique_lock<std::mutex> lk(m1);
			cv1.wait(lk);
			cout << "wake up" << endl;
		}

		while (randomDisconnectOn && !quit){
			for (int k = 0; k < numeroDeEsclavos; k++){

				// Changing TOGGLE_PROB to a different number will make it more or less
				// likely for an outstation to change state on each turn. 
				// For example, if TOGGLE_PROB=2 then it will change 1 out of 
				// 2 times (50% change probability). Changing it to 10, will 
				// result in 1 out of 10.
				binary = rand() % TOGGLE_PROB;
				if (binary == 0){
					cout << "Disable outstation = " << k << endl;
					pOutstation[k]->Disable();
				}
				else{
					cout << "Enable outstation = " << k << endl;
					pOutstation[k]->Enable();
				}

			}
			std::this_thread::sleep_for(std::chrono::milliseconds(5000));
		}
	}
}

/**
* Every one second it will change all the values of every binary and analog input
* of every outstation.
*/
void partyTime(){

	int binary, analog;
	bool bin;

	while (!quit){
		
		{
			// It's important to surround this block by braces. Otherwise, the 
			// unique_lock will be locked for the duration of the whole while
			// block
			std::unique_lock<std::mutex> lk(m2);
			cv2.wait(lk);
			
		}
		while (partyTimeOn && !quit){
			for (int k = 0; k < numeroDeEsclavos; k++){

				for (int p = 0; p < MAX_ANALOG_INPUT; p++){

					analog = rand() % 65534;
					MeasUpdate tx(pOutstation[k]);
					tx.Update(Analog(analog), p);

				}

				for (int p = 0; p < MAX_BINARY_INPUT; p++){

					binary = rand() % 2;
					if (binary == 0){
						bin = false;
					}
					else{
						bin = true;
					}
					MeasUpdate tx(pOutstation[k]);
					tx.Update(Binary(bin), p);

				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
	}
}

/**
* Signals the start of the bipolar mode to the bipolar thread.
*/
void handleBipolarModeLaunch(){
	
	std::unique_lock<std::mutex> lk1(m1);
	cv1.notify_one();

	if (randomDisconnectOn){
		cout << "Bipolar mode off" << endl;
		randomDisconnectOn = false;
	}
	else{
		cout << "Bipolar mode on!" << endl;
		randomDisconnectOn = true;
	}
}

/**
* Signals the start of the party mode to the party thread.
*/
void handlePartyModeLaunch(){

	std::unique_lock<std::mutex> lk1(m2);
	cv2.notify_one();

	if (partyTimeOn){
		cout << "The party is over" << endl;
		partyTimeOn = false;
	}
	else{
		cout << "It's party time!" << endl;
		partyTimeOn = true;
	}
}

/**
* Main thread that creates the outstations and then handles the user input.
*/
int opendnp3_slave(int connectionType, char* tcpAddress, int tcpPort, int maxChannelSize, int dnpSlaveQty, int dnpLocalAddressStart, int dnpRemoteAddress){

	logOn = true;
	numeroDeEsclavos = dnpSlaveQty;
	DNPFirstAddress = dnpLocalAddressStart;
	numeroDeCanales = (int)ceil((double)dnpSlaveQty / (double)maxChannelSize);
	int outstationsPorCanal = dnpSlaveQty - numeroDeCanales*maxChannelSize + 1;
	//Dat options
	OutstationStackConfig* stackConfigs = new OutstationStackConfig[dnpSlaveQty];
	pOutstation = new IOutstation*[dnpSlaveQty];

	// Specify what log levels to use. NORMAL is warning and above
	// You can add all the comms logging by uncommenting below.
	const uint32_t FILTERS = levels::NOTHING; //NORMAL; // | levels::ALL_COMMS;

	// This is the main point of interaction with the stack
	DNP3Manager manager(std::thread::hardware_concurrency());

	// send log messages to the console
	manager.AddLogSubscriber(ConsoleLogger::Instance());

	pChannel = new IChannel*[numeroDeCanales];
	currentState = new ChannelState[numeroDeCanales];

	//-------------------------------------------------------------------------
	cout << "Creando " << numeroDeCanales << " canales." << endl;
	int k = 0;

	for (int cc = 0; cc < numeroDeCanales; cc++){

		cout << "Channel properties: " << endl;
		if (connectionType == TCP_CLIENT){
			cout << " TCP CLIENT " << endl;
			cout << " Connecting to: " << tcpAddress << " , on port: " << tcpPort << " (Max Number of RTUs per channel: " << maxChannelSize << " )" << endl;
		}
		else if (connectionType == TCP_SERVER){

			cout << " TCP SERVER" << endl;
			cout << " Listening on: " << tcpAddress << " , on port: " << tcpPort << " (Max Number of RTUs per channel: " << maxChannelSize << " )" << endl;
		}
		if (cc == numeroDeCanales - 1)
			cout << " Building " << outstationsPorCanal << " outstations with DNP addresses from " << dnpLocalAddressStart + cc*maxChannelSize << " to " << (dnpLocalAddressStart + dnpSlaveQty - 1) << endl;
		else
			cout << " Building " << outstationsPorCanal << " outstations with DNP addresses from " << dnpLocalAddressStart + cc*maxChannelSize << " to " << (dnpLocalAddressStart + (cc + 1)*maxChannelSize - 1) << endl;

		cout << "----------------------------------------------------" << endl;



		if (connectionType == TCP_SERVER){

			// Create a TCP server (listener)	
			pChannel[cc] = manager.AddTCPServer("server", FILTERS,  ChannelRetry::Default(), "0.0.0.0", tcpPort);
		}
		else if (connectionType == TCP_CLIENT){

			// Create a TCP server (listener)	
			pChannel[cc] = manager.AddTCPClient("client", FILTERS, ChannelRetry::Default(), tcpAddress, "0.0.0.0", tcpPort);
		}

		connectionsVector.push_back(conn(tcpAddress, tcpPort));

		// Optionally, you can bind listeners to the channel to get state change notifications
		// This listener just prints the changes to the console
		int dnpa = dnpLocalAddressStart + k;
		pChannel[cc]->AddStateListener([=](ChannelState state)
		{
			std::cout << "channel #" << cc << " (DNP " << dnpa << ") : " << ChannelStateToString(state) << std::endl;

			currentState[cc] = state;

		});


		for (int m = 0; m < maxChannelSize && k < dnpSlaveQty; m++, k++){

			// You must specify the shape of your database and the size of the event buffers
			stackConfigs[k].dbTemplate = DatabaseTemplate(
				MAX_BINARY_INPUT,	//numBinary
				0,	//numDoubleBinary
				MAX_ANALOG_INPUT,	//numAnalog
				0,	//numCounter
				0,	//numFrozenCounter
				0,	//numBinaryOutputStatus
				0);	//numAnalogOutputStatus

			stackConfigs[k].outstation.eventBufferConfig = EventBufferConfig::AllTypes(2);

			// in this example, we've enabled the outstation to use unsolicited reporting
			// if the master enables it
			stackConfigs[k].outstation.params.allowUnsolicited = true;

			// You can override the default link layer settings here
			// in this example we've changed the default link layer addressing
			stackConfigs[k].link.LocalAddr = dnpLocalAddressStart + k;
			stackConfigs[k].link.RemoteAddr = dnpRemoteAddress;

			string name = std::to_string(stackConfigs[k].link.RemoteAddr);

			// Create a new outstation with a log level, command handler, and
			// config info this	returns a thread-safe interface used for
			// updating the outstation's database.
			pOutstation[k] = pChannel[cc]->AddOutstation(
				name.c_str(),
				SuccessCommandHandler::Instance(),
				DefaultOutstationApplication::Instance(),
				stackConfigs[k]);

			ConfigureDatabase(pOutstation[k]->GetConfigView());

			cout << "Added outstation:\t" << dnpLocalAddressStart + k << endl;
		}

		//Proxima ronda
		tcpPort++;
	}

	// Enable the outstation and start communications
	for (int m = 0; m < k; m++){
		pOutstation[m]->Enable();
	}

	// variables used in example loop
	string input;
	uint32_t count = 0;
	double value = 0;
	bool binary = false;
	DoubleBit dbit = DoubleBit::DETERMINED_OFF;

	string read;
	int pointnumber = 0;
	string command;
	quit = false;

	printHelp();

	while (!quit){

		getline(cin, read);

		if (read.compare("") == 0)
			continue;

		if (read.size() >= 2 && read.at(1) != ' ')
			cout << "Comando desconocido" << endl;

		switch (read.at(0)){

		case ('h') :
		case ('?') :
				   printHelp();
			break;
		case ('b') :
			handleBinaryInput(read);
			break;
		case ('a') :
			handleAnalogInput(read);
			break;
		case ('o') :
			handleCounter(read);
			break;
		case ('d') :
			handleDoubleBitBinary(read);
			break;
		case ('m') :
			handleMute(read);
			break;
		case('s') :
			handleChannelStats(read);
			break;
		case('c') :
			handleConnectionChart(read);
			break;
		case('r') :
			handleRTUStatistics(read);
			break;
		case('y') :
			handleBipolarModeLaunch();
			break;
		case('p') :
			handlePartyModeLaunch();
			break;
		case ('e') :
			handleEnableDisable(read);
			break;
		case ('x') :
		case ('q') : {
					   std::unique_lock<std::mutex> lk1(m1);
					   cv1.notify_one();
					   std::unique_lock<std::mutex> lk2(m2);
					   cv2.notify_one();
					   quit = true;
					   break;
		}
		default:
			std::cout << "Comando desconocido: " << read << std::endl;
			break;
		}
	}

	return 0;
}

/**
*	Main entry point.
*/
int main(int argc, char* argv[])
{
	std::thread mainThread;
	std::thread randomThread(randomDisconnect);
	std::thread partyThread(partyTime);

	bool run = false;

	/* initialize random seed: */
	srand(time(NULL));

	//
	cout << "Pablo Giudice " << year << endl;
	cout << "KongSlaveTest v" << versionNumber << "." << versionRevision << endl;
	cout << "Slave Test" << endl;
	cout << "----------------------------------------------------" << endl;

	if (argc == 1){
		cout << "opendnp3 client test " << endl;
		cout << " KongSlaveTest  tcpserver 0.0.0.0 14641 10 101 100" << endl;
		cout << " this creates a DNP3 Slave on the TCP server that listens on 0.0.0.0 on port 14641 with DNP ids = 101-110 and remote host=100" << endl;
		cout << " KongSlaveTest  tcpclient 127.0.01 14641 101" << endl;
		cout << " this creates a DNP3 Slave on the TCP server that connects to 127.0.0.1 on port 14641 with DNP ids = 101-110 and remote host=100" << endl;
		cout << " KongSlaveTest  tcpserver 0.0.0.0 14641 15 10 101 100" << endl;
		cout << " this creates a DNP3 Slave on the TCP server that listens on 0.0.0.0 on port 14641 with DNP ids = 101-110 and remote host=100. It creates a new channel once 15 RTUs are created." << endl;
	}
	else if (argc == 7){

		if (strcmp(argv[1], "tcpclient") == 0){
			mainThread = std::thread(opendnp3_slave, TCP_CLIENT, argv[2], atoi(argv[3]), 10000, atoi(argv[4]), atoi(argv[5]), atoi(argv[6]));
		}
		else if (strcmp(argv[1], "tcpserver") == 0){
			mainThread = std::thread(opendnp3_slave, TCP_SERVER, argv[2], atoi(argv[3]), 10000, atoi(argv[4]), atoi(argv[5]), atoi(argv[6]));
		}
		run = true;
	}
	else if (argc == 8){

		if (strcmp(argv[1], "tcpclient") == 0){
			mainThread = std::thread(opendnp3_slave, TCP_CLIENT, argv[2], atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6]), atoi(argv[7]));
		}
		else if (strcmp(argv[1], "tcpserver") == 0){
			mainThread = std::thread(opendnp3_slave, TCP_SERVER, argv[2], atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6]), atoi(argv[7]));
		}
		run = true;
	}
	else{
		cout << "Wrong number of arguments." << endl;
	}

	if (run){
		mainThread.join();
		partyThread.join();
		randomThread.join();
	}

	return 0;
}
