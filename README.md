# LoRaLib

## Arduino library for LoRa modules based on SX1278 chip

## This is not the shield you are looking for!
If you're looking for the shield to use with this library, it has its own repository: [https://github.com/jgromes/LoRenz](https://github.com/jgromes/LoRenz)

DISCLAIMER: This library is provided 'AS IS'. See `license.txt` for details.

This library enables easy long range communaction with SX1278-based LoRa modules. It was designed to be used with LoRenz Rev.B shields or modules. However, they are not required and this library can be used with any LoRa module, as long as it is based on the SX1278 chip.

## Library reference
The following is a list of currently implemented features and their description, see `/examples/` for examples of their usage.

* `LoRa(int nss, uint8_t bw, uint8_t cr, uint8_t sf)`

  Default constructor, used to create the LoRa object.  
  Usage:
  
  * Single LoRa module connected:
  
    ```c++
    LoRa lora;
    ```
    
    This will create a `lora` instance of the `LoRa` class with default values of bandwidth, coding rate and spreading factor. See Table 1. for their recommended values.
  
  * Multiple LoRa modules connected simultaneously:
    
    ```c++
    LoRa lora0(7);
    LoRa lora1(6);
    LoRa lora2(5);
    LoRa lora3(4);
    ```
    
    Multiple LoRenz Rev.B shields/modules can be connected to a single Arduino, however, on each of them, a different position on the NSS pin header has to be shorted out. Since there are 4 positions on the NSS pin header, the maximum of simultaneously connected LoRenz Rev.B shields/modules is 4. Any number of other LoRa modules can be connected and used with this library as long as their SPI NSS connection is not the same as the ones already used for LoRenz Rev.B shield(s)/module(s)

* `int tx(packet* pack)`

  This function will attempt to transmit the `packet pack` in LoRa mode. For information on the packet structure, see the third part of this document. In case the supplied packet is larger than 256 bytes, the function will return -1. In case of a successful transmission, the function will return 0.

* `packet* rx(uint8_t mode)`

  This function will attempt to receive incoming data in LoRa mode. For information on the packet structure, see the third part of this document. The default mode for this function is `RXSINGLE` mode: the function will be terminated after successful reception of a single packet, this is the recommended RX mode. In `RXCONTINUOUS` mode, the function will keep on going until terminated externally.

* `void setMode(uint8_t mode)`

  This function will set the LoRa module into a specified mode. The available modes are: `SLEEP`, `STANDBY`, `FSTX`, `FSRX`, `TX`, `RXCONTINUOUS`, `RXSINGLE` and `CAD`. For details on these modes, please refer to the SX1278 datasheet.

# Packet structure

Because of the SX1278 internal FIFO buffer limitations, the maximum size of a single packet is 256 bytes. The following packet structure is a part of this library:

```
struct packet {
  uint8_t source[8];
  uint8_t destination[8];
  const char* data;
};
```

The `source` and `destination` arrays are the addresses of the source and the destination LoRa modules. These addresses are in the form of an array of 8 bytes and can be supplied in the form of an address string, e.g. "01:23:45:67:89:AB:CD:EF".  
The `data` is a null-terminated string of up to 240 characters.

The values of all of the above can be set and retrieved by the following functions:

* `void setPacketSource(packet* pack, uint8_t* address)`

  Set the packet source in the form of an array of 8 `byte` numbers.

* `void setPacketSourceStr(packet* pack, const char* address)`

  Set the packet source in the form of an address string.

* `uint8_t* getPacketSource(packet* pack)`

  Retrieve the packet source in the form of an array of 8 `byte` numbers.

* `const char* getPacketSourceStr(packet* pack)`

  Retrieve the packet source in the form of an address string.

* `void setPacketDestination(packet* pack, uint8_t* address)`

  Set the packet destination in the form of an array of 8 `byte` numbers.

* `void setPacketDestinationStr(packet* pack, const char* address)`

  Set the packet destination in the form of an address string.

* `uint8_t* getPacketDestination(packet* pack)`

  Retrieve the packet destination in the form of an array of 8 `byte` numbers.

* `const char* getPacketDestinationStr(packet* pack)`

  Retrieve the packet destination in the form of an address string.

* `void setPacketData(packet* pack, const char* data)`

  Set the packet data in the form of a null-terminated string of up to 240 characters.

* `const char* getPacketData(packet* pack)`

  Retrieve the packet data in the form of a null-terminated string of up to 240 characters.
