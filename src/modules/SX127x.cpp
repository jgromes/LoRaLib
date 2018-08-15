#include "SX127x.h"

SX127x::SX127x(Module* mod) {
  _mod = mod;
}

int16_t SX127x::begin(uint8_t chipVersion, uint8_t syncWord, uint8_t currentLimit, uint16_t preambleLength) {
  // set module properties
  _mod->init(USE_SPI, INT_BOTH);
  
  // try to find the SX127x chip
  if(!SX127x::findChip(chipVersion)) {
    DEBUG_PRINTLN_STR("No SX127x found!");
    SPI.end();
    return(ERR_CHIP_NOT_FOUND);
  } else {
    DEBUG_PRINTLN_STR("Found SX127x!");
  }
  
  // set LoRa mode
  int16_t state = setActiveModem(SX127X_LORA);
  if(state != ERR_NONE) {
    return(state);
  }
  
  // set LoRa sync word
  state = SX127x::setSyncWord(syncWord);
  if(state != ERR_NONE) {
    return(state);
  }
  
  // set over current protection
  state = SX127x::setCurrentLimit(currentLimit);
  if(state != ERR_NONE) {
    return(state);
  }
  
  // set preamble length
  state = SX127x::setPreambleLength(preambleLength);
  if(state != ERR_NONE) {
    return(state);
  }
  
  return(state);
}

int16_t SX127x::beginFSK(uint8_t chipVersion, float br, float rxBw, float freqDev, uint8_t currentLimit) {
  // set module properties
  _mod->init(USE_SPI, INT_BOTH);
  
  // try to find the SX127x chip
  if(!SX127x::findChip(chipVersion)) {
    DEBUG_PRINTLN_STR("No SX127x found!");
    SPI.end();
    return(ERR_CHIP_NOT_FOUND);
  } else {
    DEBUG_PRINTLN_STR("Found SX127x!");
  }
  
  // set FSK mode
  int16_t state = setActiveModem(SX127X_FSK_OOK);
  if(state != ERR_NONE) {
    return(state);
  }
  
  // set bit rate
  state = SX127x::setBitRate(br);
  if(state != ERR_NONE) {
    return(state);
  }
  
  // set receiver bandwidth
  state = SX127x::setRxBandwidth(rxBw);
  if(state != ERR_NONE) {
    return(state);
  }
  
  // set frequency deviation
  state = SX127x::setFrequencyDeviation(freqDev);
  if(state != ERR_NONE) {
    return(state);
  }
  
  // set over current protection
  state = SX127x::setCurrentLimit(currentLimit);
  if(state != ERR_NONE) {
    return(state);
  }
  
  // default sync word values 0x2D01 is the same as the default in LowPowerLab RFM69 library
  uint8_t syncWord[] = {0x2D, 0x01};
  state = setSyncWordFSK(syncWord, 2);
  if(state != ERR_NONE) {
    return(state);
  }
  
  return(state);
}

int16_t SX127x::transmit(String& str) {
  return(SX127x::transmit(str.c_str()));
}

int16_t SX127x::transmit(const char* str) {
  return(SX127x::transmit((uint8_t*)str, strlen(str)));
}

