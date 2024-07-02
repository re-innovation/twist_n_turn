#pragma once

#include <stdio.h>
#include <Arduino.h>
#include "config.h"

class data_channel_ {
  public:
    int16_t data_1s;
    int32_t data_1s_holder;
    float voltage_1s;
    float voltage_1s_holder;

    
    // This is the constructor
    data_channel_() {
      data_1s = 0; 
      voltage_1s = 0.0;
    }
    
};

extern data_channel_  channels[NUM_CHANNELS];
