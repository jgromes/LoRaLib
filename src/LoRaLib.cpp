#include "LoRaLib.h"

LoRa::LoRa(int nss, uint8_t bw, uint8_t cr, uint8_t sf) {
  _nss = nss;
  _bw = bw;
  _cr = cr;
  _sf = sf;
  
  pinMode(_nss, OUTPUT);
  pinMode(_sck, OUTPUT);
  pinMode(_miso, INPUT);
  pinMode(_mosi, OUTPUT);
  pinMode(_reset, OUTPUT);
  pinMode(_dio0, INPUT);
  
  SPI.begin();
}

uint8_t LoRa::init() {
  #ifdef DEBUG
    Serial.begin(9600);
    Serial.println();
  #endif
  
  randomSeed(analogRead(1));
  
  if(EEPROM.read(8) == 255) {
    generateLoRaAdress();
    EEPROM.write(8, 0);
    delay(100);
  }
  
  #ifdef DEBUG
    Serial.print("LoRa node address string: ");
  #endif
  for(uint8_t i = 0; i < 8; i++) {
    _LoRaAddress[i] = EEPROM.read(i);
    #ifdef DEBUG
      Serial.print(_LoRaAddress[i], HEX);
      if(i < 7) {
        Serial.print(":");
      } else {
        Serial.println();
      }
    #endif
    delay(100);
  }
  
  #ifdef VERBOSE
    Serial.println("Register dump:");
    uint8_t inByte;
    for(uint8_t adr = 0x01; adr <= 0x70; adr++) {
      Serial.print(adr, HEX);
      Serial.print('\t');
      inByte = readRegister(adr);
      Serial.print(inByte, HEX);
      Serial.print('\t');
      Serial.println(inByte, BIN);
    }
    Serial.println("Done.");
  #endif
  
  if(readRegister(SX1278_REG_VERSION) != 0x12) {
    #ifdef DEBUG
      Serial.print("No SX1278 found!");
    #endif
    SPI.end();
    while(true);
  }
  #ifdef DEBUG
    else {
      Serial.println("Found SX1278! (match by SX1278_REG_VERSION == 0x12)");
    }
  #endif
  
  config(_bw, _cr, _sf);
}

int LoRa::tx(packet* pack) {
  setMode(SX1278_STANDBY);
  setRegValue(SX1278_REG_PA_DAC, SX1278_PA_BOOST_ON, 2, 0);
  setRegValue(SX1278_REG_HOP_PERIOD, SX1278_HOP_PERIOD_OFF);
  setRegValue(SX1278_REG_DIO_MAPPING_1, SX1278_DIO0_TX_DONE, 7, 6);
  clearIRQFlags();
  setRegValue(SX1278_REG_IRQ_FLAGS_MASK, SX1278_MASK_IRQ_FLAG_TX_DONE);
  
  uint8_t packetLength = getPacketLength(pack);
  if(packetLength > 256) {
    return -1;
  }
  setRegValue(SX1278_REG_PAYLOAD_LENGTH, packetLength);
  setRegValue(SX1278_REG_FIFO_TX_BASE_ADDR, SX1278_FIFO_TX_BASE_ADDR_MAX);
  setRegValue(SX1278_REG_FIFO_ADDR_PTR, SX1278_FIFO_TX_BASE_ADDR_MAX);
  
  writeRegisterBurst(SX1278_REG_FIFO, pack->source, 8);
  writeRegisterBurst(SX1278_REG_FIFO, pack->destination, 8);
  writeRegisterBurstStr(SX1278_REG_FIFO, pack->data, packetLength - 16);
  
  setMode(SX1278_TX);
  while(!digitalRead(_dio0));
  clearIRQFlags();
  
  return 0;
}

