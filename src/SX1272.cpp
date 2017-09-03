#include "SX1272.h"

SX1272::SX1272(int nss, Bandwidth bw, SpreadingFactor sf, CodingRate cr) {
  _nss = nss;
  
  switch(bw) {
    case BW_125_00_KHZ:
      _bw = SX1272_BW_125_00_KHZ;
      break;
    case BW_250_00_KHZ:
      _bw = SX1272_BW_250_00_KHZ;
      break;
    case BW_500_00_KHZ:
      _bw = SX1272_BW_500_00_KHZ;
      break;
    default:
      _bw = SX1272_BW_250_00_KHZ;
      break;
  }
  
  switch(sf) {
    case SF_6:
      _sf = SX1272_SF_6;
      break;
    case SF_7:
      _sf = SX1272_SF_7;
      break;
    case SF_8:
      _sf = SX1272_SF_8;
      break;
    case SF_9:
      _sf = SX1272_SF_9;
      break;
    case SF_10:
      _sf = SX1272_SF_10;
      break;
    case SF_11:
      _sf = SX1272_SF_11;
      break;
    case SF_12:
      _sf = SX1272_SF_12;
      break;
    default:
      _sf = SX1272_SF_12;
      break;
  }
  
  switch(cr) {
    case CR_4_5:
      _cr = SX1272_CR_4_5;
      break;
    case CR_4_6:
      _cr = SX1272_CR_4_6;
      break;
    case CR_4_7:
      _cr = SX1272_CR_4_7;
      break;
    case CR_4_8:
      _cr = SX1272_CR_4_8;
      break;
    default:
      _cr = SX1272_CR_4_5;
      break;
  }
}

uint8_t SX1272::begin(void) {
  initModule(_nss);
  
  uint8_t i = 0;
  bool flagFound = false;
  while((i < 10) && !flagFound) {
    uint8_t version = readRegister(SX1272_REG_VERSION);
    if(version == 0x12) {
      flagFound = true;
    } else {
      #ifdef DEBUG
        Serial.print("SX1272 not found! (");
        Serial.print(i + 1);
        Serial.print(" of 10 tries) SX1272_REG_VERSION == ");
        
        char buffHex[5];
        sprintf(buffHex, "0x%02X", version);
        Serial.print(buffHex);
        Serial.println();
      #endif
      delay(1000);
      i++;
    }
  }
  
  if(!flagFound) {
    #ifdef DEBUG
      Serial.println("No SX1272 found!");
    #endif
    SPI.end();
    return(ERR_CHIP_NOT_FOUND);
  }
  #ifdef DEBUG
    else {
      Serial.println("Found SX1272! (match by SX1272_REG_VERSION == 0x12)");
    }
  #endif
  
  return(config(_bw, _sf, _cr));
}

uint8_t SX1272::tx(char* data, uint8_t length) {
  setMode(SX1272_STANDBY);
  
  setRegValue(SX1272_REG_DIO_MAPPING_1, SX1272_DIO0_TX_DONE, 7, 6);
  clearIRQFlags();
  
  if(length > 256) {
    return(ERR_PACKET_TOO_LONG);
  }

  setRegValue(SX1272_REG_PAYLOAD_LENGTH, length);
  setRegValue(SX1272_REG_FIFO_TX_BASE_ADDR, SX1272_FIFO_TX_BASE_ADDR_MAX);
  setRegValue(SX1272_REG_FIFO_ADDR_PTR, SX1272_FIFO_TX_BASE_ADDR_MAX);
  
  writeRegisterBurstStr(SX1272_REG_FIFO, data, length);
  
  setMode(SX1272_TX);
  
  unsigned long start = millis();
  while(!digitalRead(_dio0)) {
    #ifdef DEBUG
      Serial.print('.');
    #endif
  }
  
  clearIRQFlags();
  
  return(ERR_NONE);
}

uint8_t SX1272::rxSingle(char* data, uint8_t* length) {
  setMode(SX1272_STANDBY);
  
  setRegValue(SX1272_REG_DIO_MAPPING_1, SX1272_DIO0_RX_DONE | SX1272_DIO1_RX_TIMEOUT, 7, 4);
  clearIRQFlags();
  
  setRegValue(SX1272_REG_FIFO_RX_BASE_ADDR, SX1272_FIFO_RX_BASE_ADDR_MAX);
  setRegValue(SX1272_REG_FIFO_ADDR_PTR, SX1272_FIFO_RX_BASE_ADDR_MAX);
  
  setMode(SX1272_RXSINGLE);
  
  while(!digitalRead(_dio0)) {
    if(digitalRead(_dio1)) {
      clearIRQFlags();
      return(ERR_RX_TIMEOUT);
    }
  }
  
  if(getRegValue(SX1272_REG_IRQ_FLAGS, 5, 5) == SX1272_CLEAR_IRQ_FLAG_PAYLOAD_CRC_ERROR) {
    return(ERR_CRC_MISMATCH);
  }
  
  uint8_t headerMode = getRegValue(SX1272_REG_MODEM_CONFIG_1, 0, 0);
  if(headerMode == SX1272_HEADER_EXPL_MODE) {
    *length = getRegValue(SX1272_REG_RX_NB_BYTES);
  }
  
  readRegisterBurstStr(SX1272_REG_FIFO, *length, data);
  
  clearIRQFlags();
  
  return(ERR_NONE);
}

