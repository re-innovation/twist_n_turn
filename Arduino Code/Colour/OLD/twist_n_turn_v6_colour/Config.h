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

#define NUM_CHANNELS                2       //Number of readings to store

#define COLOUR_TRIGGER_LEVEL        500     // This is the change from baseline to trigger colour test

#define WHITE_SETPOINT              200     // This sets the brightness of the LEDs when 'waiting' in white

#define RED_WEIGHT                  300     // The lower this number the more sensitive it is to the colour
#define GREEN_WEIGHT                400
#define BLUE_WEIGHT                 700

#define SENSITIVITY_ADJUST          50
