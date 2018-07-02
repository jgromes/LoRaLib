#include "SX1278.h"

SX1278::SX1278(int nss, float freq, uint32_t bw, uint8_t sf, uint8_t cr, int dio0, int dio1, uint8_t syncWord) : SX127x(CH_SX1272, dio0, dio1) {
  _nss = nss;
  _dio0 = dio0;
  _dio1 = dio1;
  
  _bw = bw;
  _sf = sf;
  _cr = cr;
  _freq = freq;
  _syncWord = syncWord;
}

uint8_t SX1278::tx(char* data, uint8_t length) {
  // calculate timeout
  uint16_t base = 1;
  float symbolLength = (float)(base << _sf) / (float)_bw;
  float de = 0;
  if(symbolLength >= 0.016) {
    de = 1;
  }
  float ih = (float)getRegValue(SX127X_REG_MODEM_CONFIG_1, 0, 0);
  float crc = (float)(getRegValue(SX127X_REG_MODEM_CONFIG_2, 2, 2) >> 2);
  
  float n_pre = (float)getRegValue(SX127X_REG_PREAMBLE_LSB);
  float n_pay = 8.0 + max(ceil((8.0 * (float)length - 4.0 * (float)_sf + 28.0 + 16.0 * crc - 20.0 * ih)/(4.0 * (float)_sf - 8.0 * de)) * (float)_cr, 0.0);
  uint32_t timeout = ceil(symbolLength * (n_pre + n_pay + 4.25) * 1000.0);
  
  // execute common part
  return(SX127x::tx(data, length, timeout));
}

uint8_t SX1278::begin() {
  // initialize low-level drivers
  initModule(_nss, _dio0, _dio1);
  
  // execute common part
  uint8_t status = SX127x::begin();
  if(status != ERR_NONE) {
    return(status);
  }
  
  // start configuration
  return(config(_bw, _sf, _cr, _freq, _syncWord));
}

uint8_t SX1278::rxSingle(char* data, uint8_t* length) {
  // get header mode
  bool headerExplMode = false;
  if(getRegValue(SX127X_REG_MODEM_CONFIG_1, 0, 0) == SX1278_HEADER_EXPL_MODE) {
    headerExplMode = true;
  }
  
  // execute common part
  return SX127x::rxSingle(data, length, headerExplMode);
}

uint8_t SX1278::setBandwidth(uint32_t bw) {
  uint8_t state = config(bw, _sf, _cr, _freq, _syncWord);
  if(state == ERR_NONE) {
    _bw = bw;
  }
  return(state);
}

uint8_t SX1278::setSpreadingFactor(uint8_t sf) {
  uint8_t state = config(_bw, sf, _cr, _freq, _syncWord);
  if(state == ERR_NONE) {
    _sf = sf;
  }
  return(state);
}

uint8_t SX1278::setCodingRate(uint8_t cr) {
  uint8_t state = config(_bw, _sf, cr, _freq, _syncWord);
  if(state == ERR_NONE) {
    _cr = cr;
  }
  return(state);
}

uint8_t SX1278::setFrequency(float freq) {
  uint8_t state = config(_bw, _sf, _cr, freq, _syncWord);
  if(state == ERR_NONE) {
    _freq = freq;
  }
  return(state);
}

uint8_t SX1278::setSyncWord(uint8_t syncWord) {
  uint8_t state = config(_bw, _sf, _cr, _freq, syncWord);
  if(state == ERR_NONE) {
    _syncWord = syncWord;
  }
  return(state);
}

