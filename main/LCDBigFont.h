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
 * 	LCDBigFont.h
 *  Created on: 24.10.2017
 *  Author: BinOwl
 * 
 */

#ifndef LCDBIGFONT_H_
#define LCDBIGFONT_H_

#include "LiquidCrystal.h"
#include "Arduino.h"

class LCDBigFont: public LiquidCrystal {
public:
	LCDBigFont(uint8_t rs, uint8_t enable, uint8_t d0, uint8_t d1, uint8_t d2,
			uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);
	LCDBigFont(uint8_t rs, uint8_t rw, uint8_t enable, uint8_t d0, uint8_t d1,
			uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6,
			uint8_t d7);
	LCDBigFont(uint8_t rs, uint8_t rw, uint8_t enable, uint8_t d0, uint8_t d1,
			uint8_t d2, uint8_t d3);
	LCDBigFont(uint8_t rs, uint8_t enable, uint8_t d0, uint8_t d1, uint8_t d2,
			uint8_t d3);
	void printBigDigit(uint8_t col, uint8_t line, uint8_t n);
	void printBigNumber(const char* num);
	void printNumber(const char* num);
	void begin(uint8_t cols, uint8_t rows);
	void setCursor(uint8_t col, uint8_t row);
private:
	void createChars();
	uint8_t _cols;
	uint8_t _rows;
	uint8_t _curr_col;
	uint8_t _curr_row;
};

#endif /* LCDBIGFONT_H_ */
