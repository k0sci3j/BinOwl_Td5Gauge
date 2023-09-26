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
 * 	main.cpp
 *  Created on: 15.02.2022
 *  Author: BinOwl
 * 
 */

#define VER "v1.1"
//#define POLISH
//#define LCD1602

#include "Td5Gauge.h"
#include "WEB.h"

void runKeepAlive() {
	if (kline->keepAlive()) {
		lcd->clear();
		ledcWrite(PWM1_CH, lcd_backlight);
#ifdef POLISH
		lcd->setCursor(0, 0);
		lcd->print(F("Sprawdzam"));
#ifdef LCD1602
		lcd->setCursor(0, 1);
#else
		lcd->setCursor(0, 2);
#endif
		lcd->print(F("polaczenie..."));
#else
		lcd->setCursor(0, 0);
		lcd->print(F("Checking"));
#ifdef LCD1602
		lcd->setCursor(0, 1);
#else
		lcd->setCursor(0, 2);
#endif
		lcd->print(F("connection..."));
#endif
		kline->restart();
	}
}

void runKline() {
	if (!run_kline)
		run_kline = true;
}


void runTempAlarm() {
	kline->read_temps();
	if(kline->coolant_temp >= ((int32_t) temp_alarm_set)){
		temp_alarm = 1;
	} else {
		temp_alarm = 0;
	}
}


uint backlight_state = 0;
void runScreenBlink() {
	if(temp_alarm){
		if(backlight_state == 0) {
			ledcWrite(PWM1_CH, 255);
			backlight_state = 1;
		} else {
			ledcWrite(PWM1_CH, 5);
			backlight_state = 0;
		}
	} else {
		ledcWrite(PWM1_CH, lcd_backlight);
	}
}

uint32_t curr_fuel = 0;
void runGetCurrFuel() {
	kline->read_fuel();
	kline->read_speed();
	kline->read_rpm();
	uint32_t curr_fuel_per_100;
	if (kline->speed > 0)
		curr_fuel_per_100 = ((((kline->fuel_injected / 100.0) * kline->rpm * 2.5
				* 60 / 1000000.0 * 0.83) / (kline->speed * speed_multiplier / 100.0))) * 10000;
	else
		curr_fuel_per_100 = (((kline->idle_fuel_demand / 100.0) * kline->rpm * 2.5
						* 60 / 1000000.0 * 0.83) / 100.0) * 10000;
	curr_fuel = curr_fuel_per_100;
}

#define DELAY_LONG_PRESS_TIME_MS 3000
bool check_long_press(uint8_t b) {
	for(uint16_t i=0; i<DELAY_LONG_PRESS_TIME_MS; i++){
		delay(1);
		if(digitalRead(b))
			return false;
	}
	return true;
}

