
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

void read_imu(imu_t *data)
{
  float x, y, z;
  if (IMU.accelerationAvailable())
  {
    IMU.readAcceleration(x, y, z);
    data->acc_x = x;
    data->acc_y = y;
    data->acc_z = z;
    data->acc_valid = true;
  }
  else
  {
    // MY_LOG("Acceleration not available");
    data->acc_valid = false;
  }
  if (IMU.gyroscopeAvailable())
  {
    IMU.readGyroscope(x, y, z);
    data->gyro_x = x;
    data->gyro_y = y;
    data->gyro_z = z;
    data->gyro_valid = true;
  }
  else
  {
    // MY_LOG("Gyroscope not available");
    data->gyro_valid = false;
  }
}
