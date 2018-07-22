#include "SX1276.h"

SX1276::SX1276(Module* mod) : SX1278(mod) {
  
}

uint8_t SX1276::setFrequency(float freq) {
  // check frequency range
  if((freq < 137.0) || (freq > 1020.0)) {
    return(ERR_INVALID_FREQUENCY);
  }
  
  // sensitivity optimization for 500kHz bandwidth
  // see SX1276/77/78 Errata, section 2.1 for details
  if(_bw == 500.0) {
    if((freq >= 862.0) && (freq <= 1020.0)) {
      _mod->SPIwriteRegister(0x36, 0x02);
      _mod->SPIwriteRegister(0x3a, 0x64);
    } else if((freq >= 410.0) && (freq <= 525.0)) {
      _mod->SPIwriteRegister(0x36, 0x03);
      _mod->SPIwriteRegister(0x3a, 0x65);
    }
  }
  
  // set frequency
  return(SX127x::setFrequencyRaw(freq));
}