packet* LoRa::rx(uint8_t mode, uint8_t packetLength) {
  setMode(SX1278_STANDBY);
  if(mode == SX1278_RXSINGLE) {
    setRegValue(SX1278_REG_PA_DAC, SX1278_PA_BOOST_OFF);
    setRegValue(SX1278_REG_HOP_PERIOD, SX1278_HOP_PERIOD_MAX);
    setRegValue(SX1278_REG_DIO_MAPPING_1, SX1278_DIO0_RX_DONE | SX1278_DIO1_RX_TIMEOUT, 7, 5);
    clearIRQFlags();
    setRegValue(SX1278_REG_IRQ_FLAGS_MASK, (SX1278_MASK_IRQ_FLAG_PAYLOAD_CRC_ERROR & SX1278_MASK_IRQ_FLAG_VALID_HEADER));
    
    setRegValue(SX1278_REG_FIFO_RX_BASE_ADDR, SX1278_FIFO_RX_BASE_ADDR_MAX);
    setRegValue(SX1278_REG_FIFO_ADDR_PTR, SX1278_FIFO_RX_BASE_ADDR_MAX);
    
    setMode(SX1278_RXSINGLE);
    //wait for RX done on DIO0
    while(!digitalRead(_dio0)) {
      //timeout on DIO1
      if(digitalRead(_dio1)) {
        return nullptr;
      }
    }
    
    if(readRegister(SX1278_REG_IRQ_FLAGS)) {
      return nullptr;
    }
    
    uint8_t sf = getRegValue(SX1278_REG_MODEM_CONFIG_2, 7, 4) & 0b00001111;
    if(sf != SX1278_SF_6) {
      packetLength = getRegValue(SX1278_REG_RX_NB_BYTES);
    }
    
    packet* pack = new packet;
    const char* data = new char[packetLength - 16];
    
    for(uint8_t i = 0; i < 8; i++) {
      pack->source[i] = readRegister(SX1278_REG_FIFO);
    }
    for(uint8_t i = 0; i < 8; i++) {
      pack->destination[i] = readRegister(SX1278_REG_FIFO);
    }
    data = readRegisterBurstStr(SX1278_REG_FIFO, packetLength - 16);
    pack->data = data;
    
    return(pack);
  } else if (mode == SX1278_RXCONTINUOUS) {
    //TODO: implement RXCONTINUOUS MODE
  }
  
  return nullptr;
}

void LoRa::setMode(uint8_t mode) {
  setRegValue(SX1278_REG_OP_MODE, mode, 2, 0);
}

void LoRa::setPacketSource(packet* pack, uint8_t* address) {
  for(uint8_t i = 0; i < 8; i++) {
    pack->source[i] = address[i];
  }
}

void LoRa::setPacketSourceStr(packet* pack, const char* address) {
  for(uint8_t i = 0; i < 8; i++) {
    pack->source[i] = (parseByte(address[3*i]) << 4) | parseByte(address[3*i + 1]);
  }
}

uint8_t* LoRa::getPacketSource(packet* pack) {
  uint8_t* src = new uint8_t[24];
  for(uint8_t i = 0; i < 8; i++) {
    src[i] = pack->source[i];
  }
  return(src);
}

const char* LoRa::getPacketSourceStr(packet* pack) {
  char* src = new char[24];
  for(uint8_t i = 0; i < 8; i++) {
    src[3*i] = reparseChar(pack->source[i] >> 4);
    src[3*i+1] = reparseChar(pack->source[i] & 0x0F);
    src[3*i+2] = ':';
  }
  src[23] = '\0';
  return(src);
}

void LoRa::setPacketDestination(packet* pack, uint8_t* address) {
  for(uint8_t i = 0; i < 8; i++) {
    pack->destination[i] = address[i];
  }
}

void LoRa::setPacketDestinationStr(packet* pack, const char* address) {
  for(uint8_t i = 0; i < 8; i++) {
    pack->destination[i] = (parseByte(address[3*i]) << 4) | parseByte(address[3*i + 1]);
  }
}