int16_t SX127x::transmit(uint8_t* data, size_t len) {
  // check packet length
  if(len >= 256) {
    return(ERR_PACKET_TOO_LONG);
  }
  
  // set mode to standby
  int16_t state = setMode(SX127X_STANDBY);
  
  int16_t modem = getActiveModem();
  if(modem == SX127X_LORA) {
    // calculate timeout
    uint16_t base = 1;
    float symbolLength = (float)(base << _sf) / (float)_bw;
    float de = 0;
    if(symbolLength >= 0.016) {
      de = 1;
    }
    float ih = (float)_mod->SPIgetRegValue(SX127X_REG_MODEM_CONFIG_1, 0, 0);
    float crc = (float)(_mod->SPIgetRegValue(SX127X_REG_MODEM_CONFIG_2, 2, 2) >> 2);
    float n_pre = (float)_mod->SPIgetRegValue(SX127X_REG_PREAMBLE_LSB);
    float n_pay = 8.0 + max(ceil((8.0 * (float)len - 4.0 * (float)_sf + 28.0 + 16.0 * crc - 20.0 * ih)/(4.0 * (float)_sf - 8.0 * de)) * (float)_cr, 0.0);
    uint32_t timeout = ceil(symbolLength * (n_pre + n_pay + 4.25) * 1000.0);
    
    // set DIO mapping
    _mod->SPIsetRegValue(SX127X_REG_DIO_MAPPING_1, SX127X_DIO0_TX_DONE, 7, 6);
    
    // clear interrupt flags
    clearIRQFlags();
    
    // set packet length
    state |= _mod->SPIsetRegValue(SX127X_REG_PAYLOAD_LENGTH, len);
    
    // set FIFO pointers
    state |= _mod->SPIsetRegValue(SX127X_REG_FIFO_TX_BASE_ADDR, SX127X_FIFO_TX_BASE_ADDR_MAX);
    state |= _mod->SPIsetRegValue(SX127X_REG_FIFO_ADDR_PTR, SX127X_FIFO_TX_BASE_ADDR_MAX);
    
    // write packet to FIFO
    _mod->SPIwriteRegisterBurst(SX127X_REG_FIFO, data, len);
    
    // start transmission
    state |= setMode(SX127X_TX);
    if(state != ERR_NONE) {
      return(state);
    }
    
    // wait for packet transmission or timeout
    uint32_t start = millis();
    while(!digitalRead(_mod->int0())) {
      if(millis() - start > timeout) {
        clearIRQFlags();
        return(ERR_TX_TIMEOUT);
      }
    }
    uint32_t elapsed = millis() - start;
    
    // update data rate
    dataRate = (len*8.0)/((float)elapsed/1000.0);
    
    // clear interrupt flags
    clearIRQFlags();
    
    return(ERR_NONE);
  
  } else if(modem == SX127X_FSK_OOK) {
    // set DIO mapping
    _mod->SPIsetRegValue(SX127X_REG_DIO_MAPPING_1, SX127X_DIO0_PACK_PACKET_SENT, 7, 6);
    
    // clear interrupt flags
    clearIRQFlags();
    
    // set packet length
    _mod->SPIwriteRegister(SX127X_REG_FIFO, len);
    
    // check address filtering
    
    
    // write packet to FIFO
    _mod->SPIwriteRegisterBurst(SX127X_REG_FIFO, data, len);
    
    // start transmission
    state |= setMode(SX127X_TX);
    if(state != ERR_NONE) {
      return(state);
    }
    
    // wait for transmission end
    while(!digitalRead(_mod->int0()));
    
    // clear interrupt flags
    clearIRQFlags();
    
    return(ERR_NONE);
  }
  
  return(ERR_UNKNOWN);
}

int16_t SX127x::receive(String& str, size_t len) {
  // create temporary array to store received data
  char* data = new char[len];
  int16_t state = SX127x::receive((uint8_t*)data, len);
  
  // if packet was received successfully, copy data into String
  if(state == ERR_NONE) {
    str = String(data);
  }
  
  delete[] data;
  return(state);
}

int16_t SX127x::receive(uint8_t* data, size_t len) {
  int16_t modem = getActiveModem();
  if(modem == SX127X_LORA) {
    // set mode to standby
    int16_t state = setMode(SX127X_STANDBY);
    
    // set DIO pin mapping
    state |= _mod->SPIsetRegValue(SX127X_REG_DIO_MAPPING_1, SX127X_DIO0_RX_DONE | SX127X_DIO1_RX_TIMEOUT, 7, 4);
    
    // clear interrupt flags
    clearIRQFlags();
    
    // set FIFO pointers
    state |= _mod->SPIsetRegValue(SX127X_REG_FIFO_RX_BASE_ADDR, SX127X_FIFO_RX_BASE_ADDR_MAX);
    state |= _mod->SPIsetRegValue(SX127X_REG_FIFO_ADDR_PTR, SX127X_FIFO_RX_BASE_ADDR_MAX);
    
    // set mode to receive
    state |= setMode(SX127X_RXSINGLE);
    if(state != ERR_NONE) {
      return(state);
    }
    
    // wait for packet reception or timeout
    uint32_t start = millis();
    while(!digitalRead(_mod->int0())) {
      if(digitalRead(_mod->int1())) {
        clearIRQFlags();
        return(ERR_RX_TIMEOUT);
      }
    }
    
    // check integrity CRC
    if(_mod->SPIgetRegValue(SX127X_REG_IRQ_FLAGS, 5, 5) == SX127X_CLEAR_IRQ_FLAG_PAYLOAD_CRC_ERROR) {
      return(ERR_CRC_MISMATCH);
    }
    
    // get packet length
    size_t length = len;
    if(_sf != 6) {
      length = _mod->SPIgetRegValue(SX127X_REG_RX_NB_BYTES);
    }
    
    // read packet data
    if(len == 0) {
      // argument 'len' equal to zero indicates String call, which means dynamically allocated data array
      // dispose of the original and create a new one
      delete[] data;
      data = new uint8_t[length + 1];
    }
    _mod->SPIreadRegisterBurst(SX127X_REG_FIFO, length, data);
    
    // add terminating null
    if(len == 0) {
      data[length] = 0;
    }
    
    // update RSSI and SNR
    lastPacketRSSI = -157 + _mod->SPIgetRegValue(SX127X_REG_PKT_RSSI_VALUE);
    int8_t rawSNR = (int8_t)_mod->SPIgetRegValue(SX127X_REG_PKT_SNR_VALUE);
    lastPacketSNR = rawSNR / 4.0;
    
    // clear interrupt flags
    clearIRQFlags();
    
    return(ERR_NONE);
    
  } else if(modem == SX127X_FSK_OOK) {
  
  }
  
  return(ERR_UNKNOWN);
}

