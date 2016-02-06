REM Simulacion de esclavo DNP. Llenar los siguientes parametros y correr este bat.
set port=20000
set number_of_dnp_clients=51
set number_of_rtus_per_channel=1
set start_address=101
set master_address=100
..\Release\KongSlave.exe tcpserver 0.0.0.0 %port% %number_of_rtus_per_channel% %number_of_dnp_clients%  %start_address% %master_address%