uint8_t SX1272::setMode(uint8_t mode) {
  setRegValue(SX1272_REG_OP_MODE, mode, 2, 0);
  return(ERR_NONE);
}

uint8_t SX1272::config(uint8_t bw, uint8_t sf, uint8_t cr) {
  uint8_t status = ERR_NONE;
  
  if((bw != SX1272_BW_125_00_KHZ) &&
     (bw != SX1272_BW_250_00_KHZ) &&
     (bw != SX1272_BW_500_00_KHZ)) {
       return(ERR_INVALID_BANDWIDTH);
  }
  
  if((sf != SX1272_SF_6) &&
     (sf != SX1272_SF_7) &&
     (sf != SX1272_SF_8) &&
     (sf != SX1272_SF_9) &&
     (sf != SX1272_SF_10) &&
     (sf != SX1272_SF_11) &&
     (sf != SX1272_SF_12)) {
       return(ERR_INVALID_SPREADING_FACTOR);
  }
  
    if((cr != SX1272_CR_4_5) &&
     (cr != SX1272_CR_4_6) &&
     (cr != SX1272_CR_4_7) &&
     (cr != SX1272_CR_4_8)) {
       return(ERR_INVALID_CODING_RATE);
  }
  
  status = setMode(SX1272_SLEEP);
  if(status != ERR_NONE) {
    return(status);
  }
  
  status = setRegValue(SX1272_REG_OP_MODE, SX1272_LORA, 7, 7);
  if(status != ERR_NONE) {
    return(status);
  }
  
  status = setRegValue(SX1272_REG_FRF_MSB, SX1272_FRF_MSB);
  status = setRegValue(SX1272_REG_FRF_MID, SX1272_FRF_MID);
  status = setRegValue(SX1272_REG_FRF_LSB, SX1272_FRF_LSB);
  if(status != ERR_NONE) {
    return(status);
  }
  
  status = setRegValue(SX1272_REG_PA_CONFIG, SX1272_PA_SELECT_BOOST | SX1272_OUTPUT_POWER);
  status = setRegValue(SX1272_REG_OCP, SX1272_OCP_ON | SX1272_OCP_TRIM, 5, 0);
  status = setRegValue(SX1272_REG_LNA, SX1272_LNA_GAIN_1 | SX1272_LNA_BOOST_ON);
  status = setRegValue(SX1272_REG_PA_DAC, SX1272_PA_BOOST_ON, 2, 0);
  if(status != ERR_NONE) {
    return(status);
  }
  
  status = setRegValue(SX1272_REG_HOP_PERIOD, SX1272_HOP_PERIOD_OFF);
  if(status != ERR_NONE) {
    return(status);
  }
  
  if(sf == SX1272_SF_6) {
    status = setRegValue(SX1272_REG_MODEM_CONFIG_2, SX1272_SF_6 | SX1272_TX_MODE_SINGLE | SX1272_RX_CRC_MODE_OFF, 7, 2);
    status = setRegValue(SX1272_REG_MODEM_CONFIG_1, bw | cr | SX1272_HEADER_IMPL_MODE);
    status = setRegValue(SX1272_REG_DETECT_OPTIMIZE, SX1272_DETECT_OPTIMIZE_SF_6, 2, 0);
    status = setRegValue(SX1272_REG_DETECTION_THRESHOLD, SX1272_DETECTION_THRESHOLD_SF_6);
  } else {
    status = setRegValue(SX1272_REG_MODEM_CONFIG_2, sf | SX1272_TX_MODE_SINGLE | SX1272_RX_CRC_MODE_ON, 7, 2);
    status = setRegValue(SX1272_REG_MODEM_CONFIG_1, bw | cr | SX1272_HEADER_EXPL_MODE);
    status = setRegValue(SX1272_REG_DETECT_OPTIMIZE, SX1272_DETECT_OPTIMIZE_SF_7_12, 2, 0);
    status = setRegValue(SX1272_REG_DETECTION_THRESHOLD, SX1272_DETECTION_THRESHOLD_SF_7_12);
  }
  
  if(status != ERR_NONE) {
    return(status);
  }
  
  status = setRegValue(SX1272_REG_PREAMBLE_MSB, SX1272_PREAMBLE_LENGTH_MSB);
  status = setRegValue(SX1272_REG_PREAMBLE_LSB, SX1272_PREAMBLE_LENGTH_LSB);
  if(status != ERR_NONE) {
    return(status);
  }
  
  status = setMode(SX1272_STANDBY);
  if(status != ERR_NONE) {
    return(status);
  }
  
  return(ERR_NONE);
}

int8_t SX1272::getLastPacketRSSI(void) {
  return(-157 + getRegValue(SX1272_REG_PKT_RSSI_VALUE));
}

void SX1272::clearIRQFlags(void) {
  writeRegister(SX1272_REG_IRQ_FLAGS, 0b11111111);
}
