

set(priv_requires fatfs console)

if(CONFIG_EXAMPLE_STORAGE_MEDIA_SPIFLASH)
    list(APPEND priv_requires wear_levelling esp_partition)
endif()

idf_component_register(
    SRCS
      "gfx/Arduino_Canvas_6bit.cpp"
      "tusb_msc.c"
      "main.cpp"
      "hal/hal_pressure_sensor.cpp"
      "hal/hal_pca6408a.cpp"
      "hal/hal_ls012b7dd06.cpp"
      "hal/hal_rtc.cpp"
      "hal/hal_digital_crown.cpp"
      "hal/hal_light_sensor.cpp"
      "hal/hal_va_meter.cpp"
      "hal/hal_battery_fuel_gauge.cpp"
      "hal/hal_imu.cpp"
      "hal/hal_ble_server.cpp"
      "util/memory.cpp"
      "framework/ThymeApp.cpp"
      "framework/app_manager.cpp"
      "apps/ThymeWatchMenu.cpp"
      "apps/ThymeWatchFace.cpp"
    INCLUDE_DIRS . ./gfx
    PRIV_REQUIRES "${priv_requires}"
)

# component_compile_definitions(  
#     -DCFG_TUSB_CONFIG_FILE=\"my_tusb_config.h\"  
# )

# target_compile_definitions(${COMPONENT_TARGET} PRIVATE CFG_TUD_MSC_BUFSIZE=1024)  

# 使用 ESP-IDF 提供的封装函数给组件添加编译宏  
# component_compile_definitions(  
#     -DCFG_TUD_MSC_BUFSIZE=1024  
# )
