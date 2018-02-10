// include the library
#include <LoRaLib.h>

// create instance of LoRa class with default settings
// LoRa link with this setting is a balance between range and data rate
// chip:                SX1278
// NSS pin:             7
// bandwidth:           125 kHz
// spreading factor:    9
// coding rate:         4/7
// DIO0 pin:            2
// DIO1 pin:            3
LoRa loraBalanced;

// create another LoRa instance with non-default settings
// this LoRa link will have maximum range, but very low data rate
// chip:                SX1276
// NSS pin:             6
// bandwidth:           7.8 kHz
// spreading factor:    12
// coding rate:         4/8
// DIO0 pin:            4
// DIO1 pin:            5
LoRa loraMaxRange(CH_SX1276, 6, BW_7_80_KHZ, SF_12, CR_4_8, 4, 5);

// create third LoRa instance with non-default settings
// this LoRa link will have high data rate, but lower range
// chip:                SX1272
// NSS pin:             4
// bandwidth:           500 kHz
// spreading factor:    6
// coding rate:         4/5
// DIO0 pin:            10
// DIO1 pin:            11
LoRa loraMaxDataRate(CH_SX1272, 4, BW_500_00_KHZ, SF_6, CR_4_5, 10, 11);

void setup() {
  Serial.begin(9600);

  // initialize the LoRa instance with default settings
  if(loraBalanced.begin() == ERR_NONE) {
    Serial.println("Initialization done.");
  }

  // you can also change the settings at runtime
  loraBalanced.setBandwidth(BW_250_00_KHZ);
  loraBalanced.setSpreadingFactor(SF_8);
  loraBalanced.setCodingRate(CR_4_6);
}

void loop() {
  // nothing here
}
