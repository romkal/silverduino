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

void setup()
{
	  pinMode(MD1, INPUT);
	  pinMode(KSL, INPUT);
	  pinMode(DOB, OUTPUT);
	  pinMode(CCP, INPUT);
	  pinMode(HOK, INPUT);
	  pinMode(BUTTON, INPUT_PULLUP);

	  Serial.begin(9600);
}

void loop()
{
	bool goingRight = digitalRead(HOK);
	bool insideCams = digitalRead(KSL);
	bool needle1 = digitalRead(MD1);
	bool needle = digitalRead(CCP);

	digitalWrite(DOB, !digitalRead(BUTTON));
	String text = String("right: ") + goingRight
			+ " cams: " + insideCams
			+ " needle1: " + needle1
			+ " needle: " + needle;
	Serial.println(text);
	delay(100);
}