int16_t SX127x::scanChannel() {
  int16_t modem = getActiveModem();
  if(modem == SX127X_LORA) {
    // set mode to standby
    int16_t state = setMode(SX127X_STANDBY);
    
    // set DIO pin mapping
    state |= _mod->SPIsetRegValue(SX127X_REG_DIO_MAPPING_1, SX127X_DIO0_CAD_DONE | SX127X_DIO1_CAD_DETECTED, 7, 4);
    
    // clear interrupt flags
    clearIRQFlags();
    
    // set mode to CAD
    state |= setMode(SX127X_CAD);
    if(state != ERR_NONE) {
      return(state);
    }
    
    // wait for channel activity detected or timeout
    while(!digitalRead(_mod->int0())) {
      if(digitalRead(_mod->int1())) {
        clearIRQFlags();
        return(PREAMBLE_DETECTED);
      }
    }
    
    // clear interrupt flags
    clearIRQFlags();
    
    return(CHANNEL_FREE);
    
  } else if(modem == SX127X_FSK_OOK) {
    // CAD is only available in LoRa mode
    return(ERR_WRONG_MODEM);
  }
  
  return(ERR_UNKNOWN);
}

int16_t SX127x::sleep() {
  // set mode to sleep
  return(setMode(SX127X_SLEEP));
}

int16_t SX127x::standby() {
  // set mode to standby
  return(setMode(SX127X_STANDBY));
}

int16_t SX127x::startReceive() {
  // set mode to standby
  int16_t state = setMode(SX127X_STANDBY);
  
  // set DIO pin mapping
  state |= _mod->SPIsetRegValue(SX127X_REG_DIO_MAPPING_1, SX127X_DIO0_RX_DONE | SX127X_DIO1_RX_TIMEOUT, 7, 4);
  
  // clear interrupt flags
  clearIRQFlags();
  
  // set FIFO pointers
  state |= _mod->SPIsetRegValue(SX127X_REG_FIFO_RX_BASE_ADDR, SX127X_FIFO_RX_BASE_ADDR_MAX);
  state |= _mod->SPIsetRegValue(SX127X_REG_FIFO_ADDR_PTR, SX127X_FIFO_RX_BASE_ADDR_MAX);
  if(state != ERR_NONE) {
    return(state);
  }
  
  // set mode to continuous reception
  return(setMode(SX127X_RXCONTINUOUS));
}

void SX127x::setDio0Action(void (*func)(void)) {
  attachInterrupt(digitalPinToInterrupt(_mod->int0()), func, RISING);
}

void SX127x::setDio1Action(void (*func)(void)) {
  attachInterrupt(digitalPinToInterrupt(_mod->int1()), func, RISING);
}

int16_t SX127x::startTransmit(String& str) {
  return(SX127x::startTransmit(str.c_str()));
}

int16_t SX127x::startTransmit(const char* str) {
  return(SX127x::startTransmit((uint8_t*)str, strlen(str)));
}

