#ifndef _LORALIB_TYPES_H
#define _LORALIB_TYPES_H

#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

//#define DEBUG

#ifdef DEBUG
  //#define VERBOSE
  #define BYTE_TO_BINARY(byte)  \
    ((byte & 0x80) ? '1' : '0'), \
    ((byte & 0x40) ? '1' : '0'), \
    ((byte & 0x20) ? '1' : '0'), \
    ((byte & 0x10) ? '1' : '0'), \
    ((byte & 0x08) ? '1' : '0'), \
    ((byte & 0x04) ? '1' : '0'), \
    ((byte & 0x02) ? '1' : '0'), \
    ((byte & 0x01) ? '1' : '0') 
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

enum Chip {CH_SX1272, CH_SX1273, CH_SX1276, CH_SX1277, CH_SX1278, CH_SX1279};

#endif
