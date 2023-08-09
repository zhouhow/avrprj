## avr-make
### a shell and makefile based tool for arduino based sketch by using arduino-cli
#### it has been tested by arduino uno, esp32, esp8266, rpipico and rpipicow
### usage
#### 1. cd ~ && git clone https://github.com/chowhao/avr-make.git
#### 2. export PATH=~/avr-make/bin:$PATH
### notice
#### 1. the config.txt inclues PROJ, FQBN, DEVS variables
#### PROJ is the name of project, there should be a PROJ.ino in dicrectory
#### FQBN is Fully Qualified Board Name, you can get it by command "arduino-cli
#### board listall"
#### DEVS is the port of the board, usually /dev/ttyACM* or /dev/ttyUSB* in Linux
#### and COM* in Windows
### test
#### 3. cp -r avr-make/test test && cd test
#### 4. cd avr/blink
#### 5. arv-make
