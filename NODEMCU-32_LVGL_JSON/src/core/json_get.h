#pragma once
#include "Arduino.h"

extern char nickname[20];
extern int follower;

extern char location[20];
extern char weather[15];
extern char temperature[10];


void WiFi_Connect();
void xTaskTwoWifi(void *xTask2);
