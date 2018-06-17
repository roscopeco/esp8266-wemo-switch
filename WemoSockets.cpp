
/*
 * WemoSocket - An Alexa-compatible fake-Wemo switching socket for ESP8266.
 * Copyright (c)2018 Ross Bamford & Jake Randall
 *
 * See WemoSockets.h for notes etc.
 */
#include "WemoSockets.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>

#include "WemoSockets.h"
#include "WiFi.h"
#include "HardwareControl.h"
#include "HttpServer.h"
#include "UdpServer.h"

static String deviceName = DEVICE_NAME;

static bool wifiConnected = false;
static bool udpConnected = false;

void setup() {
  Serial.begin(115200);
  initHardware();

  delay(100);

  wifiConnected = initWiFi();

  if (wifiConnected) {
    udpConnected = initUdp();

    if (udpConnected) {
      initHttp();
    }
  }

  if (wifiConnected) {
    Serial.println("Device should now be discoverable by Alexa");
    setLedEsp(true);
  } else {
    Serial.println("Device initialisation failed; Going into flashy-lights mode...");
  }
}

void loop() {
  // check if the WiFi and UDP connections were successful
  if(wifiConnected){
    handleHttpClient();

    delay(5);

    if(udpConnected){
      handleUdpRequest();
    }
  } else {
    ledSOS();
  }
}

String getDeviceName() {
  return deviceName;
}
