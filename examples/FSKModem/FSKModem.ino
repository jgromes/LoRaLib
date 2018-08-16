/*
   LoRaLib FSK Modem Example

   This example shows how to use FSK modem in SX127x chips.

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
SX1278 fsk = new LoRa;

void setup() {
  Serial.begin(9600);

  // initialize SX1278 FSK modem with default settings
  Serial.print(F("Initializing ... "));
  // carrier frequency:           434.0 MHz
  // bit rate:                    48.0 kbps
  // Rx bandwidth:                125.0 kHz
  // frequency deviation:         50.0 kHz
  // output power:                13 dBm
  // current limit:               100 mA
  // sync word:                   0x2D  0x01
  int state = fsk.beginFSK();
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

  // FSK modem supports the following settings:
  state = fsk.setFrequency(433.5);
  state = fsk.setBitRate(100.0);
  state = fsk.setRxBandwidth(250.0);
  state = fsk.setFrequencyDeviation(10.0);
  state = fsk.setOutputPower(10.0);
  state = fsk.setCurrentLimit(100);
  uint8_t syncWord[] = {0x01, 0x23, 0x45, 0x67, 
                        0x89, 0xAB, 0xCD, 0xEF};
  state = fsk.setSyncWord(syncWord, 8);
  if (state != ERR_NONE) {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }
}

void loop() {
  // FSK modem can use the same transmit/receive methods
  // as the LoRa modem, even their interrupt-driven versions
  
  // transmit FSK packet
  int state = fsk.transmit("Hello World!");
  /*
    byte byteArr[] = {0x01, 0x23, 0x45, 0x56,
                      0x78, 0xAB, 0xCD, 0xEF};
    int state = lora.transmit(byteArr, 8);
  */
  if (state == ERR_NONE) {
    Serial.println("success!");
  } else if (state == ERR_PACKET_TOO_LONG) {
    Serial.println("too long!");
  } else if (state == ERR_TX_TIMEOUT) {
    Serial.println("timeout!");
  }

  // receive FSK packet
  String str;
  state = fsk.receive(str);
  /*
    byte byteArr[8];
    int state = lora.receive(byteArr, 8);
  */
  if (state == ERR_NONE) {
    Serial.println("success!");
    Serial.print("Data:\t");
    Serial.println(str);
  } else if (state == ERR_RX_TIMEOUT) {
    Serial.println("timeout!");
  }

  // FSK modem has built-in address filtering system
  // it can be enabled by setting node address, broadcast
  // address, or both

  // set node address to 0x02
  state = fsk.setNodeAddress(0x02);
  // set broadcast address to 0xFF
  state = fsk.setBroadcastAddress(0xFF);
  if (state == ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }

  // address filtering can also be disabled
  // NOTE: calling this method will also erase previously set
  // node and broadcast address
  /*
    state = fsk.disableAddressFiltering();
    if(state == ERR_NONE) {
      Serial.println(F("success!"));
    } else {
      Serial.print(F("failed, code "));
      Serial.println(state);
      while(true);
    }
  */
}
