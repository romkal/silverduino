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
#define LED LED_BUILTIN

Carriage carriage;
Pattern* pattern = new BuiltInPattern(ROMAN, 8, 1);
PatternProgression patternProgression(pattern);
Ayab ayab;
Ui ui;
int wasInsideCams;

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

void loop()
{
	int insideCams = digitalRead(KSL);
	if (insideCams != wasInsideCams && insideCams == LOW) {
		digitalWrite(DOB, LOW);
		patternProgression.endLine();
	}
	wasInsideCams = insideCams;
	if (insideCams == HIGH) {
		carriage.eventLoop(digitalRead(HOK) == LOW, digitalRead(CCP), digitalRead(ND1) == LOW);
	} else {
		ayab.eventLoop();
	}
}
