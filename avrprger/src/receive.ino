/*###########################################################
### File Name: receive.ino
### Author: zzh
### Mail: zzh2076625923@outlook.com
### Created Time: 2023-01-14
###########################################################*/


// macro define
#define DATA 2 // shiftout data pin
#define CLCK 3 // shiftout clock pin
#define LATH 4 // shiftout latch pin
#define D0 5 // last significant bit
#define D7 12 // most significant bit
#define WREN 13 // write enable pin

#define READ 0 // read operation
#define WRITE 1 // write operation
#define ERASE 14 // erase operation


// set address on address bus
void SetAddr(int address, bool output /*eeprom out enable*/) {
  // shift out address
  shiftOut(DATA, CLCK, MSBFIRST, (address >> 8) | (output ? 0x00 : 0x80));
  shiftOut(DATA, CLCK, MSBFIRST, address);
  
  // latch address
  digitalWrite(LATH, LOW);
  digitalWrite(LATH, HIGH);
  digitalWrite(LATH, LOW);

}


// set data pin mode
void SetMode(int p0, int p7, bool read /*input or output*/) {
  for (int pin = p0; pin <= p7; pin += 1) {
    pinMode(pin, (read ? INPUT : OUTPUT));
  }

}


// read one byte from address
byte ReadByte(int address) {
  // output enable
  SetAddr(address, true);
  
  byte data = 0;
  for (int pin = D7; pin >= D0; pin -= 1) {
    data = (data << 1) + digitalRead(pin);
  }
  return data;
  
}


// write one byte to address
void WriteByte(int address, byte data) {
  // disable output
  SetAddr(address, false);

  for (int pin = D0; pin <= D7; pin += 1) {
    digitalWrite(pin, data & 1);
    data = data >> 1;
  }
  
  // write and delay  
  digitalWrite(WREN, LOW);
  delayMicroseconds(1);
  digitalWrite(WREN, HIGH);
  delay(10);

}


// read some bytes from address
void ReadPage(int org, int size) {
  // read mode
  SetMode(D0, D7, true);
  
  int sum = (size%16 != 0 ? (size/16 + 1)*16 : size);
  
  for (int i = org ; i < org+sum ; i += 16) {

    // read 16 bytes once
    byte data[16];
    for (int j = 0; j <= 15; j += 1) {
      data[j] = ReadByte(i + j);

    }
    
    // print data
    char buf[60];
    sprintf(buf, "%04x: %02x %02x %02x %02x %02x %02x %02x %02x  %02x %02x %02x %02x %02x %02x %02x %02x",
            i, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
            data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);
    Serial.println(buf);

  }
  
  // disable output
  SetAddr(int(0x7fff), false);

}


// convert char into ASCII
int Char2Int(char c){

  if (c >= '0' && c <= '9') return c - '0';
  else if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  else if (c >= 'A' && c <= 'F') return c - 'A' + 10; 
  else return -1;
  
}


// decode string
int Str2Int(String s, int index){

  int hex = 0;
  hex = Char2Int(s[index])*16+Char2Int(s[index+1]);
  return hex;

}



void setup() {
  // put your setup code here, to run once:
  pinMode(DATA, OUTPUT);
  pinMode(CLCK, OUTPUT);
  pinMode(LATH, OUTPUT);
  digitalWrite(WREN, HIGH);
  pinMode(WREN, OUTPUT);
  Serial.begin(9600);
  
}



void loop(){
  // put your main code here, to run repeatedly:

  // read command and argument
  String cmd = "";
  while (Serial.available() > 0){
    cmd += char(Serial.read());
    delay(2);
  }

  
  if (cmd.length() > 0){
   
   if (cmd.length() > 8){
      int opt = Str2Int(cmd, 0); // read, erase, write
      int orga = Str2Int(cmd, 2)*256+Str2Int(cmd, 4);
      int size = Str2Int(cmd, 6)*256+Str2Int(cmd, 8); // read or erase size bytes

      switch(opt) {
        case READ:

          // read some bytes
          ReadPage(orga, size);
          // print out read information
          char buf[60];
          sprintf(buf, "Read from 0x%04x to 0x%04x, %d bytes, Done!", orga, orga+size, size);
          Serial.println(buf);

          break;
        
        case ERASE:

          // erase some bytes, pinmode output
          SetMode(D0, D7, false);
          
          for (int addr = 0; addr < size; addr += 1) {
            WriteByte(orga + addr, int(0xff));
          }
        
          break;

        case WRITE:

          // write mode
          SetMode(D0, D7, false);
          
          /*
          serial buffer size is 256 bytes, every char is 2 bytes
          data must be no more than 60 bytes in once write operation
          */
          int len = (cmd.length()-7)/2; // data length
          /*
          cmd = 01010023651f'\n', 01 --> write
          0100 --> orga, 23651f --> data, '\n' --> enter
          */

          for (int addr = 0; addr < len; addr += 1) {
            int tmp = Str2Int(cmd, addr*2+6);
            WriteByte(orga + addr, tmp);
          }


          break;          
        
        default:
        
          break;
      }

      cmd = "";

    }
    else{
      cmd = "";      
    }
    
  } 

}
