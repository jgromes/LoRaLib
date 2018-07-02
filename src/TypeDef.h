#ifndef _LORALIB_TYPES_H
#define _LORALIB_TYPES_H

#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

//#define LORALIB_DEBUG

#ifdef LORALIB_DEBUG
  //#define LORALIB_VERBOSE
  #define BYTE_TO_BINARY(byte)  \
    ((byte & 0x80) ? '1' : '0'), \
    ((byte & 0x40) ? '1' : '0'), \
    ((byte & 0x20) ? '1' : '0'), \
    ((byte & 0x10) ? '1' : '0'), \
    ((byte & 0x08) ? '1' : '0'), \
    ((byte & 0x04) ? '1' : '0'), \
    ((byte & 0x02) ? '1' : '0'), \
    ((byte & 0x01) ? '1' : '0') 
  #define DEBUG_BEGIN(x)                Serial.begin (x)
  #define DEBUG_PRINT(x)                Serial.print (x)
  #define DEBUG_PRINT_DEC(x)            Serial.print (x, DEC)
  #define DEBUG_PRINT_HEX(x)            Serial.print (x, HEX)
  #define DEBUG_PRINTLN(x)              Serial.println (x)
  #define DEBUG_PRINT_STR(x)            Serial.print (F(x))
  #define DEBUG_PRINTLN_STR(x)          Serial.println (F(x))
#else
  #define BYTE_TO_BINARY(byte)
  #define DEBUG_BEGIN(x)
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINT_DEC(x)
  #define DEBUG_PRINT_HEX(x)
  #define DEBUG_PRINTLN(x)
  #define DEBUG_PRINT_STR(x)
  #define DEBUG_PRINTLN_STR(x)
#endif

#define ERR_NONE                        0x00
#define ERR_CHIP_NOT_FOUND              0x01
#define ERR_EEPROM_NOT_INITIALIZED      0x02

#define ERR_PACKET_TOO_LONG             0x10
#define ERR_TX_TIMEOUT                  0x11

#define ERR_RX_TIMEOUT                  0x20
#define ERR_CRC_MISMATCH                0x21

#define ERR_INVALID_BANDWIDTH           0x30
#define ERR_INVALID_SPREADING_FACTOR    0x31
#define ERR_INVALID_CODING_RATE         0x32
#define ERR_INVALID_FREQUENCY           0x33
#define ERR_INVALID_OUTPUT_POWER        0x34

#define ERR_INVALID_BIT_RANGE           0x40

#define CHANNEL_FREE                    0x50
#define PREAMBLE_DETECTED               0x51

enum Chip {CH_SX1272 = 0, CH_SX1273, CH_SX1276, CH_SX1277, CH_SX1278, CH_SX1279};

#endif
