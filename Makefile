################################################################################
# Multiple Opendnp3 slaves
# Very simple makefile to build on Linux
# Pablo Giudice
# pablogiudice@gmail.com
################################################################################
CC=g++
#Flags for debug enabled, multithreading and c++11
CFLAGS= -g -pthread -std=c++11

# Executable will be created in OUTPUTDIR/EXENAME
OUTPUTDIR=Release
EXENAME=kongslave

# Instalation directory
INSTALLDIR=/usr/bin

# For the manual page
MANDIR=/usr/share/man/man8/
MANNAME=kongslave.8

# Source files directory
SRCDIR=KongSlave

CPP_FILES := $(shell find $(SRCDIR)/ -type f -name '*.cpp')
OBJ_FILES := $(CPP_FILES:.cpp=.o)
CPP_FILES_NO_MAIN := $(filter-out main.cpp, $(CPP_FILES))
OBJ_FILES_NO_MAIN :=  $(filter-out obj/main.o, $(OBJ_FILES))

# IMPORTANT: If new directories are added to the SRCDIR directory, then 
# they should be added to this list .

INCDIR= -I$(SRCDIR) -I$(ASIO_HOME) /usr/include

# Libraries
LIBRARIES=-lasiodnp3 -lopendnp3 -lasiopal -lopenpal

default: pre-build main

pre-build:
		mkdir -p $(OUTPUTDIR)

all:default

main: $(OBJ_FILES)
		$(CC) $(CFLAGS) -o  $(OUTPUTDIR)/$(EXENAME) $^ $(INCDIR) $(LIBDIR) $(LIBRARIES)

%.o: %.cpp
		$(CC) $(CFLAGS) -c -o $@ $< $(INCDIR) $(LIBDIR) $(LIBRARIES)

.PHONY: clean
clean:
	rm -f $(OBJ_FILES)
	rm -f -r $(OUTPUTDIR)/$(EXENAME)


install: main
	if [ -d $(INSTALLDIR) ] ; \
	then \
		cp $(OUTPUTDIR)/$(EXENAME) $(INSTALLDIR); \
		chmod a+x $(INSTALLDIR)/$(EXENAME); \
		chmod og-w $(INSTALLDIR)/$(EXENAME); \
		echo $(EXENAME)" was successfully installed in $(INSTALLDIR)"; \
	else \
		echo "Can't install $(EXENAME) because $(INSTALLDIR) doesn't exist"; \
	fi
	#install the man page
	install -g 0 -o 0 -m 0644 $(MANNAME) $(MANDIR)
	gzip $(MANDIR)/$(MANNAME)

#install the man page
maninstall:
	install -g 0 -o 0 -m 0644 $(MANNAME) $(MANDIR)
	gzip $(MANDIR)/$(MANNAME)
