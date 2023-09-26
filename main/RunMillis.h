/*
 * RunMillis.h
 *
 *  Copyright BinOwl 2017
 *
 *  Created on: 09.07.2017
 *      Author: BinOwl
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
