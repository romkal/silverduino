/*
 * carriage.h
 *
 *  Created on: Feb 9, 2017
 *      Author: romkal
 */
#include <Arduino.h>
#ifndef SRC_CARRIAGE_H_
#define SRC_CARRIAGE_H_

#define ND1 6
#define KSL 5
#define DOB 4
#define CCP 3
#define HOK 2

#define CCP_SEEN 0
#define GOING_RIGHT 1
#define SEEN_N1 2

typedef struct {
	uint8_t* data;
	uint8_t size;
	uint8_t flags;
} row_t;

class Pattern
{
public:
	virtual row_t getRow(uint16_t rowNr) const = 0;
	virtual ~Pattern() {}
};

class BuiltInPattern: public Pattern
{
private:
	const uint8_t* pattern;
	uint8_t width;
	uint16_t height;
public:
	BuiltInPattern(const uint8_t* pattern, uint8_t width, uint16_t height):
		Pattern(), pattern(pattern), width(width), height(height) {}
	~BuiltInPattern() {};
	row_t getRow(uint16_t rowNr) const;
};

class PatternProgression
{
private:
	uint16_t row = 0;
	const Pattern* pattern;
	row_t current_row;
public:
	PatternProgression(const Pattern* pattern): pattern(pattern) {}
	bool endLine();
	bool needleState(int16_t needleNr) const;
};

class Carriage
{
private:
	uint8_t state = 0;
	int8_t currentNeedle;

public:
	int eventLoop(bool goingRight, bool cpp, bool needle1);
};

extern Carriage carriage;
extern PatternProgression patternProgression;
extern Pattern* pattern;

#endif /* SRC_CARRIAGE_H_ */
