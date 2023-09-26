/*
 * LCDBigFont.cpp
 *
 *  Copyright BinOwl 2022
 *
 *  Created on: 15.02.2022
 *      Author: BinOwl
 */

#include "LCDBigFont.h"
#include "Arduino.h"
#include "LiquidCrystal.h"

const uint8_t char_0[] = { 0x1f, 0x1f, 0x1f, 0x1f, 0, 0, 0, 0 }; //3
const uint8_t char_1[] = { 0xf, 0xf, 0xf, 0xf, 0xf, 0xf, 0xf, 0xf }; //1
const uint8_t char_2[] = { 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e }; //7
const uint8_t char_3[] = { 0x1e, 0x1e, 0x1e, 0x1e, 0, 0, 0, 0 }; //4
const uint8_t char_4[] = { 0xf, 0xf, 0xf, 0xf, 0, 0, 0, 0 }; //5

const uint8_t* const c_table[] = { char_0, char_1, char_2, char_3, char_4 };
//0xff - Full black
//0xfe - Full white
//0x00 - 0x4 char_table
/*
 * How to display char i.e 0:
 * { 0xff, 0x00, 0xff,
 *   0xff, 0xfe, 0xff,
 *   0x00, 0x00, 0x00 };
 */

const uint8_t n_0[] = { 0x1, 0x0, 0x2, 0x1, 0xfe, 0x2, 0x4, 0x0, 0x3 };
const uint8_t n_1[] = { 0x4, 0xff, 0xfe, 0xfe, 0xff, 0xfe, 0x4, 0x0, 0x3 };
const uint8_t n_2[] = { 0x4, 0x0, 0x2, 0x1, 0x0, 0x3, 0x4, 0x0, 0x3 };
const uint8_t n_3[] = { 0x4, 0x0, 0x2, 0xfe, 0x0, 0x2, 0x4, 0x0, 0x3 };
const uint8_t n_4[] = { 0x1, 0xfe, 0x2, 0x4, 0x0, 0x2, 0xfe, 0xfe, 0x3 };
const uint8_t n_5[] = { 0x1, 0x0, 0x3, 0x4, 0x0, 0x2, 0x4, 0x0, 0x3 };
const uint8_t n_6[] = { 0x1, 0x0, 0x3, 0x1, 0x0, 0x2, 0x4, 0x0, 0x3 };
const uint8_t n_7[] = { 0x4, 0x0, 0x2, 0xfe, 0xfe, 0x2, 0xfe, 0xfe, 0x3 };
const uint8_t n_8[] = { 0x1, 0x0, 0x2, 0x1, 0x0, 0x2, 0x4, 0x0, 0x3 };
const uint8_t n_9[] = { 0x1, 0x0, 0x2, 0x4, 0x0, 0x2, 0x4, 0x0, 0x3 };
const uint8_t minus[] = { 0xfe, 0xfe, 0xfe, 0x4, 0x0, 0x3, 0xfe, 0xfe, 0xfe };
const uint8_t dot[] = {0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0x4, 0xfe };

const uint8_t* const n_table[] = { n_0, n_1, n_2, n_3, n_4, n_5, n_6, n_7,
		n_8, n_9, minus, dot };

void LCDBigFont::createChars() {
	uint8_t new_char[8];
	for (uint8_t ii = 0; ii < 5; ii++) {
		for (uint8_t i = 0; i < 8; i++) {
			new_char[i] = c_table[ii][i];
			createChar(ii, new_char);
			delay(8);
		}
	}
}

void LCDBigFont::printBigDigit(uint8_t col, uint8_t row, uint8_t n) {
	uint8_t d_c = _cols - col;
	uint8_t d_l = _rows - row;
	if (d_c < 3 && d_l < 3)
		return; //Cannot draw such character here - To small space for 3x3 char
	LiquidCrystal::setCursor(col, row);
	uint8_t i = 0;
	while (i < 9) {
		if (i % 3 == 0)
			LiquidCrystal::setCursor(col, row++);
		uint8_t c = n_table[n][i];
		write(c);
		i++;
	}
}

void LCDBigFont::printBigNumber(const char *num) {
	uint8_t i = 0;
	uint8_t col = _curr_col;
	while (num[i] != '\0') {
		switch (num[i]) {
		case '-':
			printBigDigit(col, _curr_row, 10);
			i++;
			col += 3;
			break;
		case '.':
			LiquidCrystal::setCursor(col, _curr_row + 3);
			write(0x4);
			i++;
			col += 1;
			break;
		case ' ':
			i++;
			break;
		default:
			printBigDigit(col, _curr_row, num[i] - 0x30);
			i++;
			col += 3;
		}
	}
}

void LCDBigFont::printNumber(const char* num) {
	uint8_t i = 0;
	while(num[i] != '\0') {
		if(num[i] == ' ') {
			i++;
		} else {
			LiquidCrystal::print(num[i]);
			i++;
		}
	}
}

void LCDBigFont::begin(uint8_t cols, uint8_t rows) {
	LiquidCrystal::begin(cols, rows);
	_cols = cols;
	_rows = rows;
}

void LCDBigFont::setCursor(uint8_t col, uint8_t row) {
	_curr_col = col;
	_curr_row = row;
	LiquidCrystal::setCursor(col, row);
}
LCDBigFont::LCDBigFont(uint8_t rs, uint8_t rw, uint8_t enable, uint8_t d0,
		uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6,
		uint8_t d7) :
						LiquidCrystal(rs, rw, enable, d0, d1, d2, d3, d4, d5, d6, d7), _cols(
								20), _rows(4), _curr_col(0), _curr_row(0) {
	createChars();
}

LCDBigFont::LCDBigFont(uint8_t rs, uint8_t enable, uint8_t d0, uint8_t d1,
		uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7) :
						LiquidCrystal(rs, enable, d0, d1, d2, d3, d4, d5, d6, d7), _cols(20), _rows(
								4), _curr_col(0), _curr_row(0) {
	createChars();
}

LCDBigFont::LCDBigFont(uint8_t rs, uint8_t rw, uint8_t enable, uint8_t d0,
		uint8_t d1, uint8_t d2, uint8_t d3) :
						LiquidCrystal(rs, rw, enable, d0, d1, d2, d3), _cols(20), _rows(4), _curr_col(
								0), _curr_row(0) {
	createChars();
}

LCDBigFont::LCDBigFont(uint8_t rs, uint8_t enable, uint8_t d0, uint8_t d1,
		uint8_t d2, uint8_t d3) :
						LiquidCrystal(rs, enable, d0, d1, d2, d3), _cols(20), _rows(4), _curr_col(
								0), _curr_row(0) {
	createChars();
}