uint8_t* LoRa::getPacketDestination(packet* pack) {
  uint8_t* dest = new uint8_t[24];
  for(uint8_t i = 0; i < 8; i++) {
    dest[i] = pack->destination[i];
  }
  return(dest);
}

const char* LoRa::getPacketDestinationStr(packet* pack) {
  char* dest = new char[24];
  for(uint8_t i = 0; i < 8; i++) {
    dest[3*i] = reparseChar(pack->destination[i] >> 4);
    dest[3*i+1] = reparseChar(pack->destination[i] & 0x0F);
    dest[3*i+2] = ':';
  }
  dest[23] = '\0';
  return(dest);
}

void LoRa::setPacketData(packet* pack, const char* data) {
  pack->data = data;
}

const char* LoRa::getPacketData(packet* pack) {
  return(pack->data);
}

void LoRa::config(uint8_t bw, uint8_t cr, uint8_t sf) {
  setMode(SX1278_SLEEP);
  
  setRegValue(SX1278_REG_OP_MODE, SX1278_LORA, 7, 7);
  
  setRegValue(SX1278_REG_FRF_MSB, SX1278_FRF_MSB);
  setRegValue(SX1278_REG_FRF_MID, SX1278_FRF_MID);
  setRegValue(SX1278_REG_FRF_LSB, SX1278_FRF_LSB);
  
  setRegValue(SX1278_REG_PA_CONFIG, SX1278_PA_SELECT_BOOST | SX1278_MAX_POWER | SX1278_OUTPUT_POWER);
  setRegValue(SX1278_REG_OCP, SX1278_OCP_ON | SX1278_OCP_TRIM, 5, 0);
  setRegValue(SX1278_REG_LNA, SX1278_LNA_GAIN_1 | SX1278_LNA_BOOST_HF_ON);
  
  if(sf == SX1278_SF_6) {
    setRegValue(SX1278_REG_MODEM_CONFIG_2, SX1278_SF_6 | SX1278_TX_MODE_SINGLE | SX1278_RX_CRC_MODE_ON | SX1278_RX_TIMEOUT_MSB);
    setRegValue(SX1278_REG_MODEM_CONFIG_1, bw | cr | SX1278_HEADER_IMPL_MODE);
    setRegValue(SX1278_REG_DETECT_OPTIMIZE, SX1278_DETECT_OPTIMIZE_SF_6, 2, 0);
    setRegValue(SX1278_REG_DETECTION_THRESHOLD, SX1278_DETECTION_THRESHOLD_SF_6);
  } else {
    setRegValue(SX1278_REG_MODEM_CONFIG_2, sf | SX1278_TX_MODE_SINGLE | SX1278_RX_CRC_MODE_ON | SX1278_RX_TIMEOUT_MSB);
    setRegValue(SX1278_REG_MODEM_CONFIG_1, bw | cr | SX1278_HEADER_EXPL_MODE);
    setRegValue(SX1278_REG_DETECT_OPTIMIZE, SX1278_DETECT_OPTIMIZE_SF_7_12, 2, 0);
    setRegValue(SX1278_REG_DETECTION_THRESHOLD, SX1278_DETECTION_THRESHOLD_SF_7_12);
  }
  
  setRegValue(SX1278_REG_SYMB_TIMEOUT_LSB, SX1278_RX_TIMEOUT_LSB);
  setRegValue(SX1278_REG_PREAMBLE_MSB, SX1278_PREAMBLE_LENGTH_MSB);
  setRegValue(SX1278_REG_PREAMBLE_LSB, SX1278_PREAMBLE_LENGTH_LSB);
  
  setMode(SX1278_STANDBY);
}

uint8_t LoRa::setRegValue(uint8_t reg, uint8_t value, uint8_t msb, uint8_t lsb) {
  if((msb > 7) || (lsb > 7)) {
    return 0xFF;
  }
  uint8_t currentValue = readRegister(reg);
  uint8_t newValue = currentValue & ((0b11111111 << (msb + 1)) | (0b11111111 >> (8 - lsb)));
  writeRegister(reg, newValue | value);
}

