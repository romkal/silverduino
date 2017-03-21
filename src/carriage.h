/*
 * carriage.h
 *
 *  Created on: Feb 9, 2017
 *      Author: romkal
 */
#ifndef SRC_CARRIAGE_H_
#define SRC_CARRIAGE_H_

#include <Arduino.h>
#include <U8g2lib.h>
#define ND1 9 // needle1
#define KSL 8 //  cams
#define DOB 7 // output
#define CCP 6 // clock
#define HOK 5 // direction

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
	virtual void draw(U8G2& display) const;
	virtual row_t getRow(uint16_t rowNr) const = 0;
	virtual ~Pattern() {}
};

struct pattern_t {
		const uint8_t width;
		const uint16_t height;
		const uint8_t* pattern;
		pattern_t(uint8_t width, uint16_t height, const uint8_t* pattern):
			width(width), height(height), pattern(pattern) {}
		pattern_t(const pattern_t*);
};

class BuiltInPattern: public Pattern
{
public:

	BuiltInPattern(const pattern_t*);
	~BuiltInPattern() {};
	virtual row_t getRow(uint16_t rowNr) const;
	virtual void draw(U8G2& display) const;
private:
	const pattern_t pattern;
};

class EepromPattern: public Pattern
{
private:
	uint8_t width;
	uint16_t height;
public:
	EepromPattern();
	virtual row_t getRow(uint16_t rowNr) const;
	virtual void draw(U8G2& display) const;
};

class PatternProgression
{
private:
	uint16_t row = 0;
	row_t current_row;
	const Pattern* pattern;
	void loadRow();
public:
	PatternProgression();
	uint8_t scale_x = 1;
	uint8_t scale_y = 1;
	bool invert = false;
	bool mirror = false;
	uint8_t repeat = 1;
	void setPattern(Pattern* p) {delete pattern; pattern = p; row = 0; loadRow();}
	const Pattern* currentPattern() const {return pattern;}
	bool endLine();
	bool needleState(int16_t needleNr) const;
	void reset() {row = 0; loadRow();}
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
