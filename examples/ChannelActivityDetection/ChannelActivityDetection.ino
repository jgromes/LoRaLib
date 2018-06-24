// include the library
#include <LoRaLib.h>

// create instance of LoRa class with default settings
// chip:                SX1278
// NSS pin:             7
// carrier frequency:   434.0 MHz
// bandwidth:           125 kHz
// spreading factor:    9
// coding rate:         4/7
// DIO0 pin:            2
// DIO1 pin:            3
// Sync word:           0x12
LoRa lora;

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
