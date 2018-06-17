/*
 * WiFi.cpp - Handles WiFi connection and mDNS.
 *
 *  Created on: 17 Jun 2018
 *      Author: rosco
 */

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include "WiFi.h"
#include "WemoSockets.h"
#include "HardwareControl.h"

bool initWiFi() {
  bool state = false;
  bool espLedState = false;
  int attempts = 0;

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_KEY);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);

    // flash LED while connecting
    setLedEsp(espLedState);
    espLedState = !espLedState;

    Serial.print(".");

    if (attempts++ > 10) {
      break;
    }
  }

  state = WiFi.status() == WL_CONNECTED;

  if (state) {
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(WIFI_SSID);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // Start MDNS responder.
    if (MDNS.begin("esp8266")) {
      Serial.println("MDNS responder started");
    }
  } else {
    Serial.println("Error: cannot connect to WIFI");
  }

  return state;
}


