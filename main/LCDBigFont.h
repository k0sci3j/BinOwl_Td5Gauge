/*
 * LCDBigFont.h
 *
 *  Copyright BinOwl 2017
 *
 *  Created on: 24.10.2017
 *      Author: BinOwl
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
