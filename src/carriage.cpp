/*
 * carriage.cpp
 *
 *  Created on: Feb 9, 2017
 *      Author: romkal
 */

#include <Arduino.h>
#include "carriage.h"
#include "patterns.h"

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

bool PatternProgression::endLine()
{
	row++;
	delete(current_row.data);
	current_row = pattern->getRow(row);

	return true;
}

bool PatternProgression::needleState(int16_t needleNr) const
{
	if (needleNr < 0 || needleNr >= current_row.size) {
		return true;
	}
	uint8_t byte = current_row.data[needleNr / 8];
	return bitRead(byte, needleNr % 8);
}

row_t BuiltInPattern::getRow(uint16_t rowNr) const {
	row_t result;
	result.size = 8;
	result.flags = 1;
	result.data = new uint8_t[8 / 8];
	memcpy_P(result.data, ROMAN, 1);
	return result;
}

void Pattern::draw(U8G2& display)
{
	display.print(F("Preview not available.."));
}
