# Kong Slave  

Pablo Giudice 2016

pablogiudice@gmail.com

DNP3 RTU simulator based on the opendnp3 library. 
Generates N channels with M RTUs on each channel.
Similar to the official demo but on stereoids.
Originally built to test the opendnp3-based Autotrol DNP3 data concentrator (www.autotrol.net).

## Build 

Tested with current (feb 9 2016) opendnp3 version 2.0.x and Visual Studio 2013.
It's assumed that opendnp3 and asio libraries are installed. 
The executable will be created in the `Release` directory. 

### Build on Windows

Use the Visual Studio project

### Build on Linux 

Remember to set the variable `ASIO_HOME` before building. 

To build the project:

	make

To install the binary in `/usr/bin` and install the `man` page, use:

	sudo make install 

## Run

Call it without parameters to see the sintax or use the `.bat` (windows) or `.sh` (*nix) scripts on the `Scripts`directory to learn how
to use it.
On *nix systems, type

	man kongslave

for help on the program.

## License 

opendnp3 is licensed under the terms of the terms of the [Apache 2.0 License](http://www.apache.org/licenses/LICENSE-2.0.html).

This software is also licensed under Apache 2.0 License. Feel free to fork it, modify it and use it. If you found this useful please add a link to this repository and spread the word about opendnp3.
