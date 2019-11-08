/*
 *  Metronom v1.0
 *
 *  This program is open source, under GPLV3 license.
 *
 *  Requires Adafruit_Neopixel and r89_Buttons libraries
 */

#include <Arduino.h>
#include "TM1637.h"
#include <Adafruit_NeoPixel.h>
#include <Button.h>
#include <ButtonEventCallback.h>
#include <BasicButton.h>


// CONFIGURATION
// - APP
#define BRIGHT_MAX 100  // up to 255
#define BPM_START 60
// - PINS
#define DLD4_CLK 2
#define DLD4_DIO 3
#define BUZZ_PIN 6
#define BUTR_PIN 9
#define BUTL_PIN 10
#define NEOPX_PIN 11

// END CONFIG

TM1637 display(DLD4_CLK, DLD4_DIO);
Adafruit_NeoPixel neopixels(2, NEOPX_PIN, NEO_GRB + NEO_KHZ800);
BasicButton bL = BasicButton(BUTL_PIN);
BasicButton bR = BasicButton(BUTR_PIN);

uint32_t now, next = 0;
uint8_t bpm = BPM_START;
uint32_t period = (60.0 / bpm) * 1000;
bool bLP, bRP, mute = false;

void showNumber(uint16_t n, bool d)
{
	int8_t cifers[4];
	for (int8_t i=3; i>=0; i--)
	{
		cifers[i] = n % 10;
		n /= 10;
	}
	display.point(d);
	display.display(cifers);
}

void flashNPX() {
	uint8_t R=0 ,G=0 ,B = 0;
	if (mute) {
		switch (random(3)) {
			case 0:
				R = BRIGHT_MAX;
				break;
			case 1:
				G = BRIGHT_MAX;
				break;
			case 2:
				B = BRIGHT_MAX;
				break;
		}
	} else {
		R = BRIGHT_MAX;
		G = BRIGHT_MAX;
		B = BRIGHT_MAX;
	}
	neopixels.setPixelColor(0, neopixels.Color(R, G, B));
	neopixels.setPixelColor(1, neopixels.Color(R, G, B));
	neopixels.show();
	delay(5);
	neopixels.clear();
	neopixels.show();
}

void onButtonPressed(Button& btn, uint16_t duration){
	if (uint32_t(&btn)==uint32_t(&bL)) {
			bpm -= 1;
			period = (60.0 / bpm) * 1000;
			showNumber(bpm, false);
			bLP = true;
	} else {
			bpm += 1;
			period = (60.0 / bpm) * 1000;
			showNumber(bpm, false);
			bRP = true;
	}
	if (bLP && bRP) {
		mute = !mute;
	}
}

void onButtonReleased(Button& btn, uint16_t duration){
	if (uint32_t(&btn)==uint32_t(&bL)) bLP = false;
	else bRP = false;
}

void onButtonHeld(Button& btn, uint16_t duration, uint16_t repeatCount){
	if (uint32_t(&btn)==uint32_t(&bL)) {
			bpm -= 10;
			period = (60.0 / bpm) * 1000;
			showNumber(bpm, false);
	} else {
			bpm += 10;
			period = (60.0 / bpm) * 1000;
			showNumber(bpm, false);
	}
}

void setup() {
	// init
	display.set(7); // brightness 0 to 7
	neopixels.begin();
	neopixels.clear();
	bL.onPress(onButtonPressed);
	bR.onPress(onButtonPressed);
	bL.onRelease(onButtonReleased);
	bR.onRelease(onButtonReleased);
	bL.onHoldRepeat(1000, 500, onButtonHeld);
	bR.onHoldRepeat(1000, 500, onButtonHeld);
	// actions
	tone(BUZZ_PIN, 440, 250);
	showNumber(bpm, false);
	next = millis() + period;
}

void loop() {
	bL.update();
	bR.update();
	now = millis();
	if (now >= next) {
		if (!mute) tone(BUZZ_PIN, 880, 100);
		flashNPX();
		next += period;
	}
}
