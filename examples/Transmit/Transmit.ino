/*
 * LoRaLib Transmit Example
 * 
 * This example transmits LoRa packets with one second delays between them.
 * Each packet contains the following data:
 *  - 8-byte destination address
 *  - 8-byte source address
 *  - up to 240 bytes of payload
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
SX1278 lora = new LoRa;

void setup() {
  Serial.begin(9600);

  // initialize SX1278 with default settings
  Serial.print(F("Initializing ... "));
  // carrier frequency:                   434.0 MHz
  // bandwidth:                           125.0 kHz
  // spreading factor:                    9
  // coding rate:                         7
  // sync word:                           0x12
  // output power:                        17 dBm
  // node address in EEPROM starts at:    0
  byte state = lora.begin();
  if(state == ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code 0x"));
    Serial.println(state, HEX);
    while(true);
  }
}

void loop() {
  Serial.print("Sending packet ... ");

  // start transmitting the packet
  uint8_t state = lora.transmit("Hello World!");
  
  if(state == ERR_NONE) {
    // the packet was successfully transmitted
    Serial.println(" success!");
    
  } else if(state == ERR_PACKET_TOO_LONG) {
    // the supplied packet was longer than 256 bytes
    Serial.println(" too long!");
    
  } else if(state == ERR_TX_TIMEOUT) {
    // timeout occurred while transmitting packet
    Serial.println(" timeout!");
    
  }

  // wait a second before transmitting again
  delay(1000);
}
