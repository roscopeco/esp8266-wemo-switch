/*
 * HardwareControl.h - Handles the hardware
 *
 *  Created on: 17 Jun 2018
 *      Author: rosco
 */

#ifndef WEMO_SOCKETS_HARDWARECONTROL_H_
#define WEMO_SOCKETS_HARDWARECONTROL_H_

#include <Arduino.h>
#include "Relay.h"

void initHardware(void);

void setLedMcu(bool on);
void setLedEsp(bool on);

void setHardwarePin(uint8_t pin, bool high);

void ledSOS(void);

#endif /* WEMO_SOCKETS_HARDWARECONTROL_H_ */
