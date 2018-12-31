#ifndef _LORALIB_H
#define _LORALIB_H

/*!
  \mainpage LoRaLib Arduino Library Documentation
  
  \section intro Introduction
  Arduino library for %LoRa modules based on %LoRa chips by Semtech (%SX1272, %SX1276, %SX1278) and HopeRF (%RFM95, %RFM96, %RFM98).
  
  \copyright  Copyright (C) 2019 Jan Gromes
*/

#include "TypeDef.h"
#include "Module.h"

#include "modules/SX1272.h"
#include "modules/SX1273.h"

#include "modules/SX1278.h"
#include "modules/SX1276.h"
#include "modules/SX1277.h"
#include "modules/SX1279.h"

#include "modules/RFM95.h"
#include "modules/RFM96.h"
#include "modules/RFM97.h"

/*!
  \class LoRa
  
  \brief Main library object to control the %LoRa module. This class only exists as an alias for the class Module in the standard constructor.
  Most of the library methods are implemented in class SX127x, which serves as base for all derived classes (SX1278, SX1272 etc.). 
  Some of the methods are also implemented/overridden in the derived classes.
*/
using LoRa = Module;

#endif
