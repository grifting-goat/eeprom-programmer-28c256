#ifndef programmer_28c256
#define programmer_28c256

#include "Arduino.h"

//Pin Definitions
#define SHIFT_D 2 //connceted to shift registers, shifts data out one bit at a time
#define SHIFT_CLK 3 //connceted to shift registers, clock for shift registers
#define SHIFT_L 4 //connceted to shift registers, latch to release data from internal registar to the output pins
#define WRITE_ENABLE 5 //connected to eeprom, used to enable writing
#define DATA_0 6 //first data bus line
#define DATA_7 13 //last data bus line
#define CHIP_EN A0 //only need when disableing SDP
#define OUTPUT_EN A1 //used for checking things

//functions

//set the mode for the Arduino pins
void dataMode(bool mode) {
  for (int pin = DATA_0; pin <= DATA_7; pin++) {
    pinMode(pin, mode);
  }
}

//set address between (0x0000-0x7fff) and the state of the output enable(active low)
void setAddress(int address, bool outputEnable) {
  //shifts out the address, first bit is used to determine whether the outPut enable is high or not
  shiftOut(SHIFT_D, SHIFT_CLK, MSBFIRST,(address >> 8) | (outputEnable ? 0x0000 : 0x80));
  shiftOut(SHIFT_D, SHIFT_CLK, MSBFIRST, address);
  
  //pulses the latch on the shift registers, sending the address to the EEPROM
  digitalWrite(SHIFT_L, LOW);
  digitalWrite(SHIFT_L, HIGH);
  digitalWrite(SHIFT_L, LOW);
}

//reads and returns a single byte of data from the bus
byte readByte() {
  byte data = 0;
  for(int pin = DATA_7 ; pin >= DATA_0; pin--) {
    data = (data << 1) + digitalRead(pin);
  }
  return data;
}

//write single byte of data
void writeByte(byte data) {
  //set the bus to the appropriate bits
  for (int pin = DATA_0; pin <= DATA_7; pin++) {
    digitalWrite(pin, data & 1);
    data = data >> 1;
  }

  //pulse Write Enable
  digitalWrite(WRITE_ENABLE, LOW);
  digitalWrite(WRITE_ENABLE, HIGH);
}

//read the byte but with manual output enable & pin mode
byte readByteNOW() {
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
bool confirmWrite(byte data) {
  word timeout = 4000; //max cycles
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

byte readEEPROM(int address) {
  //set d0-d7 to input
  dataMode(INPUT);

  //set address to read from, output enable is low(active low)
  setAddress(address, true);

  //read the byte at address
  return readByte();

}

bool writeEEPROM(int address, byte data) {
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

void serialContents(int startAddress, int endAddress) {
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

void programmerSetup() {
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
#endif