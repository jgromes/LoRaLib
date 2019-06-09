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
  int16_t state = 0;
  size_t length = 0;
  
  if(len == 0)
  {
    //We can query the packet length now because the packet should have been received earlier
    getPacketLength(length);
    //Build a temporary buffer
    data = new uint8_t[length + 1];
    // read the received data
    if(data)
    {
      state = (readData(data, length));
      if(state != ERR_NONE)
        return state;
    }
    else
    {
      return ERR_INVALID_MEMORYBUFFER;
    }
    //guards against dynamic allocation issue #51 and #30
    data[length] = 0;
    //initialize the string
    str = String((char*)data);
    //clean up
    delete[] data;
  }
  else
  {
    //if the string object is non-zero, let's treat it like a regular binary receive event
    //Build a temporary buffer
    data = new uint8_t[len];
    // read the received data
    if(data)
    {
      state = (readData(data, len));
      if(state != ERR_NONE)
        return state;
    }
    else
    {
      return ERR_INVALID_MEMORYBUFFER;
    }
    //Copy c-string
    for(size_t i = 0; i < len; i++)
    {
      str[i] = data[i];
    }
    //clean up
    delete[] data;
  }
  
  return(ERR_NONE);
}

int16_t PhysicalLayer::receive(String& str, size_t len) {
  // create temporary array to store received data
  uint8_t* data = nullptr;
  int16_t state = 0;
  size_t length = 0;
  
  if(len == 0)
  {
    //We can query the packet length now because the packet should have been received earlier
    getPacketLength(length);
    //Build a temporary buffer
    data = new uint8_t[length + 1];
    // read the received data
    if(data)
    {
      state = (receive(data, length));
      if(state != ERR_NONE)
        return state;
    }
    else
    {
      return ERR_INVALID_MEMORYBUFFER;
    }
    //guards against dynamic allocation issue #51 and #30
    data[length] = 0;
    //initialize the string
    str = String((char*)data);
    //clean up
    delete[] data;
  }
  else
  {
    //if the string object is non-zero, let's treat it like a regular binary receive event
    //Build a temporary buffer
    data = new uint8_t[len];
    // read the received data
    if(data)
    {
      state = (receive(data, len));
      if(state != ERR_NONE)
        return state;
    }
    else
    {
      return ERR_INVALID_MEMORYBUFFER;
    }
    //Copy c-string
    for(size_t i = 0; i < len; i++)
    {
      str[i] = data[i];
    }
    //clean up
    delete[] data;
  }
  
  return(ERR_NONE);
}

float PhysicalLayer::getCrystalFreq() {
  return(_crystalFreq);
}

uint8_t PhysicalLayer::getDivExponent() {
  return(_divExponent);
}
