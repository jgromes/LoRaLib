#ifndef _LORALIB_SX1277_H
#define _LORALIB_SX1277_H

#include "TypeDef.h"
#include "SX1278.h"

class SX1277: public SX1278 {
  public:
    SX1277(int nss, float freq, Bandwidth bw, SpreadingFactor sf, CodingRate cr, int dio0, int dio1);
    
    uint8_t config(Bandwidth bw, SpreadingFactor sf, CodingRate cr, float freq);
};

#endif
