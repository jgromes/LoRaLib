#include "SX127x.h"

SX127x::SX127x(Chip ch, int dio0, int dio1) {
  _ch = ch;
  _dio0 = dio0;
  _dio1 = dio1;
}

uint8_t SX127x::begin() {
  uint8_t i = 0;
  bool flagFound = false;
  while((i < 10) && !flagFound) {
    uint8_t version = readRegister(SX127X_REG_VERSION);
    if(version == 0x12) {
      flagFound = true;
    } else {
      #ifdef DEBUG
        Serial.print(getChipName());
        Serial.print(" not found! (");
        Serial.print(i + 1);
        Serial.print(" of 10 tries) REG_VERSION == ");
        
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
      Serial.print(getChipName());
      Serial.println(" not found!");
    #endif
    SPI.end();
    return(ERR_CHIP_NOT_FOUND);
  }
  #ifdef DEBUG
    else {
      Serial.print(getChipName());
      Serial.println(" found! (match by REG_VERSION == 0x12)");
    }
  #endif
  
  return(ERR_NONE);
}

uint8_t SX127x::tx(char* data, uint8_t length, uint32_t timeout) {
  setMode(SX127X_STANDBY);
  
  setRegValue(SX127X_REG_DIO_MAPPING_1, SX127X_DIO0_TX_DONE, 7, 6);
  clearIRQFlags();
  
  if(length >= 256) {
    return(ERR_PACKET_TOO_LONG);
  }

  setRegValue(SX127X_REG_PAYLOAD_LENGTH, length);
  setRegValue(SX127X_REG_FIFO_TX_BASE_ADDR, SX127X_FIFO_TX_BASE_ADDR_MAX);
  setRegValue(SX127X_REG_FIFO_ADDR_PTR, SX127X_FIFO_TX_BASE_ADDR_MAX);
  
  writeRegisterBurstStr(SX127X_REG_FIFO, data, length);
  
  setMode(SX127X_TX);
  
  uint32_t start = millis();
  while(!digitalRead(_dio0)) {
    if(millis() - start > timeout) {
      clearIRQFlags();
      return(ERR_TX_TIMEOUT);
    }
  }
  
  clearIRQFlags();
  
  return(ERR_NONE);
}

uint8_t SX127x::rxSingle(char* data, uint8_t* length, bool headerExplMode) {
  setMode(SX127X_STANDBY);
  
  setRegValue(SX127X_REG_DIO_MAPPING_1, SX127X_DIO0_RX_DONE | SX127X_DIO1_RX_TIMEOUT, 7, 4);
  clearIRQFlags();
  
  setRegValue(SX127X_REG_FIFO_RX_BASE_ADDR, SX127X_FIFO_RX_BASE_ADDR_MAX);
  setRegValue(SX127X_REG_FIFO_ADDR_PTR, SX127X_FIFO_RX_BASE_ADDR_MAX);
  
  setMode(SX127X_RXSINGLE);
  
  while(!digitalRead(_dio0)) {
    if(digitalRead(_dio1)) {
      clearIRQFlags();
      return(ERR_RX_TIMEOUT);
    }
  }
  
  if(getRegValue(SX127X_REG_IRQ_FLAGS, 5, 5) == SX127X_CLEAR_IRQ_FLAG_PAYLOAD_CRC_ERROR) {
    return(ERR_CRC_MISMATCH);
  }
  
  if(headerExplMode) {
    *length = getRegValue(SX127X_REG_RX_NB_BYTES);
  }
  
  readRegisterBurstStr(SX127X_REG_FIFO, *length, data);
  
  clearIRQFlags();
  
  return(ERR_NONE);
}

uint8_t SX127x::runCAD() {
  setMode(SX127X_STANDBY);
  
  setRegValue(SX127X_REG_DIO_MAPPING_1, SX127X_DIO0_CAD_DONE | SX127X_DIO1_CAD_DETECTED, 7, 4);
  clearIRQFlags();
  
  setMode(SX127X_CAD);
  
  while(!digitalRead(_dio0)) {
    if(digitalRead(_dio1)) {
      clearIRQFlags();
      return(PREAMBLE_DETECTED);
    }
  }
  
  clearIRQFlags();
  return(CHANNEL_FREE);
}

uint8_t SX127x::setOutputPower(int8_t power) {
  setMode(SX127X_STANDBY);

  if((power < 2) || (power > 17)) {
    return(ERR_INVALID_OUTPUT_POWER);
  }
  
  return(setRegValue(SX127X_REG_PA_CONFIG, power - 2, 3, 0));
}

uint8_t SX127x::setMode(uint8_t mode) {
  setRegValue(SX127X_REG_OP_MODE, mode, 2, 0);
  return(ERR_NONE);
}

uint8_t SX127x::config(uint8_t bw, uint8_t sf, uint8_t cr, float freq, uint8_t syncWord) {
  uint8_t status = ERR_NONE;
  
  // set mode to SLEEP
  status = setMode(SX127X_SLEEP);
  if(status != ERR_NONE) {
    return(status);
  }
  
  // set LoRa mode
  status = setRegValue(SX127X_REG_OP_MODE, SX127X_LORA, 7, 7);
  if(status != ERR_NONE) {
    return(status);
  }
  
  // set carrier frequency
  uint32_t base = 2;
  uint32_t FRF = (freq * (base << 18)) / 32.0;
  status = setRegValue(SX127X_REG_FRF_MSB, (FRF & 0xFF0000) >> 16);
  status = setRegValue(SX127X_REG_FRF_MID, (FRF & 0x00FF00) >> 8);
  status = setRegValue(SX127X_REG_FRF_LSB, FRF & 0x0000FF);
  if(status != ERR_NONE) {
    return(status);
  }
  
  // output power configuration
  status = setRegValue(SX127X_REG_PA_CONFIG, SX127X_PA_SELECT_BOOST | SX127X_OUTPUT_POWER);
  status = setRegValue(SX127X_REG_OCP, SX127X_OCP_ON | SX127X_OCP_TRIM, 5, 0);
  status = setRegValue(SX127X_REG_LNA, SX127X_LNA_GAIN_1 | SX127X_LNA_BOOST_ON);
  if(status != ERR_NONE) {
    return(status);
  }
  
  // turn off frequency hopping
  status = setRegValue(SX127X_REG_HOP_PERIOD, SX127X_HOP_PERIOD_OFF);
  if(status != ERR_NONE) {
    return(status);
  }
  
  // basic setting (bw, cr, sf, header mode and CRC)
  if(sf == SX127X_SF_6) {
    status = setRegValue(SX127X_REG_MODEM_CONFIG_2, SX127X_SF_6 | SX127X_TX_MODE_SINGLE, 7, 3);
    status = setRegValue(SX127X_REG_DETECT_OPTIMIZE, SX127X_DETECT_OPTIMIZE_SF_6, 2, 0);
    status = setRegValue(SX127X_REG_DETECTION_THRESHOLD, SX127X_DETECTION_THRESHOLD_SF_6);
  } else {
    status = setRegValue(SX127X_REG_MODEM_CONFIG_2, sf | SX127X_TX_MODE_SINGLE, 7, 3);
    status = setRegValue(SX127X_REG_DETECT_OPTIMIZE, SX127X_DETECT_OPTIMIZE_SF_7_12, 2, 0);
    status = setRegValue(SX127X_REG_DETECTION_THRESHOLD, SX127X_DETECTION_THRESHOLD_SF_7_12);
  }
  
  if(status != ERR_NONE) {
    return(status);
  }
  
  // set the sync word
  status = setRegValue(SX127X_REG_SYNC_WORD, syncWord);
  if(status != ERR_NONE) {
    return(status);
  }
  
  // set default preamble length
  status = setRegValue(SX127X_REG_PREAMBLE_MSB, SX127X_PREAMBLE_LENGTH_MSB);
  status = setRegValue(SX127X_REG_PREAMBLE_LSB, SX127X_PREAMBLE_LENGTH_LSB);
  if(status != ERR_NONE) {
    return(status);
  }
  
  // set mode to STANDBY
  status = setMode(SX127X_STANDBY);
  return(status);
}

int8_t SX127x::getLastPacketRSSI() {
  return(-157 + getRegValue(SX127X_REG_PKT_RSSI_VALUE));
}

float SX127x::getLastPacketSNR() {
  int8_t rawSNR = (int8_t)getRegValue(SX127X_REG_PKT_SNR_VALUE);
  return(rawSNR / 4.0);
}

void SX127x::clearIRQFlags() {
  writeRegister(SX127X_REG_IRQ_FLAGS, 0b11111111);
}

const char* SX127x::getChipName() {
  const char* names[] = {"SX1272", "SX1273", "SX1276", "SX1277", "SX1278", "SX1279"};
	return(names[_ch]);
}
