/*
 * ui.cpp
 *
 *  Created on: Feb 17, 2017
 *      Author: romkal
 */
#include <Arduino.h>
#include "carriage.h"
#include <U8g2lib.h>
#include "ui.h"

#define MAX_TITLE 30
#define SCREEN_WIDTH 128

#define LEFT_PIN 11
#define ENTER_PIN 12
#define RIGHT_PIN 13

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0);

void Ui::begin()
{
	pinMode(LEFT_PIN, INPUT_PULLUP);
	pinMode(ENTER_PIN, INPUT_PULLUP);
	pinMode(RIGHT_PIN, INPUT_PULLUP);

	// u8g2_SetI2CAddress(u8g2.getU8g2(), 0x3c * 2);
	u8g2.begin();
}

void Ui::eventLoop()
{
	const int PINS[] = {LEFT_PIN, ENTER_PIN, RIGHT_PIN};
	int pressed = -1;
	for (int i = 0; i < 3; i++) {
		if (digitalRead(PINS[i]) == LOW) {
			pressed = PINS[i];
		}
	}
	if (previousKey == pressed) {
		return;
	}
	previousKey = pressed;
	int change;
	switch (pressed) {
	case LEFT_PIN:
	case RIGHT_PIN:
		change = pressed == LEFT_PIN ? -1 : 1;
		if (inMenu) {
			screens[screenIdx]->onChange(change);
		} else {
			screenIdx = (screenIdx + change + screensLen) % screensLen;
		}
		break;
	case ENTER_PIN:
		inMenu = !inMenu;
		break;
	default:
		// nothing
		break;
	}
	u8g2.firstPage();
	do {
		screens[screenIdx]->draw(inMenu);
	} while (u8g2.nextPage());
}

void Screen::draw(bool inMenu) const
{
	u8g2.setFont(u8g2_font_10x20_mr);
	u8g2_uint_t width = u8g2.getStrWidth(title);
	u8g2.drawStr((SCREEN_WIDTH - width) / 2, 20, title);
	this->drawContent();
}

void NumberScreen::drawContent() const
{
	u8g2.setFont(u8g2_font_inb24_mn);
	char number[4];
	itoa(holder, number, 10);
	u8g2_uint_t width = u8g2.getStrWidth(number);
	u8g2.drawStr((SCREEN_WIDTH - width) / 2, (64 - 20) / 2 + 20 + 24 / 2, number);
}

void NumberScreen::onChange(int8_t change)
{
	holder += change;
}

void BoolScreen::drawContent() const
{
	u8g2.setFont(u8g2_font_10x20_mr);
	const char* text = holder ? "true" : "false";
	u8g2_uint_t width = u8g2.getStrWidth(text);
	u8g2.drawStr((SCREEN_WIDTH - width) / 2, 40, text);
}

void BoolScreen::onChange(int8_t change)
{
	holder = !holder;
}
