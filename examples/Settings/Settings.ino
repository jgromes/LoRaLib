/*
 * LoRaLib Settings Example
 * 
 * This example shows how to change all the properties of LoRa transmission.
 * LoRaLib currently supports the following settings:
 *  - pins (SPI slave select, digital IO 0, digital IO 1)
 *  - carrier frequency
 *  - bandwidth
 *  - spreading factor
 *  - coding rate
 *  - sync word
 *  - output power during transmission
 * 
 * For more detailed information, see the LoRaLib Wiki
 * https://github.com/jgromes/LoRaLib/wiki
 */

// include the library
#include <LoRaLib.h>

// create instance of LoRa class using SX1278 module 
// this pinout corresponds to LoRenz shield: https://github.com/jgromes/LoRenz
// NSS pin:   7
// DIO0 pin:  2
// DIO1 pin:  3
SX1278 loraSX1278 = new LoRa;

// create another instance of LoRa class using SX1272 module and user-specified pinout
// NSS pin:   6
// DIO0 pin:  4
// DIO1 pin:  5
SX1272 loraSX1272 = new LoRa(6, 4, 5);

// create third instance of LoRa class using SX1276 module and user-specified pinout
// we ran out of Uno digital pins, so here we use analog ones
// NSS pin:   A0
// DIO0 pin:  A1
// DIO1 pin:  A2
SX1276 loraSX1276 = new LoRa(A0, A1, A2);

void setup() {
  Serial.begin(9600);

  // initialize the first LoRa instance with default settings
  // LoRa link with this setting is a balance between range and data rate
  //
  // carrier frequency:   434.0 MHz
  // bandwidth:           125000 Hz
  // spreading factor:    9
  // coding rate:         7
  // Sync word:           0x12
  uint8_t state = loraSX1278.begin();
  if(state != ERR_NONE) {
    Serial.print("SX1278 initialization failed, code 0x");
    Serial.println(state, HEX);
    while(true);
  }

  // initialize the second LoRa instance with non-default settings
  // this LoRa link will have maximum range, but very low data rate
  //
  // carrier frequency:   434.0 MHz
  // bandwidth:           7800 Hz
  // spreading factor:    12
  // coding rate:         8
  // Sync word:           0x13
  state = loraSX1272.begin(434.0, 7800, 12, 8, 0x13);
  if(state != ERR_NONE) {
    Serial.print("SX1272 initialization failed, code 0x");
    Serial.println(state, HEX);
    while(true);
  }

  // initialize the second LoRa instance with non-default settings
  // this LoRa link will have high data rate, but lower range
  // NOTE: when using spreading factor 6, the total packet length has to be known in advance!
  //       it can be set using the length variable of your Packet instance 
  //       Packet::length = x;
  //       where x is the total packet length including both addresses
  //
  // carrier frequency:   915.0 MHz
  // bandwidth:           500000 Hz
  // spreading factor:    6
  // coding rate:         5
  // Sync word:           0x14
  state = loraSX1276.begin(915.0, 500000, 6, 5, 0x14);
  if(state != ERR_NONE) {
    Serial.print("SX1276 initialization failed, code 0x");
    Serial.println(state, HEX);
    while(true);
  }

  // you can also change the settings at runtime
  // different modules accept different parameters
  // you can check if the setting was changed successfully

  // set bandwidth to 250 kHz
  if(loraSX1278.setBandwidth(250000) == ERR_INVALID_BANDWIDTH) {
    Serial.println("Selected bandwidth is invalid for this module!");
    while(true);
  }

  // set spreading factor to 10
  if(loraSX1278.setSpreadingFactor(10) == ERR_INVALID_SPREADING_FACTOR) {
    Serial.println("Selected spreading factor is invalid for this module!");
    while(true);
  }

  // set coding rate to 6
  if(loraSX1278.setCodingRate(6) == ERR_INVALID_CODING_RATE) {
    Serial.println("Selected coding rate is invalid for this module!");
    while(true);
  }

  // set carrier frequency to 433.5 MHz
  if(loraSX1278.setFrequency(433.5) == ERR_INVALID_FREQUENCY) {
    Serial.println("Selected frequency is invalid for this module!");
    while(true);
  }

  // set LoRa sync word to 0x14
  // NOTE: value 0x34 is reserved for LoRaWAN networks and should not be used
  if(loraSX1278.setSyncWord(0x14) != ERR_NONE) {
    Serial.println("Unable to set sync word!");
    while(true);
  }

  // set output power to 10 dBm (accepted range is 2 - 17 dBm)
  if(loraSX1278.setOutputPower(10) == ERR_INVALID_OUTPUT_POWER) {
    Serial.println("Selected output power is invalid for this module!");
    while(true);
  }

  Serial.println("All settings succesfully changed!");
}

void loop() {
  // nothing here
}
