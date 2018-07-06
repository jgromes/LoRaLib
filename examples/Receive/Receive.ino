/*
 * LoRaLib Receive Example
 * 
 * This example listens for LoRa transmissions and tries to receive them.
 * To successfully receive packets, the following settings have to be the same
 * on both transmitter and receiver:
 *  - carrier frequency
 *  - bandwidth
 *  - spreading factor
 *  - coding rate
 *  - sync word
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

// create empty instance of Packet class
Packet pack;

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
}

void loop() {
  Serial.print("Waiting for incoming transmission ... ");

  // wait for single packet
  uint8_t state = lora.receive(pack);

  if(state == ERR_NONE) {
    // packet was successfully received
    Serial.println("success!");

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

    //print the measured data rate
    Serial.print("Datarate:\t");
    Serial.print(lora.dataRate);
    Serial.println(" bps");

    //print the RSSI (Received Signal Strength Indicator) of the last received packet
    Serial.print("RSSI:\t\t");
    Serial.print(lora.lastPacketRSSI);
    Serial.println(" dBm");

    //print the SNR (Signal-to-Noise Ratio) of the last received packet
    Serial.print("SNR:\t\t");
    Serial.print(lora.lastPacketSNR);
    Serial.println(" dBm");
    
  } else if(state == ERR_RX_TIMEOUT) {
    // timeout occurred while waiting for a packet
    Serial.println("timeout!");
    
  } else if(state == ERR_CRC_MISMATCH) {
    // packet was received, but is malformed
    Serial.println("CRC error!");
    
  }
  
}

