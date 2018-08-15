/*
   LoRaLib FSK Receive Example

   This example listens for FSK transmissions and tries to
   receive them.

   For more detailed information, see the LoRaLib Wiki
   https://github.com/jgromes/LoRaLib/wiki
*/

// include the library
#include <LoRaLib.h>

// create instance of LoRa class using SX1278 module
// this pinout corresponds to LoRenz shield: 
// https://github.com/jgromes/LoRenz
// NSS pin:   7
// DIO0 pin:  2
// DIO1 pin:  3
SX1278 lora = new LoRa;

void setup() {
  Serial.begin(9600);

  // initialize SX1278 with default settings
  Serial.print(F("Initializing ... "));
  // carrier frequency:           434.0 MHz
  // bit rate:                    48.0 kbps
  // Rx bandwidth:                125.0 kHz
  // frequency deviation:         50.0 kHz
  // output power:                13 dBm
  // current limit:               100 mA
  // sync word:                   0x2D  0x01
  int state = lora.beginFSK();
  if (state == ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }
}

void loop() {
  Serial.print("Waiting for incoming transmission ... ");

  // you can receive data as an Arduino String
  String str;
  int state = lora.receive(str);

  // you can also receive data as byte array
  /*
    byte byteArr[8];
    int state = lora.receive(byteArr, 8);
  */
  
  if (state == ERR_NONE) {
    // packet was successfully received
    Serial.println("success!");

    // print data of the packet
    Serial.print("Data:\t\t\t");
    Serial.println(str);

  } else if (state == ERR_RX_TIMEOUT) {
    // timeout occurred while waiting for a packet
    Serial.println("timeout!");

  }

}

