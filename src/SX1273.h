#ifndef _LORALIB_SX1273_H
#define _LORALIB_SX1273_H

#include "TypeDef.h"
#include "SX1272.h"

class SX1273: public SX1272 {
  public:
    SX1273(int nss, float freq, Bandwidth bw, SpreadingFactor sf, CodingRate cr, int dio0, int dio1, uint8_t syncWord);
    
    uint8_t config(Bandwidth bw, SpreadingFactor sf, CodingRate cr, float freq, uint8_t syncWord);
};

#endif
