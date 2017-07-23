#ifndef _LORALIB_H
#define _LORALIB_H

#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include <SPI.h>
#include <EEPROM.h>

#include "SX1278.h"

#define DEBUG

#ifdef DEBUG
  //#define VERBOSE
#endif

#define BYTE_TO_BINARY(byte)  \
  ((byte & 0x80) ? '1' : '0'), \
  ((byte & 0x40) ? '1' : '0'), \
  ((byte & 0x20) ? '1' : '0'), \
  ((byte & 0x10) ? '1' : '0'), \
  ((byte & 0x08) ? '1' : '0'), \
  ((byte & 0x04) ? '1' : '0'), \
  ((byte & 0x02) ? '1' : '0'), \
  ((byte & 0x01) ? '1' : '0') 

//TODO: move packet class into a separate file
class packet {
  public:
    packet(void);
    packet(const char dest[24], const char dat[240]);
    packet(uint8_t dest[8], const char dat[240]);
    packet(const char src[24], const char dest[24], const char dat[240]);
    packet(uint8_t src[8], uint8_t dest[8], const char dat[240]);
    
    uint8_t source[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    uint8_t destination[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    char data[240] = {'\0'};
    uint8_t length = 0;
    
    void getSourceStr(char src[24]);
    void getDestinationStr(char dest[24]);
    
    void setSourceStr(const char src[24]);
    void setDestinationStr(const char dest[24]);
    
  private:
    void init(uint8_t src[8], uint8_t dest[8], const char dat[240]);
    void getLoRaAddress(uint8_t addr[8]);
    
    uint8_t parseByte(char c);
    char reparseChar(uint8_t b);
};

class LoRa {
  public:
    LoRa(int nss = 7, uint8_t bw = SX1278_BW_7_80_KHZ, uint8_t cr = SX1278_CR_4_5, uint8_t sf = SX1278_SF_12);
    
    uint8_t init(uint16_t addrEeprom = 0, uint16_t addrFlag = 8);
    
    //TODO: implement header mode selection
    uint8_t tx(packet& pack, uint16_t timeout = 0);
    uint8_t rx(packet& pack, uint16_t timeout = 0, uint8_t mode = SX1278_RXSINGLE);
    
    uint8_t setMode(uint8_t mode);
    uint8_t config(uint8_t bw, uint8_t cr, uint8_t sf);
    
    void getLoRaAddress(uint8_t addr[8]);
    
    uint8_t getBW(void);
    uint8_t getCR(void);
    uint8_t getSF(void);
    
    #ifdef VERBOSE
      void regDump(void);
    #endif
    
    #ifdef DEBUG
      int freeRAM(void);
      void printStatus(void);
    #endif
    
  private:
    uint8_t _address[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    uint8_t _bw, _cr, _sf;
    uint16_t _addrEeprom, _addrFlag;
    
    int _nss;
    int _dio0 = 2;
    int _dio1 = 3;
    
    #if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
      int _sck = 13;
      int _miso = 12;
      int _mosi = 11;
    #elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
      int _sck = 52;
      int _miso = 50;
      int _mosi = 51;
    #else
      #error "Unsupported board selected, please select Arduino UNO or Mega"
    #endif
    
    uint8_t setRegValue(uint8_t reg, uint8_t value, uint8_t msb = 7, uint8_t lsb = 0);
    uint8_t getRegValue(uint8_t reg, uint8_t msb = 7, uint8_t lsb = 0);
    void clearIRQFlags(void);
    
    uint8_t readRegisterBurst(uint8_t reg, uint8_t numBytes, uint8_t* inBytes);
    uint8_t readRegisterBurstStr(uint8_t reg, uint8_t numBytes, char* str);
    void writeRegisterBurst(uint8_t reg, uint8_t* data, uint8_t numBytes);
    void writeRegisterBurstStr(uint8_t reg, const char* data, uint8_t numBytes);
    
    uint8_t readRegister(uint8_t reg);
    void writeRegister(uint8_t reg, uint8_t data);
    
    void generateLoRaAdress(void);
};

#endif
