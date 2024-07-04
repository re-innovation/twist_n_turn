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

#define NUM_READINGS                50       //Number of readings to store in background buffer

//#define SOUND_SENSITIVITY           120.0     // number max must be above average for trigger

#define SOUND_MIN                   10
#define SOUND_MAX                   500

#define LED_SPEED_KNEE              100
#define LED_SPEED_MIN_1             200     // mS for quickest time - quiet sounds
#define LED_SPEED_MAX_1             800     // mS for slowest time - quiet sounds
#define LED_SPEED_MIN_2             10     // mS for quickest time - Loud sounds
#define LED_SPEED_MAX_2             100     // mS for slowest time - Loud sounds
