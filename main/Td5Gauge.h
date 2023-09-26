/*
 * Td5Gauge.h
 *
 *  Copyright BinOwl 2022
 *
 *  Created on: 20.01.2022
 *      Author: BinOwl
 */



#ifndef TD5GAUGE_H_
#define TD5GAUGE_H_

#include "Arduino.h"
#include "KLine.h"
#include "RunMillis.h"
#include "LCDBigFont.h"
#include "Effortless_SPIFFS.h"

#define LCD_BKL 33
#define PWM1_CH    0
#define PWM1_RES   8
#define PWM1_F  1000

#define BUTTON_PLUS 22
#define BUTTON_MINUS 19

#define PIN_TX 10
#define PIN_RX 9

#define LCD_RS 13
#define LCD_E 12
#define LCD_D4 14
#define LCD_D5 27
#define LCD_D6 26
#define LCD_D7 25

#define FILE_MENU_POS "/0.cfg"
#define FILE_SPEED_MULTIPLIER "/1.cfg"
#define FILE_LCD_BACKLIGHT "/2.cfg"
#define FILE_AUTO_OFF "/3.cfg"
#define FILE_RESET "/4.cfg"
#define FILE_TEMP_AL_SET "/5.cfg"


eSPIFFS fileSystem;
KLine *kline;
RunMillis *keepAliveRunner;
RunMillis *takeValueRunner;
RunMillis *currFuelRunner;
RunMillis *tempCheckRunner;
RunMillis *screenBlinkRunner;
LCDBigFont *lcd;


uint curr_func = 0;
uint run_kline = true;

uint speed_multiplier;
uint lcd_backlight;
uint auto_off;
uint reset_state;
uint temp_alarm_set;
uint temp_alarm;


#endif /* TD5GAUGE_H_ */