/*
  Twist and Turn - for Makers of Imaginary Worlds

  This code is for the nano every board.

  Device 1:
  Sound sensor - to control LED colours when sound is heard.


  Sensor used:
  https://wiki.seeedstudio.com/Grove-Loudness_Sensor/
  Uses grove connector on pin A5 (so same connections as the colour sensor.




  Device 2:
  Colour Controlled Sensor - control LED colors when colour is placed over the sensor.
  Device will see a colour sample put onto the sensor. It will then change to that colour (as best it can).
  If no item then default colour is white.
  There is some smoothing on this.

  Sensor used:
  https://wiki.dfrobot.com/TCS34725_I2C_Color_Sensor_For_Arduino_SKU__SEN0212
  Uses I2C for colour data


*/

#include <Arduino.h>
#include <EEPROM.h>
#include <Adafruit_NeoPixel.h>
#include <stdint.h>

#include "Config.h"

// ******** This is for Scheduling Tasks **************************
// Must include this library from Arduino IDE Library Manager
// https://github.com/arkhipenko/TaskScheduler
#include <TaskScheduler.h>
// Callback methods prototypes
void t1Callback();
void t1SCallback();
Task t1     (10 * TASK_MILLISECOND, TASK_FOREVER,  &t1Callback);         // Sample as base rate of 10Hz
Task t1S    (100 * TASK_MILLISECOND, TASK_FOREVER, &t1SCallback);         // Send data every 500mS to display
Scheduler runner;

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

// This is for the colour sensor:
// https://wiki.dfrobot.com/TCS34725_I2C_Color_Sensor_For_Arduino_SKU__SEN0212

#include "DFRobot_TCS34725.h"
DFRobot_TCS34725 tcs = DFRobot_TCS34725(&Wire, TCS34725_ADDRESS, TCS34725_INTEGRATIONTIME_24MS, TCS34725_GAIN_1X);


// *********** This is for the data holders *************************
volatile int data_counter_1s = 0;
volatile int flash_counter = 0;

uint16_t clear, red, green, blue;
uint32_t clear_t, red_t, green_t, blue_t;
uint8_t red_i, green_i, blue_i;

void t1Callback()
{
  tcs.getRGBC(&red, &green, &blue, &clear);
  tcs.lock();
  clear_t += clear;
  red_t += red;
  green_t += green;
  blue_t += blue;
  data_counter_1s++;
}

void t1SCallback() {
  if (!t1S.isFirstIteration())
  {
    clear_t = clear_t / data_counter_1s;
    red_t = red_t / data_counter_1s;
    green_t = green_t / data_counter_1s;
    blue_t = blue_t / data_counter_1s;
    Serial.print("C:\t"); Serial.print(clear_t);
    Serial.print("\tR:\t"); Serial.print(red_t);
    Serial.print("\tG:\t"); Serial.print(green_t);
    Serial.print("\tB:\t"); Serial.print(blue_t);
    Serial.println("\t");

    int red_i = map(red_t, 0, 1023, 0, 255);
    red_i = constrain(red_i, 0, 255);
    int green_i = map(green_t, 0, 1023, 0, 255);
    green_i = constrain(green_i, 0, 255);
    int blue_i = map(blue_t, 0, 1023, 0, 255);
    blue_i = constrain(blue_i, 0, 255);

    if ( clear_t < COLOUR_TRIGGER_LEVEL)
    {
      // Set everything to white - nothing on the sensor (probably!)
      for (int i = 0; i < strip.numPixels(); i++)
      { // For each pixel in strip...
        strip.setPixelColor(i, strip.Color(255,   255, 255));         //  Set pixel's color (in RAM)
      }
      strip.show();
    }
    else
    {
      for (int i = 0; i < strip.numPixels(); i++)
      { // For each pixel in strip...
        strip.setPixelColor(i, strip.Color(red_i,   green_i,  blue_i));         //  Set pixel's color (in RAM)
      }
      strip.show();
    }

    // Reset everything
    clear_t = 0;
    red_t = 0;
    green_t = 0;
    blue_t = 0;
    data_counter_1s = 0;   // Reset the counter
    flash_counter++;
    if (flash_counter >= 20)
    {
      digitalWrite(HEARTBEAT_LED_PIN, !digitalRead(HEARTBEAT_LED_PIN));
      Serial.println("ALIVE");
      flash_counter = 0;
    }
  }
}

void setup()
{
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(255); // Set BRIGHTNESS to about 1/5 (max = 255)
  pinMode(HEARTBEAT_LED_PIN, OUTPUT);
  digitalWrite(HEARTBEAT_LED_PIN, LOW);
  Serial.begin(9600);

  while (tcs.begin() != 0)
  {
    Serial.println("No TCS34725 found ... check your connections");
    digitalWrite(HEARTBEAT_LED_PIN, HIGH);
    delay(100);
    digitalWrite(HEARTBEAT_LED_PIN, LOW);
    delay(100);
    digitalWrite(HEARTBEAT_LED_PIN, HIGH);
    delay(100);
    digitalWrite(HEARTBEAT_LED_PIN, LOW);
    delay(600);
  }
  delay(200);

  // Use external reference voltage for analog reads
  analogReference(INTERNAL2V5);

  // Set up Scheduler:
  runner.init();
  Serial.print("Init scheduler->");
  runner.addTask(t1);
  runner.addTask(t1S);
  Serial.print("Added Tasks->");
  t1.enable();
  t1S.enable();
  Serial.println("Enabled Tasks");
  digitalWrite(HEARTBEAT_LED_PIN, HIGH);
}

void loop()
{
  // Deal with scheduled tasks
  runner.execute();

}
