/*
 * UdpServer.cpp - Encapsulates the UDP server
 *
 *  Created on: 17 Jun 2018
 *      Author: rosco
 */

#include <stdint.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include "UdpServer.h"
#include "HardwareControl.h"

static IPAddress ipMulticast(239, 255, 255, 250);
static unsigned int portMulticast = 1900;      // local port to listen on

static WiFiUDP udpServer;

static char packetBuffer[UDP_TX_PACKET_MAX_SIZE];

bool initUdp(){
  bool state = false;

  Serial.println("");
  Serial.println("Connecting to UDP");

  if(udpServer.beginMulticast(WiFi.localIP(), ipMulticast, portMulticast)) {
    Serial.println("Connection successful");
    state = true;
  }
  else{
    Serial.println("Connection failed");
  }

  return state;
}

void uPnPLoop() {
  // if there’s data available, read a packet
  int packetSize = udpServer.parsePacket();

  if(packetSize) {
    IPAddress remoteIP = udpServer.remoteIP();
    uint16_t remotePort = udpServer.remotePort();

    Serial.println("");
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    Serial.println(remoteIP);

    for (int i =0; i < 4; i++) {
      Serial.print(remoteIP[i], DEC);
      if (i < 3) {
        Serial.print(".");
      }
    }

    Serial.print(", port ");
    Serial.println(remotePort);

    int len = udpServer.read(packetBuffer, 255);

    if (len > 0) {
        packetBuffer[len] = 0;
    }

    String request = packetBuffer;
    //Serial.println("Request:");
    //Serial.println(request);

    // Issue https://github.com/kakopappa/arduino-esp8266-alexa-wemo-switch/issues/24 fix
    if(request.indexOf("M-SEARCH") >= 0) {
      // Issue https://github.com/kakopappa/arduino-esp8266-alexa-multiple-wemo-switch/issues/22 fix
      //if(request.indexOf("urn:Belkin:device:**") > 0) {
      if((request.indexOf("urn:Belkin:device:**") > 0) || (request.indexOf("ssdp:all") > 0) || (request.indexOf("upnp:rootdevice") > 0)) {
        for (int i = 0; i < getNumRelays(); i++) {
          Relay *relay = getRelay(i);

          if (relay != NULL) {
            relay->respondToUpnpSearch(&remoteIP, remotePort);
            delay(5);
          }
        }
      }
    }
  }
}


