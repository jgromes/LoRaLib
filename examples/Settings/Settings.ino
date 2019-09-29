/*
   LoRaLib Settings Example

   This example shows how to change all the properties
   of LoRa transmission. LoRaLib currently supports
   the following settings:
    - pins (SPI slave select, digital IO 0, digital IO 1)
    - carrier frequency
    - bandwidth
    - spreading factor
    - coding rate
    - sync word
    - output power during transmission
    - over current protection limit
    - LoRa preamble length
    - amplifier gain

   For more detailed information, see the LoRaLib Wiki
   https://github.com/jgromes/LoRaLib/wiki

   For full API reference, see the GitHub Pages
   https://jgromes.github.io/LoRaLib/
*/

// include the library
#include <LoRaLib.h>

// create instance of LoRa class using SX1278 module
// this pinout corresponds to RadioShield
// https://github.com/jgromes/RadioShield
// NSS pin:   10 (4 on ESP32/ESP8266 boards)
// DIO0 pin:  2
// DIO1 pin:  3
SX1278 loraSX1278 = new LoRa;

// create another instance of LoRa class using
// SX1272 module and user-specified pinout
// NSS pin:   6
// DIO0 pin:  4
// DIO1 pin:  5
SX1272 loraSX1272 = new LoRa(6, 4, 5);

// create third instance of LoRa class using
// SX1276 module and user-specified pinout
// we ran out of Uno digital pins, so here we use
// analog ones
// NSS pin:   14 (A0)
// DIO0 pin:  15 (A1)
// DIO1 pin:  16 (A2)
SX1276 loraSX1276 = new LoRa(14, 15, 16);

// to use non-default SPI interface or software SPI
// implementation, pass the SPI interface to the LoRa
// constructor - this interface must be compatible
// with Arduino SPIClass!
//
// Software SPI library
// https://github.com/MajenkoLibraries/SoftSPI
//#include "SoftSPI.h"
//SoftSPI mySPI(4, 5, 6) ;
//SX1278 softLora = new LoRa(2, 3, 10, mySPI);

void setup() {
  Serial.begin(9600);

  // initialize the LoRa module with default settings
  Serial.print(F("Initializing SX1278 ... "));
  // carrier frequency:           434.0 MHz
  // bandwidth:                   125.0 kHz
  // spreading factor:            9
  // coding rate:                 7
  // sync word:                   0x12
  // output power:                17 dBm
  // current limit:               100 mA
  // preamble length:             8 symbols
  // amplifier gain:              0 (automatic gain control)
  int state = loraSX1278.begin();
  if (state == ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }

  // initialize the second LoRa instance with
  // non-default settings
  // this LoRa link will have maximum range,
  // but very low data rate
  Serial.print(F("Initializing SX1276 ... "));
  // carrier frequency:           434.0 MHz
  // bandwidth:                   7.8 kHz
  // spreading factor:            12
  // coding rate:                 8
  // sync word:                   0x13
  // output power:                17 dBm
  // current limit:               100 mA
  // preamble length:             8 symbols
  // amplifier gain:              0 (automatic gain control)
  state = loraSX1276.begin(434.0, 7.8, 12, 8, 0x13);
  if (state == ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }

  // initialize the third LoRa instance with
  // non-default settings
  // this LoRa link will have high data rate,
  // but lower range
  // NOTE: when using spreading factor 6, the total packet
  //       length has to be known in advance!
  //       you have to pass the number of expected bytes
  //       to the receive() method
  Serial.print(F("Initializing SX1272 ... "));
  // carrier frequency:           915.0 MHz
  // bandwidth:                   500.0 kHz
  // spreading factor:            6
  // coding rate:                 5
  // sync word:                   0x14
  // output power:                2 dBm
  // current limit:               50 mA
  // preamble length:             20 symbols
  // amplifier gain:              1 (maximum gain)
  state = loraSX1272.begin(915.0, 500.0, 6, 5, 0x14, 2, 50, 20);
  if (state == ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }

  // you can also change the settings at runtime

  // different modules accept different parameters, see
  // https://github.com/jgromes/LoRaLib/wiki/Supported-LoRa-modules

  // you can check if the setting was changed successfully

  // set bandwidth to 250 kHz
  if (loraSX1278.setBandwidth(250.0) == ERR_INVALID_BANDWIDTH) {
    Serial.println(F("Selected bandwidth is invalid for this module!"));
    while (true);
  }

  // set spreading factor to 10
  if (loraSX1278.setSpreadingFactor(10) == ERR_INVALID_SPREADING_FACTOR) {
    Serial.println(F("Selected spreading factor is invalid for this module!"));
    while (true);
  }

  // set coding rate to 6
  if (loraSX1278.setCodingRate(6) == ERR_INVALID_CODING_RATE) {
    Serial.println(F("Selected coding rate is invalid for this module!"));
    while (true);
  }

  // set carrier frequency to 433.5 MHz
  if (loraSX1278.setFrequency(433.5) == ERR_INVALID_FREQUENCY) {
    Serial.println(F("Selected frequency is invalid for this module!"));
    while (true);
  }

  // set LoRa sync word to 0x14
  // NOTE: value 0x34 is reserved for LoRaWAN networks and should not be used
  if (loraSX1278.setSyncWord(0x14) != ERR_NONE) {
    Serial.println(F("Unable to set sync word!"));
    while (true);
  }

  // set output power to 10 dBm (accepted range is -3 - 17 dBm)
  // NOTE: 20 dBm value allows high power operation, but transmission
  //       duty cycle MUST NOT exceed 1%
  if (loraSX1278.setOutputPower(10) == ERR_INVALID_OUTPUT_POWER) {
    Serial.println(F("Selected output power is invalid for this module!"));
    while (true);
  }

  // set over current protection limit to 80 mA (accepted range is 45 - 240 mA)
  // NOTE: set value to 0 to disable overcurrent protection
  if (loraSX1278.setCurrentLimit(80) == ERR_INVALID_CURRENT_LIMIT) {
    Serial.println(F("Selected current limit is invalid for this module!"));
    while (true);
  }

  // set LoRa preamble length to 15 symbols (accepted range is 6 - 65535)
  if (loraSX1278.setPreambleLength(15) == ERR_INVALID_PREAMBLE_LENGTH) {
    Serial.println(F("Selected preamble length is invalid for this module!"));
    while (true);
  }

  // set amplifier gain to 1 (accepted range is 1 - 6, where 1 is maximum gain)
  // NOTE: set value to 0 to enable automatic gain control
  //       leave at 0 unless you know what you're doing
  if (loraSX1278.setGain(1) == ERR_INVALID_GAIN) {
    Serial.println(F("Selected gain is invalid for this module!"));
    while (true);
  }

  // disable automatic CRC check on received packets
  // NOTE: packets that do not pass the check are discarded automatically
  if (loraSX1278.setCRC(false) != ERR_NONE) {
    Serial.println(F("Unable to set CRC!"));
    while (true);
  }

  Serial.println(F("All settings successfully changed!"));
}

void loop() {
  // nothing here
}
