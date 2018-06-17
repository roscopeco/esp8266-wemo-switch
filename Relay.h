/*
 * Relay.h
 *
 *  Created on: 17 Jun 2018
 *      Author: rosco
 */

#ifndef WEMO_SOCKETS_RELAY_H_
#define WEMO_SOCKETS_RELAY_H_

#include <ESP8266WebServer.h>

#define NUM_RELAYS 4

#define RELAY_PIN_0 D1
#define RELAY_PIN_1 D3
#define RELAY_PIN_2 D5
#define RELAY_PIN_3 D7

#define RELAY_PORT_0 80
#define RELAY_PORT_1 81
#define RELAY_PORT_2 82
#define RELAY_PORT_3 83

class Relay {
private:
  String deviceName;
  uint8_t pin;
  uint16_t port;
  bool switchedOn;

  String serial;
  String persistentUuid;

  ESP8266WebServer *httpServer;

  void prepareIds(void);

  void handleRoot(void);
  void handleNotFound(void);
  void handleUpnpEvent(void);
  void handleEventServiceXml(void);
  void handleSetupXml(void);

  void handleTurnOn(void);
  void handleTurnOff(void);
  void handleGetState(void);

public:
  Relay(String deviceName, uint8_t pin, uint16_t port);
  ~Relay(void);

  String getDeviceName(void);
  String getHardwareSerial(void);
  String getPersistentUuid(void);

  void setState(bool on);
  bool getState(void);

  bool startHttp(void);
  void loop(void);
  void respondToUpnpSearch(IPAddress *remoteIP, uint16_t remotePort);
};

void initRelays();
uint8_t getNumRelays();
Relay* getRelay(int relayNum);

#endif /* WEMO_SOCKETS_RELAY_H_ */
