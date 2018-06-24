#ifndef _LORALIB_SX1279_H
#define _LORALIB_SX1279_H

#include "TypeDef.h"
#include "SX1278.h"

class SX1279: public SX1278 {
  public:
    SX1279(int nss, float freq, Bandwidth bw, SpreadingFactor sf, CodingRate cr, int dio0, int dio1, uint8_t syncWord);
    
    uint8_t config(Bandwidth bw, SpreadingFactor sf, CodingRate cr, float freq, uint8_t syncWord);
};

#endif
