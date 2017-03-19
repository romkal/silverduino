/*
 * carriage.cpp
 *
 *  Created on: Feb 9, 2017
 *      Author: romkal
 */

#include <Arduino.h>
#include <EEPROM.h>
#include "carriage.h"
#include "patterns.h"
pattern_t::pattern_t(const pattern_t* patternPtr):
		width(pgm_read_byte(&(patternPtr->width))),
		height(pgm_read_word(&(patternPtr->height))),
//		width(patternPtr->width),
//		height(patternPtr->height),
		pattern(patternPtr->pattern){
}

int Carriage::eventLoop(bool goingRight, bool ccp, bool needle1)
{
	bool lastCcp = bitRead(state, CCP_SEEN);
	bitWrite(state, CCP_SEEN, ccp);
	if (!ccp || ccp == lastCcp)
	{
		return -1;
	}

	bool wasGoingRight = bitRead(state, GOING_RIGHT);
	if (goingRight != wasGoingRight) {
		bitClear(state, SEEN_N1);
	} else {
		int direction = goingRight ? 1 : -1;
		currentNeedle += direction;
	}
	if (needle1 && !bitRead(state, SEEN_N1)) {
		bitSet(state, SEEN_N1);
		currentNeedle = 0;
	}

	return patternProgression.needleState(currentNeedle);
}

PatternProgression::PatternProgression():pattern(new BuiltInPattern(PATTERNS[0])) {}

bool PatternProgression::endLine()
{
	if (pattern == NULL)
		return false;
	row++;
	if (row % scale_y == 0) {
		loadRow();
	}
	return true;
}

void PatternProgression::loadRow()
{
	delete(current_row.data);
	current_row = pattern->getRow(row / scale_y);
}

bool PatternProgression::needleState(int16_t needleNr) const
{
	if (needleNr < 0 || needleNr >= current_row.size * scale_x * repeat || pattern == NULL) {
		return true;
	}
	uint8_t bitInPattern = (needleNr / scale_x) % current_row.size;
	bitInPattern = mirror ? (current_row.size - 1) - bitInPattern : bitInPattern;
	uint8_t byte = current_row.data[bitInPattern / 8];
	bool bit = bitRead(byte, bitInPattern % 8);
	return invert ^ bit; // flip bit if invert
}

BuiltInPattern::BuiltInPattern(const pattern_t* patternMem):Pattern(),
	pattern(patternMem)
{
}

row_t BuiltInPattern::getRow(uint16_t rowNr) const {
	row_t result;
	result.size = pattern.width;
	result.flags = (rowNr == pattern.height - 1);
	uint8_t rowBytes = pattern.width / 8 + (pattern.width % 8 != 0);
	result.data = new uint8_t[rowBytes];
	memcpy_P(result.data, pattern.pattern, rowBytes);
	return result;
}

void BuiltInPattern::draw(U8G2& display) const
{
	u8g2_uint_t x = 64 - pattern.width / 2;
	u8g2_uint_t y = 32 + 11 - pattern.height / 2;
	display.drawXBMP(x, y, pattern.width, pattern.height, pattern.pattern);
}

EepromPattern::EepromPattern()
{
	EEPROM.get(0, width);
	EEPROM.get(1, height);
}

row_t EepromPattern::getRow(uint16_t rowNr) const
{
	row_t result;
	result.size = width;
	result.flags = (rowNr >= height - 1);
	int rowSize = (width + 7) / 8;
	result.data = new uint8_t[rowSize];
	for (int i = 0; i < rowSize; i++) {
		*(result.data + i) = EEPROM.read(i + 3);
	}
	return result;
}

void EepromPattern::draw(U8G2& display) const
{
	display.print(F("Preview not available.."));
}

void Pattern::draw(U8G2& display) const
{
	display.print(F("Preview not available.."));
}