uint8_t SX1278::config(uint32_t bw, uint8_t sf, uint8_t cr, float freq, uint8_t syncWord) {
  uint8_t status = ERR_NONE;
  uint8_t newBandwidth, newSpreadingFactor, newCodingRate;
  
  // check the supplied BW, CR and SF values
  switch(bw) {
    case 7800:
      newBandwidth = SX1278_BW_7_80_KHZ;
      break;
    case 10400:
      newBandwidth = SX1278_BW_10_40_KHZ;
      break;
    case 15600:
      newBandwidth = SX1278_BW_15_60_KHZ;
      break;
    case 20800:
      newBandwidth = SX1278_BW_20_80_KHZ;
      break;
    case 31250:
      newBandwidth = SX1278_BW_31_25_KHZ;
      break;
    case 41700:
      newBandwidth = SX1278_BW_41_70_KHZ;
      break;
    case 62500:
      newBandwidth = SX1278_BW_62_50_KHZ;
      break;
    case 125000:
      newBandwidth = SX1278_BW_125_00_KHZ;
      break;
    case 250000:
      newBandwidth = SX1278_BW_250_00_KHZ;
      break;
    case 500000:
      newBandwidth = SX1278_BW_500_00_KHZ;
      break;
    default:
      return(ERR_INVALID_BANDWIDTH);
  }
  
  switch(sf) {
    case 6:
      newSpreadingFactor = SX127X_SF_6;
      break;
    case 7:
      newSpreadingFactor = SX127X_SF_7;
      break;
    case 8:
      newSpreadingFactor = SX127X_SF_8;
      break;
    case 9:
      newSpreadingFactor = SX127X_SF_9;
      break;
    case 10:
      newSpreadingFactor = SX127X_SF_10;
      break;
    case 11:
      newSpreadingFactor = SX127X_SF_11;
      break;
    case 12:
      newSpreadingFactor = SX127X_SF_12;
      break;
    default:
      return(ERR_INVALID_SPREADING_FACTOR);
  }
  
  switch(cr) {
    case 5:
      newCodingRate = SX1278_CR_4_5;
      break;
    case 6:
      newCodingRate = SX1278_CR_4_6;
      break;
    case 7:
      newCodingRate = SX1278_CR_4_7;
      break;
    case 8:
      newCodingRate = SX1278_CR_4_8;
      break;
    default:
      return(ERR_INVALID_CODING_RATE);
  }
  
  if((freq < 137.0) || (freq > 525.0)) {
    return(ERR_INVALID_FREQUENCY);
  }
  
  // execute common part
  status = configCommon(newBandwidth, newSpreadingFactor, newCodingRate, freq, syncWord);
  if(status != ERR_NONE) {
    return(status);
  }
  
  // configuration successful, save the new settings
  _bw = bw;
  _sf = sf;
  _cr = cr;
  _freq = freq;
  
  return(ERR_NONE);
}

uint8_t SX1278::configCommon(uint8_t bw, uint8_t sf, uint8_t cr, float freq, uint8_t syncWord) {
  // configure common registers
  uint8_t status = SX127x::config(bw, sf, cr, freq, syncWord);
  if(status != ERR_NONE) {
    return(status);
  }
  
  // output power configuration
  status = setRegValue(SX127X_REG_PA_CONFIG, SX1278_MAX_POWER, 6, 4);
  status = setRegValue(SX1278_REG_PA_DAC, SX127X_PA_BOOST_ON, 2, 0);
  if(status != ERR_NONE) {
    return(status);
  }
  
  // enable LNA gain setting by register 
  status = setRegValue(SX1278_REG_MODEM_CONFIG_3, SX1278_AGC_AUTO_OFF, 2, 2);
  if(status != ERR_NONE) {
    return(status);
  }
  
  // calculate symbol length and set low datarate optimization, if needed
  uint16_t base = 1;
  float symbolLength = (float)(base << _sf) / (float)_bw;
  if(symbolLength >= 0.016) {
    status = setRegValue(SX1278_REG_MODEM_CONFIG_3, SX1278_LOW_DATA_RATE_OPT_ON,  0, 0);
  } else {
    status = setRegValue(SX1278_REG_MODEM_CONFIG_3, SX1278_LOW_DATA_RATE_OPT_OFF,  0, 0);
  }
  if(status != ERR_NONE) {
    return(status);
  }
  
  // set SF6 optimizations
  if(sf == SX127X_SF_6) {
    status = setRegValue(SX127X_REG_MODEM_CONFIG_2, SX1278_RX_CRC_MODE_OFF, 2, 2);
    status = setRegValue(SX127X_REG_MODEM_CONFIG_1, bw | cr | SX1278_HEADER_IMPL_MODE);
  } else {
    status = setRegValue(SX127X_REG_MODEM_CONFIG_2, SX1278_RX_CRC_MODE_ON, 2, 2);
    status = setRegValue(SX127X_REG_MODEM_CONFIG_1, bw | cr | SX1278_HEADER_EXPL_MODE);
  }
  
  return(status);
}
