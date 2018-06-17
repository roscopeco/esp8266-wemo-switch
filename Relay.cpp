/*
 * Relay.cpp - Relay class implementation
 *
 *  Created on: 17 Jun 2018
 *      Author: rosco
 */

#include <WiFiUdp.h>

#include "Relay.h"
#include "HardwareControl.h"

static WiFiUDP UDP;

// TODO this way of init and managing is fucking atrocious...
static Relay relays[] = {
    Relay("Switch 1", RELAY_PIN_0, RELAY_PORT_0),
    Relay("Switch 2", RELAY_PIN_1, RELAY_PORT_1),
    Relay("Switch 3", RELAY_PIN_2, RELAY_PORT_2),
    Relay("Switch 4", RELAY_PIN_3, RELAY_PORT_3)
};

void initRelays() {
  for (int i = 0; i < NUM_RELAYS; i++) {
    relays[i].startHttp();
  }
}

uint8_t getNumRelays() {
  return NUM_RELAYS;
}

Relay* getRelay(int relayNum) {
  if (relayNum < NUM_RELAYS) {
    return &relays[relayNum];
  } else {
    Serial.print("Unsupported relay #");
    Serial.print(relayNum);
    Serial.printf(" in getRelay (0-%d are valid)\n", NUM_RELAYS);
    return NULL;
  }
}

Relay::Relay(String deviceName, uint8_t pin, uint16_t port) {
  this->deviceName = deviceName;
  this->pin = pin;
  this->port = port;
  this->switchedOn = false;

  prepareIds();
}

Relay::~Relay() {
  delete(this->httpServer);
}

String Relay::getDeviceName() {
  return deviceName;
}

String Relay::getHardwareSerial() {
  return serial;
}

String Relay::getPersistentUuid() {
  return persistentUuid;
}

void Relay::setState(bool state) {
  setHardwarePin(pin, state);
  switchedOn = state;
}

bool Relay::getState() {
  return switchedOn;
}

void Relay::loop() {
  httpServer->handleClient();
}

void Relay::respondToUpnpSearch(IPAddress *remoteIP, uint16_t remotePort) {
  Serial.print("Relay '");
  Serial.print(deviceName);
  Serial.println("' responding to uPnP search.");
  Serial.print("Sending response to ");
  Serial.println(*remoteIP);
  Serial.print("Port : ");
  Serial.println(remotePort);

  IPAddress localIP = WiFi.localIP();
  char s[16];
  sprintf(s, "%d.%d.%d.%d", localIP[0], localIP[1], localIP[2], localIP[3]);

  String response =
       "HTTP/1.1 200 OK\r\n"
       "CACHE-CONTROL: max-age=86400\r\n"
       "DATE: Fri, 15 Apr 2016 04:56:29 GMT\r\n"
       "EXT:\r\n"
       "LOCATION: http://" + String(s) + ":" + String(port) + "/setup.xml\r\n"
       "OPT: \"http://schemas.upnp.org/upnp/1/0/\"; ns=01\r\n"
       "01-NLS: b9200ebb-736d-4b93-bf03-835149d13983\r\n"
       "SERVER: Unspecified, UPnP/1.0, Unspecified\r\n"
       "ST: urn:Belkin:device:**\r\n"
       "USN: uuid:" + getPersistentUuid() + "::urn:Belkin:device:**\r\n"
       "X-User-Agent: redsonic\r\n\r\n";

  // Try changing to this if you have problems discovering
  /* https://github.com/kakopappa/arduino-esp8266-alexa-wemo-switch/issues/26
  String response =
    "HTTP/1.1 200 OK\r\n"
    "CACHE-CONTROL: max-age=86400\r\n"
    "DATE: Fri, 15 Apr 2016 04:56:29 GMT\r\n"
    "EXT:\r\n"
       "LOCATION: http://" + String(s) + ":" + String(relay->port) + "/setup.xml\r\n"
    "OPT: "http://schemas.upnp.org/upnp/1/0/\"; ns=01\r\n"
    "01-NLS: b9200ebb-736d-4b93-bf03-835149d13983\r\n"
    "SERVER: Unspecified, UPnP/1.0, Unspecified\r\n"
    "ST: ssdp:all\r\n"
    "USN: uuid:" + persistent_uuid + "::upnp:rootdevice\r\n"
    "X-User-Agent: redsonic\r\n\r\n";
  */

  UDP.beginPacket(*remoteIP, remotePort);
  UDP.write(response.c_str());
  UDP.endPacket();

  Serial.println("Response sent !");
}

