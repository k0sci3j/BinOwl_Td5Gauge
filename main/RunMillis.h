/* 
 * This file is part of the Td5Gauge Firmware (https://github.com/k0sci3j/Td5Gauge).
 * Copyright (c) 2017 Michal Kosciowski BinOwl.
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
 * 	RunMillis.h
 *  Created on: 09.07.2017
 *  Author: BinOwl
 * 
 */

#ifndef RUNMILLIS_H_
#define RUNMILLIS_H_

#include <Arduino.h>

class RunMillis
{
public:
	RunMillis(uint32_t interval, void (*callback)(void), const bool update_after=true):
		_previousMillis(0),
		_interval(interval),
		_currentMillis(0),
		_callback(callback),
		_update_after(update_after) {
	}

	void setCallback(void (*callback)(void)) {
		_callback = callback;
	}

	bool run() {
		_currentMillis = millis();
		if (_currentMillis - _previousMillis >= _interval) {
			if(!_update_after)
				updateTimer();
			_callback();
		    if(_update_after)
		    	updateTimer();
		    return true;
		}
		return false;
	}

	void updateTimer() {
		_previousMillis = _currentMillis;
	}


private:
	uint32_t _previousMillis;
	uint32_t _interval;
	uint32_t _currentMillis;
	void (*_callback)();
	bool _update_after;
};




#endif /* RUNMILLIS_H_ */
