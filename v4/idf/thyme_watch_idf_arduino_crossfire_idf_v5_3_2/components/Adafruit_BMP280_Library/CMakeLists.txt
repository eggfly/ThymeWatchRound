
idf_component_register(SRCS "Adafruit_BMP280.cpp" 
                       INCLUDE_DIRS "."
                       REQUIRES arduino-esp32 Adafruit_Sensor Adafruit_BusIO)

project(Adafruit_BMP280_Library)