bool Relay::startHttp() {
  this->httpServer = new ESP8266WebServer(port);

  httpServer->on("/", HTTP_GET, [&]() {
    handleRoot();
  });

  httpServer->on("/index.html", HTTP_GET, [&]() {
    handleRoot();
  });

  httpServer->on("/upnp/control/basicevent1", HTTP_POST, [&]() {
    handleUpnpEvent();
  });

  httpServer->on("/eventservice.xml", HTTP_GET, [&]() {
    handleEventServiceXml();
  });

  httpServer->on("/setup.xml", HTTP_GET, [&]() {
    handleSetupXml();
  });

  httpServer->onNotFound([&]() {
    handleNotFound();
  });

  httpServer->begin();
  Serial.println("HTTP Server started for: " + deviceName);

  return true;
}

void Relay::prepareIds() {
  uint32_t chipId = ESP.getChipId();
  char uuid[64];
  sprintf_P(uuid, PSTR("38323636-4558-4dda-9188-cda0e6%02x%02x%02x"),
        (uint16_t) ((chipId >> 16) & 0xff),
        (uint16_t) ((chipId >>  8) & 0xff),
        (uint16_t)   chipId        & 0xff);

  this->serial = String(uuid);
  this->persistentUuid = "Socket-1_0-" + serial + String(port);
}

void Relay::handleRoot() {
  Serial.println("Got Request index.html");
  httpServer->send(200, "text/plain", "J&R's Four Way Mains Switch v0.02 (Ready)");
}

