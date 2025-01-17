# cmake_minimum_required(VERSION 3.5)

idf_component_register(SRCS "Adafruit_Sensor.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES arduino-esp32)

project(Adafruit_Unified_Sensor)
