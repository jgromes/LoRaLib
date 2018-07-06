/*
 * LoRaLib Channel Activity Detection Example
 * 
 * This example scans the current LoRa channel and detects valid LoRa preambles.
 * Preamble is the first part of LoRa transmission, so this can be used to
 * check if the LoRa channel is free, or if you hsould start receiving a message.
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

  // initialize the LoRa module with default settings
  uint8_t state = lora.begin();
  if(state != ERR_NONE) {
    Serial.print("Initialization failed, code 0x");
    Serial.println(state, HEX);
    while(true);
  }
}

void loop() {
  Serial.print("Scanning channel for LoRa preamble ... ");

  // start scanning current channel
  uint8_t state = lora.scanChannel();
  
  if(state == PREAMBLE_DETECTED) {
    // LoRa preamble was detected
    Serial.println(" detected preamble!");
    
  } else if(state == CHANNEL_FREE) {
    // no preamble was detected, channel is free
    Serial.println(" channel is free!");
    
  }

  // wait 100 ms before new scan
  delay(100);
}
