#include "SX1278.h"

SX1278::SX1278(int nss, Bandwidth bw, SpreadingFactor sf, CodingRate cr) {
  _nss = nss;
  
  switch(bw) {
    case BW_7_80_KHZ:
      _bw = SX1278_BW_7_80_KHZ;
      break;
    case BW_10_40_KHZ:
      _bw = SX1278_BW_10_40_KHZ;
      break;
    case BW_15_60_KHZ:
      _bw = SX1278_BW_15_60_KHZ;
      break;
    case BW_20_80_KHZ:
      _bw = SX1278_BW_20_80_KHZ;
      break;
    case BW_31_25_KHZ:
      _bw = SX1278_BW_31_25_KHZ;
      break;
      case BW_41_70_KHZ:
      _bw = SX1278_BW_41_70_KHZ;
      break;
    case BW_62_50_KHZ:
      _bw = SX1278_BW_62_50_KHZ;
      break;
    case BW_125_00_KHZ:
      _bw = SX1278_BW_125_00_KHZ;
      break;
    case BW_250_00_KHZ:
      _bw = SX1278_BW_250_00_KHZ;
      break;
    case BW_500_00_KHZ:
      _bw = SX1278_BW_500_00_KHZ;
      break;
    default:
      _bw = SX1278_BW_250_00_KHZ;
      break;
  }
  
  switch(sf) {
    case SF_6:
      _sf = SX1278_SF_6;
      break;
    case SF_7:
      _sf = SX1278_SF_7;
      break;
    case SF_8:
      _sf = SX1278_SF_8;
      break;
    case SF_9:
      _sf = SX1278_SF_9;
      break;
    case SF_10:
      _sf = SX1278_SF_10;
      break;
    case SF_11:
      _sf = SX1278_SF_11;
      break;
    case SF_12:
      _sf = SX1278_SF_12;
      break;
    default:
      _sf = SX1278_SF_12;
      break;
  }
  
  switch(cr) {
    case CR_4_5:
      _cr = SX1278_CR_4_5;
      break;
    case CR_4_6:
      _cr = SX1278_CR_4_6;
      break;
    case CR_4_7:
      _cr = SX1278_CR_4_7;
      break;
    case CR_4_8:
      _cr = SX1278_CR_4_8;
      break;
    default:
      _cr = SX1278_CR_4_5;
      break;
  }
}

