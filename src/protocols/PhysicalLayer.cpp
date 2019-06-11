#include "PhysicalLayer.h"

PhysicalLayer::PhysicalLayer(float crysFreq, uint8_t divExp) {
  _crystalFreq = crysFreq;
  _divExponent = divExp;
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
  // create temporary array to store received data
  uint8_t* data = nullptr;
  int16_t state = ERR_NONE;
  
  if(len == 0)
  {
    //We can query the packet length now because the packet should have been received earlier
    getPacketLength(len);
  }  
  
    //Build a temporary buffer
    data = new uint8_t[len + 1];
    // read the received data
    if(data)
    {
      state = (readData(data, len));
    }
    else
    {
      state = ERR_INVALID_MEMORYBUFFER;
    }
    //guards against dynamic allocation issue #51 and #30
    data[len] = 0;
    //initialize the string
    str = String((char*)data);
    //clean up
    delete[] data;
  
  return(state);
}

int16_t PhysicalLayer::receive(String& str, size_t len) {
  // create temporary array to store received data
  uint8_t* data = nullptr;
  int16_t state = ERR_NONE;
  
  if(len == 0)
  {
    //We can query the packet length now because the packet should have been received earlier
    getPacketLength(len);
  }  
  
    //Build a temporary buffer
    data = new uint8_t[len + 1];
    // read the received data
    if(data)
    {
      state = (receive(data, len));
    }
    else
    {
      state = ERR_INVALID_MEMORYBUFFER;
    }
    //guards against dynamic allocation issue #51 and #30
    data[len] = 0;
    //initialize the string
    str = String((char*)data);
    //clean up
    delete[] data;
  
  return(state);
}

float PhysicalLayer::getCrystalFreq() {
  return(_crystalFreq);
}

uint8_t PhysicalLayer::getDivExponent() {
  return(_divExponent);
}
