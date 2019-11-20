#ifndef _LORALIB_H
#define _LORALIB_H

/*!
  \mainpage LoRaLib Documentation

  LoRaLib is Arduino library for %LoRa modules based on %LoRa chips by Semtech (%SX1272, %SX1276, %SX1278) and HopeRF (%RFM95, %RFM96, %RFM98).

  \par Quick Links
  Documentation for most common methods can be found in SX127x reference.\n
  Some methods (mainly configuration) are also overridden in derived classes, such as SX1272, SX1278, RFM96 etc.\n
  \ref status_codes have their own page.\n

  \see https://github.com/jgromes/LoRaLib

  \copyright  Copyright (c) 2019 Jan Gromes
*/

#include "TypeDef.h"
#include "Module.h"

#ifdef RADIOLIB_GODMODE
  #warning "God mode active, I hope it was intentional. Buckle up, lads."
#endif

#include "modules/SX127x/SX1272.h"
#include "modules/SX127x/SX1273.h"
#include "modules/SX127x/SX1278.h"
#include "modules/SX127x/SX1276.h"
#include "modules/SX127x/SX1277.h"
#include "modules/SX127x/SX1279.h"

#include "modules/RFM9x/RFM95.h"
#include "modules/RFM9x/RFM96.h"
#include "modules/RFM9x/RFM97.h"

/*!
  \class LoRa

  \brief Main library object to control the %LoRa module. This class only exists as an alias for the class Module in the standard constructor.
  Most of the library methods are implemented in class SX127x, which serves as base for all derived classes (SX1278, SX1272 etc.).
  Some of the methods are also implemented/overridden in the derived classes.
*/
using LoRa = Module;

#endif
