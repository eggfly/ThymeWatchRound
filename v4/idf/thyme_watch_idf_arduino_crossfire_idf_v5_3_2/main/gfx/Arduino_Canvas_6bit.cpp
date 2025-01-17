#include "Arduino_DataBus.h"
#if !defined(LITTLE_FOOT_PRINT)

#include "Arduino_GFX.h"
#include "Arduino_Canvas_6bit.h"

Arduino_Canvas_6bit::Arduino_Canvas_6bit(int16_t w, int16_t h, Arduino_G *output, int16_t output_x, int16_t output_y)
    : Arduino_GFX(w, h), _framebuffer(nullptr), _output(output), _output_x(output_x), _output_y(output_y)
{
}

void Arduino_Canvas_6bit::setCustomFrameBuffer(uint8_t *framebuffer)
{
    _framebuffer = framebuffer;
}

bool Arduino_Canvas_6bit::begin(int32_t speed)
{
    // 初始化底层输出设备（如不需要让底层begin，可以传入GFX_SKIP_OUTPUT_BEGIN）
    if (speed != GFX_SKIP_OUTPUT_BEGIN && _output)
    {
        if (!_output->begin(speed))
        {
            return false;
        }
    }

    // 为帧缓冲分配空间：这里简单地用 1字节/像素，只用低6位储存(6bit)颜色
    size_t s = (size_t)_width * (size_t)_height;
#if defined(ESP32)
    if (psramFound())
    {
        _framebuffer = (uint8_t *)ps_malloc(s);
    }
    else
    {
        _framebuffer = (uint8_t *)malloc(s);
    }
#else
    _framebuffer = (uint8_t *)malloc(s);
#endif

    if (!_framebuffer)
    {
        return false;
    }

    // 可以考虑初始化帧缓冲，比如全部清零
    // memset(_framebuffer, 0, s);

    return true;
}

void Arduino_Canvas_6bit::writePixelPreclipped(int16_t x, int16_t y, uint16_t color)
{
    // 将16bit(5-6-5)颜色压缩到 2-2-2 (6bit)
    // 先提取 R/G/B
    // R 在 [11..15], G 在 [5..10], B 在 [0..4]
    uint8_t r = (color >> 11) & 0x1F; // 5 bits
    uint8_t g = (color >> 5) & 0x3F;  // 6 bits
    uint8_t b = color & 0x1F;         // 5 bits

    // 缩减到2 bits
    uint8_t r2 = r >> 3; // 5 bits -> 2 bits
    uint8_t g2 = g >> 4; // 6 bits -> 2 bits
    uint8_t b2 = b >> 3; // 5 bits -> 2 bits

    // 合成 6bit 颜色： [r2:2] [g2:2] [b2:2]
    // 存在 1 字节中，仅用低6位
    uint8_t c = (r2 << 4) | (g2 << 2) | b2;

    // 写入帧缓冲
    int32_t pos = x + (y * _width);
    _framebuffer[pos] = c;
}

void Arduino_Canvas_6bit::flush()
{
    // nop
    // 将帧缓冲中的6bit图像数据刷新到目标输出
    // 你需要在 Arduino_G 中实现一个类似 draw6bitRGBBitmap() 的函数
    // (与 draw3bitRGBBitmap 原理类似)，以下为演示调用

    //   if (_output)
    //   {
    //     _output->draw6bitRGBBitmap(_output_x, _output_y, _framebuffer, _width, _height);
    //   }
}

uint8_t *Arduino_Canvas_6bit::getFramebuffer()
{
    return _framebuffer;
}

#endif // !defined(LITTLE_FOOT_PRINT)
