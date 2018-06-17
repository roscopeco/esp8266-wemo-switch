/*
 * HardwareControl.cpp - Handles the hardware
 *
 *  Created on: 16 Jun 2018
 *      Author: rosco
 */

#include "HardwareControl.h"
#include "Relay.h"

#include <stdint.h>

#define LED_MCU LED_BUILTIN
#define LED_ESP D4

void initHardware() {
  pinMode(LED_MCU, OUTPUT);
  pinMode(LED_ESP, OUTPUT);
  pinMode(RELAY_PIN_0, OUTPUT);
  pinMode(RELAY_PIN_1, OUTPUT);
  pinMode(RELAY_PIN_2, OUTPUT);
  pinMode(RELAY_PIN_3, OUTPUT);

  digitalWrite(LED_MCU, LOW);
  digitalWrite(LED_ESP, HIGH);
}

void setLedMcu(bool on) {
  // LEDs are active low
  digitalWrite(LED_MCU, on ? LOW : HIGH);
}

void setLedEsp(bool on) {
  // LEDs are active low
  digitalWrite(LED_ESP, on ? LOW : HIGH);
}

void setHardwarePin(uint8_t pin, bool high) {
  digitalWrite(pin, high ? HIGH : LOW);
}

static inline void ledReset() {
  digitalWrite(LED_MCU, HIGH);
  digitalWrite(LED_ESP, HIGH);
  delay(750);
}

// Morse DOT
static inline void ledDot(uint8_t ledPin) {
  digitalWrite(ledPin, LOW);
  delay(200);

  digitalWrite(ledPin, HIGH);
  delay(250);
}

// Morse DASH
static inline void ledDash(uint8_t ledPin) {
  // DASH
  digitalWrite(ledPin, LOW);
  delay(600);

  digitalWrite(ledPin, HIGH);
  delay(250);
}

void ledSOS() {
  ledReset();

  ledDot(LED_MCU);
  ledDot(LED_MCU);
  ledDot(LED_MCU);

  ledDash(LED_ESP);
  ledDash(LED_ESP);
  ledDash(LED_ESP);

  ledDot(LED_MCU);
  ledDot(LED_MCU);
  ledDot(LED_MCU);
}
