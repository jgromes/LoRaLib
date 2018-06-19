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
    LoRa(Chip ch = CH_SX1278, int nss = 7, float freq = 434.0, Bandwidth bw = BW_125_00_KHZ, SpreadingFactor sf = SF_9, CodingRate cr = CR_4_7, int dio0 = 2, int dio1 = 3);
    
    float dataRate;
    int8_t lastPacketRSSI;
    float lastPacketSNR;
    
    uint8_t begin(uint16_t addrEeprom = 0);
    
    uint8_t transmit(Packet& pack);
    uint8_t receive(Packet& pack);
    //TODO: CAD mode
    
    uint8_t sleep(void);
    uint8_t standby(void);
    
    uint8_t setBandwidth(Bandwidth bw);
    uint8_t setSpreadingFactor(SpreadingFactor sf);
    uint8_t setCodingRate(CodingRate cr);
    uint8_t setFrequency(float freq);
  
  private:
    Module* _mod;
    
    uint8_t _address[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    uint16_t _addrEeprom;
    
    void generateLoRaAdress(void);
};

#endif
