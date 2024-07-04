/*
  Twist and Turn - for Makers of Imaginary Worlds

  This code is for the nano every board.

  Device 1:
  Sound sensor - to control LED colours when sound is heard.
  Run through rainbow colours when triggered. red->orange->yellow-> green-> blue->indigo->violet
  Speed of run through colours depends upon sound max above ambient background

  Sensor used:
  https://wiki.seeedstudio.com/Grove-Loudness_Sensor/
  Uses grove connector on pin A5 (so same connections as the colour sensor.


*/

#include <Arduino.h>
#include <EEPROM.h>
#include <Adafruit_NeoPixel.h>
#include <stdint.h>
#include <avr/wdt.h>      // for the watch dog timer

#include "Config.h"

// ******** This is for Scheduling Tasks **************************
// Must include this library from Arduino IDE Library Manager
// https://github.com/arkhipenko/TaskScheduler
#include <TaskScheduler.h>
// Callback methods prototypes
void t1Callback();
void t1SCallback();
Task t1     (5 * TASK_MILLISECOND, TASK_FOREVER,  &t1Callback);         // Sample as base rate of 10Hz
Task t1S    (200 * TASK_MILLISECOND, TASK_FOREVER, &t1SCallback);         // Send data every 500mS to display
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

uint32_t  red = strip.Color(255, 0, 0); //red
uint32_t  orange = strip.Color(255, 135, 0); //orange
uint32_t  yellow = strip.Color(255, 255, 0); //yellow
uint32_t  green = strip.Color(0, 255, 0);//green
uint32_t  blue = strip.Color(0, 0, 255); //blue
uint32_t  cyan = strip.Color(0, 255, 255); //cyan
uint32_t  purple = strip.Color(128, 0, 128); //purple

//red->orange->yellow-> green-> blue->indigo->violet
uint32_t* RainbowColours[] = {&red, &orange, &yellow, &green, &blue, &cyan, &purple};


// *********** This is for the data holders *************************
volatile float sound_background[NUM_READINGS];    // This creates our background sound array (for averaging)
volatile float sound_sample;
volatile uint32_t sound_sample_holder;
volatile int sound_max;

volatile int data_counter_1s = 0;

long int start_time;
int i = 0;


volatile bool led_trigger_flag = false;
volatile bool on_flag = false;
volatile int led_speed = 10;    // Speed for doing wipe - slow if quite, quick if loud

void t1Callback()
{
  int loudness_reading = analogRead(5);
  if (loudness_reading > sound_max)
  {
    sound_max = loudness_reading;
  }
  else
  {
    // Dont want to keep the max in the average reading as it skews it...
    sound_sample_holder += loudness_reading;
    data_counter_1s++;
  }
}

void t1SCallback() {
  if (!t1S.isFirstIteration())
  {
    sound_sample = (float)sound_sample_holder / (float)data_counter_1s;

    // Sort out the background sound array
    for (int y = 1; y <= NUM_READINGS; y++)
    {
      sound_background[NUM_READINGS - y ] = sound_background[NUM_READINGS - y - 1];
    }
    sound_background[0] = sound_sample;

    float sound_average = 0.0;
    for (int y = 0; y < NUM_READINGS; y++)
    {
      sound_average += sound_background[y];
    }
    sound_average = sound_average / (float)NUM_READINGS;

    //    // Show the sound_background array
    //    for (int y = 0; y < NUM_READINGS; y++)
    //    {
    //      Serial.print(sound_background[y]);
    //      Serial.print(":");
    //    }
    //    Serial.print("AVE:");
    //    Serial.println(sound_average);

//    Serial.print("N:\t");
//    Serial.print(data_counter_1s);
//    Serial.print("\t AVE:\t");
//    Serial.print(sound_sample);
//    Serial.print("\t MAX:\t");
//    Serial.print(sound_max);
//    Serial.print("\t BAK:\t");
//    Serial.println(sound_average);

    // Sort out the trigger:
    if (sound_max > (sound_average + SOUND_SENSITIVITY) && led_trigger_flag == false)
    {

      led_speed = map((sound_max - (sound_average + SOUND_SENSITIVITY)), 10, 1023, LED_SPEED_MAX, LED_SPEED_MIN);
      led_speed = constrain(led_speed, LED_SPEED_MIN, LED_SPEED_MAX);
      
      Serial.print("TRIG:");
      Serial.print(sound_max - (sound_average + SOUND_SENSITIVITY));
      Serial.print("SP:");
      Serial.println(led_speed);
      
      start_time = millis();
      on_flag = true;
      i = 0;
      led_trigger_flag = true;
    }

    sound_max = 0;
    sound_sample_holder = 0;
    data_counter_1s = 0;   // Reset the counter
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
  // Reset the sound_background array:
  for (int y = 0; y < NUM_READINGS; y++)
  {
    sound_background[y] = 0;
  }
  // set up the watch dog timer:
  wdt_enable(WDTO_2S);    // set for 2 seconds
}

void loop()
{
  // Deal with scheduled tasks
  runner.execute();

  // feed the WDT - every 2s at least
  wdt_reset();

  if (led_trigger_flag == true)
  {
    if (millis() > (start_time) && on_flag == true)
    {
      start_time = millis() + led_speed;
      for (int x = 0; x < strip.numPixels(); x++)
      { // For each pixel in strip...
        strip.setPixelColor(x, *RainbowColours[i]);
        //  Update strip to match
      }
      strip.show();
      i++;
      if (i > 7)
      {
        on_flag = false;
        i = 0;
      }
    }
    else if (on_flag == false)
    {
      //start_time = millis();
      for (int x = 0; x < strip.numPixels(); x++)
      { // For each pixel in strip...
        strip.setPixelColor(x, strip.Color(0,   0,   0));
        //  Update strip to match
      }
      strip.show();

      //      strip.setPixelColor(i, strip.Color(0,   0,   0));         //  Set pixel's color (in RAM)
      //      strip.show();                          //  Update strip to match
      //      i++;
      //      if (i >= strip.numPixels())
      //      {
      led_trigger_flag = false;
      i = 0;
      Serial.println("LED OFF");
    }
  }
}
