
选择带背光的 LS012B7DD06
* 并口 RGB222

I2C:

```
// Primary I2C:
// Found address: 32 (0x20)  -> PCA6408A (IO expander)
// Found address: 50 (0x32)  -> INS5699S (RTC)
// Found address: 54 (0x36)  -> MAX17048 Fuel Gauge
// Found address: 105 (0x69) -> 6-Axis BMI270(BMI2_I2C_SEC_ADDR)
// Secondary I2C:
// Found address: 64 (0x40)  -> INA219
// Found address: 68 (0x44)  -> OPT3001
// Found address: 69 (0x45)  -> SHT30
// Found address: 117 (0x75) -> PAT9125
// Found address: 118 (0x76) -> BMP280
```


LPM012M134B code: (选型放弃这款，颜色不好看)
* https://down.cnwans.com/archives/509
* https://github.com/andelf/rp-embassy-playground/blob/master/src/bin/jdi-1in2-round.rs



# 关于功耗和常亮
* 需要 esp32-s3 深睡眠之前，调用 gpio_hold_en 或者开启全部 gpio_deep_sleep_hold_en()
* 上面两者实测没有功耗差别
* pwm ledc pin 没有被 hold，背光会熄灭
