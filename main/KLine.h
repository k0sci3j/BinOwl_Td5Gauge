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
 * 	KLine.h
 *  Created on: 15.02.2022
 *  Author: BinOwl
 * 
 */

#ifndef KLINE_H_
#define KLINE_H_
#include "Arduino.h"


class KLine {
public:
	KLine();
	void initialize();
	void read_rpm();
	void read_voltage();
	void read_temps();
	void read_speed();
	void read_throttle();
	void read_fuel();
	void read_inj_balance();
	void read_pressure();
	void read_maf_map();
	void read_wastegate();
	bool keepAlive();
	void restart();

	int32_t coolant_temp;
	int32_t air_temp;
	int32_t fuel_temp;
	uint16_t volt;
	uint16_t rpm;
	uint8_t speed;
	int32_t driver_fuel_demand; //fu1
	int32_t idle_fuel_demand; //fu8
	int32_t fuel_injected; //fu4
	uint16_t press1;
	uint16_t press2;
	uint16_t press3;
	uint16_t map;
	uint16_t maf;
	uint16_t thr1;
	uint16_t thr2;
	uint16_t thr3;
	uint16_t supp;
	uint16_t wastegate;
	int32_t inj_bal1;
	int32_t inj_bal2;
	int32_t inj_bal3;
	int32_t inj_bal4;
	int32_t inj_bal5;


private:
	uint8_t _response[50];
	uint8_t _response_len;
	bool _response_status;
	uint8_t _keepAlive_err_cnt;
	bool _is_nnn;
	void send_recv_cmd(const byte *cmd, bool pgm);
	uint16_t keygen();
	void send_key_response(uint16_t key);
	void fast_init();

};

#endif /* KLINE_H_ */
