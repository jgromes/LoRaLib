#ifndef _LORALIB_H
#define _LORALIB_H

#include <EEPROM.h>

#include "TypeDef.h"
#include "Packet.h"

#include "Module.h"

#include "SX1272.h"
#include "SX1273.h"

#include "SX1278.h"
#include "SX1276.h"
#include "SX1277.h"
#include "SX1279.h"

class LoRa {
  public:
    LoRa(Chip ch = CH_SX1278, int nss = 7, float freq = 434.0, uint32_t bw = 125000, uint8_t sf = 9, uint8_t cr = 7, int dio0 = 2, int dio1 = 3, uint8_t syncWord = SX127X_SYNC_WORD);
    
    float dataRate;
    int8_t lastPacketRSSI;
    float lastPacketSNR;
    
    uint8_t begin(uint16_t addrEeprom = 0);
    
    uint8_t transmit(Packet& pack);
    uint8_t receive(Packet& pack);
    uint8_t scanChannel();
    
    uint8_t sleep();
    uint8_t standby();
    
    uint8_t setBandwidth(uint32_t bw);
    uint8_t setSpreadingFactor(uint8_t sf);
    uint8_t setCodingRate(uint8_t cr);
    uint8_t setFrequency(float freq);
    uint8_t setSyncWord(uint8_t syncword);
    uint8_t setOutputPower(int8_t power);
  
  private:
    Module* _mod;
    
    uint8_t _address[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    uint16_t _addrEeprom;
    
    void generateLoRaAdress();
};

#endif
