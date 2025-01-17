
get_filename_component(dir ${CMAKE_CURRENT_LIST_FILE} PATH)
FILE(GLOB_RECURSE app_sources ${dir}/*.cpp)

idf_component_register(SRCS ${app_sources}      
                    INCLUDE_DIRS "."
                    REQUIRES arduino-esp32 Adafruit_BusIO
)