int16_t SX127x::startTransmit(uint8_t* data, size_t len) {
  // check packet length
  if(len >= 256) {
    return(ERR_PACKET_TOO_LONG);
  }
  
  // set mode to standby
  int16_t state = setMode(SX127X_STANDBY);
  
  // set DIO mapping
  _mod->SPIsetRegValue(SX127X_REG_DIO_MAPPING_1, SX127X_DIO0_TX_DONE, 7, 6);
  
  // clear interrupt flags
  clearIRQFlags();
  
  // set packet length
  state |= _mod->SPIsetRegValue(SX127X_REG_PAYLOAD_LENGTH, len);
  
  // set FIFO pointers
  state |= _mod->SPIsetRegValue(SX127X_REG_FIFO_TX_BASE_ADDR, SX127X_FIFO_TX_BASE_ADDR_MAX);
  state |= _mod->SPIsetRegValue(SX127X_REG_FIFO_ADDR_PTR, SX127X_FIFO_TX_BASE_ADDR_MAX);
  
  // write packet to FIFO
  _mod->SPIwriteRegisterBurst(SX127X_REG_FIFO, data, len);
  
  // start transmission
  state |= setMode(SX127X_TX);
  if(state != ERR_NONE) {
    return(state);
  }
}

int16_t SX127x::readData(String& str, size_t len) {
  // create temporary array to store received data
  char* data = new char[len];
  int16_t state = SX127x::readData((uint8_t*)data, len);
  
  // if packet was received successfully, copy data into String
  if(state == ERR_NONE) {
    str = String(data);
  }
  
  delete[] data;
  return(state);
}

int16_t SX127x::readData(uint8_t* data, size_t len) {
  // check integrity CRC
  if(_mod->SPIgetRegValue(SX127X_REG_IRQ_FLAGS, 5, 5) == SX127X_CLEAR_IRQ_FLAG_PAYLOAD_CRC_ERROR) {
    return(ERR_CRC_MISMATCH);
  }
  
  // get packet length
  size_t length = len;
  if(_sf != 6) {
    length = _mod->SPIgetRegValue(SX127X_REG_RX_NB_BYTES);
  }
  
  // read packet data
  if(len == 0) {
    // argument len equal to zero indicates String call, which means dynamically allocated data array
    // dispose of the original and create a new one
    delete[] data;
    data = new uint8_t[length + 1];
  }
  _mod->SPIreadRegisterBurst(SX127X_REG_FIFO, length, data);
  
  // add terminating null
  if(len == 0) {
    data[length] = 0;
  }
  
  // update RSSI and SNR
  lastPacketRSSI = -157 + _mod->SPIgetRegValue(SX127X_REG_PKT_RSSI_VALUE);
  int8_t rawSNR = (int8_t)_mod->SPIgetRegValue(SX127X_REG_PKT_SNR_VALUE);
  lastPacketSNR = rawSNR / 4.0;
  
  // clear interrupt flags
  clearIRQFlags();
  
  return(ERR_NONE);
}

int16_t SX127x::setSyncWord(uint8_t syncWord) {
  // set mode to standby
  setMode(SX127X_STANDBY);
  
  // write register 
  return(_mod->SPIsetRegValue(SX127X_REG_SYNC_WORD, syncWord));
}

int16_t SX127x::setCurrentLimit(uint8_t currentLimit) {
  // check allowed range
  if(!(((currentLimit >= 45) && (currentLimit <= 240)) || (currentLimit == 0))) {
    return(ERR_INVALID_CURRENT_LIMIT);
  }
  
  // set mode to standby
  int16_t state = setMode(SX127X_STANDBY);
  
  // set OCP limit
  uint8_t raw;
  if(currentLimit == 0) {
    // limit set to 0, disable OCP
    state |= _mod->SPIsetRegValue(SX127X_REG_OCP, SX127X_OCP_OFF, 5, 5);
  } else if(currentLimit <= 120) {
    raw = (currentLimit - 45) / 5;
    state |= _mod->SPIsetRegValue(SX127X_REG_OCP, SX127X_OCP_ON | raw, 5, 0);
  } else if(currentLimit <= 240) {
    raw = (currentLimit + 30) / 10;
    state |= _mod->SPIsetRegValue(SX127X_REG_OCP, SX127X_OCP_ON | raw, 5, 0);
  }
  return(state);
}

