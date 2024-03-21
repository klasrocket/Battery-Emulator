#include <Arduino.h>

#ifndef BATTERIES_H
#define BATTERIES_H

#include "../../src/USER_SETTINGS.h"



#ifdef NISSAN_LEAF_BATTERY
#include "NISSAN-LEAF-BATTERY.h"  //See this file for more LEAF battery settings
#endif



#ifdef TEST_FAKE_BATTERY
#include "TEST-FAKE-BATTERY.h"  //See this file for more Fake battery settings
#endif


#ifdef SERIAL_LINK_RECEIVER  // The serial thing does its thing
void receive_can_battery();
#else
void receive_can_battery(twai_message_t rx_frame);
#endif
void update_values_battery();
void send_can_battery();
void setup_battery(void);

#endif
