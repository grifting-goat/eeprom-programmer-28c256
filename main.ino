//eeprom programmer for 28c256

#include "Programmer28c256.h"
#include "binaries.h"
#include <Arduino.h>

//sets everything to 
void noOP(Programmer28c256& ep, int startAddress, int endAddress) {
  for (int base = startAddress; base <= endAddress; base++) {
    ep.writeEEPROM(base, 0xea);
  }
}

void unlocker(Programmer28c256& ep) { 
    ep.writeEEPROM(0x5555, 0xAA);  // Write 0xAA to address 0x5555
    ep.writeEEPROM(0x2AAA, 0x55);  // Write 0x55 to address 0x2AAA
    ep.writeEEPROM(0x5555, 0x80);  // Write 0x80 to address 0x5555
    ep.writeEEPROM(0x5555, 0xAA);  // Write 0xAA to address 0x5555
    ep.writeEEPROM(0x2AAA, 0x55);  // Write 0x55 to address 0x2AAA
    ep.writeEEPROM(0x5555, 0x20);  // Write 0x20 to address 0x5555
}

Programmer28c256 ep;

void setup() {
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

  //write
  
  //writeTestCode();
  unlocker(ep);

  noOP(ep, 0x0000, 0x00FF);
  
}


void loop() {
  delay(4000);
  ep.serialContents(0,0xff);

  /*
  ep.serialContents(0x5500,0x6000);
  for (int i = 0; i < 256; i++) {
    ep.setAddress(i, true);
    delay(30);
  }*/

  
}