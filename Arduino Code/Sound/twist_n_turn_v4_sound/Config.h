#pragma once

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define DEBUG                       false // Control if we see debug messages or not

#define HEARTBEAT_LED_PIN           9

// Which pin on the Arduino is connected to the NeoPixels?
#define LED_PIN    5
// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 100
// Cool components says 60/m, but 5m strip only seems to need 100.

#define NUM_READINGS                200       //Number of readings to store in background buffer

#define SOUND_SENSITIVITY           120.0     // number max must be above average for trigger

#define LED_SPEED_MIN               10     // mS for quickest time
#define LED_SPEED_MAX              500     // mS for slowest time
