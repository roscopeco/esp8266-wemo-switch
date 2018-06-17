# WeMo (uPnP) compatible firmware for a Smart Switch
## For the ESP8266 WiFi module.

## Copyright (c)2018 Ross Bamford & Jake Randall. 
### Licensed under the MIT License - see LICENSE for details.

### What is this?

This is a simple Arduino project that uses the ESP8266 WiFi module to 
support switching of relays, by emulating a WeMo device (using UDP multicast
and HTTP). The aim of the project is to support switching of mains sockets
using solid state relays, with control coming from the Amazon Echo.

It is currently tested with the Node MCU V2 development board, but should
work with any ESP8266 module, although you may need to change some of the
pin assignments in `HardwareControl.h` to suit your module and wiring.

Currently only one relay is supported, although we plan to extend this
to support four sockets in the future. The aim is to build an integrated
solution in a standard four-gang extension.

### Installation

* Clone repo
* Edit settings in WemoSockets.h to suit your WiFi environment.
* Build with appropriate board settings (We use [Sloeber](http://eclipse.baeyens.it/)
  which is awesome, but you can probably build with Arduino IDE.
* Flash to device
* Boot device. You should see the MCU LED light up, and the ESP LED flash while it
  searches for WiFi. Once it connects, both LEDs will be solid.
  * If it can't connect, it'll blink out SOS on the LEDs.
  * Connect a serial terminal to see what's happening.
* Use the Alexa app to search for new smart home devices. She should detect
  it as a Belkin plug (that's what we emulate at the moment).

### Thanks

Huge props to [this project](https://github.com/kakopappa/arduino-esp8266-alexa-wemo-switch),
from which we learned a lot about getting stuff to work with Alexa!
