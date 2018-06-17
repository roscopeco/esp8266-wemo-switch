/*
 * HttpHandlers.h
 *
 *  Created on: 16 Jun 2018
 *      Author: rosco
 */

#ifndef WEMO_SOCKETS_HTTPHANDLERS_H_
#define WEMO_SOCKETS_HTTPHANDLERS_H_

#include <ESP8266WebServer.h>

bool initHttp(void);
void handleHttpClient(void);

#endif /* WEMO_SOCKETS_HTTPHANDLERS_H_ */
