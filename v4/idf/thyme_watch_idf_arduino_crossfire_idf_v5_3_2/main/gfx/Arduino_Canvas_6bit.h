#ifndef _ARDUINO_CANVAS_6BIT_H_
#define _ARDUINO_CANVAS_6BIT_H_

#include "../Arduino_GFX.h"

class Arduino_Canvas_6bit : public Arduino_GFX
{
public:
  /**
   * @param w         Canvas宽度
   * @param h         Canvas高度
   * @param output    底层输出设备(例如硬件屏幕对象)
   * @param output_x  将Canvas内容flush到硬件时，目标硬件的起始X坐标
   * @param output_y  将Canvas内容flush到硬件时，目标硬件的起始Y坐标
   */
  Arduino_Canvas_6bit(int16_t w, int16_t h, Arduino_G *output, int16_t output_x = 0, int16_t output_y = 0);
  
  void setCustomFrameBuffer(uint8_t *framebuffer);

  bool begin(int32_t speed = GFX_NOT_DEFINED) override;

  /**
   * @brief 写像素到内存（已裁剪到可见范围）
   */
  void writePixelPreclipped(int16_t x, int16_t y, uint16_t color) override;

  /**
   * @brief 将内存中的图像内容刷新到底层输出设备
   */
  void flush(void) override;

  /**
   * @brief 获取底层帧缓冲指针，可自由操作
   */
  uint8_t *getFramebuffer();

protected:
  uint8_t *_framebuffer; // 用于存储 6bit 像素值，1字节仅低6位有效
  Arduino_G *_output;
  int16_t _output_x, _output_y;
};

#endif // _ARDUINO_CANVAS_6BIT_H_
