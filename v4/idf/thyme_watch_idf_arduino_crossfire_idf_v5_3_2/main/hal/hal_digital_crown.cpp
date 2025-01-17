
#include <Arduino.h>
#include <Wire.h>
#include "pat9125.h"
#include "common.h"

#define TAG "CROWN"
PAT9125 PAT(&Wire1, 0x75);

void init_pat9125()
{
  PAT.pat9125_init();
  PAT.pat9125_set_res(240, 240, true);
}

long read_pat9125()
{
  PAT.pat9125_update();
  PAT.pat9125_update_x();
  PAT.pat9125_update_y();
  PAT.pat9125_update_y2();
  PAT.pat9125_update_x2();
  MY_LOG("PAT9125: %ld %ld %d %d %ld %ld", PAT.pat9125_x, PAT.pat9125_y, PAT.pat9125_b, PAT.pat9125_s, PAT.pat9125_x2, PAT.pat9125_y2);
  return PAT.pat9125_x;
}
