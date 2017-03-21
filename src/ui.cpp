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

#define ROTARY_CW 0x53CA
#define ROTARY_CCW 0x3A5C
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R2);

void Ui::begin()
{
	pinMode(A_PIN, INPUT_PULLUP);
	pinMode(ENTER_PIN, INPUT_PULLUP);
	pinMode(B_PIN, INPUT_PULLUP);

	attachInterrupt(digitalPinToInterrupt(A_PIN), Ui::handleInterrupt, CHANGE);
	attachInterrupt(digitalPinToInterrupt(B_PIN), Ui::handleInterrupt, CHANGE);
	// u8g2_SetI2CAddress(u8g2.getU8g2(), 0x3c * 2);
	u8g2.begin();
}

void Ui::handleInterrupt()
{
	static uint8_t state = 0;
	static int8_t localChange = 0;

	uint8_t current = (PIND >> 2) & 0x03;
	state = ((state << 2) | current) & 0x0F;

	bool cw = ROTARY_CW & (1 << state);
	bool ccw = ROTARY_CCW & (1 << state);

	if (cw && ccw) {
		localChange = 0;
	}
	if (cw == ccw) {
		return;
	}
	if (cw) {
		localChange++;
	}
	if (ccw) {
		localChange--;
	}
	ui.change += localChange / 4;
	localChange %= 4;
}

void Ui::eventLoop()
{
	bool done = false;
	if (change != 0) {
		done = true;
		if (inMenu) {
			inMenu = screens[screenIdx]->onChange(change);
		} else {
			screenIdx = (screenIdx + change + screensLen) % screensLen;
		}
	}
	change = 0;

	bool enterPressed = (digitalRead(ENTER_PIN) == LOW);
	if (enterPressed && !previousPressed) {
		done = true;
		if (inMenu) {
			inMenu = screens[screenIdx]->clicked();
		} else {
			inMenu = screens[screenIdx]->enter();
		}

	}
	previousPressed = enterPressed;

	if (!done) return;
	done = false;
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

bool NumberScreen::onChange(int8_t change)
{
	holder += change;
	return true;
}

void BoolScreen::drawContent() const
{
	u8g2.setFont(u8g2_font_10x20_mr);
	const char* text = holder ? "true" : "false";
	u8g2_uint_t width = u8g2.getStrWidth(text);
	u8g2.drawStr((SCREEN_WIDTH - width) / 2, 40, text);
}

bool BoolScreen::onChange(int8_t change)
{
	if (change)
		holder = !holder;
	return true;
}

void PatternScreen::drawContent() const
{
	patternProgression.currentPattern()->draw(u8g2);
}

bool PatternScreen::onChange(int8_t change)
{
	index = (index + size + change) % size;
	Pattern* newPattern = new BuiltInPattern(patterns[index]);
	patternProgression.setPattern(newPattern);
	return true;
}

void CarriageFunctionScreen::drawContent() const
{
	if (!done) {
		return;
	}
	u8g2.setFont(u8g2_font_10x20_mr);
	static const char* text = "done";
	u8g2_uint_t width = u8g2.getStrWidth(text);
	u8g2.drawStr((SCREEN_WIDTH - width) / 2, 40, text);
}

bool CarriageFunctionScreen::enter()
{
	(patternProgression.*method)();
	done = true;
	return false;
}

void AyabScreen::drawContent() const
{
}

bool AyabScreen::enter()
{
	return false;
}
