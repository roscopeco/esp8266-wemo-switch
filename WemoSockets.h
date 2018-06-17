/*
 * WemoSocket - An Alexa-compatible fake-Wemo switching socket for ESP8266.
 * Copyright (c)2018 Ross Bamford & Jake Randall
 *
 * This firmware supports switching four sockets using relays on
 * the pins RELAY_PIN_0 to RELAY_PIN_4.
 *
 * Wiring for these on the Node MCU dev board is to D1, D3, D5, and D7.
 * If using a different board or the bare ESP, you will probably need to
 * change the pins in the defines, in HardwareControl.h.
 */

#ifndef WEMO_SOCKETS_H_
#define WEMO_SOCKETS_H_

#include "Arduino.h"

// Make this whatever you like
#define DEVICE_NAME "JR's Four Way Mains Switch"

// Replace these with your appropriate access params.
#define WIFI_SSID "<YOUR WIFI SSID>"
#define WIFI_KEY "<YOUR WIFI KEY>"

#endif /* _WEMO_SOCKETS_H_ */
