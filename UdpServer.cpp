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

static void udpSearchHandler();

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

void handleUdpRequest() {
  // if there’s data available, read a packet
  int packetSize = udpServer.parsePacket();

  if(packetSize) {
    IPAddress remote = udpServer.remoteIP();

    Serial.println("");
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    Serial.println(remote);

    for (int i =0; i < 4; i++) {
      Serial.print(remote[i], DEC);
      if (i < 3) {
        Serial.print(".");
      }
    }

    Serial.print(", port ");
    Serial.println(udpServer.remotePort());

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
            Serial.println("Responding to search request ...");
            udpSearchHandler();
         }
    }
  }

  delay(10);
}

static void udpSearchHandler() {
  Serial.println("");
  Serial.print("Sending response to ");
  Serial.println(udpServer.remoteIP());
  Serial.print("Port : ");
  Serial.println(udpServer.remotePort());

  IPAddress localIP = WiFi.localIP();
  char s[16];
  sprintf(s, "%d.%d.%d.%d", localIP[0], localIP[1], localIP[2], localIP[3]);

  String response =
       "HTTP/1.1 200 OK\r\n"
       "CACHE-CONTROL: max-age=86400\r\n"
       "DATE: Fri, 15 Apr 2016 04:56:29 GMT\r\n"
       "EXT:\r\n"
       "LOCATION: http://" + String(s) + ":80/setup.xml\r\n"
       "OPT: \"http://schemas.upnp.org/upnp/1/0/\"; ns=01\r\n"
       "01-NLS: b9200ebb-736d-4b93-bf03-835149d13983\r\n"
       "SERVER: Unspecified, UPnP/1.0, Unspecified\r\n"
       "ST: urn:Belkin:device:**\r\n"
       "USN: uuid:" + getHardwarePersistentUuid() + "::urn:Belkin:device:**\r\n"
       "X-User-Agent: redsonic\r\n\r\n";

  // Try changing to this if you have problems discovering
  /* https://github.com/kakopappa/arduino-esp8266-alexa-wemo-switch/issues/26
  String response =
    "HTTP/1.1 200 OK\r\n"
    "CACHE-CONTROL: max-age=86400\r\n"
    "DATE: Fri, 15 Apr 2016 04:56:29 GMT\r\n"
    "EXT:\r\n"
    "LOCATION: http://" + String(s) + ":80/setup.xml\r\n"
    "OPT: "http://schemas.upnp.org/upnp/1/0/\"; ns=01\r\n"
    "01-NLS: b9200ebb-736d-4b93-bf03-835149d13983\r\n"
    "SERVER: Unspecified, UPnP/1.0, Unspecified\r\n"
    "ST: ssdp:all\r\n"
    "USN: uuid:" + persistent_uuid + "::upnp:rootdevice\r\n"
    "X-User-Agent: redsonic\r\n\r\n";
  */

  udpServer.beginPacket(udpServer.remoteIP(), udpServer.remotePort());
  udpServer.write(response.c_str());
  udpServer.endPacket();

  Serial.println("Response sent !");
}