void Relay::handleNotFound() {
  Serial.println("Handling not found request");

  String message = "File Not Found\n\n";
  message += "URI: ";
  message += httpServer->uri();
  message += "\nMethod: ";
  message += (httpServer->method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += httpServer->args();
  message += "\n";

  for (uint8_t i = 0; i < httpServer->args(); i++){
    message += " " + httpServer->argName(i) + ": " + httpServer->arg(i) + "\n";
  }

  httpServer->send(404, "text/plain", message);
}

void Relay::handleUpnpEvent() {
  Serial.println("Handling /upnp/control/basicevent1 request");

  String request = httpServer->arg(0);
  Serial.print("request:");
  Serial.println(request);

  if(request.indexOf("SetBinaryState") >= 0) {
    if(request.indexOf("<BinaryState>1</BinaryState>") >= 0) {
        Serial.println("Got Turn on request");
        this->handleTurnOn();
    } else if(request.indexOf("<BinaryState>0</BinaryState>") >= 0) {
        Serial.println("Got Turn off request");
        this->handleTurnOff();
    } else {
      Serial.println("Got unknown request; Sending 404");
      httpServer->send(404, "text/plain", "Unknown request");
    }
  } else if (request.indexOf("GetBinaryState") >= 0) {
    Serial.println("Got binary state request");
    this->handleGetState();
  } else {
    Serial.println("Got unknown request; Sending 404");
    httpServer->send(404, "text/plain", "Unknown request");
  }
}

void Relay::handleEventServiceXml() {
  Serial.println("Handling /eventservice.xml request");

  String eventservice_xml = "<scpd xmlns=\"urn:Belkin:service-1-0\">"
    "<actionList>"
      "<action>"
        "<name>SetBinaryState</name>"
        "<argumentList>"
          "<argument>"
            "<retval/>"
            "<name>BinaryState</name>"
            "<relatedStateVariable>BinaryState</relatedStateVariable>"
            "<direction>in</direction>"
            "</argument>"
        "</argumentList>"
      "</action>"
      "<action>"
        "<name>GetBinaryState</name>"
        "<argumentList>"
          "<argument>"
            "<retval/>"
            "<name>BinaryState</name>"
            "<relatedStateVariable>BinaryState</relatedStateVariable>"
            "<direction>out</direction>"
            "</argument>"
        "</argumentList>"
      "</action>"
  "</actionList>"
    "<serviceStateTable>"
      "<stateVariable sendEvents=\"yes\">"
        "<name>BinaryState</name>"
        "<dataType>Boolean</dataType>"
        "<defaultValue>0</defaultValue>"
       "</stateVariable>"
       "<stateVariable sendEvents=\"yes\">"
          "<name>level</name>"
          "<dataType>string</dataType>"
          "<defaultValue>0</defaultValue>"
       "</stateVariable>"
    "</serviceStateTable>"
    "</scpd>\r\n"
    "\r\n";

  httpServer->send(200, "text/plain", eventservice_xml.c_str());
}

void Relay::handleSetupXml() {
  Serial.println("Handling /setup.xml request");

  IPAddress localIP = WiFi.localIP();
  char s[16];
  sprintf(s, "%d.%d.%d.%d", localIP[0], localIP[1], localIP[2], localIP[3]);

  String setup_xml = "<?xml version=\"1.0\"?>"
        "<root>"
         "<device>"
            "<deviceType>urn:Belkin:device:controllee:1</deviceType>"
            "<friendlyName>"+ getDeviceName() +"</friendlyName>"
            "<manufacturer>Belkin International Inc.</manufacturer>"
            "<modelName>Socket</modelName>"
            "<modelNumber>3.1415</modelNumber>"
            "<modelDescription>Belkin Plugin Socket 1.0</modelDescription>\r\n"
            "<UDN>uuid:"+ getPersistentUuid() +"</UDN>"
            "<serialNumber>221517K0101769</serialNumber>"
            "<binaryState>0</binaryState>"
            "<serviceList>"
              "<service>"
                  "<serviceType>urn:Belkin:service:basicevent:1</serviceType>"
                  "<serviceId>urn:Belkin:serviceId:basicevent1</serviceId>"
                  "<controlURL>/upnp/control/basicevent1</controlURL>"
                  "<eventSubURL>/upnp/event/basicevent1</eventSubURL>"
                  "<SCPDURL>/eventservice.xml</SCPDURL>"
              "</service>"
          "</serviceList>"
          "</device>"
        "</root>\r\n"
        "\r\n";

  httpServer->send(200, "text/xml", setup_xml.c_str());

  Serial.print("Sending :");
  Serial.println(setup_xml);
}

void Relay::handleTurnOn() {
  setState(true);

  String body =
      "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body>\r\n"
      "<u:SetBinaryStateResponse xmlns:u=\"urn:Belkin:service:basicevent:1\">\r\n"
      "<BinaryState>0</BinaryState>\r\n"
      "</u:SetBinaryStateResponse>\r\n"
      "</s:Body> </s:Envelope>";

  httpServer->send(200, "text/xml", body.c_str());

  Serial.print("Sending :");
  Serial.println(body);
}

void Relay::handleTurnOff() {
  setState(false);

  String body =
      "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body>\r\n"
      "<u:SetBinaryStateResponse xmlns:u=\"urn:Belkin:service:basicevent:1\">\r\n"
      "<BinaryState>0</BinaryState>\r\n"
      "</u:SetBinaryStateResponse>\r\n"
      "</s:Body> </s:Envelope>";

  httpServer->send(200, "text/xml", body.c_str());

  Serial.print("Sending :");
  Serial.println(body);
}

void Relay::handleGetState() {
  String body =
      "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body>\r\n"
      "<u:GetBinaryStateResponse xmlns:u=\"urn:Belkin:service:basicevent:1\">\r\n"
      "<BinaryState>";

  body += (getState() ? "1" : "0");

  body += "</BinaryState>\r\n"
      "</u:GetBinaryStateResponse>\r\n"
      "</s:Body> </s:Envelope>\r\n";


  Serial.print("Sending :");
  Serial.println(body);

  httpServer->send(200, "text/xml", body.c_str());
}




