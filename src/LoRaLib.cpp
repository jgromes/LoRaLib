#include "LoRaLib.h"

packet::packet(void) {
  // get own node address
  uint8_t src[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  getLoRaAddress(src);
  
  // leave destination and data empty
  uint8_t dest[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  char data[240] = {'\0'};
  
  // common initializer
  init(src, dest, data);
}

packet::packet(const char dest[24], const char dat[240]) {
  // get own node address
  uint8_t src[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  getLoRaAddress(src);
  
  // parse destination string into byte array
  uint8_t destTmp[8];
  for(uint8_t i = 0; i < 8; i++) {
    destTmp[i] = (parseByte(dest[3*i]) << 4) | parseByte(dest[3*i + 1]);
  }
  
  // common initializer
  init(src, destTmp, dat);
}

packet::packet(uint8_t dest[8], const char dat[240]) {
  // get own node address
  uint8_t src[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  getLoRaAddress(src);
  
  // common initializer
  init(src, dest, dat);
}

packet::packet(const char src[24], const char dest[24], const char dat[240]) {
  // parse source string into byte array
  uint8_t srcTmp[8];
  for(uint8_t i = 0; i < 8; i++) {
    srcTmp[i] = (parseByte(src[3*i]) << 4) | parseByte(src[3*i + 1]);
  }
  
  // parse destination string into byte array
  uint8_t destTmp[8];
  for(uint8_t i = 0; i < 8; i++) {
    destTmp[i] = (parseByte(dest[3*i]) << 4) | parseByte(dest[3*i + 1]);
  }
  
  // common initializer
  init(srcTmp, destTmp, dat);
}

packet::packet(uint8_t src[8], uint8_t dest[8], const char dat[240]) {
  // common initializer
  init(src, dest, dat);
}

void packet::init(uint8_t src[8], uint8_t dest[8], const char dat[240]) {
  //copy source & destination
  for(uint8_t i = 0; i < 8; i++) {
    source[i] = src[i];
    destination[i] = dest[i];
  }
  
  //copy data, get & save length
  length = 0;
  for(uint8_t i = 0; i < 240; i++) {
    data[i] = dat[i];
    if(data[i] == '\0') {
      length = i + 17;
      break;
    }
  }
}

void packet::getSourceStr(char src[24]) {
  for(uint8_t i = 0; i < 8; i++) {
    src[3*i] = reparseChar(source[i] >> 4);
    src[3*i+1] = reparseChar(source[i] & 0x0F);
    src[3*i+2] = ':';
  }
  src[23] = '\0';
}

void packet::getDestinationStr(char dest[24]) {
  for(uint8_t i = 0; i < 8; i++) {
    dest[3*i] = reparseChar(destination[i] >> 4);
    dest[3*i+1] = reparseChar(destination[i] & 0x0F);
    dest[3*i+2] = ':';
  }
  dest[23] = '\0';
}

void packet::setSourceStr(const char src[24]) {
  // parse source string into byte array
  for(uint8_t i = 0; i < 8; i++) {
    source[i] = (parseByte(src[3*i]) << 4) | parseByte(src[3*i + 1]);
  }
}

void packet::setDestinationStr(const char dest[24]) {
  // parse destination string into byte array
  for(uint8_t i = 0; i < 8; i++) {
    destination[i] = (parseByte(dest[3*i]) << 4) | parseByte(dest[3*i + 1]);
  }
}

void packet::getLoRaAddress(uint8_t addr[8]) {
  for(uint8_t i = 0; i < 8; i++) {
    addr[i] = EEPROM.read(i);
  }
}

uint8_t packet::parseByte(char c) {
  if((c >= 48) && (c <= 57)) {
    return(c - 48);
  } else if((c >= 65) && (c <= 70)) {
    return(c - 55);
  } else if((c >= 97) && (c <= 102)) {
    return(c - 87);
  }
  return(0);
}

char packet::reparseChar(uint8_t b) {
  if(b <= 9) {
    return(b + 48);
  } else if((b >= 10) && (b <= 16)) {
    return(b + 55);
  }
  return(0);
}

LoRa::LoRa(int nss, uint8_t bw, uint8_t cr, uint8_t sf) {
  // save user settings into global variables
  _nss = nss;
  _bw = bw;
  _cr = cr;
  _sf = sf;
  
  // set Arduino pin modes
  pinMode(_nss, OUTPUT);
  pinMode(_sck, OUTPUT);
  pinMode(_miso, INPUT);
  pinMode(_mosi, OUTPUT);
  pinMode(_dio0, INPUT);
  pinMode(_dio1, INPUT);
  
  // start SPI
  SPI.begin();
}

uint8_t LoRa::init(uint16_t addrEeprom, uint16_t addrFlag) {
  #ifdef DEBUG
    Serial.begin(9600);
    Serial.println();
  #endif
  
  _addrEeprom = addrEeprom;
  _addrFlag = addrFlag;
  
  // check if this node has an address
  if(EEPROM.read(_addrEeprom) == 255) {
    // if not, generate new random address
    randomSeed(analogRead(5));
    generateLoRaAdress();
    delay(100);
  }
  
  #ifdef DEBUG
    Serial.print("LoRa node address string: ");
  #endif
  // read node address from Arduino EEPROM
  for(uint8_t i = 0; i < 8; i++) {
    _address[i] = EEPROM.read(i);
    #ifdef DEBUG
      Serial.print(_address[i], HEX);
      if(i < 7) {
        Serial.print(":");
      } else {
        Serial.println();
      }
    #endif
    delay(100);
  }
  
  uint8_t i = 0;
  bool flagFound = false;
  // try to find SX1278
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
  
  // SX1278 not found in 10 tries, terminate
  if(!flagFound) {
    #ifdef DEBUG
      Serial.println("No SX1278 found!");
    #endif
    SPI.end();
    while(true);
  }
  #ifdef DEBUG
    else {
      Serial.println("Found SX1278! (match by SX1278_REG_VERSION == 0x12)");
    }
  #endif
  
  // configure SX1278
  uint8_t status = config(_bw, _cr, _sf);
  if(status != 0) {
    return(status);
  }
  
  return(0x00);
}

uint8_t LoRa::tx(packet& pack, uint16_t timeout) {
  // set mode to STANDBY
  setMode(SX1278_STANDBY);
  
  // tx init
  setRegValue(SX1278_REG_DIO_MAPPING_1, SX1278_DIO0_TX_DONE, 7, 6);
  clearIRQFlags();
  
  // check packet length
  if(pack.length > 256) {
    return(0x02);
  }

  // write packet info and setup FIFO
  setRegValue(SX1278_REG_PAYLOAD_LENGTH, pack.length);
  setRegValue(SX1278_REG_FIFO_TX_BASE_ADDR, SX1278_FIFO_TX_BASE_ADDR_MAX);
  setRegValue(SX1278_REG_FIFO_ADDR_PTR, SX1278_FIFO_TX_BASE_ADDR_MAX);
  
  // write packet to FIFO
  writeRegisterBurst(SX1278_REG_FIFO, pack.source, 8);
  writeRegisterBurst(SX1278_REG_FIFO, pack.destination, 8);
  writeRegisterBurstStr(SX1278_REG_FIFO, pack.data, pack.length - 16);
  
  // start transmission
  setMode(SX1278_TX);
  
  unsigned long start = millis();
  
  // wait for TxDone flag
  while(!digitalRead(_dio0)) {
    #ifdef DEBUG
      Serial.print('.');
    #endif
    // check the timeout
    if((timeout != 0) && ((millis() - start) > timeout)) {
      clearIRQFlags();
      return(0x01);
    }
  }
  
  // clear all flags
  clearIRQFlags();
  
  return(0x00);
}

uint8_t LoRa::rx(packet& pack, uint16_t timeout, uint8_t mode) {
  // set mode to STANDBY
  setMode(SX1278_STANDBY);
  
  //TODO: implement RX timeout as an argument in ms
  
  if(timeout > 1023) {
    return(0x03);
  }
  
  // set rx timeout length
  if(timeout != 0) {
    setRegValue(SX1278_REG_MODEM_CONFIG_2, (uint8_t)(timeout >> 8), 1, 0);
    setRegValue(SX1278_REG_SYMB_TIMEOUT_LSB, (uint8_t)timeout);
  }
  
  if(mode == SX1278_RXSINGLE) {
    // rx init
    setRegValue(SX1278_REG_DIO_MAPPING_1, SX1278_DIO0_RX_DONE | SX1278_DIO1_RX_TIMEOUT, 7, 4);
    clearIRQFlags();
    
    // initialize FIFO
    setRegValue(SX1278_REG_FIFO_RX_BASE_ADDR, SX1278_FIFO_RX_BASE_ADDR_MAX);
    setRegValue(SX1278_REG_FIFO_ADDR_PTR, SX1278_FIFO_RX_BASE_ADDR_MAX);
    
    // receive a single packet
    setMode(SX1278_RXSINGLE);
    
    // wait for RX done on DIO0
    while(!digitalRead(_dio0)) {
      // check timeout on DIO1
      if(digitalRead(_dio1)) {
        clearIRQFlags();
        return(0x01);
      }
    }
    
    // check PayloadCrcError flag
    if(getRegValue(SX1278_REG_IRQ_FLAGS, 5, 5) == SX1278_CLEAR_IRQ_FLAG_PAYLOAD_CRC_ERROR) {
      return(0x02);
    }
    
    // in explicit header mode, we can get the number of received bytes
    uint8_t headerMode = getRegValue(SX1278_REG_MODEM_CONFIG_1, 0, 0);
    if(headerMode == SX1278_HEADER_EXPL_MODE) {
      pack.length = getRegValue(SX1278_REG_RX_NB_BYTES);
    }
    
    // read packet source
    for(uint8_t i = 0; i < 8; i++) {
      pack.source[i] = readRegister(SX1278_REG_FIFO);
    }
    
    // read packet destination
    for(uint8_t i = 0; i < 8; i++) {
      pack.destination[i] = readRegister(SX1278_REG_FIFO);
    }
    
    // read packet data
    readRegisterBurstStr(SX1278_REG_FIFO, pack.length - 16, pack.data);
    
    // clear all flags
    clearIRQFlags();
    
    return(0x00);
    
  } else if (mode == SX1278_RXCONTINUOUS) {
    // TODO: implement RXCONTINUOUS MODE
  }
  
  return(0x03);
}

uint8_t LoRa::setMode(uint8_t mode) {
  setRegValue(SX1278_REG_OP_MODE, mode, 2, 0);
  /*if(getRegValue(SX1278_REG_OP_MODE, 2, 0) == mode) {
    return(0x00);
  }*/
  return(0x00);
}

//TODO: add user power configuration
uint8_t LoRa::config(uint8_t bw, uint8_t cr, uint8_t sf) {
  uint8_t status = 0x01;
  
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
       return(0x10);
  }
  
  if((cr != SX1278_CR_4_5) &&
     (cr != SX1278_CR_4_6) &&
     (cr != SX1278_CR_4_7) &&
     (cr != SX1278_CR_4_8)) {
       return(0x11);
  }
  
  if((sf != SX1278_SF_6) &&
     (sf != SX1278_SF_7) &&
     (sf != SX1278_SF_8) &&
     (sf != SX1278_SF_9) &&
     (sf != SX1278_SF_10) &&
     (sf != SX1278_SF_11) &&
     (sf != SX1278_SF_12)) {
       return(0x12);
  }
  
  // set mode to SLEEP
  status = setMode(SX1278_SLEEP);
  if(status != 0x00) {
    return(status);
  }
  
  // set LoRa mode
  status = setRegValue(SX1278_REG_OP_MODE, SX1278_LORA, 7, 7);
  if(status != 0x00) {
    return(status);
  }
  
  // set carrier frequency
  status = setRegValue(SX1278_REG_FRF_MSB, SX1278_FRF_MSB);
  status = setRegValue(SX1278_REG_FRF_MID, SX1278_FRF_MID);
  status = setRegValue(SX1278_REG_FRF_LSB, SX1278_FRF_LSB);
  if(status != 0x00) {
    return(status);
  }
  
  // output power configuration
  status = setRegValue(SX1278_REG_PA_CONFIG, SX1278_PA_SELECT_BOOST | SX1278_MAX_POWER | SX1278_OUTPUT_POWER);
  status = setRegValue(SX1278_REG_OCP, SX1278_OCP_ON | SX1278_OCP_TRIM, 5, 0);
  status = setRegValue(SX1278_REG_LNA, SX1278_LNA_GAIN_1 | SX1278_LNA_BOOST_HF_ON);
  status = setRegValue(SX1278_REG_PA_DAC, SX1278_PA_BOOST_ON, 2, 0);
  if(status != 0x00) {
    return(status);
  }
  
  // turn off frequency hopping
  status = setRegValue(SX1278_REG_HOP_PERIOD, SX1278_HOP_PERIOD_OFF);
  if(status != 0x00) {
    return(status);
  }
  
  // basic setting (bw, cr, sf, header mode and CRC)
  if(sf == SX1278_SF_6) {
    status = setRegValue(SX1278_REG_MODEM_CONFIG_2, SX1278_SF_6 | SX1278_TX_MODE_SINGLE | SX1278_RX_CRC_MODE_ON, 7, 2);
    status = setRegValue(SX1278_REG_MODEM_CONFIG_1, bw | cr | SX1278_HEADER_IMPL_MODE);
    status = setRegValue(SX1278_REG_DETECT_OPTIMIZE, SX1278_DETECT_OPTIMIZE_SF_6, 2, 0);
    status = setRegValue(SX1278_REG_DETECTION_THRESHOLD, SX1278_DETECTION_THRESHOLD_SF_6);
  } else {
    status = setRegValue(SX1278_REG_MODEM_CONFIG_2, sf | SX1278_TX_MODE_SINGLE | SX1278_RX_CRC_MODE_ON, 7, 2);
    status = setRegValue(SX1278_REG_MODEM_CONFIG_1, bw | cr | SX1278_HEADER_EXPL_MODE);
    status = setRegValue(SX1278_REG_DETECT_OPTIMIZE, SX1278_DETECT_OPTIMIZE_SF_7_12, 2, 0);
    status = setRegValue(SX1278_REG_DETECTION_THRESHOLD, SX1278_DETECTION_THRESHOLD_SF_7_12);
  }
  
  if(status != 0x00) {
    return(status);
  }
  
  // set default preamble length
  status = setRegValue(SX1278_REG_PREAMBLE_MSB, SX1278_PREAMBLE_LENGTH_MSB);
  status = setRegValue(SX1278_REG_PREAMBLE_LSB, SX1278_PREAMBLE_LENGTH_LSB);
  if(status != 0x00) {
    return(status);
  }
  
  // set mode to STANDBY
  status = setMode(SX1278_STANDBY);
  if(status != 0x00) {
    return(status);
  }
  
  return(0x00);
}

void LoRa::getLoRaAddress(uint8_t addr[8]) {
  for(uint8_t i = 0; i < 8; i++) {
    addr[i] = EEPROM.read(i);
  }
}

uint8_t LoRa::getBW(void) {
  return(getRegValue(SX1278_REG_MODEM_CONFIG_1, 7, 4));
}

uint8_t LoRa::getCR(void) {
  return(getRegValue(SX1278_REG_MODEM_CONFIG_1, 3, 1));
}

uint8_t LoRa::getSF(void) {
  return(getRegValue(SX1278_REG_MODEM_CONFIG_2, 7, 4));
}

uint8_t LoRa::setRegValue(uint8_t reg, uint8_t value, uint8_t msb, uint8_t lsb) {
  if((msb > 7) || (lsb > 7) || (lsb > msb)) {
    return(0xFF);
  }
  
  uint8_t currentValue = readRegister(reg);
  uint8_t newValue = currentValue & ((0b11111111 << (msb + 1)) & (0b11111111 >> (8 - lsb)));
  writeRegister(reg, newValue | value);
  /*if(readRegister(reg) != newValue | value) {
    return(0xFE);
  }*/
  return(0x00);
}

uint8_t LoRa::getRegValue(uint8_t reg, uint8_t msb, uint8_t lsb) {
  if((msb > 7) || (lsb > 7) || (lsb > msb)) {
    return(0xFF);
  }
  
  uint8_t rawValue = readRegister(reg);
  uint8_t maskedValue = rawValue & ((0b11111111 << lsb) & (0b11111111 >> (7 - msb)));
  return(maskedValue);
}

void LoRa::clearIRQFlags(void) {
  writeRegister(SX1278_REG_IRQ_FLAGS, 0b11111111);
}

uint8_t LoRa::readRegisterBurst(uint8_t reg, uint8_t numBytes, uint8_t* inBytes) {
  digitalWrite(_nss, LOW);
  SPI.transfer(reg | SX1278_READ);
  for(uint8_t i = 0; i< numBytes; i++) {
    inBytes[i] = SPI.transfer(reg);
  }
  digitalWrite(_nss, HIGH);
  return(0x00);
}

uint8_t LoRa::readRegisterBurstStr(uint8_t reg, uint8_t numBytes, char* inBytes) {
  digitalWrite(_nss, LOW);
  SPI.transfer(reg | SX1278_READ);
  for(uint8_t i = 0; i< numBytes; i++) {
    inBytes[i] = SPI.transfer(reg);
  }
  digitalWrite(_nss, HIGH);
  return(0x00);
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
  for(uint8_t i = _addrEeprom; i < (_addrEeprom + 8); i++) {
    EEPROM.write(i, (uint8_t)random(0, 256));
  }
  EEPROM.write(_addrFlag, 0);
}

#ifdef VERBOSE
  void LoRa::regDump(void) {
    Serial.println("Register dump:");
    char buffHex[5];
    char buffBin[10];
    for(uint8_t adr = 0x01; adr <= 0x70; adr++) {
      sprintf(buffHex, "0x%02X", adr);
      Serial.print(buffHex);
      Serial.print('\t');
      
      uint8_t inByte = readRegister(adr);
      sprintf(buffHex, "0x%02X", inByte);
      Serial.print(buffHex);
      Serial.print('\t');
      
      sprintf(buffBin, "%c%c%c%c %c%c%c%c", BYTE_TO_BINARY(inByte));
      Serial.println(buffBin);
    }
    Serial.println("Done.");
  }
#endif

#ifdef DEBUG
  int LoRa::freeRAM(void) {
    extern int __heap_start, *__brkval; 
    int v; 
    return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
  }
  
  void LoRa::printStatus(void) {
    // modulation
    Serial.print("Modulation:\t\t");
    switch(getRegValue(SX1278_REG_OP_MODE, 7, 7)) {
      case SX1278_LORA:
        Serial.println("LoRa");
        break;
      case SX1278_FSK_OOK:
        Serial.println("FSK/OOK");
        break;
      default:
        Serial.println("unknown");
    }
    
    // op mode
    Serial.print("Mode:\t\t\t");
    switch(getRegValue(SX1278_REG_OP_MODE, 2, 0)) {
      case SX1278_SLEEP:
        Serial.println("SX1278_SLEEP");
        break;
      case SX1278_STANDBY:
        Serial.println("SX1278_STANDBY");
        break;
      case SX1278_FSTX:
        Serial.println("SX1278_FSTX");
        break;
      case SX1278_TX:
        Serial.println("SX1278_TX");
        break;
      case SX1278_FSRX:
        Serial.println("SX1278_FSRX");
        break;
      case SX1278_RXCONTINUOUS:
        Serial.println("SX1278_RXCONTINUOUS");
        break;
      case SX1278_RXSINGLE:
        Serial.println("SX1278_RXSINGLE");
        break;
      case SX1278_CAD:
        Serial.println("SX1278_CAD");
        break;
      default:
        Serial.println("unknown");
    }
    
    // bandwidth
    Serial.print("Bandwidth:\t\t");
    switch(getRegValue(SX1278_REG_MODEM_CONFIG_1, 7, 4)) {
      case SX1278_BW_7_80_KHZ:
        Serial.println("7.80 kHz");
        break;
      case SX1278_BW_10_40_KHZ:
        Serial.println("10.40 kHz");
        break;
      case SX1278_BW_15_60_KHZ:
        Serial.println("15.60 kHz");
        break;
      case SX1278_BW_20_80_KHZ:
        Serial.println("20.80 kHz");
        break;
      case SX1278_BW_31_25_KHZ:
        Serial.println("31.25 kHz");
        break;
      case SX1278_BW_41_70_KHZ:
        Serial.println("41.70 kHz");
        break;
      case SX1278_BW_62_50_KHZ:
        Serial.println("62.50 kHz");
        break;
      case SX1278_BW_125_00_KHZ:
        Serial.println("125.00 kHz");
        break;
      case SX1278_BW_250_00_KHZ:
        Serial.println("250.00 kHz");
        break;
      case SX1278_BW_500_00_KHZ:
        Serial.println("500.00 kHz");
        break;
      default:
        Serial.println("unknown");
    }
    
    // error coding rate
    Serial.print("Error Coding Rate:\t");
    switch(getRegValue(SX1278_REG_MODEM_CONFIG_1, 3, 1)) {
      case SX1278_CR_4_5:
        Serial.println("4/5");
        break;
      case SX1278_CR_4_6:
        Serial.println("4/6");
        break;
      case SX1278_CR_4_7:
        Serial.println("4/7");
        break;
      case SX1278_CR_4_8:
        Serial.println("4/8");
        break;
      default:
        Serial.println("unknown");
    }
    
    // spreading factor
    Serial.print("Spreading Factor:\t");
    switch(getRegValue(SX1278_REG_MODEM_CONFIG_2, 7, 4)) {
      case SX1278_SF_6:
        Serial.println("64");
        break;
      case SX1278_SF_7:
        Serial.println("128");
        break;
      case SX1278_SF_8:
        Serial.println("256");
        break;
      case SX1278_SF_9:
        Serial.println("512");
        break;
      case SX1278_SF_10:
        Serial.println("1024");
        break;
      case SX1278_SF_11:
        Serial.println("2048");
        break;
      case SX1278_SF_12:
        Serial.println("4096");
        break;
      default:
        Serial.println("unknown");
    }
    
    // header mode
    Serial.print("Header Mode:\t\t");
    switch(getRegValue(SX1278_REG_MODEM_CONFIG_1, 0, 0)) {
      case SX1278_HEADER_EXPL_MODE:
        Serial.println("explicit");
        break;
      case SX1278_HEADER_IMPL_MODE:
        Serial.println("implicit");
        break;
      default:
        Serial.println("unknown");
    }
    
    // crc
    Serial.print("RX CRC:\t\t\t");
    switch(getRegValue(SX1278_REG_MODEM_CONFIG_1, 0, 0)) {
      case SX1278_RX_CRC_MODE_OFF:
        Serial.println("disabled");
        break;
      case SX1278_RX_CRC_MODE_ON:
        Serial.println("enabled");
        break;
      default:
        Serial.println("unknown");
    }
  }
#endif
