/*
 * main.cpp
 *
 *  Created on: Jan 26, 2017
 *      Author: romkal
 */

#include <Arduino.h>
#include <pins_arduino.h>
#include "carriage.h"
#include "Ayab.h"
#include "ui.h"
#include "patterns.h"

#define ND1 6 // needle1
#define KSL 5 //  cams
#define DOB 4 // output
#define CCP 3 // clock
#define HOK 2 // direction

Carriage carriage;
Pattern* pattern = new BuiltInPattern(&ROMAN);
PatternProgression patternProgression(pattern);
Ayab ayab;

NumberScreen repeatScreen("Repeat", patternProgression.repeat);
BoolScreen invertScreen("Invert", patternProgression.invert);
BoolScreen mirrorScreen("Mirror", patternProgression.mirror);
const Screen* screens[] {
		&repeatScreen,
		&invertScreen,
		&mirrorScreen
};
Ui ui(screens, 2);
int wasInsideCams;

void setup()
{
	  pinMode(ND1, INPUT);
	  pinMode(KSL, INPUT);
	  pinMode(DOB, OUTPUT);
	  pinMode(CCP, INPUT);
	  pinMode(HOK, INPUT);

	  while(!Serial);
	  digitalWrite(DOB, LOW);
	  Serial.begin(115200);
	  ui.begin();
}

void loop()
{
	int insideCams = digitalRead(KSL);
	if (insideCams != wasInsideCams) {
		if (insideCams == LOW) {
			digitalWrite(DOB, LOW);
			patternProgression.endLine();
		} else {
			// entered cams
		}
	}
	wasInsideCams = insideCams;
	if (false) {
//	if (insideCams == HIGH) {
		carriage.eventLoop(digitalRead(HOK) == LOW, digitalRead(CCP), digitalRead(ND1) == LOW);
	} else {
		ui.eventLoop();
		ayab.eventLoop();
	}
}
