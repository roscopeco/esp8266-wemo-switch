/*
 * HardwareControl.h - Handles the hardware
 *
 *  Created on: 17 Jun 2018
 *      Author: rosco
 */

#ifndef WEMO_SOCKETS_HARDWARECONTROL_H_
#define WEMO_SOCKETS_HARDWARECONTROL_H_

#include <Arduino.h>

void initHardware(void);

String getHardwareSerial(void);
String getHardwarePersistentUuid(void);

void setLedMcu(bool on);
void setLedEsp(bool on);

void setRelayState(int relayNum, bool on);
bool getRelayState(int relayNum);

void ledSOS(void);

#endif /* WEMO_SOCKETS_HARDWARECONTROL_H_ */
