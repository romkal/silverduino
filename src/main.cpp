/*
 * main.cpp
 *
 *  Created on: Jan 26, 2017
 *      Author: romkal
 */

#include <Arduino.h>

const int MD1 = 6;
const int KSL = 5;
const int DOB = 4;
const int CCP = 3; // Has to be 1 or 2
const int HOK = 2;
const int BUTTON = 8;
const int LED = 9;

const uint8_t* PING_RESPONSE = new uint8_t[2] {0xc3, 0x03};
const uint8_t* START_RESPONSE = new uint8_t[2] {0xc1, 0x01};
const int MAX = 200;

const int STATE_START = 1;
const int STATE_REQUEST_ROW = 2;
const int STATE_ROW_REQUESTED = 3;
const int STATE_KNIT = 4;
const int STATE_LAST = 5;

uint8_t* patternBuffer = new uint8_t[MAX / 8];
volatile int patternSize;
volatile int row = 0;
volatile int state = STATE_START;
volatile int patternStart = 0;
int currentNeedle = 0;
int* cams = new int[2];
bool wasBetweenCams = true;

void updatePosition() {
	bool goingRight = digitalRead(HOK) == LOW;
	bool insideCams = digitalRead(KSL) == HIGH;
	bool needle1 = digitalRead(MD1) == LOW;
	int direction = (goingRight ? 1 : -1);

	// Left cams
	if (!insideCams && wasBetweenCams) {
		switch (state) {
		case STATE_KNIT:
			row++;
			state = STATE_REQUEST_ROW;
			break;
		case STATE_START:
		case STATE_LAST:
			break;
		case STATE_REQUEST_ROW:
		case STATE_ROW_REQUESTED:
			// BAD
			break;
		}
		cams[goingRight] = currentNeedle;
	}
	currentNeedle += direction;

	// entered cams
	if (!wasBetweenCams && insideCams)
	{
		currentNeedle = cams[!goingRight];
	}

	if (needle1) {
		cams[0] -= currentNeedle;
		cams[1] -= currentNeedle;
	}
	wasBetweenCams = insideCams;
}

void updateNeedle()
{
	if (!wasBetweenCams || row < 0)
	{
		digitalWrite(DOB, LOW);
		return;
	}
	int patternBitNr = (currentNeedle + patternStart) % patternSize;
	uint8_t patternByte = patternBuffer[patternBitNr / 8];
	bool isHigh = patternByte & 1 << (patternBitNr % 8);
	digitalWrite(DOB, isHigh);
}

void onNeedle()
{
	updatePosition();
	updateNeedle();
}

void setup()
{
	  pinMode(MD1, INPUT);
	  pinMode(KSL, INPUT);
	  pinMode(DOB, OUTPUT);
	  pinMode(CCP, INPUT);
	  pinMode(HOK, INPUT);
	  pinMode(BUTTON, INPUT_PULLUP);
	  pinMode(LED, OUTPUT);

	  digitalWrite(DOB, LOW);
	  attachInterrupt(digitalPinToInterrupt(CCP), onNeedle, RISING);
	  Serial.begin(9600);
}


void pingCommand()
{
	Serial.write(PING_RESPONSE, 2);
}

void startCommand()
{
	patternStart = Serial.read();
	patternSize = Serial.read() - patternStart;
	Serial.write(START_RESPONSE, 2);
	row = 1;
	state = STATE_REQUEST_ROW;
}

void rowCommand()
{
	Serial.read(); // rowNumber
	Serial.readBytes(patternBuffer, MAX / 8);
	int flags = Serial.read();
	state = flags ? STATE_LAST : STATE_KNIT;
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
}

void loop()
{
	if (digitalRead(BUTTON) == LOW) {
		String text = String("currurent needle: ") + currentNeedle
				+ " cams: " + cams[0] + ":" + cams[1]
				+ " inside: " + wasBetweenCams
				+ " row: " + row;
		Serial.println(text);
	}
	if (state == STATE_REQUEST_ROW) {
		getRow();
	}
	communicate();
	digitalWrite(LED, state == STATE_REQUEST_ROW || state == STATE_ROW_REQUESTED);
	delay(100);
}
