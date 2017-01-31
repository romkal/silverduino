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
const int BUTTON = 8;

int state = LOW;
int wasNeedle;

void setup()
{
	  pinMode(ND1, INPUT);
	  pinMode(KSL, INPUT);
	  pinMode(DOB, OUTPUT);
	  pinMode(CCP, INPUT);
	  pinMode(HOK, INPUT);
	  pinMode(BUTTON, INPUT_PULLUP);

	  pinMode(A0, INPUT);

	  Serial.begin(115200);
}

void loop()
{
	int goingRight = digitalRead(HOK);
	int insideCams = digitalRead(KSL);
	int needle1 = digitalRead(ND1);
	int needle = digitalRead(CCP);

	if (needle && !wasNeedle) {
		state = insideCams;
	}
	wasNeedle = needle;

	digitalWrite(DOB, state);
//	String text = String("") + goingRight
//			+ ", " + digitalRead(HOK) * 1000
//			+ ", " + insideCams
//			+ ", " + digitalRead(KSL) * 1000
//			+ ", " + needle1
//			+ ", " + digitalRead(ND1) * 1000
//			+ ", " + needle
//			+ ", " + digitalRead(CCP) * 1000;
//
//	Serial.println(text);
}
