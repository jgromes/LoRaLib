#ifndef _LORALIB_MODULE_H
#define _LORALIB_MODULE_H

#include <SPI.h>

#include "TypeDef.h"

#define SPI_READ  0b00000000
#define SPI_WRITE 0b10000000

#ifdef ESP32
  // ESP32 boards: pin 7 conflicts with ESP32 flash connections
  #define LORALIB_DEFAULT_SPI_CS                      4
#else
  // all other architectures: pin 7 does not conflict with anything
  #define LORALIB_DEFAULT_SPI_CS                      7
#endif

class Module {
  public:
    Module(int cs = LORALIB_DEFAULT_SPI_CS, int int0 = 2, int int1 = 3);
    
    void init(uint8_t interface, uint8_t gpio);
    
    int16_t SPIgetRegValue(uint8_t reg, uint8_t msb = 7, uint8_t lsb = 0);
    int16_t SPIsetRegValue(uint8_t reg, uint8_t value, uint8_t msb = 7, uint8_t lsb = 0);
    
    void SPIreadRegisterBurst(uint8_t reg, uint8_t numBytes, uint8_t* inBytes);
    uint8_t SPIreadRegister(uint8_t reg);
    
    void SPIwriteRegisterBurst(uint8_t reg, uint8_t* data, uint8_t numBytes);
    void SPIwriteRegister(uint8_t reg, uint8_t data);
    
    int int0() const { return(_int0); }
    int int1() const { return(_int1); }
  
  private:
    int _cs;
    int _int0;
    int _int1;

};

#endif
