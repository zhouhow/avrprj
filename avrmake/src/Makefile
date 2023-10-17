# Makefile for Arduino based scketches
#
# This Makefile uses the arduino-cli, the Arduino command line interface
# and has been designed and tested to run on Linux, not on Windows.
#
# Please note that:
#
#   1. each sketch must reside in his own folder with this Makefile
#
#   2. the main make targets are:
#      - all     compiles and upload the sketch
#      - verify  only compiles, don't upload the sketch
#      - upload  upload sketch to device via serial port
#      - monitor open the serial monitor if it is available
#      - config  show the configuration of this sketch
#      - clean   clean the output binary files' directory 
#      - install install libraries according to libraries.txt
#
#   3. default is "all"
#
# Some environment variables should be set in config.txt:
#
#   FQBN        you can get FQBN of board by following command: 
#               "arduino-cli board listall"
#
#   DEVS        the port of device, usually /dev/ttyACM* or 
#               /dev/ttyUSB* in Linux system
#
#   PROJ        the name of this project, the $(PROJ).ino should
#               be in the current directory


# the default configuration file of project
# it includes PROJ, FQBN, DEVS variables
CFG        = config.txt
include $(CFG)
# one simple example as follows
#PROJ       = blink
#FQBN       = arduino:avr:uno
#DEVS       = /dev/ttyACM0


# three default environment variables
SRC        = $(PROJ).ino
DIR        = build
LIB        = libraries.txt


# verify and upload the sketch
all: verify upload

# print out environment variables
config:
	@echo "Fully Qualified Board Name: "$(FQBN);
	@echo "The Serial Port of Board: "$(DEVS);
	@echo "The Name of Project: "$(PROJ);
	@echo "The Build Directory: "$(DIR);
	@echo "The Source File: "$(SRC);
	@echo "The Library File: "$(LIB);

# compile source code to binary files
verify:
	@if [ ! -e $(SRC) ]; \
		then echo "\033[;31mERROR: Can't find the source file, please check SRC variable \033[0m"; \
	else echo "Compiling the source files..."; \
		arduino-cli compile -b $(FQBN) --output-dir $(DIR); \
	fi

# upload the binary file to board
upload:
	@if [ ! -c $(DEVS) ]; \
		then echo "\033[;31mERROR: Device not available, please check DEVS variable \033[0m"; \
	else echo "Checking build directory..."; \
		if [ ! -d $(DIR) ]; \
			then echo "\033[;31mERROR: Build directory doesn't exits, please make verify first \033[0m"; \
		else echo "Uploading Sketch to Device..."; \
			arduino-cli upload -b $(FQBN) -p $(DEVS); \
		fi; \
	fi

# open the arduino serial monitor
monitor:
	@if [ ! -c $(DEVS) ]; \
		then echo "\033[;31mERROR: Device not available, please check DEVS variable \033[0m"; \
	else echo "Opening the arduino monitor..."; \
		arduino-cli monitor -p $(DEVS) ; \
	fi

# install libraries in libraries.txt
install:
	@if [ -e $(LIB) ]; \
	then while read -r i ; do echo ; \
		echo "Installing " '"'$$i'"' ; \
	  	arduino-cli lib install "$$i" ; \
	done < $(LIB); \
	else echo "\033[;31mERROR: libraries.txt dosen't exits \033[0m"; \
		echo "Create" $(LIB) "if is needed"; \
	fi



# remove genereted binary files
clean:
	@if [ -d $(DIR) ]; \
		then echo "Cleaning the build directory..."; \
        rm -rf $(DIR); \
    else echo "\033[;33mWARNING: The build directory doesn't exits \033[0m"; \
        echo "\033[;33mWARNING: You don't need to remove it at all\033[0m"; \
    fi