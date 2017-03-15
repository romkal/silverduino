/*
 * Ayab.h
 *
 *  Created on: Feb 10, 2017
 *      Author: romkal
 */
#ifndef SRC_AYAB_H_
#define SRC_AYAB_H_
#include <Arduino.h>
#include "carriage.h"

class Ayab {
private:
	unsigned long rowRequestedMillis = 0;
	uint8_t patternSize = 0;
	void communicate();
	void rowCommand();
	void startCommand();
	void pingCommand();
public:
	row_t row;
	virtual ~Ayab() {};
	void eventLoop();
	void getRow(uint8_t row_nr);
};

class AyabPattern : public Pattern
{
private:
	Ayab& ayab;
public:
	AyabPattern(Ayab& ayab): Pattern(), ayab(ayab) {};
	virtual void draw(U8G2& display) const;
	virtual row_t getRow(uint16_t rowNr) const;
};
#endif /* SRC_AYAB_H_ */
