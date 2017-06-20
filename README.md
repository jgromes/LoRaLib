# LoRaLib

## Arduino library for LoRa modules based on SX1278 chip

---

## Work in progress warning
This library is still early in developement. New core features might be added and the ones that are in might be changed at any moment! Watch this repo for progress updates. To get notifications about every commit, use the following link to set up an RSS feed reader: https://github.com/jgromes/LoRaLib/commits/master.atom.

---

### This is not the shield you are looking for!
If you're looking for the shield to use with this library, it has its own repository: [https://github.com/jgromes/LoRenz](https://github.com/jgromes/LoRenz)

---

DISCLAIMER: This library is provided 'AS IS'. See `license.txt` for details.

This library enables easy long range communaction with SX1278-based LoRa modules. It was designed to be used with LoRenz Rev.B shields. However, they are not required and this library can be used with any LoRa module, as long as it is based on the SX1278 chip.

## Library reference

The following is a list of currently implemented public member functions and variables, as well as their description. See `/examples/` for examples of their usage.

### LoRa class

#### Public member variables

Currently, there are no public member variables in the LoRa class.

#### Public member functions

* `LoRa(int nss, uint8_t bw, uint8_t cr, uint8_t sf)`

  Default constructor, used to create the LoRa object.
  
  * Single LoRa module connected:
  
    ```c++
    LoRa lora;
    ```
    
    This will create a `lora` instance of the `LoRa` class with default values of bandwidth, coding rate and spreading factor.
  
  * Multiple LoRa modules connected simultaneously:
    
    ```c++
    LoRa lora0(7);
    LoRa lora1(6);
    LoRa lora2(5);
    LoRa lora3(4);
    ```
    
    Multiple LoRenz Rev.B shields can be connected to a single Arduino, however, on each of them, a different position on the NSS pin header has to be shorted out. Since there are 4 positions on the NSS pin header, the maximum of simultaneously connected LoRenz Rev.B shields is 4. Any number of other LoRa modules can be connected and used with this library as long as their SPI NSS connection is not the same as the ones already used for LoRenz Rev.B shield(s).

* `uint8_t init(uint16_t addrEeprom, uint16_t addrFlag)` 

  This method has to be called inside Arduino `setup()` function before any calls to other methods from this library. Much like in other libraries, this method handles the initial setup of the SX1278 chip. This method also handles creating the node address and writing it into Arduino EEPROM.

* `uint8_t tx(packet& pack)`

  This function will attempt to transmit the `packet pack` in LoRa mode. For information on the packet class, second part of this reference. In case the supplied packet is larger than 256 bytes, the function will return 1. In case of a successful transmission, the function will return 0.

* `uint8_t rx(packet& pack, uint8_t mode)`

  This function will attempt to receive incoming data in LoRa mode. For information on the packet class, second part of this reference The default mode for this function is `RXSINGLE` mode: the function will be terminated after successful reception of a single packet, this is the recommended RX mode. In `RXCONTINUOUS` mode, the function will keep on going until terminated externally. This function will return 0 on sucessful packet reception, 1 if there was a timeout or 2 if the received packet is malformed.

* `void config(uint8_t bw, uint8_t cr, uint8_t sf);`

  This function will reconfigure the LoRa module to the supllied bandwidth, coding rate and spreading factor. For their values, see the header file.
  
  * `void getLoraAddress(uint8_t addr[8]);`

This function can be used to get address of the current node.

To be able to create networks using this library, each node in the network should have a unique 8-byte address. The address is generated randomly and then written into Arduino EEPROM to addresses `addrEeprom` to `addrEeprom + 7`. The default value of `addrEeprom` is 0, so when using the default initialization `LoRa::init()`, the address will be written to address 0 to 7. To prevent overwrite of existing address on the next start, 0 will be written to EEPROM address `addrFlag` (EEPROM address 8 by default).
  
  * `uint8_t address[8]`

The address of the current node.

To be able to create networks using this library, each node in the network should have a unique 8-byte address. The address is generated randomly and then written into Arduino EEPROM to addresses `addrEeprom` to `addrEeprom + 7`. The default value of `addrEeprom` is 0, so when using the default initialization `LoRa::init()`, the address will be written to address 0 to 7. To prevent overwrite of existing address on the next start, 0 will be written to EEPROM address `addrFlag` (EEPROM address 8 by default).

### Packet class

#### Public member variables

* `uint8_t source[8]`

An array of 8 `byte` numbers representing the address of the packet's source.

* `uint8_t destination[8]`

An array of 8 `byte` numbers representing the address of the packet's source.

* `char data[240]`

A null-terminated string of up to 240 characters. This limit is due to the limitations of SX1278 internal FIFO buffer, which can only hold 256 bytes of data. Since 8 bytes are used for the packet source and another 8 for destination, 240 bytes are left for the actual data.

* `uint8_t length = 0`

The total length of the packet in bytes. This includes the source (8 bytes), destination (8 bytes) and data (including the terminating null character).

#### Public member functions

* `packet(void)`

The default constructor. The constructed packet will have the source of the node which constructed it, destination "00:00:00:00:00:00:00:00" and empty data.

* `packet(const char dest[24], const char dat[240])`

An overloaded constructor for creating packets with given source and destination addresses. The constructed packet will have the source of the node which constructed it, destination is in the form of null-terminated address string. The data are a null-terminated string of data to be sent in the packet.

* `packet(uint8_t dest[8], const char dat[240])`

An overloaded constructor for creating packets with given source and destination addresses. The constructed packet will have the source of the node which constructed it, destination is in the form of an array of 8 bytes. The data are a null-terminated string of data to be sent in the packet.

* `packet(const char src[24], const char dest[24], const char dat[240])`

An overloaded constructor for creating packets with given source and destination addresses. Both source and destination are in the form of null-terminated address strings. The data are a null-terminated string of data to be sent in the packet.

* `packet(uint8_t src[8], uint8_t dest[8], const char dat[240])`

An overloaded constructor for creating packets with given source and destination addresses. Both source and destination are in the form of an array of 8 bytes. The data are a null-terminated string of data to be sent in the packet.

* `uint8_t getSourceStr(char src[24])`

This function converts the 8-byte-array source address of the packet into more human-readable null-terminated address string in the form "01:23:45:67:89:AB:CD:EF".

* `uint8_t getDestinationStr(char dest[24])`

This function converts the 8-byte-array destination address of the packet into more human-readable null-terminated address string in the form "01:23:45:67:89:AB:CD:EF".
