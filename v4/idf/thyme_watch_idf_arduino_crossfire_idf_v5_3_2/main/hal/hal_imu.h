#pragma once

#include "Arduino_BMI270_BMM150.h"

typedef struct
{
    float acc_x;
    float acc_y;
    float acc_z;
} imu_t;

void init_imu();
bool read_imu(imu_t *data);
