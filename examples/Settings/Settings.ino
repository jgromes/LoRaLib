/*
 * LoRaLib Settings Example
 * 
 * This example shows how to change all the properties of LoRa transmission.
 * LoRaLib currently supports the following settings:
 *  - bandwidth
 *  - spreading factor
 *  - coding rate
 *  - carrier frequency
 *  - pins (SPI slave select, digital IO 0, digital IO 1)
 *  - sync word
 *  - output power during transmission
 * 
 * For more detailed information, see the LoRaLib Wiki
 * https://github.com/jgromes/LoRaLib/wiki
 */

// include the library
#include <LoRaLib.h>

// create instance of LoRa class with default settings
// LoRa link with this setting is a balance between range and data rate
//
// chip:                SX1278
// NSS pin:             7
// carrier frequency:   434.0 MHz
// bandwidth:           125000 Hz
// spreading factor:    9
// coding rate:         7
// DIO0 pin:            2
// DIO1 pin:            3
// Sync word:           0x12
LoRa loraBalanced;

// create another LoRa instance with non-default settings
// this LoRa link will have maximum range, but very low data rate
//
// chip:                SX1276
// NSS pin:             6
// carrier frequency:   434.0 MHz
// bandwidth:           7800 Hz
// spreading factor:    12
// coding rate:         8
// DIO0 pin:            4
// DIO1 pin:            5
// Sync word:           0x13
LoRa loraMaxRange(CH_SX1276, 6, 434.0, 7800, 12, 8, 4, 5, 0x13);

// create third LoRa instance with non-default settings
// this LoRa link will have high data rate, but lower range
// NOTE: when using spreading factor 6, the total packet length has to be known in advance!
//       it can be set using the length variable of your Packet instance 
//       Packet::length = x;
//       where x is the total packet length including both addresses
//
// chip:                SX1272
// NSS pin:             4
// carrier frequency:   915.0 MHz
// bandwidth:           500000 Hz
// spreading factor:    6
// coding rate:         5
// DIO0 pin:            10
// DIO1 pin:            11
// Sync word:           0x14
LoRa loraMaxDataRate(CH_SX1272, 4, 915.0, 500000, 6, 5, 10, 11, 0x14);

void setup() {
  Serial.begin(9600);

  // initialize the LoRa instance with default settings
  uint8_t state = lora.begin();
  if(state != ERR_NONE) {
    Serial.print("Initialization failed, code 0x");
    Serial.println(state, HEX);
    while(true);
  }

  // you can also change the settings at runtime
  // different modules accept different parameters
  // you can check if the setting was changed successfully

  // set bandwidth to 250 kHz
  if(loraBalanced.setBandwidth(250000) == ERR_INVALID_BANDWIDTH) {
    Serial.println("Selected bandwidth is invalid for this module!");
  }

  // set spreading factor to 10
  if(loraBalanced.setSpreadingFactor(10) == ERR_INVALID_SPREADING_FACTOR) {
    Serial.println("Selected spreading factor is invalid for this module!");
  }

  // set coding rate to 6
  if(loraBalanced.setCodingRate(6) == ERR_INVALID_CODING_RATE) {
    Serial.println("Selected coding rate is invalid for this module!");
  }

  // set carrier frequency to 433.5 MHz
  if(loraBalanced.setFrequency(433.5) == ERR_INVALID_FREQUENCY) {
    Serial.println("Selected frequency is invalid for this module!");
  }

  // set LoRa sync word to 0x14
  // NOTE: value 0x34 is reserved for LoRaWAN networks and should not be used
  if(loraBalanced.setSyncWord(0x14) != ERR_NONE) {
    Serial.println("Unable to set sync word!");
  }

  // set output power to 10 dBm (accepted range is 2 - 17 dBm)
  if(loraBalanced.setOutputPower(10) != ERR_INVALID_OUTPUT_POWER) {
    Serial.println("Selected output power is invalid for this module!");
  }
}

void loop() {
  // nothing here
}
