#pragma once

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define DEBUG                       false // Control if we see debug messages or not

#define HEARTBEAT_LED_PIN           13

// Which pin on the Arduino is connected to the NeoPixels?
#define LED_PIN    5
// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 100
// Cool components says 60/m, but 5m strip only seems to need 100.

#define NUM_CHANNELS                2       //Number of readings to store

#define SOUND_SENSITIVITY           50     // number max must be above average for trigger
