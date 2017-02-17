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
	void (*rowCallback)(row_t) = NULL;
	unsigned long rowRequestedMillis = 0;
	uint8_t patternSize = 0;
	void communicate();
	void rowCommand();
	void startCommand();
	void pingCommand();
public:
	Ayab();
	virtual ~Ayab();
	void eventLoop();
	void getRow(uint8_t row, void (*callback)(row_t));
};

#endif /* SRC_AYAB_H_ */
