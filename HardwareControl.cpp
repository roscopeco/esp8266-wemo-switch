/*
 * SoapHandlers.cpp -
 *
 *  Created on: 16 Jun 2018
 *      Author: rosco
 */

#include "HardwareControl.h"

#include <stdint.h>

#define LED_MCU LED_BUILTIN
#define LED_ESP D4

#define RELAY_PIN_0 D1
#define RELAY_PIN_1 D3
#define RELAY_PIN_2 D5
#define RELAY_PIN_3 D7

typedef struct _relay {
  uint8_t pin;
  bool switchedOn;
} relay;

static relay relays[] = {
    { .pin = RELAY_PIN_0, .switchedOn = false },
    { .pin = RELAY_PIN_1, .switchedOn = false },
    { .pin = RELAY_PIN_2, .switchedOn = false },
    { .pin = RELAY_PIN_3, .switchedOn = false },
};

static String serial;
static String persistentUuid;

static void prepareIds() {
  uint32_t chipId = ESP.getChipId();
  char uuid[64];
  sprintf_P(uuid, PSTR("38323636-4558-4dda-9188-cda0e6%02x%02x%02x"),
        (uint16_t) ((chipId >> 16) & 0xff),
        (uint16_t) ((chipId >>  8) & 0xff),
        (uint16_t)   chipId        & 0xff);

  serial = String(uuid);
  persistentUuid = "Socket-1_0-" + serial;
}

void initHardware() {
  pinMode(LED_MCU, OUTPUT);
  pinMode(LED_ESP, OUTPUT);
  pinMode(RELAY_PIN_0, OUTPUT);
  pinMode(RELAY_PIN_1, OUTPUT);
  pinMode(RELAY_PIN_2, OUTPUT);
  pinMode(RELAY_PIN_3, OUTPUT);

  digitalWrite(LED_MCU, LOW);
  digitalWrite(LED_ESP, HIGH);

  prepareIds();
}

String getHardwareSerial() {
  return serial;
}

String getHardwarePersistentUuid() {
  return persistentUuid;
}

void setLedMcu(bool on) {
  // LEDs are active low
  digitalWrite(LED_MCU, on ? LOW : HIGH);
}

void setLedEsp(bool on) {
  // LEDs are active low
  digitalWrite(LED_ESP, on ? LOW : HIGH);
}

void setRelayState(int relayNum, bool on) {
  if (relayNum < 4) {
    relay *theRelay = &relays[relayNum];
    digitalWrite(theRelay->pin, on ? HIGH : LOW);
    theRelay->switchedOn = on;
  } else {
    Serial.print("Unsupported relay #");
    Serial.print(relayNum);
    Serial.println(" in switchRelay (0-3 are valid)");
  }
}

bool getRelayState(int relayNum) {
  if (relayNum < 4) {
    return relays[relayNum].switchedOn;
  } else {
    Serial.print("Unsupported relay #");
    Serial.print(relayNum);
    Serial.println(" in relayGetStatus (0-3 are valid)");
    return false;
  }
}

void ledSOS() {
  // SOS forever
  digitalWrite(LED_MCU, HIGH);
  digitalWrite(LED_ESP, HIGH);
  delay(750);

  // DOT
  digitalWrite(LED_MCU, LOW);
  delay(200);

  digitalWrite(LED_MCU, HIGH);
  delay(250);

  // DOT
  digitalWrite(LED_MCU, LOW);
  delay(200);

  digitalWrite(LED_MCU, HIGH);
  delay(250);

  // DOT
  digitalWrite(LED_MCU, LOW);
  delay(200);

  digitalWrite(LED_MCU, HIGH);
  delay(250);

  // DASH
  digitalWrite(LED_ESP, LOW);
  delay(600);

  digitalWrite(LED_ESP, HIGH);
  delay(250);

  // DASH
  digitalWrite(LED_ESP, LOW);
  delay(600);

  digitalWrite(LED_ESP, HIGH);
  delay(250);

  // DASH
  digitalWrite(LED_ESP, LOW);
  delay(600);

  digitalWrite(LED_ESP, HIGH);
  delay(250);

  // DOT
  digitalWrite(LED_MCU, LOW);
  delay(200);

  digitalWrite(LED_MCU, HIGH);
  delay(250);

  // DOT
  digitalWrite(LED_MCU, LOW);
  delay(200);

  digitalWrite(LED_MCU, HIGH);
  delay(250);

  // DOT
  digitalWrite(LED_MCU, LOW);
  delay(200);

  digitalWrite(LED_MCU, HIGH);
}
