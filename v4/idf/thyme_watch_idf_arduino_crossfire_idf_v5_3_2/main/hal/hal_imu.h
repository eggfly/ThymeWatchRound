#pragma once

#include "Arduino_BMI270_BMM150.h"

typedef struct
{
    bool acc_valid;
    float acc_x;
    float acc_y;
    float acc_z;
    bool gyro_valid;
    float gyro_x;
    float gyro_y;
    float gyro_z;
} imu_t;

void init_imu();
void read_imu(imu_t *data);
