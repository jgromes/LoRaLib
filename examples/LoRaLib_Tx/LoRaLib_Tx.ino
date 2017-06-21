// include the library
#include <LoRaLib.h>

// create instance of LoRa class with default settings
LoRa lora;

// create instance of packet class with destination "01:23:45:67:89:AB:CD:EF" and data "Hello World !"
packet pack("01:23:45:67:89:AB:CD:EF", "Hello World!");

void setup() {
  Serial.begin(9600);

  // initialize the LoRa module with default settings
  lora.init();

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

  //lora.printStatus();
}

void loop() {
  Serial.print("Sending packet ");

  // start transmitting the packet
  uint8_t state = lora.tx(pack);
  
  if(state == 0) {
    // if the function returned 0, a packet was suceesfully transmitted
    Serial.println(" success!");
    
  } else if(state == 1) {
    // if the function returned 1, the supplied packet was longer than 256 bytes
    Serial.println(" too long!");
    
  }

  // wait a second before transmitting again
  delay(1000);
}
