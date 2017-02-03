/*
 * main.cpp
 *
 *  Created on: Jan 26, 2017
 *      Author: romkal
 */

#include <Arduino.h>

const int ND1 = 6;
const int KSL = 5;
const int DOB = 4;
const int CCP = 3; // Has to be 1 or 2
const int HOK = 2;
const int LED = LED_BUILTIN;

const uint8_t* PING_RESPONSE = new uint8_t[2] {0xc3, 0x03};
const uint8_t* START_RESPONSE = new uint8_t[2] {0xc1, 0x01};
const int MAX = 200;

const int STATE_START = 1;
const int STATE_REQUEST_ROW = 2;
const int STATE_ROW_REQUESTED = 3;
const int STATE_KNIT = 4;
const int STATE_LAST = 5;

uint8_t* patternBuffer = new uint8_t[MAX / 8];
int patternSize;
int row = 0;
int state = STATE_START;
int patternStart = 0;
int currentNeedle = 0;
bool wasBetweenCams = false;
bool wasCcp;
bool seenNeedle1 = false;
unsigned long rowRequestedMillis = 0;

void updatePosition(bool goingRight) {
	int direction = (goingRight ? 1 : -1);
	currentNeedle += direction;
}

bool patternNeedle()
{
	if ((state != STATE_KNIT && state != STATE_LAST)) {
		return false;
	}
	int patternBitNr = (currentNeedle % patternSize);
	if (patternBitNr < 0)
	{
		patternBitNr += patternSize;
	}
	patternBitNr += patternStart;
	uint8_t patternByte = patternBuffer[patternBitNr / 8];
	return patternByte & 1 << (patternBitNr % 8);
}

void onCams(bool goingRight)
{
	// Left cams
	if (wasBetweenCams) {
		switch (state) {
		case STATE_KNIT:
			row++;
			state = STATE_REQUEST_ROW;
			break;
		case STATE_LAST:
			state = STATE_START;
			seenNeedle1 = false;
			break;
		case STATE_START:
			break;
		case STATE_REQUEST_ROW:
		case STATE_ROW_REQUESTED:
			// BAD
			break;
		}
		currentNeedle += (goingRight ? 1 : -1);
	}
}

void onNeedle1()
{
	currentNeedle = 0;
	seenNeedle1 = true;
}

void setup()
{
	  pinMode(ND1, INPUT);
	  pinMode(KSL, INPUT);
	  pinMode(DOB, OUTPUT);
	  pinMode(CCP, INPUT);
	  pinMode(HOK, INPUT);
	  pinMode(LED, OUTPUT);

	  digitalWrite(DOB, LOW);
	  Serial.begin(115200);
}


void pingCommand()
{
	Serial.write(PING_RESPONSE, 2);
}

void startCommand()
{
	uint8_t* startEnd = new uint8_t[2];
	Serial.readBytes(startEnd, 2);
	patternStart = startEnd[0];
	patternSize = startEnd[1] - patternStart + 1;
	Serial.write(START_RESPONSE, 2);
	row = 0;
	state = STATE_REQUEST_ROW;
}

void rowCommand()
{
	uint8_t trash;
	Serial.readBytes(&trash, 1); // rowNumber - ignore
	Serial.readBytes(patternBuffer, MAX / 8);
	uint8_t flags;
	Serial.readBytes(&flags, 1);
	state = flags ? STATE_LAST : STATE_KNIT;
	Serial.readBytes(&trash, 1); //crc - ignore
}

void communicate()
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

void getRow()
{
	Serial.write(0x82);
	Serial.write(row);
	state = STATE_ROW_REQUESTED;
	rowRequestedMillis = millis();
}

void processMove()
{
	bool goingRight = digitalRead(HOK) == LOW;
	bool insideCams = digitalRead(KSL) == HIGH;
	bool needle1 = digitalRead(ND1) == LOW;
	bool ccp = digitalRead(CCP) == HIGH;
	if (ccp == wasCcp) {
		return;
	}
	wasCcp = ccp;
	if (ccp == LOW) {
		return;
	}
	if (wasBetweenCams != insideCams) {
		onCams(goingRight);
		wasBetweenCams = insideCams;
	}
	if (insideCams) {
		updatePosition(goingRight);
		if (needle1 && wasBetweenCams && !seenNeedle1 && goingRight) {
			onNeedle1();
			seenNeedle1 = true;
		}
		digitalWrite(DOB, patternNeedle());
	} else {
		digitalWrite(DOB, LOW);
	}
}

void loop()
{
	if (state == STATE_REQUEST_ROW || (state == STATE_ROW_REQUESTED && millis() > rowRequestedMillis + 2000)) {
		getRow();
	}
	communicate();
	digitalWrite(LED, state == STATE_REQUEST_ROW || state == STATE_ROW_REQUESTED);
	processMove();
}
