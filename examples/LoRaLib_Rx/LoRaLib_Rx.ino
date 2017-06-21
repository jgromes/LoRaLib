// include the library
#include <LoRaLib.h>

// create instances of LoRa and packet classes with default settings
LoRa lora;
packet pack;

void setup() {
  Serial.begin(9600);

  // initialize the LoRa module with default settings
  lora.init();
}

void loop() {
  Serial.print("Waiting for incoming transmission ... ");
  
  // start receiving single packet
  uint8_t state = lora.rx(pack);

  if(state == 0) {
    // if the function returned 0, a packet was suceesfully received
    Serial.println("success!");

    // create a string to store the packet information
    char str[24];

    // print the source of the packet
    pack.getSourceStr(str);
    Serial.println(str);

    // print the destination of the packet
    pack.getDestinationStr(str);
    Serial.println(str);

    // print the length of the packet
    Serial.println(pack.length);

    // print the data of the packet
    Serial.println(pack.data);
    
  } else if(state == 1) {
    // if the function returned 1, a timeout occured while waiting for a packet
    Serial.println("timeout!");
    
  } else if(state == 2) {
    // if the function returned 2, a packet was received, but is malformed
    Serial.println("CRC error!");
    
  }
  
}

