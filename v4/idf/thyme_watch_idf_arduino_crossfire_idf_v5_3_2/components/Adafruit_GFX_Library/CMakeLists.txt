# Adafruit GFX Library
# https://github.com/adafruit/Adafruit-GFX-Library
# BSD License

cmake_minimum_required(VERSION 3.5)

idf_component_register(SRCS "Adafruit_GFX.cpp" "glcdfont.c"
                       INCLUDE_DIRS "."
                       REQUIRES arduino-esp32)

project(Adafruit-GFX-Library)