int16_t SX127x::setPreambleLength(uint16_t preambleLength) {
  // check allowed range
  if(preambleLength < 6) {
    return(ERR_INVALID_PREAMBLE_LENGTH);
  }
  
  // set mode to standby
  int16_t state = setMode(SX127X_STANDBY);
  
  // set preamble length
  state |= _mod->SPIsetRegValue(SX127X_REG_PREAMBLE_MSB, (preambleLength & 0xFF00) >> 8);
  state |= _mod->SPIsetRegValue(SX127X_REG_PREAMBLE_LSB, preambleLength & 0x00FF);
  return(state);
}

float SX127x::getFrequencyError() {
  // get raw frequency error
  uint32_t raw = _mod->SPIgetRegValue(SX127X_REG_FEI_MSB, 3, 0) << 16;
  raw |= _mod->SPIgetRegValue(SX127X_REG_FEI_MID) << 8;
  raw |= _mod->SPIgetRegValue(SX127X_REG_FEI_LSB);
  
  uint32_t base = (uint32_t)2 << 23;
  float error;
  
  // check the first bit
  if(raw & 0x80000) {
    // frequency error is negative
    raw = ~raw + 1;
    error = (((float)raw * (float)base)/32000000.0) * (_bw/500.0) * -1.0;
  } else {
    error = (((float)raw * (float)base)/32000000.0) * (_bw/500.0);
  }
  
  return(error);
}

int16_t SX127x::setBitRate(float br) {
  // check allowed bitrate
  if((br < 1.2) || (br > 300.0)) {
    return(ERR_INVALID_BIT_RATE);
  }
  
  // set mode to STANDBY
  int16_t state = setMode(SX127X_STANDBY);
  if(state != ERR_NONE) {
    return(state);
  }
  
  // set bit rate
  uint16_t bitRate = 32000 / br;
  state = _mod->SPIsetRegValue(SX127X_REG_BITRATE_MSB, (bitRate & 0xFF00) >> 8, 7, 0);
  state |= _mod->SPIsetRegValue(SX127X_REG_BITRATE_MSB, bitRate & 0x00FF, 7, 0);
  if(state == ERR_NONE) {
    SX127x::_br = br;
  }
  return(state);
}

int16_t SX127x::setRxBandwidth(float rxBw) {
  // check allowed bandwidth values
  uint8_t bwMant, bwExp;
  if(rxBw == 2.6) {
    bwMant = SX127X_RX_BW_MANT_24;
    bwExp = 7;
  } else if(rxBw == 3.1) {
    bwMant = SX127X_RX_BW_MANT_20;
    bwExp = 7;
  } else if(rxBw == 3.9) {
    bwMant = SX127X_RX_BW_MANT_16;
    bwExp = 7;
  } else if(rxBw == 5.2) {
    bwMant = SX127X_RX_BW_MANT_24;
    bwExp = 6;
  } else if(rxBw == 6.3) {
    bwMant = SX127X_RX_BW_MANT_20;
    bwExp = 6;
  } else if(rxBw == 7.8) {
    bwMant = SX127X_RX_BW_MANT_16;
    bwExp = 6;
  } else if(rxBw == 10.4) {
    bwMant = SX127X_RX_BW_MANT_24;
    bwExp = 5;
  } else if(rxBw == 12.5) {
    bwMant = SX127X_RX_BW_MANT_20;
    bwExp = 5;
  } else if(rxBw == 15.6) {
    bwMant = SX127X_RX_BW_MANT_16;
    bwExp = 5;
  } else if(rxBw == 20.8) {
    bwMant = SX127X_RX_BW_MANT_24;
    bwExp = 4;
  } else if(rxBw == 25.0) {
    bwMant = SX127X_RX_BW_MANT_20;
    bwExp = 4;
  } else if(rxBw == 31.3) {
    bwMant = SX127X_RX_BW_MANT_16;
    bwExp = 4;
  } else if(rxBw == 41.7) {
    bwMant = SX127X_RX_BW_MANT_24;
    bwExp = 3;
  } else if(rxBw == 50.0) {
    bwMant = SX127X_RX_BW_MANT_20;
    bwExp = 3;
  } else if(rxBw == 62.5) {
    bwMant = SX127X_RX_BW_MANT_16;
    bwExp = 3;
  } else if(rxBw == 83.3) {
    bwMant = SX127X_RX_BW_MANT_24;
    bwExp = 2;
  } else if(rxBw == 100.0) {
    bwMant = SX127X_RX_BW_MANT_20;
    bwExp = 2;
  } else if(rxBw == 125.0) {
    bwMant = SX127X_RX_BW_MANT_16;
    bwExp = 2;
  } else if(rxBw == 166.7) {
    bwMant = SX127X_RX_BW_MANT_24;
    bwExp = 1;
  } else if(rxBw == 200.0) {
    bwMant = SX127X_RX_BW_MANT_20;
    bwExp = 1;
  } else if(rxBw == 250.0) {
    bwMant = SX127X_RX_BW_MANT_16;
    bwExp = 1;
  } else {
    return(ERR_INVALID_RX_BANDWIDTH);
  }
  
  // set mode to STANDBY
  int16_t state = setMode(SX127X_STANDBY);
  if(state != ERR_NONE) {
    return(state);
  }
  
  // set Rx bandwidth
  state = _mod->SPIsetRegValue(SX127X_REG_RX_BW, bwMant | bwExp, 4, 0);
  if(state == ERR_NONE) {
    SX127x::_rxBw = rxBw;
  }
  return(state);
}

