/*
 * Ayab.cpp
 *
 *  Created on: Feb 10, 2017
 *      Author: romkal
 */
#include <Arduino.h>
#include "Ayab.h"

void Ayab::pingCommand()
{
	uint8_t pingResponse[] {0xc3, 0x03};
	Serial.write(pingResponse, 2);
}

void Ayab::startCommand()
{
	uint8_t startEnd[2];;
	Serial.readBytes(startEnd, 2);
	uint8_t patternStart = startEnd[0];
	patternSize = startEnd[1] - patternStart + 1;
	uint8_t startResponse[] {0xc1, 0x01};
	Serial.write(startResponse, 2);
}

void Ayab::rowCommand()
{
	uint8_t trash;
	Serial.readBytes(&trash, 1); // rowNumber - ignore
	uint8_t bytesToRead = min(24, patternSize / 8) + 1;
	row.size = patternSize;
	row.data = new uint8_t[bytesToRead];
	Serial.readBytes(row.data, bytesToRead);
	for (int i = bytesToRead; i < 25; i++) {
		Serial.readBytes(&trash, 1);
	}
	Serial.readBytes(&row.flags, 1);
	Serial.readBytes(&trash, 1); //crc - ignore
}

void Ayab::communicate()
{
	while(Serial.available()) {
		int command = Serial.read();
		switch (command) {
			case 0x03 :
				pingCommand();
				break;
			case 0x01 :
				startCommand();
				break;
			case 0x42 :
				rowCommand();
				break;
			default:
				// skip byte;
				break;
		}
	}
}

void Ayab::getRow(uint8_t row)
{
	Serial.write(0x82);
	Serial.write(row);
	uint8_t trash;
	Serial.readBytes(&trash, 1);
	rowCommand();
}

void Ayab::eventLoop()
{
	communicate();
}

void AyabPattern::draw(U8G2& display) const
{
	static const char * ayabStr = "Ayab";
	u8g2_uint_t x = 64 - display.getStrWidth(ayabStr) / 2;
	display.drawStr(x, 40, ayabStr);
}



row_t AyabPattern::getRow(uint16_t rowNr) const
{
	ayab.getRow(rowNr);
	return ayab.row;
}
