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

// create instance of Packet class with destination "01:23:45:67:89:AB:CD:EF" and data "Hello World !"
Packet pack("01:23:45:67:89:AB:CD:EF", "Hello World!");

void setup() {
  Serial.begin(9600);

  // initialize the LoRa module with default settings
  // carrier frequency:                   434.0 MHz
  // bandwidth:                           125.0 kHz
  // spreading factor:                    9
  // coding rate:                         7
  // sync word:                           0x12
  // output power:                        17 dBm
  // node address in EEPROM starts at:    0
  uint8_t state = lora.begin();
  if(state != ERR_NONE) {
    Serial.print("Initialization failed, code 0x");
    Serial.println(state, HEX);
    while(true);
  }

  // print the source of the packet
  Serial.print("Source:\t\t");
  Serial.println(pack.getSourceStr());

  // print the destination of the packet
  Serial.print("Destination:\t");
  Serial.println(pack.getDestinationStr());

  // print the length of the packet
  Serial.print("Length:\t\t");
  Serial.println(pack.length);
  
  // print the data of the packet
  Serial.print("Data:\t\t");
  Serial.println(pack.data);
}

void loop() {
  Serial.print("Sending packet ... ");

  // start transmitting the packet
  uint8_t state = lora.transmit(pack);
  
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
