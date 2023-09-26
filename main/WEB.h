/* 
 * This file is part of the Td5Gauge Firmware (https://github.com/k0sci3j/Td5Gauge).
 * Copyright (c) 2022 Michal Kosciowski BinOwl.
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * 	WEB.h
 *  Created on: 7.04.2022
 *  Author: BinOwl
 * 
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