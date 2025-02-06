
#include "hal_imu.h"
#include <Arduino.h>
#include "common.h"

#define TAG "IMU"

void init_imu()
{
  IMU.debug(Serial);
  
  // CDCSerial.println("IMU debug");
  // IMU is a macro to IMU_BMI270_BMM150
  if (!IMU.begin(BOSCH_ACCELEROMETER_ONLY)) // BOSCH_ACCELEROMETER_ONLY
  {
    MY_LOG("Failed to initialize IMU!");
    // Serial.println();
    while (1)
      ;
  }

  // Serial.print("Accelerometer sample rate = ");
  // Serial.print(IMU.accelerationSampleRate());
  // Serial.println(" Hz");
  // Serial.println();
  // Serial.println("Acceleration in G's");
  // Serial.println("X\tY\tZ");
  MY_LOG("Accelerometer sample rate = %f Hz", IMU.accelerationSampleRate());
}

bool read_imu(imu_t *data)
{
  float x, y, z;
  if (IMU.accelerationAvailable())
  {
    IMU.readAcceleration(x, y, z);
    // Serial.print(x);
    // Serial.print('\t');
    // Serial.print(y);
    // Serial.print('\t');
    // Serial.println(z);
    data->acc_x = x;
    data->acc_y = y;
    data->acc_z = z;
    return true;
  }
  else
  {
    // MY_LOG("Acceleration not available");
    return false;
  }
}