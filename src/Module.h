#ifndef _LORALIB_MODULE_H
#define _LORALIB_MODULE_H

#include <SPI.h>

#include "TypeDef.h"

#define SPI_READ  0b00000000
#define SPI_WRITE 0b10000000

#if defined(ESP32) || defined(ESP8266)
  // ESP32/ESP8266 boards (pin 10 conflicts with ESP32/ESP8266 flash connections)
  #define LORALIB_DEFAULT_SPI_CS                      4
#else
  // all other architectures
  #define LORALIB_DEFAULT_SPI_CS                      10
#endif

/*!
  \class Module

  \brief Implements all common low-level SPI methods to control the %LoRa chip. The base class SX127x contains private instance of this class.
*/
class Module {
  public:

    /*!
      \brief Default constructor. Called internally when creating new LoRa instance.

      \param cs Arduino pin that will be used as chip select signal for SPI. Defaults to 4 on ESP32/ESP8266 boards, or to 10 on other architectures.

      \param int0 Arduino pin that will be used as interrupt/GPIO 0. Connect to SX127x/RFM9x pin DIO0.
      Does not need to be connected to Arduino interrupt pin, unless Arduino sketch is using interrupt-driven transmit/receive methods.

      \param int1 Arduino pin that will be used as interrupt/GPIO 1. Connect to SX127x/RFM9x pin DIO1.
      Does not need to be connected to Arduino interrupt pin, unless Arduino sketch is using interrupt-driven transmit/receive methods.

      \param spi SPIClass instance that will be used for SPI bus control. This can be hardware SPI or some software SPI driver.
    */
    Module(int cs = LORALIB_DEFAULT_SPI_CS, int int0 = 2, int int1 = 3, SPIClass& spi = SPI);

    /*!
      \brief Initialization method. Called internally when connecting to the %LoRa chip and should not be called explicitly from Arduino code.

      \param interface %Module interface that should be used. Required for RadioLib compatibility, will always be set to USE_SPI.

      \param gpio Determines which interrupt/GPIO should be used. Required for RadioLib compatibility, will always be set to INT_BOTH.
    */
    void init(uint8_t interface, uint8_t gpio);

    /*!
      \brief Termination method. Called internally when required %LoRa chip is not found and should not be called explicitly from Arduino code.
    */
    void term();



    /*!
      \brief SPI read method that automatically masks unused bits. This method is the preferred SPI read mechanism.

      \param reg Address of SPI register to read.

      \param msb Most significant bit of the register variable. Bits above this one will be masked out.

      \param lsb Least significant bit of the register variable. Bits below this one will be masked out.

      \returns Masked register value or status code.
    */
    int16_t SPIgetRegValue(uint8_t reg, uint8_t msb = 7, uint8_t lsb = 0);

    /*!
      \brief Overwrite-safe SPI write method with verification. This method is the preferred SPI write mechanism.

      \param reg Address of SPI register to write.

      \param value Single byte value that will be written to the SPI register.

      \param msb Most significant bit of the register variable. Bits above this one will not be affected by the write operation.

      \param lsb Least significant bit of the register variable. Bits below this one will not be affected by the write operation.

      \param checkInterval Number of milliseconds between register writing and verification reading. Some registers need up to 10ms to process the change.

      \returns \ref status_codes
    */
    int16_t SPIsetRegValue(uint8_t reg, uint8_t value, uint8_t msb = 7, uint8_t lsb = 0, uint8_t checkInterval = 2);



    /*!
      \brief SPI burst read method.

      \param reg Address of SPI register to read.

      \param numBytes Number of bytes that will be read.

      \param inBytes Pointer to array that will hold the read data.
    */
    void SPIreadRegisterBurst(uint8_t reg, uint8_t numBytes, uint8_t* inBytes);

    /*!
      \brief SPI basic read method. Use of this method is reserved for special cases, SPIgetRegValue should be used instead.

      \param reg Address of SPI register to read.

      \returns Value that was read from register.
    */
    uint8_t SPIreadRegister(uint8_t reg);



    /*!
      \brief SPI burst write method.

      \param reg Address of SPI register to write.

      \param data Pointer to array that holds the data that will be written.

      \param numBytes Number of bytes that will be written.
    */
    void SPIwriteRegisterBurst(uint8_t reg, uint8_t* data, uint8_t numBytes);

    /*!
      \brief SPI basic write method. Use of this method is reserved for special cases, SPIsetRegValue should be used instead.

      \param reg Address of SPI register to write.

      \param data Value that will be written to the register.
    */
    void SPIwriteRegister(uint8_t reg, uint8_t data);

    /*!
      \brief SPI single transfer method.

      \param cmd SPI access command (read/write/burst/...).

      \param reg Address of SPI register to transfer to/from.

      \param dataOut Data that will be transfered from master to slave.

      \param dataIn Data that was transfered from slave to master.

      \param numBytes Number of bytes to transfer.
    */
    void SPItransfer(uint8_t cmd, uint8_t reg, uint8_t* dataOut, uint8_t* dataIn, uint8_t numBytes);


    /*!
      \brief Access method to get the pin number of interrupt/GPIO 0.

      \returns Pin number of interrupt/GPIO 0 configured in the constructor.
    */
    int getInt0() const { return(_int0); }

    /*!
      \brief Access method to get the pin number of interrupt/GPIO 1.

      \returns Pin number of interrupt/GPIO 1 configured in the constructor.
    */
    int getInt1() const { return(_int1); }

  private:
    int _cs;
    int _int0;
    int _int1;

    SPIClass* _spi;
};

#endif