int16_t SX127x::setFrequencyDeviation(float freqDev) {
  // check frequency deviation range
  if(!((freqDev + _br/2.0 <= 250.0) && (freqDev >= 0.6) && (freqDev <= 200.0))) {
    return(ERR_INVALID_FREQUENCY_DEVIATION);
  }

  // set mode to STANDBY
  int16_t state = setMode(SX127X_STANDBY);
  if(state != ERR_NONE) {
    return(state);
  }
  
  // set allowed frequency deviation
  uint32_t base = 1;
  uint32_t FDEV = (freqDev * (base << 19)) / 32000;
  state = _mod->SPIsetRegValue(SX127X_REG_FDEV_MSB, (FDEV & 0xFF00) >> 8, 5, 0);
  state |= _mod->SPIsetRegValue(SX127X_REG_FDEV_LSB, FDEV & 0x00FF, 7, 0);
  
  return(state);
}

int16_t SX127x::setSyncWord(uint8_t* syncWord, size_t len) {
  // check constraints
  if(len > 7) {
    return(ERR_INVALID_SYNC_WORD);
  }
  
  // sync word must not contain value 0x00
  for(uint8_t i = 0; i < len; i++) {
    if(syncWord[i] == 0x00) {
      return(ERR_INVALID_SYNC_WORD);
    }
  }
  
  // enable sync word recognition
  int16_t state = _mod->SPIsetRegValue(SX127X_REG_SYNC_CONFIG, SX127X_SYNC_ON, 4, 4);
  state |= _mod->SPIsetRegValue(SX127X_REG_SYNC_CONFIG, len, 2, 0);
  if(state != ERR_NONE) {
    return(state);
  }
  
  // set sync word
  _mod->SPIwriteRegisterBurst(SX127X_SYNC_VALUE_1, syncWord, len);
  return(ERR_NONE);
}

int16_t SX127x::setFrequencyRaw(float newFreq) {
  // set mode to standby
  int16_t state = setMode(SX127X_STANDBY);
  
  // calculate register values
  uint32_t base = 1;
  uint32_t FRF = (newFreq * (base << 19)) / 32.0;
  
  // write registers
  state |= _mod->SPIsetRegValue(SX127X_REG_FRF_MSB, (FRF & 0xFF0000) >> 16);
  state |= _mod->SPIsetRegValue(SX127X_REG_FRF_MID, (FRF & 0x00FF00) >> 8);
  state |= _mod->SPIsetRegValue(SX127X_REG_FRF_LSB, FRF & 0x0000FF);
  return(state);
}

int16_t SX127x::config() {
  // turn off frequency hopping
  int16_t state = _mod->SPIsetRegValue(SX127X_REG_HOP_PERIOD, SX127X_HOP_PERIOD_OFF);
  return(state);
}

