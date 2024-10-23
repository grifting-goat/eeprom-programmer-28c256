#include "Arduino.h"
#include "Programmer28c256.h"

//functions

//set the mode for the Arduino pins
void Programmer28c256::dataMode(bool mode) {
  for (int pin = DATA_0; pin <= DATA_7; pin++) {
    pinMode(pin, mode);
  }
}

//set address between (0x0000-0x7fff) and the state of the output enable(active low)
void Programmer28c256::setAddress(int address, bool outputEnable) {
  //shifts out the address, first bit is used to determine whether the outPut enable is high or not
  shiftOut(SHIFT_D, SHIFT_CLK, MSBFIRST,(address >> 8) | (outputEnable ? 0x0000 : 0x80));
  shiftOut(SHIFT_D, SHIFT_CLK, MSBFIRST, address);
  
  //pulses the latch on the shift registers, sending the address to the EEPROM
  digitalWrite(SHIFT_L, LOW);
  digitalWrite(SHIFT_L, HIGH);
  digitalWrite(SHIFT_L, LOW);
}

//reads and returns a single byte of data from the bus
byte Programmer28c256::readByte() {
  byte data = 0;
  for(int pin = DATA_7 ; pin >= DATA_0; pin--) {
    data = (data << 1) + digitalRead(pin);
  }
  return data;
}

//write single byte of data
void Programmer28c256::writeByte(byte data) {
  //set the bus to the appropriate bits
  for (int pin = DATA_0; pin <= DATA_7; pin++) {
    digitalWrite(pin, data & 1);
    data = data >> 1;
  }

  //pulse Write Enable
  digitalWrite(WRITE_ENABLE, LOW);
  delayMicroseconds(1);
  digitalWrite(WRITE_ENABLE, HIGH);
  delay(10);
}

//read the byte but with manual output enable & pin mode
byte Programmer28c256::readByteNOW() {
  dataMode(INPUT);
  digitalWrite(OUTPUT_EN, LOW);
  byte data = 0;
  for(int pin = DATA_7 ; pin >= DATA_0; pin--) {
    data = (data << 1) + digitalRead(pin);
  }
  digitalWrite(OUTPUT_EN, HIGH);
  Serial.println(".");
  return data;
}

//check if the write cycle worked
bool Programmer28c256::confirmWrite(byte data) {
  word timeout = 4000; //max time
  while (timeout > 0 && readByteNOW() != data)
  {
    timeout--;
  }
  if (!timeout)
  {
    Serial.print("Write fail ");
  }
  return timeout;
}

//Routines

byte Programmer28c256::readEEPROM(int address) {
  //set d0-d7 to input
  dataMode(INPUT);

  //set address to read from, output enable is low(active low)
  setAddress(address, true);

  //read the byte at address
  return readByte();

}

bool Programmer28c256::writeEEPROM(int address, byte data) {
  if (readEEPROM(address) == data) {
    return true;
  }
  //set address to write to, output enable is high(active low)
  setAddress(address, false);

  //set d0-d7 to output
  dataMode(OUTPUT);

  //write data
  writeByte(data);

  return confirmWrite(data);
}

void Programmer28c256::serialContents(int startAddress, int endAddress) {
  for (int base = startAddress; base <= endAddress; base += 16) {
    //stores sixteen bytes
    byte data[16];
    //reads all sixteen bytes
    for (int offset = 0; offset <= 15; offset++) {
      data[offset] = readEEPROM(base + offset);
    }
    //throws them into a formated string that gets outputed to the serial monitor
    char buffer[60];
    sprintf(buffer, "0x%04x:  %02x %02x %02x %02x %02x %02x %02x %02x   %02x %02x %02x %02x %02x %02x %02x %02x",
            base, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
            data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);
    Serial.println(buffer);
  }
}

void Programmer28c256::programmerSetup() {
  //shift registers
  pinMode(SHIFT_D, OUTPUT);
  pinMode(SHIFT_CLK, OUTPUT);
  pinMode(SHIFT_L, OUTPUT);

  //write enable setup
  digitalWrite(WRITE_ENABLE, HIGH);
  pinMode(WRITE_ENABLE , OUTPUT);

  //output Enable
  digitalWrite(OUTPUT_EN, HIGH);
  pinMode(OUTPUT_EN, OUTPUT);
  
  //start serial interface
  Serial.begin(57600);
}