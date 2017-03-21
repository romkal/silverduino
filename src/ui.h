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
#include "Ayab.h"

#define MAX_TITLE 30
#define SCREEN_WIDTH 128

#define A_PIN 2
#define B_PIN 3
#define ENTER_PIN A0

class Screen
{
private:
	const char * title;
public:
	Screen(const char * title): title(title) {}
	void draw(bool inMenu) const;
	virtual void drawContent() const = 0;
	virtual bool onChange(int8_t change) {return true;}
	virtual bool enter() {return true;}
	virtual bool clicked() {return false;}
	virtual void leave() {};
};

class NumberScreen:public Screen
{
private:
	uint8_t& holder;
public:
	NumberScreen(const char * title, uint8_t& holder): Screen(title), holder(holder) {};
	virtual void drawContent() const;
	virtual bool onChange(int8_t change);
};

class BoolScreen: public Screen
{
private:
	bool& holder;
public:
	BoolScreen(const char * title, bool& holder): Screen(title), holder(holder) {};
	virtual void drawContent() const;
	virtual bool onChange(int8_t change);
};

class PatternScreen: public Screen
{
private:
	const pattern_t* const* patterns;
	const size_t size;
	PatternProgression& patternProgression;
	int index = 0;
public:
	PatternScreen(const char* title, const pattern_t* const* patterns, size_t size, PatternProgression& pattern):
		Screen(title), patterns(patterns), size(size), patternProgression(pattern) {};
	virtual void drawContent() const;
	virtual bool onChange(int8_t change);
};

class CarriageFunctionScreen: public Screen
{
private:
	void (PatternProgression::*method)();
	bool done = false;
public:
	CarriageFunctionScreen(const char* title, void (PatternProgression::*method)()): Screen(title), method(method) {};
	virtual void drawContent() const;
	virtual bool enter();
	virtual void leave() {done = false;}
};

class AyabScreen: public Screen
{
private:
	Ayab& ayab;
	bool done = false;
public:
	AyabScreen(Ayab& ayab): Screen("Store ayab"), ayab(ayab) {};
	virtual void drawContent() const;
	virtual bool enter();
	virtual void leave() {done = false;}
};

class Ui
{
private:
	bool previousPressed = false;
	volatile int8_t change = 0;
	bool inMenu = false;
	Screen** screens;
	uint8_t screensLen;
	uint8_t screenIdx = 0;
public:
	Ui(Screen** screens, uint8_t screensLen): screens(screens), screensLen(screensLen) {}
	void begin();
	void eventLoop();
	static void handleInterrupt();
};

extern Ui ui;

#endif /* SRC_UI_H_ */