void setup() {
	pinMode(LCD_E, OUTPUT);
	digitalWrite(LCD_E, HIGH);
	lcd = new LCDBigFont(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
#ifdef LCD1602
	lcd->begin(16, 2);
#else
	lcd->begin(20, 4);
#endif
	lcd->noDisplay();
	pinMode(BUTTON_PLUS, INPUT_PULLUP);
	pinMode(BUTTON_MINUS, INPUT_PULLUP);
	pinMode(PIN_TX, OUTPUT);
	pinMode(PIN_RX, OUTPUT);
	digitalWrite(PIN_TX, HIGH);
	digitalWrite(PIN_RX, HIGH);
	ledcAttachPin(LCD_BKL, PWM1_CH);
  	ledcSetup(PWM1_CH, PWM1_F, PWM1_RES);
	
	ledcWrite(PWM1_CH, 0);

    if(!fileSystem.openFromFile(FILE_MENU_POS, curr_func)){
        curr_func=0;
        fileSystem.saveToFile(FILE_MENU_POS, curr_func);
    }
    if(!fileSystem.openFromFile(FILE_SPEED_MULTIPLIER, speed_multiplier)){
        speed_multiplier=100;
        fileSystem.saveToFile(FILE_SPEED_MULTIPLIER, speed_multiplier);
    }
	if(speed_multiplier < 100) {
		speed_multiplier = 100;
        fileSystem.saveToFile(FILE_SPEED_MULTIPLIER, speed_multiplier);
	}
    if(!fileSystem.openFromFile(FILE_TEMP_AL_SET, temp_alarm_set)){
        temp_alarm_set=95;
        fileSystem.saveToFile(FILE_TEMP_AL_SET, temp_alarm_set);
    }
	if(temp_alarm_set < 60) {
		temp_alarm_set = 95;
        fileSystem.saveToFile(FILE_TEMP_AL_SET, temp_alarm_set);
	}
    if(!fileSystem.openFromFile(FILE_AUTO_OFF, auto_off)){
        auto_off=0;
        fileSystem.saveToFile(FILE_AUTO_OFF, auto_off);
    }
    if(!fileSystem.openFromFile(FILE_LCD_BACKLIGHT, lcd_backlight)){
        lcd_backlight=255;
        fileSystem.saveToFile(FILE_LCD_BACKLIGHT, lcd_backlight);
    }
    if(!fileSystem.openFromFile(FILE_RESET, reset_state)){
        reset_state=0;
        fileSystem.saveToFile(FILE_RESET, reset_state);
    }

	if(!digitalRead(BUTTON_PLUS) || !digitalRead(BUTTON_MINUS)){ // Here is Auto OFF reset. After long press on system start Auto OFF will be disabled.
		auto_off=0;
        fileSystem.saveToFile(FILE_AUTO_OFF, auto_off);
		lcd->display();
		ledcWrite(PWM1_CH, lcd_backlight);
		lcd->clear();
		lcd->setCursor(0, 0);
		lcd->print(F("AutoOFF=Off"));
		lcd->setCursor(0, 1);
		RunWebServer();
		delay(500);
		for(int i=36000;i>=0;i--){
			delay(10);
			server.handleClient();
			if(i%10 == 0){
				lcd->clear();
				lcd->setCursor(0, 0);
				lcd->print(F("Firmware"));
				lcd->setCursor(0, 1);
				#ifdef POLISH
				lcd->printf("Czekam %ds...", i/100);
				#else
				lcd->printf("Waiting %ds...", i/100);
				#endif
			}
		}
		ESP.restart();
	}
	if(auto_off && reset_state){
		lcd->noDisplay();
		delay(10000);
		Serial1.begin(10400);
		kline = new KLine();
	}
	else {
		lcd->display();
		ledcWrite(PWM1_CH, lcd_backlight);
		lcd->clear();
		#ifdef LCD1602
		lcd->setCursor(1, 0);
		lcd->print(F("www.binowl.com"));
		#else
		lcd->setCursor(3, 1);
		lcd->print(F("www.binowl.com"));
		#endif
		delay(3000);
		lcd->clear();
		#ifdef LCD1602
		lcd->setCursor(0, 1);
		lcd->print(F(VER));
		#else
		lcd->setCursor(0, 3);
		lcd->print(F(VER));
		#endif
		delay(1000);
		lcd->clear();
#ifdef POLISH
		lcd->setCursor(0, 0);
		lcd->print(F("Inicjalizacja"));
#else
		lcd->setCursor(0, 0);
		lcd->print(F("Initializing"));
#endif
		for(int i=0;i<=2;i++){
			lcd->print('.');
			delay(2000);
		}
		Serial1.begin(10400);
		kline = new KLine();
	}
	lcd->display();
	ledcWrite(PWM1_CH, lcd_backlight);
	
	RunWebServer();

	keepAliveRunner = new RunMillis(2000, runKeepAlive);
	takeValueRunner = new RunMillis(750, runKline);
	currFuelRunner = new RunMillis(5000, runGetCurrFuel);
	tempCheckRunner = new RunMillis(10000, runTempAlarm);
	screenBlinkRunner = new RunMillis(500, runScreenBlink);
}

void voltage_run() {
	kline->read_voltage();
	lcd->clear();
	char val[8];
	dtostrf(kline->volt / 1000.0, 5, 2, val);
#ifdef POLISH
	lcd->setCursor(0, 0);
	lcd->print(F("Napiecie:"));
	ajaxValue = "{\"n\":\"Napiecie\", \"v\":" + String(val) + ",\"u\":\"V\"}";
#else
	lcd->setCursor(0, 0);
	lcd->print(F("Voltage:"));
	ajaxValue = "{\"n\":\"Voltage\", \"v\":" + String(val) + ",\"u\":\"V\"}";
#endif
	lcd->setCursor(0, 1);
#ifdef LCD1602
	lcd->printNumber(val);
	lcd->setCursor(15, 1);
#else
	lcd->printBigNumber(val);
	lcd->setCursor(19, 4);
#endif
	lcd->print('V');
}

void rpm_run() {
	kline->read_rpm();
	lcd->clear();
	char val[6];
	dtostrf(kline->rpm, 5, 0, val);
#ifdef POLISH
	lcd->setCursor(0, 0);
	lcd->print(F("Obroty:"));
	ajaxValue = "{\"n\":\"Obroty silnika\", \"v\":" + String(val) + ",\"u\":\"obr/min\"}";
#else
	lcd->setCursor(0, 0);
	lcd->print(F("Curr.RPM:"));
	ajaxValue = "{\"n\":\"Current RPM\", \"v\":" + String(val) + ",\"u\":\"revs/min\"}";
#endif
	lcd->setCursor(0, 1);
#ifdef LCD1602
	lcd->printNumber(val);
	lcd->setCursor(13,1);
#else
	lcd->printBigNumber(val);
	lcd->setCursor(17, 4);
#endif
	lcd->print(F("rpm"));
}

void coolant_temp_run() {
	kline->read_temps();
	lcd->clear();
	char val[5];
	dtostrf(kline->coolant_temp, 4, 0, val);
#ifdef POLISH
	lcd->setCursor(0, 0);
	lcd->print(F("Temp.Chlod.:"));
	ajaxValue = "{\"n\":\"Temperatura plynu chlodniczego\", \"v\":" + String(val) + ",\"u\":\"&deg;C\"}";
#else
	lcd->setCursor(0, 0);
	lcd->print(F("Cool.Temp:"));
	ajaxValue = "{\"n\":\"Coolant temperature\", \"v\":" + String(val) + ",\"u\":\"&deg;C\"}";
#endif
	lcd->setCursor(0, 1);
#ifdef LCD1602
	lcd->printNumber(val);
	lcd->setCursor(14,1);
#else
	lcd->printBigNumber(val);
	lcd->setCursor(18, 4);
#endif
	lcd->print((char) 223);
	lcd->print('C');
}

void air_temp_run() {
	kline->read_temps();
	lcd->clear();
	char val[5];
	dtostrf(kline->air_temp, 4, 0, val);
#ifdef POLISH
	lcd->setCursor(0, 0);
	lcd->print(F("Temp.Ukl.Dol.:"));
	ajaxValue = "{\"n\":\"Temperatura powietrza\", \"v\":" + String(val) + ",\"u\":\"&deg;C\"}";
#else
	lcd->setCursor(0, 0);
	lcd->print(F("Air Temp:"));
	ajaxValue = "{\"n\":\"Air temperature\", \"v\":" + String(val) + ",\"u\":\"&deg;C\"}";
#endif
	lcd->setCursor(0, 1);
#ifdef LCD1602
	lcd->printNumber(val);
	lcd->setCursor(14,1);
#else
	lcd->printBigNumber(val);
	lcd->setCursor(18, 4);
#endif
	lcd->print((char) 223);
	lcd->print('C');
}

void fuel_temp_run() {
	kline->read_temps();
	lcd->clear();
	char val[5];
	dtostrf(kline->fuel_temp, 4, 0, val);
#ifdef POLISH
	lcd->setCursor(0, 0);
	lcd->print(F("Temp.Paliwo:"));
	ajaxValue = "{\"n\":\"Temperatura paliwa\", \"v\":" + String(val) + ",\"u\":\"&deg;C\"}";
#else
	lcd->setCursor(0, 0);
	lcd->print(F("Fuel Temp:"));
	ajaxValue = "{\"n\":\"Fuel temperature\", \"v\":" + String(val) + ",\"u\":\"&deg;C\"}";
#endif
	lcd->setCursor(0, 1);
#ifdef LCD1602
	lcd->printNumber(val);
	lcd->setCursor(14,1);
#else
	lcd->printBigNumber(val);
	lcd->setCursor(18, 4);
#endif
	lcd->print((char) 223);
	lcd->print('C');
}

void speed_run() {
	kline->read_speed();
	lcd->clear();
	char val[5];
	dtostrf(kline->speed * speed_multiplier / 100.0, 4, 0, val);
#ifdef POLISH
	lcd->setCursor(0, 0);
	lcd->print(F("Predkosc:"));
	ajaxValue = "{\"n\":\"Predkosc\", \"v\":" + String(val) + ",\"u\":\"km/h\"}";
#else
	lcd->setCursor(0, 0);
	lcd->print(F("Speed:"));
	ajaxValue = "{\"n\":\"Speed\", \"v\":" + String(val) + ",\"u\":\"km/h\"}";
#endif
	lcd->setCursor(0, 1);
#ifdef LCD1602
	lcd->printNumber(val);
	lcd->setCursor(12,1);
#else
	lcd->printBigNumber(val);
	lcd->setCursor(16, 4);
#endif
	lcd->print(F("km/h"));
}

void fuel_demand_run() {
	kline->read_fuel();
	lcd->clear();
	char val[8];
	dtostrf(kline->driver_fuel_demand / 100.0, 6, 1, val);
#ifdef POLISH
	lcd->setCursor(0, 0);
	lcd->print(F("Zadanie:"));
	ajaxValue = "{\"n\":\"Zadanie paliwo\", \"v\":" + String(val) + ",\"u\":\"mg/stroke\"}";
#else
	lcd->setCursor(0, 0);
	lcd->print(F("F.Demand:"));
	ajaxValue = "{\"n\":\"Fuel demand\", \"v\":" + String(val) + ",\"u\":\"mg/stroke\"}";
#endif
	lcd->setCursor(0, 1);
#ifdef LCD1602
	lcd->printNumber(val);
	lcd->setCursor(12,1);
#else
	lcd->printBigNumber(val);
	lcd->setCursor(16, 4);
#endif
	lcd->print(F("m/st"));
}

void idle_fuel_run() {
	kline->read_fuel();
	lcd->clear();
	char val[8];
	dtostrf(kline->idle_fuel_demand / 100.0, 6, 1, val);
#ifdef POLISH
	lcd->setCursor(0, 0);
	lcd->print(F("Wtr.Idle:"));
	ajaxValue = "{\"n\":\"Zadanie paliwo na Idle\", \"v\":" + String(val) + ",\"u\":\"mg/stroke\"}";
#else
	lcd->setCursor(0, 0);
	lcd->print(F("Idle Fuel:"));
	ajaxValue = "{\"n\":\"Fuel demand on Idle\", \"v\":" + String(val) + ",\"u\":\"mg/stroke\"}";
#endif
	lcd->setCursor(0, 1);
#ifdef LCD1602
	lcd->printNumber(val);
	lcd->setCursor(12,1);
#else
	lcd->printBigNumber(val);
	lcd->setCursor(16, 4);
#endif
	lcd->print(F("m/st"));
}

void injected_fuel_run() {
	kline->read_fuel();
	lcd->clear();
#ifdef POLISH
	lcd->setCursor(0, 0);
	lcd->print(F("Wtrysk Calk.:"));
	char val[8];
	dtostrf(kline->fuel_injected / 100.0, 6, 1, val);
	ajaxValue = "{\"n\":\"Wtrysk paliwa całkowity\", \"v\":" + String(val) + ",\"u\":\"mg/stroke\"}";
#else
	lcd->setCursor(0, 0);
	lcd->print(F("Fuel Inj.:"));
	char val[8];
	dtostrf(kline->fuel_injected / 100.0, 6, 1, val);
	ajaxValue = "{\"n\":\"Fuel injected\", \"v\":" + String(val) + ",\"u\":\"mg/stroke\"}";
#endif
	lcd->setCursor(0, 1);
#ifdef LCD1602
	lcd->printNumber(val);
	lcd->setCursor(12,1);
#else
	lcd->printBigNumber(val);
	lcd->setCursor(16, 4);
#endif
	lcd->print(F("m/st"));
}

void maf_sensor_run() {
	kline->read_maf_map();
	lcd->clear();
	char val[7];
	dtostrf(kline->maf / 10.0, 4, 2, val);
#ifdef POLISH
	lcd->setCursor(0, 0);
	lcd->print(F("MAF:"));
	ajaxValue = "{\"n\":\"Masa powietrza ukladu dolotowego (MAF)\", \"v\":" + String(val) + ",\"u\":\"kg/h\"}";
#else
	lcd->setCursor(0, 0);
	lcd->print(F("MAF:"));
	ajaxValue = "{\"n\":\"Mass Air Flow (MAF)\", \"v\":" + String(val) + ",\"u\":\"kg/h\"}";
#endif
	lcd->setCursor(0, 1);
#ifdef LCD1602
	lcd->printNumber(val);
	lcd->setCursor(12,1);
#else
	lcd->printBigNumber(val);
	lcd->setCursor(16, 4);
#endif
	lcd->print(F("kg/h"));
}

void aap_sensor_run() {
	kline->read_maf_map();
	lcd->clear();
	char val[8];
	dtostrf(kline->map / 100.0, 6, 1, val);
#ifdef POLISH
	lcd->setCursor(0, 0);
	lcd->print(F("MAP:"));
	ajaxValue = "{\"n\":\"Cisnienie powietrza w ukladzie dolotowym (MAP)\", \"v\":" + String(val) + ",\"u\":\"kPa\"}";
#else
	lcd->setCursor(0, 0);
	lcd->print(F("MAP:"));
	ajaxValue = "{\"n\":\"Manifold air pressure (MAP)\", \"v\":" + String(val) + ",\"u\":\"kPa\"}";
#endif
	lcd->setCursor(0, 1);
#ifdef LCD1602
	lcd->printNumber(val);
	lcd->setCursor(13,1);
#else
	lcd->printBigNumber(val);
	lcd->setCursor(17, 4);
#endif
	lcd->print(F("kPa"));
}

void press1_sensor_run() {
	kline->read_pressure();
	lcd->clear();
	char val[8];
	dtostrf(kline->press1 / 100.0, 6, 1, val);
#ifdef POLISH
	lcd->setCursor(0, 0);
	lcd->print(F("AAP:"));
	ajaxValue = "{\"n\":\"Cisnienie atmosferyczne (AAP)\", \"v\":" + String(val) + ",\"u\":\"kPa\"}";
#else
	lcd->setCursor(0, 0);
	lcd->print(F("AAP:"));
	ajaxValue = "{\"n\":\"Ambient air pressure (AAP)\", \"v\":" + String(val) + ",\"u\":\"kPa\"}";
#endif
	lcd->setCursor(0, 1);
#ifdef LCD1602
	lcd->printNumber(val);
	lcd->setCursor(13,1);
#else
	lcd->printBigNumber(val);
	lcd->setCursor(17, 4);
#endif
	lcd->print(F("kPa"));
}

void press2_sensor_run() {
	kline->read_pressure();
	lcd->clear();
	char val[8];
	dtostrf(kline->press2 / 100.0, 6, 1, val);
#ifdef POLISH
	lcd->setCursor(0, 0);
	lcd->print(F("AAP RAW:"));
	ajaxValue = "{\"n\":\"Cisnienie atmosferyczne dane bezposrednie (AAP-RAW)\", \"v\":" + String(val) + ",\"u\":\"kPa\"}";
#else
	lcd->setCursor(0, 0);
	lcd->print(F("AAP RAW:"));
	ajaxValue = "{\"n\":\"Ambient air pressure RAW data (AAP-RAW)\", \"v\":" + String(val) + ",\"u\":\"kPa\"}";
#endif
	lcd->setCursor(0, 1);
#ifdef LCD1602
	lcd->printNumber(val);
	lcd->setCursor(13,1);
#else
	lcd->printBigNumber(val);
	lcd->setCursor(17, 4);
#endif
	lcd->print(F("kPa"));
}

void press3_sensor_run() {
	kline->read_maf_map();
	lcd->clear();
	char val[8];
	dtostrf(kline->press3 / 100.0, 6, 1, val);
#ifdef POLISH
	lcd->setCursor(0, 0);
	lcd->print(F("MAP RAW:"));
	ajaxValue = "{\"n\":\"Cisnienie powietrza w ukladzie dolotowym dane bezposrednie (MAP-RAW)\", \"v\":" + String(val) + ",\"u\":\"kPa\"}";
#else
	lcd->setCursor(0, 0);
	lcd->print(F("MAP RAW:"));
	ajaxValue = "{\"n\":\"Manifold air pressure RAW data (MAP-RAW)\", \"v\":" + String(val) + ",\"u\":\"kPa\"}";
#endif
	lcd->setCursor(0, 1);
#ifdef LCD1602
	lcd->printNumber(val);
	lcd->setCursor(13,1);
#else
	lcd->printBigNumber(val);
	lcd->setCursor(17, 4);
#endif
	lcd->print(F("kPa"));
}

void turbo_charge_run() {
	kline->read_maf_map();
	kline->read_pressure();
	char val[8];
	dtostrf((kline->map - kline->press1) / 100.0, 6, 1, val);
	lcd->clear();
#ifdef POLISH
	lcd->setCursor(0, 0);
	lcd->print(F("Turbo:"));
	ajaxValue = "{\"n\":\"Cisnienie doladowania\", \"v\":" + String(val) + ",\"u\":\"kPa\"}";
#else
	lcd->setCursor(0, 0);
	lcd->print(F("Boost:"));
	ajaxValue = "{\"n\":\"Turbo boost\", \"v\":" + String(val) + ",\"u\":\"kPa\"}";
#endif
	lcd->setCursor(0, 1);
#ifdef LCD1602
	lcd->printNumber(val);
	lcd->setCursor(13,1);
#else
	lcd->printBigNumber(val);
	lcd->setCursor(17, 4);
#endif
	lcd->print(F("kPa"));
}

void wastegate_mod_run() {
	kline->read_wastegate();
	lcd->clear();
	char val[8];
	dtostrf(kline->wastegate / 1000.0, 6, 1, val);
#ifdef POLISH
	lcd->setCursor(0, 0);
	lcd->print(F("Wastegate:"));
	ajaxValue = "{\"n\":\"Sterowanie zaworu upustowego turbiny\", \"v\":" + String(val) + ",\"u\":\"%\"}";
#else
	lcd->setCursor(0, 0);
	lcd->print(F("Wastegate:"));
	ajaxValue = "{\"n\":\"Turbo wastegate modulator\", \"v\":" + String(val) + ",\"u\":\"%\"}";
#endif
	lcd->setCursor(0, 1);
#ifdef LCD1602
	lcd->printNumber(val);
	lcd->setCursor(15,1);
#else
	lcd->printBigNumber(val);
	lcd->setCursor(19, 4);
#endif
	lcd->print('%');
}

void inj_bal_run() {
	kline->read_inj_balance();
	lcd->clear();
	String val = "\"1: "+String(kline->inj_bal1)+"<br />2: "+String(kline->inj_bal2)+"<br />3: "+String(kline->inj_bal3)+"<br />4: "+String(kline->inj_bal4)+"<br />5: "+String(kline->inj_bal5)+"\"";
#ifdef POLISH
	lcd->setCursor(0, 0);
	lcd->print(F("Wtr.Balans:"));
	ajaxValue = "{\"n\":\"Balans wtryskiwaczy\", \"v\":" + val + ",\"u\":\"\"}";
#else
	lcd->setCursor(0, 0);
	lcd->print(F("Inj.Balance:"));
	ajaxValue = "{\"n\":\"Injector balance\", \"v\":" + val + ",\"u\":\"\"}";
#endif
	lcd->setCursor(0,1);
#ifdef LCD1602
	lcd->print(kline->inj_bal1);
	lcd->setCursor(3,1);
	lcd->print(kline->inj_bal2);
	lcd->setCursor(6,1);
	lcd->print(kline->inj_bal3);
	lcd->setCursor(9,1);
	lcd->print(kline->inj_bal4);
	lcd->setCursor(12,1);
	lcd->print(kline->inj_bal5);
#else
	lcd->print(F("1:"));
	lcd->setCursor(3, 1);
	lcd->print(kline->inj_bal1);

	lcd->setCursor(0, 2);
	lcd->print(F("3:"));
	lcd->setCursor(3, 2);
	lcd->print(kline->inj_bal3);

	lcd->setCursor(10, 1);
	lcd->print(F("2:"));
	lcd->setCursor(13, 1);
	lcd->print(kline->inj_bal2);

	lcd->setCursor(10, 2);
	lcd->print(F("4:"));
	lcd->setCursor(13, 2);
	lcd->print(kline->inj_bal4);

	lcd->setCursor(0, 3);
	lcd->print(F("5:"));
	lcd->setCursor(3, 3);
	lcd->print(kline->inj_bal5);
#endif

}

void throttle_run() {
	kline->read_throttle();
	lcd->clear();
	String valAjax = "\"P1: "+String(kline->thr1 / 1000.0)+"<br />P2: "+String(kline->thr2 / 1000.0)+"<br />P3: "+String(kline->thr3 / 1000.0)+"<br />Supply: "+String(kline->supp / 1000.0)+"\"";
#ifdef POLISH
	lcd->setCursor(0, 0);
	lcd->print(F("Akcelerator:"));
	ajaxValue = "{\"n\":\"Dane akceleratora\", \"v\":" + valAjax + ",\"u\":\"\"}";
#else
	lcd->setCursor(0, 0);
	lcd->print(F("Throttle:"));
	ajaxValue = "{\"n\":\"Throttle data\", \"v\":" + valAjax + ",\"u\":\"\"}";
#endif
	lcd->setCursor(0, 1);
#ifdef LCD1602
	char val[5];
	dtostrf(kline->thr1 / 1000.0, 3, 1, val);
	lcd->setCursor(0,1);
	lcd->printNumber(val);
	dtostrf(kline->thr2 / 1000.0, 3, 1, val);
	lcd->setCursor(4,1);
	lcd->printNumber(val);
	dtostrf(kline->thr3 / 1000.0, 3, 1, val);
	lcd->setCursor(8,1);
	lcd->printNumber(val);
	dtostrf(kline->supp / 1000.0, 3, 1, val);
	lcd->setCursor(12,1);
	lcd->printNumber(val);

#else
	lcd->print(F("P1:"));
	lcd->setCursor(4, 1);
	char val[6];
	dtostrf(kline->thr1 / 1000.0, 3, 2, val);
	lcd->printNumber(val);

	lcd->setCursor(0, 2);
	lcd->print(F("P3:"));
	dtostrf(kline->thr3 / 1000.0, 3, 2, val);
	lcd->setCursor(4, 2);
	lcd->printNumber(val);

	lcd->setCursor(10, 1);
	lcd->print(F("P2:"));
	lcd->setCursor(14, 1);
	dtostrf(kline->thr2 / 1000.0, 3, 2, val);
	lcd->printNumber(val);

	lcd->setCursor(0, 3);
	lcd->print(F("Supply:"));
	lcd->setCursor(8, 3);
	dtostrf(kline->supp / 1000.0, 3, 2, val);
	lcd->printNumber(val);
#endif

}

void fuel_consumption() {
	kline->read_fuel();
	kline->read_rpm();
	lcd->clear();
#ifdef POLISH
	lcd->setCursor(0, 0);
	lcd->print(F("Chwil.Spalanie:"));
#else
	lcd->setCursor(0, 0);
	lcd->print(F("Fuel cons.:"));
#endif
	lcd->setCursor(0, 2);
	char val[8];
	float f = (kline->fuel_injected / 100.0) * kline->rpm * 2.5 * 60 / 1000000.0 * 0.83;
	dtostrf(f, 5, 2, val);
	lcd->setCursor(0, 1);
#ifdef LCD1602
	lcd->printNumber(val);
	lcd->setCursor(13,1);
#else
	lcd->printBigNumber(val);
	lcd->setCursor(17, 4);
#endif
	lcd->print(F("l/h"));
}

void fuel_consumption_current_display() {
	lcd->clear();
#ifdef POLISH
	lcd->setCursor(0, 0);
	lcd->print(F("Spalanie:"));
#else
	lcd->setCursor(0, 0);
	lcd->print(F("Fuel Cons:"));
#endif
	lcd->setCursor(0, 2);
	char val[8];
	float f = curr_fuel / 100.0;
	if(f > 100)
		f = 99.99;
	dtostrf(f, 5, 2, val);
	lcd->setCursor(0, 1);
#ifdef LCD1602
	lcd->printNumber(val);
	lcd->setCursor(11,1);
#else
	lcd->printBigNumber(val);
	lcd->setCursor(15, 4);
#endif
	lcd->print(F("l/100"));
}

#define FUNCTIONS 19

void show_confirmation() {
	lcd->clear();
	lcd->setCursor(0, 0);
	lcd->print(F("OK!"));
	delay(3000);
}

//Speed multiplier jest przechowywany w zmiennej byte jako całkowity.
//Zakres od 1 - 1.3 zrobić jako zakres 100 - 130 i podzielić przez 100.
void set_speed_multiplier() {
	ledcWrite(PWM1_CH, lcd_backlight);
	lcd->clear();
	#ifdef POLISH
		lcd->setCursor(0, 0);
		lcd->print(F("Mnoznik:"));
	#else
		lcd->setCursor(0, 0);
		lcd->print(F("Speed mul.:"));
	#endif
	while(!digitalRead(BUTTON_PLUS)) {
		delay(1000);
		if(speed_multiplier < 130)
			++speed_multiplier;
		else
			speed_multiplier = 130;
		char val[7];
		dtostrf(speed_multiplier / 100.0, 4, 2, val);
		lcd->setCursor(0, 1);
#ifdef LCD1602
		lcd->printNumber(val);
#else
		lcd->printBigNumber(val);
#endif
	}
	while(!digitalRead(BUTTON_MINUS)) {
		delay(1000);
		if(speed_multiplier > 100)
			--speed_multiplier;
		else
			speed_multiplier = 100;
		char val[7];
		dtostrf(speed_multiplier / 100.0, 4, 2, val);
		lcd->setCursor(0, 1);
#ifdef LCD1602
		lcd->printNumber(val);
#else
		lcd->printBigNumber(val);
#endif
	}
    fileSystem.saveToFile(FILE_SPEED_MULTIPLIER, speed_multiplier);
	show_confirmation();
}

#define MENU_FUNC_SPEED_MULTIPLIER 5
#define MENU_FUNC_AUTO_OFF 10
#define MENU_FUNC_TEMP_ALARM 8

//Backlight PWM jest przechowywany w zmiennej byte jako całkowity.
//Zakres od 0 - 255.
void set_lcd_backlight() {
	ledcWrite(PWM1_CH, lcd_backlight);
	lcd->clear();
	#ifdef POLISH
		lcd->setCursor(0, 0);
		lcd->print(F("Podswietlenie:"));
	#else
		lcd->setCursor(0, 0);
		lcd->print(F("Backlight:"));
	#endif
	while(!digitalRead(BUTTON_PLUS)) {
		delay(32);
		if(lcd_backlight < 255)
			lcd_backlight += 1;
		else
			lcd_backlight = 255;
		ledcWrite(PWM1_CH, lcd_backlight);
		char val[5];
		uint v = map(lcd_backlight, 0, 255, 0, 100);
		dtostrf(v, 4, 0, val);
#ifdef LCD1602
		lcd->setCursor(0, 1);
		lcd->printNumber(val);
#else
		if(v == 9){ //Clear screen on change from double cipher number to one cipher number
			lcd->clear();
			#ifdef POLISH
			lcd->setCursor(0, 0);
			lcd->print(F("Podswietlenie:"));
			#else
			lcd->setCursor(0, 0);
			lcd->print(F("Backlight:"));
			#endif
		}
		lcd->setCursor(0, 1);
		lcd->printBigNumber(val);
#endif
	}
	while(!digitalRead(BUTTON_MINUS)) {
		delay(32);
		if(lcd_backlight > 0)
			lcd_backlight -= 1;
		else
			lcd_backlight = 0;
		ledcWrite(PWM1_CH, lcd_backlight);
		char val[5];
		uint v = map(lcd_backlight, 0, 255, 0, 100);
		dtostrf(v, 4, 0, val);
		lcd->setCursor(0, 1);
#ifdef LCD1602
		lcd->setCursor(0, 1);
		lcd->printNumber(val);
#else
		if(v == 9){
			lcd->clear();
			#ifdef POLISH
			lcd->setCursor(0, 0);
			lcd->print(F("Podswietlenie:"));
			#else
			lcd->setCursor(0, 0);
			lcd->print(F("Backlight:"));
			#endif
		}
		lcd->setCursor(0, 1);
		lcd->printBigNumber(val);
#endif
	}
    fileSystem.saveToFile(FILE_LCD_BACKLIGHT, lcd_backlight);
	show_confirmation();
}

// Auto OFF is done by LCD disable and periodic check of ECU availability on K-Line
void set_auto_off() {
	ledcWrite(lcd_backlight, 0);
	lcd->clear();
	lcd->setCursor(0, 0);
	lcd->print(F("Auto off:"));
	while(!digitalRead(BUTTON_PLUS) || !digitalRead(BUTTON_MINUS)) {
		lcd->setCursor(0, 1);
		if(auto_off == 1) {
			auto_off = 0; // We want to keep screen on on first run - Default value is 0
			lcd->print(F("OFF"));
		}
		else {
			auto_off = 1;
			lcd->print(F("ON "));
		}
		delay(2000);
	}
    fileSystem.saveToFile(FILE_AUTO_OFF, auto_off);
	show_confirmation();
}

// Temperature alarm - Periodically blink screen on >temp_alarm_set
void set_temp_alarm() {
	ledcWrite(PWM1_CH, lcd_backlight);
	lcd->clear();
	lcd->setCursor(0, 0);
	lcd->print(F("Max.Temp:"));
	while(!digitalRead(BUTTON_PLUS)) {
		uint t = temp_alarm_set;
		if(t < 140)
			t += 1;
		else
			t = 140;
		if(t == 99){
			lcd->clear();
			lcd->setCursor(0, 0);
			lcd->print(F("Max.Temp:"));
		}
		char val[4];
		dtostrf(t, 3, 0, val);
#ifdef LCD1602
		lcd->setCursor(0, 1);
		lcd->printNumber(val);
#else
		lcd->setCursor(0, 1);
		lcd->printBigNumber(val);
#endif
		temp_alarm_set = t;
		delay(500);
	}
	while(!digitalRead(BUTTON_MINUS)) {
		uint t = temp_alarm_set;
		if(t > 60)
			t -= 1;
		else
			t = 60;
		if(t == 99){
			lcd->clear();
			lcd->setCursor(0, 0);
			lcd->print(F("Max.Temp:"));
		}
		char val[4];
		dtostrf(t, 3, 0, val);
#ifdef LCD1602
		lcd->setCursor(0, 1);
		lcd->printNumber(val);
#else
		lcd->setCursor(0, 1);
		lcd->printBigNumber(val);
#endif
		temp_alarm_set = t;
		delay(500);
	}
    fileSystem.saveToFile(FILE_TEMP_AL_SET, temp_alarm_set);
	show_confirmation();
}

void runCheckButton() {
	if (!digitalRead(BUTTON_PLUS)) {
		delay(50); //anti flicker
		if (!digitalRead(BUTTON_PLUS)) {
			switch(curr_func) {
			case MENU_FUNC_SPEED_MULTIPLIER:
				if(check_long_press(BUTTON_PLUS)){
					set_speed_multiplier();
					return;
				}
				break;
			case MENU_FUNC_AUTO_OFF:
				if(check_long_press(BUTTON_PLUS)){
					set_auto_off();
					return;
				}
				break;
			case MENU_FUNC_TEMP_ALARM:
				if(check_long_press(BUTTON_PLUS)){
					set_temp_alarm();
					return;
				}
				break;
			default:
				if(check_long_press(BUTTON_PLUS)){
					set_lcd_backlight();
					return;
				}
				break;
			}
			run_kline = true;
			if (curr_func < FUNCTIONS)
				++curr_func;
			else
				curr_func = 0;
            fileSystem.saveToFile(FILE_MENU_POS, curr_func);
		}
	}
	if (!digitalRead(BUTTON_MINUS)) {
		delay(50); //anti flicker
		if (!digitalRead(BUTTON_MINUS)) {
			switch(curr_func) {
			case MENU_FUNC_SPEED_MULTIPLIER:
				if(check_long_press(BUTTON_MINUS)){
					set_speed_multiplier();
					return;
				}
				break;
			case MENU_FUNC_AUTO_OFF:
				if(check_long_press(BUTTON_MINUS)){
					set_auto_off();
					return;
				}
				break;
			case MENU_FUNC_TEMP_ALARM:
				if(check_long_press(BUTTON_MINUS)){
					set_temp_alarm();
					return;
				}
				break;
			default:
				if(check_long_press(BUTTON_MINUS)){
					set_lcd_backlight();
					return;
				}
				break;
			}
			run_kline = true;
			if (curr_func > 0)
				--curr_func;
			else
				curr_func = FUNCTIONS;
            
            fileSystem.saveToFile(FILE_MENU_POS, curr_func);
		}
	}
}

typedef void (*FP)();
const FP funcs[] = { &fuel_consumption, &fuel_consumption_current_display,
		&fuel_demand_run, &idle_fuel_run,
		&injected_fuel_run, &speed_run, &fuel_temp_run,
		&air_temp_run, &coolant_temp_run, &rpm_run, &voltage_run,
		&maf_sensor_run, &aap_sensor_run, &press1_sensor_run,
		&press2_sensor_run, &press3_sensor_run, &turbo_charge_run, &wastegate_mod_run, &inj_bal_run, &throttle_run };

void loop() {
	keepAliveRunner->run();
	takeValueRunner->run();
	currFuelRunner->run();
	tempCheckRunner->run();
	screenBlinkRunner->run();
	runCheckButton();
	if (run_kline) {
		funcs[curr_func]();
		run_kline = false;
	}
	uint r;
	fileSystem.openFromFile(FILE_RESET, r);
	if(r == 1){
		r = 0;
    	fileSystem.saveToFile(FILE_RESET, r);
    }
	server.handleClient();
}
