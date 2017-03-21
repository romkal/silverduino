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

Carriage carriage;
PatternProgression patternProgression;
Ayab ayab;

PatternScreen patternScreen("Pattern", PATTERNS, 2, patternProgression);
NumberScreen repeatScreen("Repeat", patternProgression.repeat);
BoolScreen invertScreen("Invert", patternProgression.invert);
BoolScreen mirrorScreen("Mirror", patternProgression.mirror);
CarriageFunctionScreen resetScreen ("Reset", &PatternProgression::reset);
Screen* screens[] {
		&patternScreen,
		&repeatScreen,
		&invertScreen,
		&mirrorScreen,
		&resetScreen
};
Ui ui(screens, sizeof(screens) / sizeof(Screen*));
int wasInsideCams;

void setup()
{
	  pinMode(ND1, INPUT);
	  pinMode(KSL, INPUT);
	  pinMode(DOB, OUTPUT);
	  pinMode(CCP, INPUT);
	  pinMode(HOK, INPUT);

	  Serial.begin(9600);
	  while(!Serial);
	  digitalWrite(DOB, LOW);
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
