/*
   LoRaLib FSK Modem Example

   This example shows how to use FSK modem in SX127x chips.
   NOTE: The code below is just a guide, do not attempt
         to run it!

   For more detailed information, see the LoRaLib Wiki
   https://github.com/jgromes/LoRaLib/wiki

   For more information on FSK modem, see
   https://github.com/jgromes/LoRaLib/wiki/FSK-Modem
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
  // frequency deviation:         50.0 kHz
  // Rx bandwidth:                125.0 kHz
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
  state = fsk.setFrequencyDeviation(10.0);
  state = fsk.setRxBandwidth(250.0);
  state = fsk.setOutputPower(10.0);
  state = fsk.setCurrentLimit(100);
  uint8_t syncWord[] = {0x01, 0x23, 0x45, 0x67, 
                        0x89, 0xAB, 0xCD, 0xEF};
  state = fsk.setSyncWord(syncWord, 8);
  if (state != ERR_NONE) {
    Serial.print(F("Unable to set configuration, code "));
    Serial.println(state);
    while (true);
  }
}

void loop() {
  // FSK modem can use the same transmit/receive methods
  // as the LoRa modem, even their interrupt-driven versions
  // NOTE: FSK modem maximum packet length is 63 bytes!
  
  // transmit FSK packet
  int state = fsk.transmit("Hello World!");
  /*
    byte byteArr[] = {0x01, 0x23, 0x45, 0x56,
                      0x78, 0xAB, 0xCD, 0xEF};
    int state = lora.transmit(byteArr, 8);
  */
  if (state == ERR_NONE) {
    Serial.println(F("Packet transmitted successfully!"));
  } else if (state == ERR_PACKET_TOO_LONG) {
    Serial.println(F("Packet too long!"));
  } else if (state == ERR_TX_TIMEOUT) {
    Serial.println(F("Timed out while transmitting!"));
  } else {
    Serial.println(F("Failed to transmit packet, code "));
  }

  // receive FSK packet
  String str;
  state = fsk.receive(str);
  /*
    byte byteArr[8];
    int state = lora.receive(byteArr, 8);
  */
  if (state == ERR_NONE) {
    Serial.println(F("Received packet!"));
    Serial.print(F("Data:\t"));
    Serial.println(str);
  } else if (state == ERR_RX_TIMEOUT) {
    Serial.println(F("Timed out while waiting for packet!"));
  } else {
    Serial.println(F("Failed to receive packet, code "));
  }

  // FSK modem has built-in address filtering system
  // it can be enabled by setting node address, broadcast
  // address, or both
  //
  // to transmit packet to a particular address, 
  // use the following methods:
  //
  // fsk.transmit("Hello World!", address);
  // fsk.startTransmit("Hello World!", address);

  // set node address to 0x02
  state = fsk.setNodeAddress(0x02);
  // set broadcast address to 0xFF
  state = fsk.setBroadcastAddress(0xFF);
  if (state != ERR_NONE) {
    Serial.println(F("Unable to set address filter, code "));
  }

  // address filtering can also be disabled
  // NOTE: calling this method will also erase previously set
  // node and broadcast address
  /*
    state = fsk.disableAddressFiltering();
    if (state != ERR_NONE) {
      Serial.println(F("Unable to remove address filter, code "));
    }
  */

  // FSK modem supports direct data transmission
  // in this mode, SX127x directly transmits any data
  // received on DIO1 (data) and DIO2 (clock)

  // activate direct mode
  state = fsk.directMode();
  if (state != ERR_NONE) {
    Serial.println(F("Unable to start direct mode, code "));
  }

  // using the direct mode, it is possible to transmit
  // FM notes with Arduino tone() function
  // transmit FM note at 1000 Hz for 1 second
  // (DIO2 is connected to Arduino pin 4)
  tone(4, 1000);
  delay(1000);
  // transmit FM note at 500 Hz for 1 second
  tone(4, 500);
  delay(1000);
  
  // NOTE: you will not be able to send or receive packets
  // while direct mode is active! to deactivate it, call method
  // fsk.packetMode()

  // "directMode()" method also has an override that allows
  // you to set raw frequency as 24-bit number
  // this allows you to send RTTY data

  // set frequency deviation to 0 (required for RTTY)
  fsk.setFrequencyDeviation(0);
  // start baud rate timer
  unsigned long start = micros();
  // send space (low; 0x6C9999 * 61 Hz = 434.149 749 MHz)
  fsk.directMode(0x6C9999);
  // wait for baud rate 45
  while(micros() - start < 22222);
  // restart baud rate timer
  start = micros();
  // send mark (high; 0x6C999C * 61 Hz = 434.149 932 MHz; 183 Hz shift)
  fsk.directMode(0x6C9999);
  // wait for baud rate 45
  while(micros() - start < 22222);
}
