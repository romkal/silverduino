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

const int MAX = 200;

volatile int currentNeedle = 0;
volatile int camLeft = -MAX * 2;
volatile int camRight = MAX * 2;
volatile bool wasBetweenCams = false;
volatile bool wasGoingRight = true;
volatile int row = 0;

void setCams(bool goingRight) {
	if (goingRight && camRight > MAX) {
		camRight = currentNeedle;
	}
	if (!goingRight && camLeft < MAX) {
		camLeft = currentNeedle;
	}
}

void updatePosition() {
	bool goingRight = digitalRead(HOK) == LOW;
	bool insideCams = digitalRead(KSL) == HIGH;
	bool needle1 = digitalRead(MD1) == LOW;
	bool changedDirection = wasGoingRight != goingRight;
	int direction = (goingRight ? 1 : -1);

	// Left cams
	if (!insideCams && wasBetweenCams) {
		row++;
		setCams(goingRight);
	}
	currentNeedle += direction;

	// entered cams
	if (!wasBetweenCams && insideCams)
	{
		setCams(goingRight);
		currentNeedle = goingRight ? camLeft : camRight;
	}

	if (needle1) {
		camLeft -= currentNeedle;
		camRight -= currentNeedle;
	}
	wasBetweenCams = insideCams;
	wasGoingRight = goingRight;
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
	  attachInterrupt(digitalPinToInterrupt(CCP), onNeedle, RISING);
	  Serial.begin(9600);
}

void loop()
{
	if (digitalRead(BUTTON) == LOW) {
		String text = String("currurent needle: ") + currentNeedle
				+ " cams: " + camLeft + ":" + camRight
				+ " inside: " + wasBetweenCams
				+ " row: " + row;
		Serial.println(text);
	}
	delay(1000);
}
