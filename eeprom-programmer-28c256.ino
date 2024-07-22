//eeprom programmer for 28c256

#include "programmer_28c256.h"

//test code from Ben Eater's 6502 series - part 2
void writeTestCode() { 
  writeEEPROM(0, 0xa9); //lda #$ff
  writeEEPROM(1, 0xff);

  writeEEPROM(2, 0x8d); //sta $6002
  writeEEPROM(3, 0x02);
  writeEEPROM(4, 0x60);

  writeEEPROM(5, 0xa9); //lda #$55
  writeEEPROM(6, 0x55);

  writeEEPROM(7, 0x8d); //sta $6000
  writeEEPROM(8, 0x00);
  writeEEPROM(9, 0x60);

  writeEEPROM(10, 0xa9); //lda #$aa
  writeEEPROM(11, 0xaa);

  writeEEPROM(12, 0x8d); //sta $6000
  writeEEPROM(13, 0x00);
  writeEEPROM(14, 0x60);

  writeEEPROM(15, 0x4c); //jmp $8005  
  writeEEPROM(16, 0x05);
  writeEEPROM(17, 0x80);

  writeEEPROM(0x7ffc, 0x00); //set entry point
  writeEEPROM(0x7ffd, 0x80);
}

//file the void with ea(NO OP instruction on the 6502)
void noOP(int startAddress, int endAddress) {
  for (int base = startAddress; base <= endAddress; base++) {
    writeEEPROM(base, 0xea);
  }
}

void setup() {
  programmerSetup();
}

void loop() {
  delay(100);
  serialContents(0,0xff);
  serialContents(0x7F00,0x7FFF);
  for (int i = 0; i < 256; i++) {
    setAddress(i, true);
    delay(30);
  }
}