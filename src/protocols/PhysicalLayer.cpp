#include "PhysicalLayer.h"

PhysicalLayer::PhysicalLayer(float crysFreq, uint8_t divExp, size_t maxPacketLength) {
  _crystalFreq = crysFreq;
  _divExponent = divExp;
  _maxPacketLength = maxPacketLength;
}

int16_t PhysicalLayer::transmit(__FlashStringHelper* fstr, uint8_t addr) {
  // read flash string length
  size_t len = 0;
  PGM_P p = reinterpret_cast<PGM_P>(fstr);
  while(true) {
    char c = pgm_read_byte(p++);
    len++;
    if(c == '\0') {
      break;
    }
  }

  // dynamically allocate memory
  char* str = new char[len];

  // copy string from flash
  p = reinterpret_cast<PGM_P>(fstr);
  for(size_t i = 0; i < len; i++) {
    str[i] = pgm_read_byte(p + i);
  }

  // transmit string
  int16_t state = transmit(str, addr);
  delete[] str;
  return(state);
}

int16_t PhysicalLayer::transmit(String& str, uint8_t addr) {
  return(transmit(str.c_str(), addr));
}

int16_t PhysicalLayer::transmit(const char* str, uint8_t addr) {
  return(transmit((uint8_t*)str, strlen(str), addr));
}

int16_t PhysicalLayer::startTransmit(String& str, uint8_t addr) {
  return(startTransmit(str.c_str(), addr));
}

int16_t PhysicalLayer::startTransmit(const char* str, uint8_t addr) {
  return(startTransmit((uint8_t*)str, strlen(str), addr));
}

int16_t PhysicalLayer::readData(String& str, size_t len) {
  int16_t state = ERR_NONE;

  if(len == 0) {
    // query packet length
    len = getPacketLength();
  }

  // build a temporary buffer
  uint8_t* data = new uint8_t[len + 1];
  if(!data) {
    return(ERR_MEMORY_ALLOCATION_FAILED);
  }

  // read the received data
  state = readData(data, len);

  // add null terminator
  data[len] = 0;

  // initialize Arduino String class
  str = String((char*)data);

  // deallocate temporary buffer
  delete[] data;

  return(state);
}

int16_t PhysicalLayer::receive(String& str, size_t len) {
  int16_t state = ERR_NONE;

  if(len == 0) {
    // unknown packet length, set to maximum
    len = _maxPacketLength;
  }

  // build a temporary buffer
  uint8_t* data = new uint8_t[len + 1];
  if(!data) {
    return(ERR_MEMORY_ALLOCATION_FAILED);
  }

  // read the received data
  state = receive(data, len);

  if(state == ERR_NONE) {
    // read the number of actually received bytes
    len = getPacketLength();

    // add null terminator
    data[len] = 0;

    // initialize Arduino String class
    str = String((char*)data);
  }

  // deallocate temporary buffer
  delete[] data;

  return(state);
}

float PhysicalLayer::getCrystalFreq() {
  return(_crystalFreq);
}

uint8_t PhysicalLayer::getDivExponent() {
  return(_divExponent);
}