int16_t SX127x::configFSK() {
  // set mode to SLEEP
  int16_t state = setMode(SX127X_SLEEP);
  
  // set FSK mode
  state |= _mod->SPIsetRegValue(SX127X_REG_OP_MODE, SX127X_FSK_OOK, 7, 7);
  
  // set mode to STANDBY
  state |= setMode(SX127X_STANDBY);
  if(state != ERR_NONE) {
    return(state);
  }
  
  // set RSSI threshold
  state = _mod->SPIsetRegValue(SX127X_REG_RSSI_THRESH, SX127X_RSSI_THRESHOLD);
  if(state != ERR_NONE) {
    return(state);
  }
  
  // reset FIFO flag
  _mod->SPIwriteRegister(SX127X_REG_IRQ_FLAGS_2, SX127X_FLAG_FIFO_OVERRUN);
  
  // disable ClkOut on DIO5
  /*state = _mod->SPIsetRegValue();
  if(state != ERR_NONE) {
    return(state);
  }*/
  
  // set packet configuration
  state = _mod->SPIsetRegValue(SX127X_REG_PACKET_CONFIG_1, SX127X_PACKET_VARIABLE | SX127X_DC_FREE_NONE | SX127X_CRC_ON | SX127X_CRC_AUTOCLEAR_ON | SX127X_ADDRESS_FILTERING_OFF, 7, 1);
  state |= _mod->SPIsetRegValue(SX127X_REG_PACKET_CONFIG_2, SX127X_DATA_MODE_PACKET | SX127X_IO_HOME_OFF, 6, 5);
  if(state != ERR_NONE) {
    return(state);
  }
  
  // set payload length
  state = _mod->SPIsetRegValue(SX127X_REG_PAYLOAD_LENGTH_FSK, 0x40);
  if(state != ERR_NONE) {
    return(state);
  }
  
  // set FIFO threshold
  state = _mod->SPIsetRegValue(SX127X_REG_FIFO_THRESH, SX127X_TX_START_FIFO_NOT_EMPTY, 7, 7);
  state |= _mod->SPIsetRegValue(SX127X_REG_FIFO_THRESH, SX127X_FIFO_THRESH, 5, 0);
  if(state != ERR_NONE) {
    return(state);
  }
  
  // set Rx timeouts
  state = _mod->SPIsetRegValue(SX127X_REG_RX_TIMEOUT_1, 0xFF);
  state |= _mod->SPIsetRegValue(SX127X_REG_RX_TIMEOUT_2, 0xFF);
  state |= _mod->SPIsetRegValue(SX127X_REG_RX_TIMEOUT_3, 0xFF);
  if(state != ERR_NONE) {
    return(state);
  }
  
  // enable improved fading margin
  /*state = _mod->SPIsetRegValue();
  if(state != ERR_NONE) {
    return(state);
  }*/
  
  return(state);
}

bool SX127x::findChip(uint8_t ver) {
  uint8_t i = 0;
  bool flagFound = false;
  while((i < 10) && !flagFound) {
    uint8_t version = _mod->SPIreadRegister(SX127X_REG_VERSION);
    if(version == ver) {
      flagFound = true;
    } else {
      #ifdef KITELIB_DEBUG
        Serial.print(F("SX127x not found! ("));
        Serial.print(i + 1);
        Serial.print(F(" of 10 tries) SX127X_REG_VERSION == "));
        
        char buffHex[5];
        sprintf(buffHex, "0x%02X", version);
        Serial.print(buffHex);
        Serial.print(F(", expected 0x00"));
        Serial.print(ver, HEX);
        Serial.println();
      #endif
      delay(1000);
      i++;
    }
  }
  
  return(flagFound);
}

int16_t SX127x::setMode(uint8_t mode) {
  return(_mod->SPIsetRegValue(SX127X_REG_OP_MODE, mode, 2, 0));
}

int16_t SX127x::getActiveModem() {
  return(_mod->SPIgetRegValue(SX127X_REG_OP_MODE, 7, 7));
}

int16_t SX127x::setActiveModem(uint8_t modem) {
  // set mode to SLEEP
  int16_t state = setMode(SX127X_SLEEP);
  
  // set LoRa mode
  state |= _mod->SPIsetRegValue(SX127X_REG_OP_MODE, modem, 7, 7);
  
  // set mode to STANDBY
  state |= setMode(SX127X_STANDBY);
  return(state);
}

void SX127x::clearIRQFlags() {
  int16_t modem = getActiveModem();
  if(modem == SX127X_LORA) {
    _mod->SPIwriteRegister(SX127X_REG_IRQ_FLAGS, 0b11111111);
  } else if(modem == SX127X_FSK_OOK) {
    _mod->SPIwriteRegister(SX127X_REG_IRQ_FLAGS_1, 0b11111111);
    _mod->SPIwriteRegister(SX127X_REG_IRQ_FLAGS_2, 0b11111111);
  }
}
