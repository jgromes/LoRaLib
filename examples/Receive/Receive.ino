// include the library
#include <LoRaLib.h>

// create instance of LoRa class with default settings
// chip:                SX1278
// NSS pin:             7
// bandwidth:           125 kHz
// spreading factor:    9
// coding rate:         4/7
// DIO0 pin:            2
// DIO1 pin:            3
LoRa lora;

// create empty instance of Packet class
Packet pack;

void setup() {
  Serial.begin(9600);

  // initialize the LoRa module with default settings
  if(lora.begin() == ERR_NONE) {
    Serial.println("Initialization done.");
  }
}

void loop() {
  Serial.print("Waiting for incoming transmission ... ");

  // wait for single packet
  uint8_t state = lora.receive(pack);

  if(state == ERR_NONE) {
    // packet was successfully received
    Serial.println("success!");

    // create a string to store the packet information
    char str[24];

    // print the source of the packet
    pack.getSourceStr(str);
    Serial.print("Source:\t\t");
    Serial.println(str);

    // print the destination of the packet
    pack.getDestinationStr(str);
    Serial.print("Destination:\t");
    Serial.println(str);

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
    
  } else if(state == ERR_RX_TIMEOUT) {
    // timeout occurred while waiting for a packet
    Serial.println("timeout!");
    
  } else if(state == ERR_CRC_MISMATCH) {
    // packet was received, but is malformed
    Serial.println("CRC error!");
    
  }
  
}

