/*
 * WEB.h
 *
 *  Copyright BinOwl 2022
 *
 *  Created on: 7.04.2022
 *      Author: BinOwl
 */



#ifndef WEB_H_
#define WEB_H_

#include "WiFi.h"
#include "ESPmDNS.h"
#include "WebServer.h"
#include "HTTPUpdateServer.h"

#define NAME "Td5Gauge"

extern WebServer server;
extern String ajaxValue;

void RunWebServer();

#endif /* WEB_H_ */