uint8_t SX1278::begin(void) {
  initModule(_nss);
  
  uint8_t i = 0;
  bool flagFound = false;
  while((i < 10) && !flagFound) {
    uint8_t version = readRegister(SX1278_REG_VERSION);
    if(version == 0x12) {
      flagFound = true;
    } else {
      #ifdef DEBUG
        Serial.print("SX1278 not found! (");
        Serial.print(i + 1);
        Serial.print(" of 10 tries) SX1278_REG_VERSION == ");
        
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
      Serial.println("No SX1278 found!");
    #endif
    SPI.end();
    return(ERR_CHIP_NOT_FOUND);
  }
  #ifdef DEBUG
    else {
      Serial.println("Found SX1278! (match by SX1278_REG_VERSION == 0x12)");
    }
  #endif
  
  return(config(_bw, _sf, _cr));
}

uint8_t SX1278::tx(char* data, uint8_t length) {
  setMode(SX1278_STANDBY);
  
  setRegValue(SX1278_REG_DIO_MAPPING_1, SX1278_DIO0_TX_DONE, 7, 6);
  clearIRQFlags();
  
  if(length > 256) {
    return(ERR_PACKET_TOO_LONG);
  }

  setRegValue(SX1278_REG_PAYLOAD_LENGTH, length);
  setRegValue(SX1278_REG_FIFO_TX_BASE_ADDR, SX1278_FIFO_TX_BASE_ADDR_MAX);
  setRegValue(SX1278_REG_FIFO_ADDR_PTR, SX1278_FIFO_TX_BASE_ADDR_MAX);
  
  writeRegisterBurstStr(SX1278_REG_FIFO, data, length);
  
  setMode(SX1278_TX);
  
  unsigned long start = millis();
  while(!digitalRead(_dio0)) {
    #ifdef DEBUG
      Serial.print('.');
    #endif
  }
  
  clearIRQFlags();
  
  return(ERR_NONE);
}

uint8_t SX1278::rxSingle(char* data, uint8_t* length) {
  setMode(SX1278_STANDBY);
  
  setRegValue(SX1278_REG_DIO_MAPPING_1, SX1278_DIO0_RX_DONE | SX1278_DIO1_RX_TIMEOUT, 7, 4);
  clearIRQFlags();
  
  setRegValue(SX1278_REG_FIFO_RX_BASE_ADDR, SX1278_FIFO_RX_BASE_ADDR_MAX);
  setRegValue(SX1278_REG_FIFO_ADDR_PTR, SX1278_FIFO_RX_BASE_ADDR_MAX);
  
  setMode(SX1278_RXSINGLE);
  
  while(!digitalRead(_dio0)) {
    if(digitalRead(_dio1)) {
      clearIRQFlags();
      return(ERR_RX_TIMEOUT);
    }
  }
  
  if(getRegValue(SX1278_REG_IRQ_FLAGS, 5, 5) == SX1278_CLEAR_IRQ_FLAG_PAYLOAD_CRC_ERROR) {
    return(ERR_CRC_MISMATCH);
  }
  
  uint8_t headerMode = getRegValue(SX1278_REG_MODEM_CONFIG_1, 0, 0);
  if(headerMode == SX1278_HEADER_EXPL_MODE) {
    *length = getRegValue(SX1278_REG_RX_NB_BYTES);
  }
  
  readRegisterBurstStr(SX1278_REG_FIFO, *length, data);
  
  clearIRQFlags();
  
  return(ERR_NONE);
}

uint8_t SX1278::setMode(uint8_t mode) {
  setRegValue(SX1278_REG_OP_MODE, mode, 2, 0);
  return(ERR_NONE);
}

uint8_t SX1278::config(uint8_t bw, uint8_t sf, uint8_t cr) {
  uint8_t status = ERR_NONE;
  
  //check the supplied bw, cr and sf values
  if((bw != SX1278_BW_7_80_KHZ) &&
     (bw != SX1278_BW_10_40_KHZ) &&
     (bw != SX1278_BW_15_60_KHZ) &&
     (bw != SX1278_BW_20_80_KHZ) &&
     (bw != SX1278_BW_31_25_KHZ) &&
     (bw != SX1278_BW_41_70_KHZ) &&
     (bw != SX1278_BW_62_50_KHZ) &&
     (bw != SX1278_BW_125_00_KHZ) &&
     (bw != SX1278_BW_250_00_KHZ) &&
     (bw != SX1278_BW_500_00_KHZ)) {
       return(ERR_INVALID_BANDWIDTH);
  }
  
  if((sf != SX1278_SF_6) &&
     (sf != SX1278_SF_7) &&
     (sf != SX1278_SF_8) &&
     (sf != SX1278_SF_9) &&
     (sf != SX1278_SF_10) &&
     (sf != SX1278_SF_11) &&
     (sf != SX1278_SF_12)) {
       return(ERR_INVALID_SPREADING_FACTOR);
  }
  
  if((cr != SX1278_CR_4_5) &&
     (cr != SX1278_CR_4_6) &&
     (cr != SX1278_CR_4_7) &&
     (cr != SX1278_CR_4_8)) {
       return(ERR_INVALID_CODING_RATE);
  }
  
  // set mode to SLEEP
  status = setMode(SX1278_SLEEP);
  if(status != ERR_NONE) {
    return(status);
  }
  
  // set LoRa mode
  status = setRegValue(SX1278_REG_OP_MODE, SX1278_LORA, 7, 7);
  if(status != ERR_NONE) {
    return(status);
  }
  
  // set carrier frequency
  status = setRegValue(SX1278_REG_FRF_MSB, SX1278_FRF_MSB);
  status = setRegValue(SX1278_REG_FRF_MID, SX1278_FRF_MID);
  status = setRegValue(SX1278_REG_FRF_LSB, SX1278_FRF_LSB);
  if(status != ERR_NONE) {
    return(status);
  }
  
  // output power configuration
  status = setRegValue(SX1278_REG_PA_CONFIG, SX1278_PA_SELECT_BOOST | SX1278_MAX_POWER | SX1278_OUTPUT_POWER);
  status = setRegValue(SX1278_REG_OCP, SX1278_OCP_ON | SX1278_OCP_TRIM, 5, 0);
  status = setRegValue(SX1278_REG_LNA, SX1278_LNA_GAIN_1 | SX1278_LNA_BOOST_HF_ON);
  status = setRegValue(SX1278_REG_PA_DAC, SX1278_PA_BOOST_ON, 2, 0);
  if(status != ERR_NONE) {
    return(status);
  }
  
  // turn off frequency hopping
  status = setRegValue(SX1278_REG_HOP_PERIOD, SX1278_HOP_PERIOD_OFF);
  if(status != ERR_NONE) {
    return(status);
  }
  
  // basic setting (bw, cr, sf, header mode and CRC)
  if(sf == SX1278_SF_6) {
    status = setRegValue(SX1278_REG_MODEM_CONFIG_2, SX1278_SF_6 | SX1278_TX_MODE_SINGLE | SX1278_RX_CRC_MODE_OFF, 7, 2);
    status = setRegValue(SX1278_REG_MODEM_CONFIG_1, bw | cr | SX1278_HEADER_IMPL_MODE);
    status = setRegValue(SX1278_REG_DETECT_OPTIMIZE, SX1278_DETECT_OPTIMIZE_SF_6, 2, 0);
    status = setRegValue(SX1278_REG_DETECTION_THRESHOLD, SX1278_DETECTION_THRESHOLD_SF_6);
  } else {
    status = setRegValue(SX1278_REG_MODEM_CONFIG_2, sf | SX1278_TX_MODE_SINGLE | SX1278_RX_CRC_MODE_ON, 7, 2);
    status = setRegValue(SX1278_REG_MODEM_CONFIG_1, bw | cr | SX1278_HEADER_EXPL_MODE);
    status = setRegValue(SX1278_REG_DETECT_OPTIMIZE, SX1278_DETECT_OPTIMIZE_SF_7_12, 2, 0);
    status = setRegValue(SX1278_REG_DETECTION_THRESHOLD, SX1278_DETECTION_THRESHOLD_SF_7_12);
  }
  
  if(status != ERR_NONE) {
    return(status);
  }
  
  // set default preamble length
  status = setRegValue(SX1278_REG_PREAMBLE_MSB, SX1278_PREAMBLE_LENGTH_MSB);
  status = setRegValue(SX1278_REG_PREAMBLE_LSB, SX1278_PREAMBLE_LENGTH_LSB);
  if(status != ERR_NONE) {
    return(status);
  }
  
  // set mode to STANDBY
  status = setMode(SX1278_STANDBY);
  if(status != ERR_NONE) {
    return(status);
  }
  
  return(ERR_NONE);
}

int8_t SX1278::getLastPacketRSSI(void) {
  return(-164 + getRegValue(SX1278_REG_PKT_RSSI_VALUE));
}

void SX1278::clearIRQFlags(void) {
  writeRegister(SX1278_REG_IRQ_FLAGS, 0b11111111);
}
