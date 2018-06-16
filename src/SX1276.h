#ifndef _LORALIB_SX1276_H
#define _LORALIB_SX1276_H

#include "TypeDef.h"
#include "SX1278.h"

class SX1276: public SX1278 {
  public:
    SX1276(int nss, Bandwidth bw, SpreadingFactor sf, CodingRate cr, int dio0, int dio1);
    
    uint8_t config(Bandwidth bw, SpreadingFactor sf, CodingRate cr, float freq);
};

#endif
