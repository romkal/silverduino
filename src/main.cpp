/*
 * main.cpp
 *
 *  Created on: Jan 26, 2017
 *      Author: romkal
 */

#include "Arduino.h"

const int MD1 = 11;
const int KSL = 7;
const int DOB = 6;
const int CCP = 5; // Has to be 1 or 2
const int HOK = 4;
const int BUTTON = 8;

volatile int currentNeedle = 0;
volatile int camLeft = 0;
volatile int camRight = 0;
volatile bool wasBetweenCams = false;

volatile int row = 0;

void updatePosition() {
	bool goingRight = digitalRead(HOK) == HIGH;
	bool insideCams = digitalRead(KSL) == HIGH;
	bool needle1 = digitalRead(MD1) == LOW;
	currentNeedle += (goingRight ? 1 : -1);
	// we left cams
	if (!insideCams && wasBetweenCams) {
		row++;
		if (wasBetweenCams) {
			// We went outside of cams
			if (goingRight) {
				camRight = currentNeedle;
			} else {
				camLeft = currentNeedle;

			}

		}
	}
	// we entered cams
	if (insideCams && !wasBetweenCams) {
		currentNeedle = goingRight ? camLeft : camRight;
	}
	if (needle1) {
		camLeft -= currentNeedle;
		camRight -= currentNeedle;
		currentNeedle = 0;
	}
	wasBetweenCams = insideCams;
}

void updateNeedle()
{
	if (!wasBetweenCams)
	{
		digitalWrite(DOB, LOW);
		return;
	}
	bool isHigh = (currentNeedle + row) % 3 == 0;
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

	  digitalWrite(DOB, LOW);
	  attachInterrupt(digitalPinToInterrupt(CCP), onNeedle, FALLING);
	  Serial.begin(9600);
}

void loop()
{
	if (digitalRead(BUTTON) == LOW) {
		String text = String("current needle: ") + currentNeedle
				+ " cams: " + camLeft + ":" + camRight
				+ " inside: " + wasBetweenCams
				+ " row: " + row;
		Serial.println(text);
		row = 0;
	}
	delay(100);
}
