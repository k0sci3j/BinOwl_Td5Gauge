/*
 * KLine.cpp
 *
 *  Copyright BinOwl 2022
 *
 *  Created on: 15.02.2022
 *      Author: BinOwl
 */
#include "Arduino.h"
#include "KLine.h"
#include "Effortless_SPIFFS.h"


#define PIN_TX 10
#define FILE_RESET "/4.cfg"

extern uint auto_off;

//Format: { cmd_len, resp_len, delay_time, ...msg... }
const uint8_t cmd_init[] = { 4, 5, 35, 0x81, 0x13, 0xf7, 0x81 };
const uint8_t cmd_diagnostic[] = { 3, 3, 35, 0x2, 0x10, 0xa0 };
const uint8_t cmd_get_seed[] = { 3, 6, 50, 0x2, 0x27, 0x1 };
const uint8_t cmd_read_rpm[] = { 3, 6, 50, 0x2, 0x21, 0x9 };
const uint8_t cmd_read_voltage[] = { 3, 8, 50, 0x2, 0x21, 0x10 };
const uint8_t cmd_read_temp[] = { 3, 20, 100, 0x2, 0x21, 0x1A };
const uint8_t cmd_read_speed[] = { 3, 5, 50, 0x2, 0x21, 0xd };
const uint8_t cmd_read_fuel[] = { 3, 22, 100, 0x2, 0x21, 0x1d };
const uint8_t cmd_read_press[] = { 3, 8, 50, 0x2, 0x21, 0x23 };
const uint8_t cmd_read_maf_map[] = { 3, 12, 50, 0x2, 0x21, 0x1c };
const uint8_t cmd_read_inj_balance[] = { 3, 14, 50, 0x2, 0x21, 0x40 };
const uint8_t cmd_keep_alive[] = { 3, 3, 30, 0x2, 0x3e, 0x1 };
const uint8_t cmd_throttle_msb[] = { 3, 12, 50, 0x2, 0x21, 0x1b };
const uint8_t cmd_throttle_nnn[] = { 3, 14, 50, 0x2, 0x21, 0x1b };
const uint8_t cmd_wastegate[] = { 3, 6, 50, 0x2, 0x21, 0x38 };

KLine::KLine() :
		coolant_temp(0), air_temp(0), fuel_temp(0), volt(0), rpm(
				0), speed(0), driver_fuel_demand(0), idle_fuel_demand(0), fuel_injected(
				0), press1(0), press2(0), press3(0), map(0), maf(0), inj_bal1(0), inj_bal2(
				0), inj_bal3(0), inj_bal4(0), inj_bal5(0), _response_len(0), _response_status(
		false), _keepAlive_err_cnt(0), _is_nnn(false) {
#ifndef DEBUG
	initialize();
#endif
}

void KLine::send_recv_cmd(const uint8_t *cmd, bool pgm = true) {
	uint16_t sum = 0;
	uint8_t cmd_len;
	uint8_t resp_len;
	uint8_t delay_time;
	cmd_len = cmd[0];
	resp_len = cmd[1];
	delay_time = cmd[2];
	for (uint8_t i = 0; i < cmd_len; i++) {
		uint8_t c = cmd[i + 3];
		Serial1.write(c);
		delay(3);
		sum += c;
	}
	Serial1.write(sum & 0xff);
	delay(delay_time);
	uint8_t l = 0, c = 0;
	sum = 0;
	_response_status = false;
	while (Serial1.available() > 0) {
		if (l >= cmd_len + 1) {
			sum += (_response[c++] = Serial1.read());
		} else
			Serial1.read();
		l++;
	}
	sum = sum - _response[c - 1];
	if ((sum & 0xff) == _response[c - 1] && c == resp_len)
		_response_status = true;
	_response_len = c;

}

void KLine::fast_init() {
	bool serial_rerun = false;
	if (Serial1) {
		serial_rerun = true;
		Serial1.end();
	}
	digitalWrite(PIN_TX, HIGH);
	pinMode(PIN_TX, OUTPUT);
	digitalWrite(PIN_TX, LOW);
	delay(25);
	delayMicroseconds(500);
	digitalWrite(PIN_TX, HIGH);
	delay(25);
	delayMicroseconds(500);
	if (serial_rerun) {
		Serial1.begin(10400);
		while (!Serial1)
			;
	}
}

uint16_t KLine::keygen() {
	uint16_t q = (_response[4] | _response[3] << 8);
	uint16_t t_ = 0;
	uint8_t c = 0;
	uint8_t i;
	uint8_t b = 0;
	c = ((q >> 0xC & 0x8) | (q >> 0x5 & 0x4) | (q >> 0x3 & 0x2)
			| (q & 0x1)) + 1;
	for (i = 0; i < c; i++) {
		b = ((q >> 1) ^ (q >> 2) ^ (q >> 8) ^ (q >> 9)) & 1;
		t_ = ((q >> 1) | (b << 0xF));
		if ((q >> 0x3 & 1) && (q >> 0xD & 1)) {
			q = t_ & ~1;
		} else {
			q = t_ | 1;
		}
	}
	return q;
}

void KLine::send_key_response(uint16_t key) {
	uint8_t cmd_key_resp[] = { 5, 4, 50, 0x4, 0x27, 0x2, 0x0, 0x0 };
	cmd_key_resp[6] = key >> 8;
	cmd_key_resp[7] = key & 0xff;
	send_recv_cmd(cmd_key_resp, false);
}

void KLine::read_rpm() {
	send_recv_cmd(cmd_read_rpm);
	if (_response_status)
		rpm = (_response[4] | (_response[3] << 8));
}

