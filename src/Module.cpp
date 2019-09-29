#include "Module.h"

Module::Module(int cs, int int0, int int1, SPIClass& spi) {
  // save pins numbers to private global variables
  _cs = cs;
  _int0 = int0;
  _int1 = int1;
  _spi = &spi;
}

void Module::init(uint8_t interface, uint8_t gpio) {
  // select interface
  switch(interface) {
    case USE_SPI:
      pinMode(_cs, OUTPUT);
      digitalWrite(_cs, HIGH);
      _spi->begin();
      break;
    case USE_UART:
      break;
    case USE_I2C:
      break;
  }

  // select GPIO
  switch(gpio) {
    case INT_NONE:
      break;
    case INT_0:
      pinMode(_int0, INPUT);
      break;
    case INT_1:
      pinMode(_int1, INPUT);
      break;
    case INT_BOTH:
      pinMode(_int0, INPUT);
      pinMode(_int1, INPUT);
      break;
  }
}

void Module::term() {
  // stop SPI
  _spi->end();
}

int16_t Module::SPIgetRegValue(uint8_t reg, uint8_t msb, uint8_t lsb) {
  // check MSB/LSB range
  if((msb > 7) || (lsb > 7) || (lsb > msb)) {
    return(ERR_INVALID_BIT_RANGE);
  }

  // get current register value
  uint8_t rawValue = SPIreadRegister(reg);

  // mask the register value
  uint8_t maskedValue = rawValue & ((0b11111111 << lsb) & (0b11111111 >> (7 - msb)));
  return(maskedValue);
}

int16_t Module::SPIsetRegValue(uint8_t reg, uint8_t value, uint8_t msb, uint8_t lsb, uint8_t checkInterval) {
  // check MSB/LSB range
  if((msb > 7) || (lsb > 7) || (lsb > msb)) {
    return(ERR_INVALID_BIT_RANGE);
  }

  // get current raw register value
  uint8_t currentValue = SPIreadRegister(reg);

  // mask the bits that should be kept
  uint8_t mask = ~((0b11111111 << (msb + 1)) | (0b11111111 >> (8 - lsb)));

  // calculate the new raw register value
  uint8_t newValue = (currentValue & ~mask) | (value & mask);

  // write the new raw value into register
  SPIwriteRegister(reg, newValue);

  // check register value each millisecond until check interval is reached
  // some registers need a bit of time to process the change (e.g. SX127X_REG_OP_MODE)
  uint32_t start = micros();
  uint8_t readValue = 0;
  while(micros() - start < (checkInterval * 1000)) {
    readValue = SPIreadRegister(reg);
    if(readValue == newValue) {
      // check passed, we can stop the loop
      return(ERR_NONE);
    }
  }

  // check failed, print debug info
  RADIOLIB_DEBUG_PRINTLN();
  RADIOLIB_DEBUG_PRINT(F("address:\t0x"));
  RADIOLIB_DEBUG_PRINTLN(reg, HEX);
  RADIOLIB_DEBUG_PRINT(F("bits:\t\t"));
  RADIOLIB_DEBUG_PRINT(msb);
  RADIOLIB_DEBUG_PRINT(' ');
  RADIOLIB_DEBUG_PRINTLN(lsb);
  RADIOLIB_DEBUG_PRINT(F("value:\t\t0b"));
  RADIOLIB_DEBUG_PRINTLN(value, BIN);
  RADIOLIB_DEBUG_PRINT(F("current:\t0b"));
  RADIOLIB_DEBUG_PRINTLN(currentValue, BIN);
  RADIOLIB_DEBUG_PRINT(F("mask:\t\t0b"));
  RADIOLIB_DEBUG_PRINTLN(mask, BIN);
  RADIOLIB_DEBUG_PRINT(F("new:\t\t0b"));
  RADIOLIB_DEBUG_PRINTLN(newValue, BIN);
  RADIOLIB_DEBUG_PRINT(F("read:\t\t0b"));
  RADIOLIB_DEBUG_PRINTLN(readValue, BIN);
  RADIOLIB_DEBUG_PRINTLN();

  return(ERR_SPI_WRITE_FAILED);
}

void Module::SPIreadRegisterBurst(uint8_t reg, uint8_t numBytes, uint8_t* inBytes) {
  SPItransfer(SPI_READ, reg, NULL, inBytes, numBytes);
}

uint8_t Module::SPIreadRegister(uint8_t reg) {
  uint8_t resp;
  SPItransfer(SPI_READ, reg, NULL, &resp, 1);
  return(resp);
}

void Module::SPIwriteRegisterBurst(uint8_t reg, uint8_t* data, uint8_t numBytes) {
  SPItransfer(SPI_WRITE, reg, data, NULL, numBytes);
}

void Module::SPIwriteRegister(uint8_t reg, uint8_t data) {
  SPItransfer(SPI_WRITE, reg, &data, NULL, 1);
}

void Module::SPItransfer(uint8_t cmd, uint8_t reg, uint8_t* dataOut, uint8_t* dataIn, uint8_t numBytes) {
  // start SPI transaction
  _spi->beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));

  // pull CS low
  digitalWrite(_cs, LOW);

  // send SPI register address with access command
  _spi->transfer(reg | cmd);

  // send data or get response
  switch(cmd) {
    case SPI_WRITE:
      for(size_t n = 0; n < numBytes; n++) {
        _spi->transfer(dataOut[n]);
      }
      break;
    case SPI_READ:
      for(size_t n = 0; n < numBytes; n++) {
        dataIn[n] = _spi->transfer(0x00);
      }
      break;
    default:
      break;
  }

  // release CS
  digitalWrite(_cs, HIGH);

  // end SPI transaction
  _spi->endTransaction();
}
