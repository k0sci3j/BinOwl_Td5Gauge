/*
 * KLine.h
 *
 *  Copyright BinOwl 2022
 *
 *  Created on: 15.02.2022
 *      Author: BinOwl
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
