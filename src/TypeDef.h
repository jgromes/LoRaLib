#ifndef _LORALIB_TYPES_H
#define _LORALIB_TYPES_H

#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #error "Unsupported Arduino version (< 1.0.0)"
#endif

/*
 * Uncomment to enable debug output.
 * Warning: Debug output will slow down the whole system significantly.
 *          Also, it will result in larger compiled binary.
 * Levels: debug - only main info
 *         verbose - full transcript of all SPI/UART communication
 */

//#define RADIOLIB_DEBUG
//#define RADIOLIB_VERBOSE

// set which Serial port should be used for debug output
#define RADIOLIB_DEBUG_PORT   Serial

#ifdef RADIOLIB_DEBUG
  #define RADIOLIB_DEBUG_PRINT(...) { RADIOLIB_DEBUG_PORT.print(__VA_ARGS__); }
  #define RADIOLIB_DEBUG_PRINTLN(...) { RADIOLIB_DEBUG_PORT.println(__VA_ARGS__); }
#else
  #define RADIOLIB_DEBUG_PRINT(...) {}
  #define RADIOLIB_DEBUG_PRINTLN(...) {}
#endif

#ifdef RADIOLIB_VERBOSE
  #define RADIOLIB_VERBOSE_PRINT(...) { RADIOLIB_DEBUG_PORT.print(__VA_ARGS__); }
  #define RADIOLIB_VERBOSE_PRINTLN(...) { RADIOLIB_DEBUG_PORT.println(__VA_ARGS__); }
#else
  #define RADIOLIB_VERBOSE_PRINT(...) {}
  #define RADIOLIB_VERBOSE_PRINTLN(...) {}
#endif

/*
 * Uncomment to enable god mode - all methods and member variables in all classes will be made public, thus making them accessible from Arduino code.
 * Warning: Come on, it's called GOD mode - obviously only use this if you know what you're doing.
 *          Failure to heed the above warning may result in bricked module.
 */
//#define RADIOLIB_GODMODE

// Shield configuration
#define RADIOLIB_USE_SPI                      0x00
#define RADIOLIB_USE_UART                     0x01
#define RADIOLIB_USE_I2C                      0x02
#define RADIOLIB_INT_NONE                     0x00
#define RADIOLIB_INT_0                        0x01
#define RADIOLIB_INT_1                        0x02
#define RADIOLIB_INT_BOTH                     0x03

// Status/error codes

/*!
  \defgroup status_codes Status Codes

  \{
*/

/*!
  \brief No error, method executed successfully.
*/
#define ERR_NONE                               0

/*!
  \brief There was an unexpected, unknown error. If you see this, something went incredibly wrong.
  Your Arduino may be possessed, contact your local exorcist to resolve this error.
*/
#define ERR_UNKNOWN                           -1

/*!
  \brief %SX127x chip was not found during initialization. This can be caused by specifying wrong chip type in the constructor
  (i.e. calling SX1272 constructor for SX1278 chip) or by a fault in your wiring (incorrect slave select pin).
*/
#define ERR_CHIP_NOT_FOUND                    -2

/*!
  \brief Failed to allocate memory for temporary buffer. This can be cause by not enough RAM or by passing invalid pointer.
*/
#define ERR_MEMORY_ALLOCATION_FAILED          -3

/*!
  \brief Packet supplied to transmission method was longer than 255 bytes.
  %SX127x chips can not send more than 255 bytes in a single %LoRa transmission.
  Length limit is reduced to 63 bytes for FSK transmissions.
*/
#define ERR_PACKET_TOO_LONG                   -4

/*!
  \brief Timed out waiting for TxDone signal from %SX127x. This error can be caused by faulty wiring (unreliable connection at DIO0).
*/
#define ERR_TX_TIMEOUT                        -5

/*!
  \brief No packet arrived within a timeframe.
*/
#define ERR_RX_TIMEOUT                        -6

/*!
  \brief The calculated and expected CRCs of received packet do not match.
  This means that the packet was damaged during transmission and should be sent again.
*/
#define ERR_CRC_MISMATCH                      -7

/*!
  \brief The supplied bandwidth value is invalid for this module.
*/
#define ERR_INVALID_BANDWIDTH                 -8

/*!
  \brief The supplied spreading factor value is invalid for this module.
*/
#define ERR_INVALID_SPREADING_FACTOR          -9

/*!
  \brief The supplied coding rate value is invalid.
*/
#define ERR_INVALID_CODING_RATE               -10

/*!
  \brief Internal only.
*/
#define ERR_INVALID_BIT_RANGE                 -11

/*!
  \brief The supplied frequency value is invalid for this module.
*/
#define ERR_INVALID_FREQUENCY                 -12

/*!
  \brief The supplied output power is invalid.
*/
#define ERR_INVALID_OUTPUT_POWER              -13

/*!
  \brief %LoRa preamble was detected during channel activity detection.
  This means that there is some %LoRa device currently transmitting in your channel.
*/
#define PREAMBLE_DETECTED                     -14

/*!
  \brief No %LoRa preambles were detected during channel activity detection. Your channel is free.
*/
#define CHANNEL_FREE                          -15

/*!
  \brief Real value in SPI register does not match the expected one. This can be caused by faulty SPI wiring.
*/
#define ERR_SPI_WRITE_FAILED                  -16

/*!
  \brief The supplied current limit value is invalid.
*/
#define ERR_INVALID_CURRENT_LIMIT             -17

/*!
  \brief The supplied preamble length is invalid.
*/
#define ERR_INVALID_PREAMBLE_LENGTH           -18

/*!
  \brief The supplied gain value is invalid.
*/
#define ERR_INVALID_GAIN                      -19

/*!
  \brief User tried to execute modem-exclusive method on a wrong modem.
  For example, this can happen when you try to change %LoRa configuration when FSK modem is active.
*/
#define ERR_WRONG_MODEM                       -20

/*!
  \brief The supplied bit rate value is invalid.
*/
#define ERR_INVALID_BIT_RATE                  -21

/*!
  \brief The supplied receiver bandwidth value is invalid.
*/
#define ERR_INVALID_RX_BANDWIDTH              -22

/*!
  \brief The supplied frequency deviation value is invalid.
*/
#define ERR_INVALID_FREQUENCY_DEVIATION       -23

/*!
  \brief The supplied FSK sync word is invalid. The sync word is either longer than 8 bytes or contains null-bytes.
*/
#define ERR_INVALID_SYNC_WORD                 -24

/*!
  \brief The supplied FSK data shaping option is invalid.
*/
#define ERR_INVALID_DATA_SHAPING              -25

/*!
  \brief The current modulation is invalid for the requested operation.
*/
#define ERR_INVALID_MODULATION                -26

/*!
  \brief The supplied number of RSSI samples is invalid.
*/
#define ERR_INVALID_NUM_SAMPLES               -27

/*!
  \brief The supplied RSSI offset is invalid.
*/
#define ERR_INVALID_RSSI_OFFSET               -28

/*!
  \brief The supplied encoding is invalid.
*/
#define ERR_INVALID_ENCODING                  -29

/*!
  \}
*/

#endif
