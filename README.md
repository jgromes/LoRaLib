# End-of-Life Notice: LoRaLib is now unsupported, use [RadioLib](https://github.com/jgromes/RadioLib) instead!

### Full explanation and rationale can be found [here](https://github.com/jgromes/LoRaLib/blob/master/EOL.md).

---

# LoRaLib [![Build Status](https://travis-ci.org/jgromes/LoRaLib.svg?branch=master)](https://travis-ci.org/jgromes/LoRaLib)

### Arduino library for LoRa modules based on LoRa chips by Semtech (SX1272, SX1276, SX1278) and HopeRF (RFM95, RFM96, RFM98)

### See the [Wiki](https://github.com/jgromes/LoRaLib/wiki) for further information. See the [GitHub Pages](https://jgromes.github.io/LoRaLib) for detailed and up-to-date API reference.

### This is not the shield you are looking for!
If you're looking for an open-source shield to use with this library, it has its own repository: [https://github.com/jgromes/LoRenz](https://github.com/jgromes/LoRenz)

---

**DISCLAIMER: This library is provided 'AS IS'. See `license.txt` for details.**

This library enables easy long range communication using the SX127x family of LoRa modules. It was designed to be used with LoRenz Rev.B shields. However, they are not required and this library can be used with any LoRa module, as long as it is based on of the supported LoRa chips.

Currently supported chips:
* SX1272 and SX1273
* SX1278, SX1276, SX1277 and SX1279
* RFM95, RFM96, RFM97 and RFM98

Currently supported platforms:
* All Arduino AVR boards (tested on Uno and Mega)
* ESP32
* ESP8266 NodeMCU
