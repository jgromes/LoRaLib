/*
   LoRaLib FSK Transmit Example

   This example transmits FSK packets with one second delays
   between them. Each packet contains up to 256 bytes
   of data, in the form of:
    - Arduino String
    - null-terminated char array (C-string)
    - arbitrary binary data (byte array)

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

  // initialize SX1278 FSK modem with default settings
  Serial.print(F("Initializing ... "));
  // carrier frequency:           434.0 MHz
  // bit rate:                    48.0 kbps
  // Rx bandwidth:                125.0 kHz
  // frequency deviation:         50.0 kHz
  // output power:                13 dBm
  // sync word:                   0x2D  0x01
  int state = lora.beginFSK();
  if (state == ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }

  // if needed, you can switch between LoRa and FSK modes
  //
  // lora.begin()       start LoRa mode (and disable FSK)
  // lora.beginFSK()    start FSK mode (and disable LoRa)
}

void loop() {
  Serial.print("Sending packet ... ");

  // you can transmit C-string or Arduino string up to
  // 256 characters long
  int state = lora.transmit("Hello World!");

  // you can also transmit byte array up to 256 bytes long
  /*
    byte byteArr[] = {0x01, 0x23, 0x45, 0x56,
                      0x78, 0xAB, 0xCD, 0xEF};
    int state = lora.transmit(byteArr, 8);
  */

  if (state == ERR_NONE) {
    // the packet was successfully transmitted
    Serial.println("success!");

  } else if (state == ERR_PACKET_TOO_LONG) {
    // the supplied packet was longer than 256 bytes
    Serial.println("too long!");

  } else if (state == ERR_TX_TIMEOUT) {
    // timeout occurred while transmitting packet
    Serial.println("timeout!");

  }

  // wait a second before transmitting again
  delay(1000);
}