uint8_t LoRa::getRegValue(uint8_t reg, uint8_t msb, uint8_t lsb) {
  if((msb > 7) || (lsb > 7)) {
    return 0xFF;
  }
  uint8_t rawValue = readRegister(reg);
  uint8_t maskedValue = rawValue & ((0b11111111 << lsb) | (0b11111111 >> (7 - msb)));
  return(maskedValue);
}

void LoRa::clearIRQFlags(void) {
  writeRegister(SX1278_REG_IRQ_FLAGS, 0b11111111);
}

uint8_t* LoRa::readRegisterBurst(uint8_t reg, uint8_t numBytes) {
  uint8_t* inBytes = new uint8_t[numBytes];
  digitalWrite(_nss, LOW);
  SPI.transfer(reg | SX1278_READ);
  for(uint8_t i = 0; i< numBytes; i++) {
    inBytes[i] = SPI.transfer(reg);
  }
  digitalWrite(_nss, HIGH);
  return(inBytes);
}

const char* LoRa::readRegisterBurstStr(uint8_t reg, uint8_t numBytes) {
  char* inBytes = new char[numBytes];
  digitalWrite(_nss, LOW);
  SPI.transfer(reg | SX1278_READ);
  for(uint8_t i = 0; i< numBytes; i++) {
    inBytes[i] = SPI.transfer(reg);
  }
  digitalWrite(_nss, HIGH);
  return(inBytes);
}

void LoRa::writeRegisterBurst(uint8_t reg, uint8_t* data, uint8_t numBytes) {
  digitalWrite(_nss, LOW);
  SPI.transfer(reg | SX1278_WRITE);
  for(uint8_t i = 0; i< numBytes; i++) {
    SPI.transfer(data[i]);
  }
  digitalWrite(_nss, HIGH);
}

void LoRa::writeRegisterBurstStr(uint8_t reg, const char* data, uint8_t numBytes) {
  digitalWrite(_nss, LOW);
  SPI.transfer(reg | SX1278_WRITE);
  for(uint8_t i = 0; i< numBytes; i++) {
    SPI.transfer(data[i]);
  }
  digitalWrite(_nss, HIGH);
}

uint8_t LoRa::readRegister(uint8_t reg) {
  uint8_t inByte;
  digitalWrite(_nss, LOW);
  SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
  SPI.transfer(reg | SX1278_READ);
  SPI.endTransaction();
  inByte = SPI.transfer(0x00);
  digitalWrite(_nss, HIGH);
  return(inByte);
}

void LoRa::writeRegister(uint8_t reg, uint8_t data) {
  digitalWrite(_nss, LOW);
  SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
  SPI.transfer(reg | SX1278_WRITE);
  SPI.transfer(data);
  SPI.endTransaction();
  digitalWrite(_nss, HIGH);
}

void LoRa::generateLoRaAdress(void) {
  for(uint8_t i = 0; i < 8; i++) {
    EEPROM.write(i, (uint8_t)random(0, 256));
  }
}

uint8_t LoRa::getPacketLength(packet* pack) {
  for(uint8_t i = 0; i < 240; i++) {
    if(pack->data[i] == '\0') {
      return(i + 17);
    }
  }
}

uint8_t LoRa::parseByte(char c) {
  if((c >= 48) && (c <= 57)) {
    return(c - 48);
  } else if((c >= 65) && (c <= 70)) {
    return(c - 55);
  } else if((c >= 97) && (c <= 102)) {
    return(c - 87);
  }
  return 0;
}

char LoRa::reparseChar(uint8_t b) {
  if((b >= 0) && (b <= 9)) {
    return(b + 48);
  } else if((b >= 10) && (b <= 16)) {
    return(b + 55);
  }
  return 0;
}