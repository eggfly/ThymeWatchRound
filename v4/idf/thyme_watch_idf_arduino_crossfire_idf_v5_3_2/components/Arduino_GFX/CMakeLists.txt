
# Source files directories
set(GFX_SRC_DIRS
    src/
    src/canvas
    src/databus
    src/display
    src/font
)

# Head files directories
set(GFX_INC_DIRS
    src/
    src/canvas
    src/databus
    src/display
    src/font
)

# Public component requirement
set(GFX_REQUIRES
    arduino-esp32
    esp_lcd
    "esp_timer"
    U8g2_Arduino
)

# Private component requirement
set(GFX_PRIV_REQUIRES
)

# Register component
idf_component_register(SRC_DIRS ${GFX_SRC_DIRS}
                  INCLUDE_DIRS ${GFX_INC_DIRS}
                  REQUIRES ${GFX_REQUIRES}
                  PRIV_REQUIRES ${GFX_PRIV_REQUIRES}
)

