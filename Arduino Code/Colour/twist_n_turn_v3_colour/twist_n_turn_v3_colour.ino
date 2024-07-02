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
#include "AverageData.h"


// ******** This is for Scheduling Tasks **************************
// Must include this library from Arduino IDE Library Manager
// https://github.com/arkhipenko/TaskScheduler
#include <TaskScheduler.h>
// Callback methods prototypes
void t1Callback();
void t1SCallback();
Task t1     (50 * TASK_MILLISECOND, TASK_FOREVER,  &t1Callback);         // Sample as base rate of 10Hz
Task t1S    (50 * TASK_MILLISECOND, TASK_FOREVER, &t1SCallback);         // Send data every 500mS to display
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

// This is for the colur sensor:
#include "DFRobot_TCS34725.h"

DFRobot_TCS34725 tcs = DFRobot_TCS34725(&Wire, TCS34725_ADDRESS, TCS34725_INTEGRATIONTIME_24MS, TCS34725_GAIN_1X);


// *********** This is for the data holders *************************
// Want to create a data class for each channel, up to max of NUM_CHANNELS
data_channel_ channels[NUM_CHANNELS];    // This creates our data channels
volatile int data_counter_1s = 0;

volatile bool led_trigger_flag = false;
volatile bool on_flag = false;
volatile int led_speed = 10;    // Speed for doing wipe - slow if quite, quick if loud

long int loudness;
int max_loudness = 0;
long int start_time;
int i = 0;

uint16_t clear, red, green, blue;

void t1Callback()
{
  //  int loudness_reading = analogRead(5);
  //  loudness += loudness_reading;
  //  if (loudness_reading > max_loudness)
  //  {
  //    max_loudness = loudness_reading;
  //  }
  data_counter_1s++;
}

void t1SCallback() {
  if (!t1S.isFirstIteration())
  {
    //    Serial.print("RUN: ");
    //    Serial.println(data_counter_1s);
    //    float average_loudness = (float)loudness / (float)data_counter_1s;
    //    Serial.print("AVE SOUND: ");
    //    Serial.println(average_loudness);
    //    Serial.print("MAX SOUND: ");
    //    Serial.println(max_loudness);
    //    if (max_loudness > (average_loudness + SOUND_SENSITIVITY) && led_trigger_flag == false)
    //    {
    //      Serial.print("TRIG:");
    //      led_speed = ((average_loudness + SOUND_SENSITIVITY) * 30) / max_loudness;
    //      Serial.println(led_speed );
    //      start_time = millis();
    //      on_flag = true;
    //      i = 0;
    //      led_trigger_flag = true;
    //    }
    //    max_loudness = 0;
    //    loudness = 0;

    tcs.getRGBC(&red, &green, &blue, &clear);
    tcs.lock();
    Serial.print("C:\t"); Serial.print(clear);
    Serial.print("\tR:\t"); Serial.print(red);
    Serial.print("\tG:\t"); Serial.print(green);
    Serial.print("\tB:\t"); Serial.print(blue);
    Serial.println("\t");

    //    if (red > green && red > blue)
    //    {
    //      red = 255;
    //      green = 0;
    //      blue = 0;
    //    }
    //    else if (green > red && green > blue)
    //    {
    //      red = 0;
    //      green = 255;
    //      blue = 0;
    //    }
    //    else if (blue > red && blue > green)
    //    {
    //      red = 0;
    //      green = 0;
    //      blue = 255;
    //    }

    int red_i = map(red, 0, 1023, 0, 255);
    red_i = constrain(red_i, 0, 255);
    int green_i = map(green, 0, 1023, 0, 255);
    green_i = constrain(green_i, 0, 255);
    int blue_i = map(blue, 0, 1023, 0, 255);
    blue_i = constrain(blue_i, 0, 255);

    for (int i = 0; i < strip.numPixels(); i++)
    { // For each pixel in strip...
      strip.setPixelColor(i, strip.Color(red_i,   green_i,  blue_i));         //  Set pixel's color (in RAM)
    }
    strip.show();

    data_counter_1s = 0;   // Reset the counter

  }
}

void setup()
{
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(255); // Set BRIGHTNESS to about 1/5 (max = 255)

  Serial.begin(9600);

  while (tcs.begin() != 0)
  {
    Serial.println("No TCS34725 found ... check your connections");
    delay(1000);
  }

  delay(300);

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
}

void loop()
{
  // Deal with scheduled tasks
  runner.execute();

  //  if (led_trigger_flag == true)
  //  {
  //
  //    if (millis() > (start_time + led_speed) && i < strip.numPixels() && on_flag == true)
  //    {
  //      start_time = millis();
  //      strip.setPixelColor(i, strip.Color(255,   0,   0));         //  Set pixel's color (in RAM)
  //      strip.show();                          //  Update strip to match
  //      i++;
  //      if (i >= strip.numPixels())
  //      {
  //        on_flag = false;
  //        i = 0;
  //        Serial.println("END ON");
  //      }
  //    }
  //    else if (millis() > (start_time + led_speed) && i < strip.numPixels() && on_flag == false)
  //    {
  //      start_time = millis();
  //      strip.setPixelColor(i, strip.Color(0,   0,   0));         //  Set pixel's color (in RAM)
  //      strip.show();                          //  Update strip to match
  //      i++;
  //      if (i >= strip.numPixels())
  //      {
  //        led_trigger_flag = false;
  //        i = 0;
  //        Serial.println("END OFF");
  //      }
  //    }
  //  }
}
