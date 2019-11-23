## End-of-Life Announcement
About 2 years ago, I started working on [RadioLib](https://github.com/jgromes/RadioLib) - universal wireless Arduino library. It was built on top of LoRaLib's SX127x driver, and extended to provide support for may additional modules, protocols and platforms. Unfortunately, maintaining and actively developing both projects at the same time is proving quite challenging and adds no additional advantages, since RadioLib is using the same SX127x driver as LoRaLib.

Due to all of the reasons listed above, **new and existing users are highly encouraged to switch to RadioLib**. To do this, the only thing that must be changed is the include statement in existing sketches and changing the name of constructor(s) from `LoRa` to `Module` - because RadioLib is fully compatible with LoRaLib, no other changes in the code are required!

Furthermore, RadioLib provides additional features, such as (at the time of writing):
* Compatibility with additional platforms, such as SAM, STM32 and SAMD
* Protocol support, including RTTY or Morse code transmissions using SX127x or other radio modules
* Loads of other wireless modules, seamlessly integrated into a single library with the same simple API - including the next-gen SX126x LoRa chips
* And much more!

LoRaLib repository will be archived and removed from Arduino library manager once LoRaWAN driver is implemented in RadioLib. Until this point, LoRaLib will still receive the same updates as SX127x driver in RadioLib, however, **updates will be made in RadioLib first and only pushed to LoRaLib after release of new RadioLib version**.
