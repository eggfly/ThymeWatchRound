#pragma once

#include <TimeLib.h> //https://github.com/PaulStoffregen/Time

void initRTC();
tmElements_t readRTC();
void printDateTime(tmElements_t tm);