void KLine::read_voltage() {
	send_recv_cmd(cmd_read_voltage);
	if (_response_status)
		volt = ((_response[6]) | (_response[5] << 8) | _response[7]);
}

void KLine::read_temps() {
	send_recv_cmd(cmd_read_temp);
	if (_response_status) {
		coolant_temp = ((_response[4] | (_response[3] << 8))) / 10 - 273; //Kelvin
		air_temp = ((_response[8] | (_response[7] << 8))) / 10 - 273; //Kelvin
		fuel_temp = ((_response[16] | (_response[15] << 8))) / 10 - 273; //Kelvin
		if (coolant_temp > 32768)
			coolant_temp -= 65537;
		if (air_temp > 32768)
			air_temp -= 65537;
		if (fuel_temp > 32768)
			fuel_temp -= 65537;
	}
}

void KLine::read_speed() {
	send_recv_cmd(cmd_read_speed);
	if (_response_status)
		speed = _response[3];
}

void KLine::read_throttle() {
	if(!_is_nnn){
		send_recv_cmd(cmd_throttle_msb);
		if (_response_status) {
			thr1 = (_response[4] | (_response[3] << 8));
			thr2 = (_response[6] | (_response[5] << 8));
			thr3 = (_response[8] | (_response[7] << 8));
			supp = (_response[10] | (_response[9] << 8));
		} else {
			_is_nnn = true;
		}
	}
	if(_is_nnn){
		send_recv_cmd(cmd_throttle_nnn);
		if (_response_status) {
			thr1 = (_response[4] | (_response[3] << 8));
			thr2 = (_response[6] | (_response[5] << 8));
			thr3 = (_response[8] | (_response[7] << 8));
			supp = (_response[12] | (_response[11] << 8));
		} else {
			_is_nnn = false;
		}
	}
}

void KLine::read_fuel() {
	send_recv_cmd(cmd_read_fuel);
	if (_response_status) {
		driver_fuel_demand = (_response[4] | (_response[3] << 8));
		fuel_injected = (_response[10] | (_response[9] << 8));
		idle_fuel_demand = (_response[18] | (_response[17] << 8));
		if (driver_fuel_demand > 32768)
			driver_fuel_demand -= 65537;
		if (fuel_injected > 32768)
			fuel_injected -= 65537;
		if (idle_fuel_demand > 32768)
			idle_fuel_demand -= 65537;
	}
}

void KLine::read_inj_balance() {
	send_recv_cmd(cmd_read_inj_balance);
	if (_response_status) {
		inj_bal1 = _response[4] | (_response[3] << 8);
		inj_bal2 = _response[6] | (_response[5] << 8);
		inj_bal3 = _response[8] | (_response[7] << 8);
		inj_bal4 = _response[10] | (_response[9] << 8);
		inj_bal5 = _response[12] | (_response[11] << 8);
		if (inj_bal1 > 32768)
			inj_bal1 -= 65537;
		if (inj_bal2 > 32768)
			inj_bal2 -= 65537;
		if (inj_bal3 > 32768)
			inj_bal3 -= 65537;
		if (inj_bal4 > 32768)
			inj_bal4 -= 65537;
		if (inj_bal5 > 32768)
			inj_bal5 -= 65537;
	}

}

void KLine::read_pressure() {
	send_recv_cmd(cmd_read_press);
	if (_response_status) {
		press1 = (_response[4] | (_response[3] << 8));
		press2 = (_response[6] | (_response[5] << 8));
	}

}

void KLine::read_maf_map() {
	send_recv_cmd(cmd_read_maf_map);
	if (_response_status) {
		map = (_response[4] | (_response[3] << 8));
		maf = (_response[8] | (_response[7] << 8));
		press3 = (_response[6] | (_response[5] << 8));
	}
}
void KLine::read_wastegate() {
	send_recv_cmd(cmd_wastegate);
	if (_response_status)
		wastegate = (_response[4] | (_response[3] << 8));
}

bool KLine::keepAlive() {
	/*
	BUG with restart after keepalive:
	Analysis:
		1. Make analysis on TX/RX near MCU and on K-Line - TX/RX are OK.
		2. Check timings: There is always such problem:
			Data: 0x1 0x7e 0x7f 0x0 <----GOOD
			Data: 0xc 0x61 0x1b 0x0 <----BAD - After it there is restart. It is ALWAYS the same. It is beginning from previous frame (at this example it is throttle potentiometers)

	Ideas:
		1. Check if data received are ok TWICE! Rerun keepAlive - Done 10 times check.
		2. Change timings(?) - Not needed.
	*/
	send_recv_cmd(cmd_keep_alive);
	if(!_response_status){
		_keepAlive_err_cnt++; //if response is wrong increase counter
	} else {
		_keepAlive_err_cnt = 0;
	}
	if(_keepAlive_err_cnt<=10){ //Here we are checking if counter is 10
		return false;
	} else {
		return true; //if reached 10 times keep alive is broken - ECU is not responding
	}
}
extern eSPIFFS fileSystem;
void KLine::restart() {
	if(auto_off == 1){
		uint r = 1;
    	fileSystem.saveToFile(FILE_RESET, r);
	}
	delay(10000);
	ESP.restart();
}

void KLine::initialize() {
	fast_init();
	send_recv_cmd(cmd_init);
	if (!_response_status)
		restart();
	send_recv_cmd(cmd_diagnostic);
	if (!_response_status)
		restart();
	send_recv_cmd(cmd_get_seed);
	if (!_response_status)
		restart();
	send_key_response(keygen());
	if (!_response_status)
		restart();
}
