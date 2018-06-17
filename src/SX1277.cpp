#include "SX1277.h"

SX1277::SX1277(int nss, float freq, Bandwidth bw, SpreadingFactor sf, CodingRate cr, int dio0, int dio1) : SX1278(nss, freq, bw, sf, cr, dio0, dio1) {
  
}

uint8_t SX1277::config(Bandwidth bw, SpreadingFactor sf, CodingRate cr, float freq) {
  uint8_t status = ERR_NONE;
  uint8_t newBandwidth, newSpreadingFactor, newCodingRate;
  
  // check the supplied BW, CR and SF values
  switch(bw) {
    case BW_125_00_KHZ:
      newBandwidth = SX1278_BW_125_00_KHZ;
      break;
    case BW_250_00_KHZ:
      newBandwidth = SX1278_BW_250_00_KHZ;
      break;
    case BW_500_00_KHZ:
      newBandwidth = SX1278_BW_500_00_KHZ;
      break;
    default:
      return(ERR_INVALID_BANDWIDTH);
  }
  
  switch(sf) {
    case SF_6:
      newSpreadingFactor = SX127X_SF_6;
      break;
    case SF_7:
      newSpreadingFactor = SX127X_SF_7;
      break;
    case SF_8:
      newSpreadingFactor = SX127X_SF_8;
      break;
    case SF_9:
      newSpreadingFactor = SX127X_SF_9;
      break;
    default:
      return(ERR_INVALID_SPREADING_FACTOR);
  }
  
  switch(cr) {
    case CR_4_5:
      newCodingRate = SX1278_CR_4_5;
      break;
    case CR_4_6:
      newCodingRate = SX1278_CR_4_6;
      break;
    case CR_4_7:
      newCodingRate = SX1278_CR_4_7;
      break;
    case CR_4_8:
      newCodingRate = SX1278_CR_4_8;
      break;
    default:
      return(ERR_INVALID_CODING_RATE);
  }
  
  if((freq < 137.0) || (freq > 1020.0)) {
    return(ERR_INVALID_FREQUENCY);
  }
  
  // execute common part
  status = configCommon(newBandwidth, newSpreadingFactor, newCodingRate, freq);
  if(status != ERR_NONE) {
    return(status);
  }
  
  // configuration successful, save the new settings
  _bw = bw;
  _sf = sf;
  _cr = cr;
  
  return(ERR_NONE);
}
