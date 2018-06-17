/*
 * HttpServer.cpp - Encapsulates the HTTP server
 *
 *  Created on: 16 Jun 2018
 *      Author: rosco
 */
#include <ESP8266WebServer.h>
#include "HardwareControl.h"
#include "WemoSockets.h"
#include "HttpServer.h"

static ESP8266WebServer httpServer(80);

static void handleRoot();
static void handleBasicEvent();
static void handleEventServiceXml();
static void handleSetupXml();
static void handleNotFound();
static void handleTurnOn(int relayNum);
static void handleTurnOff(int relayNum);
static void handleGetState(int relayNum);

bool initHttp() {
  httpServer.on("/", HTTP_GET, handleRoot);
  httpServer.on("/index.html", HTTP_GET, handleRoot);
  httpServer.on("/upnp/control/basicevent1", HTTP_POST, handleBasicEvent);
  httpServer.on("/eventservice.xml", HTTP_GET, handleEventServiceXml);
  httpServer.on("/setup.xml", HTTP_GET, handleSetupXml);
  httpServer.onNotFound(handleNotFound);

  httpServer.begin();
  Serial.println("HTTP Server started");

  return true;
}

void handleHttpRequest() {
  httpServer.handleClient();
}

static void handleRoot() {
  Serial.println("Got Request index.html");
  httpServer.send(200, "text/plain", "J&R's Four Way Mains Switch v0.01 (Online; Mode: FAKEBELKIN)");
}

static void handleNotFound() {
  Serial.println("Handling not found request");
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += httpServer.uri();
  message += "\nMethod: ";
  message += (httpServer.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += httpServer.args();
  message += "\n";
  for (uint8_t i=0; i<httpServer.args(); i++){
    message += " " + httpServer.argName(i) + ": " + httpServer.arg(i) + "\n";
  }
  httpServer.send(404, "text/plain", message);
}

static void handleBasicEvent() {
  Serial.println("Handling /upnp/control/basicevent1 request");

  String request = httpServer.arg(0);
  Serial.print("request:");
  Serial.println(request);

  if(request.indexOf("SetBinaryState") >= 0) {
    if(request.indexOf("<BinaryState>1</BinaryState>") >= 0) {
        Serial.println("Got Turn on request");
        handleTurnOn(0);
    } else if(request.indexOf("<BinaryState>0</BinaryState>") >= 0) {
        Serial.println("Got Turn off request");
        handleTurnOff(0);
    } else {
      Serial.println("Got unknown request; Sending 404");
      httpServer.send(404, "text/plain", "Unknown request");
    }
  } else if (request.indexOf("GetBinaryState") >= 0) {
    Serial.println("Got binary state request");
    handleGetState(0);
  } else {
    Serial.println("Got unknown request; Sending 404");
    httpServer.send(404, "text/plain", "Unknown request");
  }
}

static void handleEventServiceXml() {
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

    httpServer.send(200, "text/plain", eventservice_xml.c_str());
}

static void handleSetupXml() {
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
            "<UDN>uuid:"+ getHardwarePersistentUuid() +"</UDN>"
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

  httpServer.send(200, "text/xml", setup_xml.c_str());

  Serial.print("Sending :");
  Serial.println(setup_xml);
}

static void handleTurnOn(int relayNum) {
  setRelayState(relayNum, true);

  String body =
      "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body>\r\n"
      "<u:SetBinaryStateResponse xmlns:u=\"urn:Belkin:service:basicevent:1\">\r\n"
      "<BinaryState>1</BinaryState>\r\n"
      "</u:SetBinaryStateResponse>\r\n"
      "</s:Body> </s:Envelope>";

  httpServer.send(200, "text/xml", body.c_str());

  Serial.print("Sending :");
  Serial.println(body);
}

static void handleTurnOff(int relayNum) {
  setRelayState(relayNum, false);

  String body =
      "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body>\r\n"
      "<u:SetBinaryStateResponse xmlns:u=\"urn:Belkin:service:basicevent:1\">\r\n"
      "<BinaryState>0</BinaryState>\r\n"
      "</u:SetBinaryStateResponse>\r\n"
      "</s:Body> </s:Envelope>";

  httpServer.send(200, "text/xml", body.c_str());

  Serial.print("Sending :");
  Serial.println(body);
}

static void handleGetState(int relayNum) {
  String body =
      "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body>\r\n"
      "<u:GetBinaryStateResponse xmlns:u=\"urn:Belkin:service:basicevent:1\">\r\n"
      "<BinaryState>";

  body += (getRelayState(relayNum) ? "1" : "0");

  body += "</BinaryState>\r\n"
      "</u:GetBinaryStateResponse>\r\n"
      "</s:Body> </s:Envelope>\r\n";

   httpServer.send(200, "text/xml", body.c_str());
}
