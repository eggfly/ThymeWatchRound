#pragma once

#include <Arduino.h>
#include "pat9125.h"

extern PAT9125 PAT;

void init_pat9125();
long read_pat9125();
void reset_pat9125();
