/*
 * ui.h
 *
 *  Created on: Feb 9, 2017
 *      Author: romkal
 */
#ifndef SRC_UI_H_
#define SRC_UI_H_
#include <Arduino.h>
#include "carriage.h"
#include <U8g2lib.h>

class Screen
{
private:
	const char * title;
public:
	Screen(const char * title): title(title) {}
	void draw(bool inMenu) const;
	virtual void drawContent() const = 0;
	virtual void onChange(int8_t change) = 0;

};

class NumberScreen:public Screen
{
private:
	uint8_t& holder;
public:
	NumberScreen(const char * title, uint8_t& holder): Screen(title), holder(holder) {};
	virtual void drawContent() const;
	virtual void onChange(int8_t change);
};

class BoolScreen: public Screen
{
private:
	bool& holder;
public:
	BoolScreen(const char * title, bool& holder): Screen(title), holder(holder) {};
	virtual void drawContent() const;
	virtual void onChange(int8_t change);
};

class Ui
{
private:
	uint8_t previousKey = 0;
	bool inMenu = false;
	const Screen** screens;
	uint8_t screensLen;
	uint8_t screenIdx = 0;
public:
	Ui(const Screen** screens, uint8_t screensLen): screens(screens), screensLen(screensLen) {}
	void begin();
	void eventLoop();
};

#endif /* SRC_UI_H_ */
