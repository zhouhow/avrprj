## avr-make
a shell and makefile based tool for arduino based sketch by using arduino-cli, it has been tested by arduino uno, esp32, esp8266, rpipico and rpipicow
### setup
1. cd ~ && git clone https://github.com/chowhao/avr-make.git
2. export PATH=~/avr-make/bin:$PATH or echo "PATH=~/avr-make/bin:$PATH" >> .bashrc && source .bashrc

### usage
you can try "avr-make help" to get help page as follows

<img src="png/help.png" width="600" height="200">   

### example
1. cd ~ && mkdir hello && cd hello
2. touch hello.ino 
3. type "avr-make help" for help
4. avr-make init
5. edit hello.ino and config.txt 
6. type "avr-make" for verify and upload

### notice
###### 1. the config.txt inclues PROJ, FQBN, DEVS variables
1. PROJ is the name of project, there should be a PROJ.ino in current project dicrectory
1. FQBN is Fully Qualified Board Name, you can get it by command "arduino-cli board listall"
3. DEVS is the port of the board, usually /dev/ttyACM* or /dev/ttyUSB* in Linux and COM* in Windows
###### 2. there are some eaxmples in test directory
1. avr is for ardunio, esp/dev is for esp32, esp/node is for esp8266, rpi/pico is for rpipico board, rpi/picow is for rpipicow board
2. blink for blink the on board led, hello is for print out "hello world" to serial monitor, wifi is for rpipicow only for wifi connection
