// include the library
#include <LoRaLib.h>

// create instance of LoRa class with default settings
// chip:                SX1278
// NSS pin:             7
// carrier frequency:   434.0 MHz
// bandwidth:           125000 Hz
// spreading factor:    9
// coding rate:         7
// DIO0 pin:            2
// DIO1 pin:            3
// Sync word:           0x12
LoRa lora;

// create instance of Packet class with destination "01:23:45:67:89:AB:CD:EF" and data "Hello World !"
Packet pack("01:23:45:67:89:AB:CD:EF", "Hello World!");

void setup() {
  Serial.begin(9600);

  // initialize the LoRa module with default settings
  if (lora.begin() == ERR_NONE) {
    Serial.println("Initialization done.");
  }

  // print the default data of the packet
  Serial.print("Data:\t\t");
  Serial.println(pack.data);
}

void loop() {
  // change the packet data to some other string
  pack.setPacketData("Goodbye Moon!");
  Serial.print("Data:\t\t");
  Serial.println(pack.data);

  // packet data can be also changed to int
  pack.setPacketData(42);
  Serial.print("Data:\t\t");
  Serial.println(pack.data);

  // or float
  pack.setPacketData(PI);
  Serial.print("Data:\t\t");
  Serial.println(pack.data);

   // float precision can be set by parameter (3 digits by default)
  pack.setPacketData(PI, 5);
  Serial.print("Data:\t\t");
  Serial.println(pack.data);

  Serial.print("Sending packet ... ");

  // start transmitting the packet
  uint8_t state = lora.transmit(pack);

  if (state == ERR_NONE) {
    // the packet was successfully transmitted
    Serial.println(" success!");

  } else if (state == ERR_PACKET_TOO_LONG) {
    // the supplied packet was longer than 256 bytes
    Serial.println(" too long!");

  } else if (state == ERR_TX_TIMEOUT) {
    // timeout occured while transmitting packet
    Serial.println(" timeout!");

  }

  // wait a second before transmitting again
  delay(1000);
}
