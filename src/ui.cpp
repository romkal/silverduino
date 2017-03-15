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

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0);

void ui_int_a()
{
	ui.handleInterrupt(1);
}

void ui_int_b()
{
	ui.handleInterrupt(-1);
}

void Ui::begin()
{
	pinMode(A_PIN, INPUT_PULLUP);
	pinMode(ENTER_PIN, INPUT_PULLUP);
	pinMode(B_PIN, INPUT_PULLUP);

	attachInterrupt(digitalPinToInterrupt(A_PIN), ui_int_a, CHANGE);
	attachInterrupt(digitalPinToInterrupt(A_PIN), ui_int_b, CHANGE);
	// u8g2_SetI2CAddress(u8g2.getU8g2(), 0x3c * 2);
	u8g2.begin();
}

void Ui::handleInterrupt(int8_t change)
{

}

void Ui::eventLoop()
{
	if (change != 0) {
		if (inMenu) {
			inMenu = screens[screenIdx]->onChange(change);
		} else {
			screenIdx = (screenIdx + change + screensLen) % screensLen;
		}
	}

	bool enterPressed = (digitalRead(ENTER_PIN) == LOW);
	if (enterPressed && !previousPressed) {
		if (inMenu) {
			inMenu = screens[screenIdx]->clicked();
		} else {
			inMenu = screens[screenIdx]->enter();
		}

	}
	previousPressed = enterPressed;

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
	patternProgression.currentPattern().draw(u8g2);
}

bool PatternScreen::onChange(int8_t change)
{
	index = (index + size + change) % size;
	Pattern* newPattern = new BuiltInPattern(&(patterns[index]));
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
