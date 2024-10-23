#ifndef PROGRAMMER28C256_H  // Corrected include guard
#define PROGRAMMER28C256_H


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
class Programmer28c256 {

public:
  //set the mode for the Arduino pins
  void dataMode(bool mode);

  //set address between (0x0000-0x7fff) and the state of the output enable(active low)
  void setAddress(int address, bool outputEnable);

  //reads and returns a single byte of data from the bus
  byte readByte();

  //write single byte of data
  void writeByte(byte data);

  //read the byte but with manual output enable & pin mode
  byte readByteNOW();

  //check if the write cycle worked
  bool confirmWrite(byte data);

  //Routines

  //read a specific byte from the ROM
  byte readEEPROM(int address);

  //write to a specific address in the ROM
  bool writeEEPROM(int address, byte data);

  //print the contents to the serial monitor
  void serialContents(int startAddress, int endAddress);


  void programmerSetup();
};
#endif