
# Source files directories
set(U8G2_SRC_DIRS
    src/
    src/clib
)

# Head files directories
set(U8G2_INC_DIRS
    src/
    src/clib
)

# Public component requirement
set(U8G2_REQUIRES
    arduino-esp32
)

# Private component requirement
set(U8G2_PRIV_REQUIRES
)

# Register component
idf_component_register(SRC_DIRS ${U8G2_SRC_DIRS}
                  INCLUDE_DIRS ${U8G2_INC_DIRS}
                  REQUIRES ${U8G2_REQUIRES}
                  PRIV_REQUIRES ${U8G2_PRIV_REQUIRES}
)
