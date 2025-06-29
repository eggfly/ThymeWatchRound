#include "pat9125.h"
#include <Wire.h>

PAT9125::PAT9125(TwoWire *wire, uint8_t addr)
{
  _addr = addr;
  _wire = wire;
  //_xres = xres;
  //_yres = yres;
}

void PAT9125::pat9125_init()
{
  write_reg(PAT9125_CONFIG, 0x97); // reset
}

void PAT9125::pat9125_reset()
{
  write_reg(PAT9125_CONFIG, 0x97); // reset
  // write_reg(PAT9125_CONFIG, 0x98);
  delay(1);
  pat9125_set_res(_yres, _xres, _bitres12);
  delay(1);
  pat9125_x = 0;
  pat9125_y = 0;
  pat9125_x2 = 0;
  pat9125_y2 = 0;
  pat9125_b = 0;
  pat9125_s = 0;
}

bool PAT9125::pat9125_read_pid()
{
  pat9125_PID1 = read_reg(PAT9125_PID1); // 0x31 = 49dec
  pat9125_PID2 = read_reg(PAT9125_PID2); // 0x91 = 145dex
  if ((pat9125_PID1 == 0x31) && (pat9125_PID2 == 0x91))
    return true;
  else
    return false;
}

void PAT9125::pat9125_set_res(uint8_t xres, uint8_t yres, bool bitres12)
{
  if (bitres12)
    write_reg(PAT9125_ORIENTATION, 0x04); // 12bit resolution
  write_reg(PAT9125_RES_X, xres);
  write_reg(PAT9125_RES_Y, yres);
  _yres = yres;
  _xres = xres;
  _bitres12 = _bitres12;
}

void PAT9125::pat9125_set_res_x(uint8_t xres)
{
  write_reg(PAT9125_RES_X, xres);
  _xres = xres;
}

void PAT9125::pat9125_set_res_y(uint8_t yres)
{
  write_reg(PAT9125_RES_Y, yres);
  _yres = yres;
}

bool PAT9125::pat9125_read_test()
{

  pat9125_b = read_reg(PAT9125_FRAME);
  pat9125_s = read_reg(PAT9125_SHUTTER);

  if ((pat9125_s == -1) or (pat9125_b == -1))
    return true;
  return false;
}

void PAT9125::pat9125_update()
{
  ucMotion = read_reg(PAT9125_MOTION);
  pat9125_b = read_reg(PAT9125_FRAME);
  pat9125_s = read_reg(PAT9125_SHUTTER);
  if (ucMotion & 0x80)
  {
    ucXL = read_reg(PAT9125_DELTA_XL);
    ucYL = read_reg(PAT9125_DELTA_YL);
    ucXYH = read_reg(PAT9125_DELTA_XYH);

    // pat9125_x2 += ucXL;
    // pat9125_y2 -= ucYL;

    iDX = ucXL | ((ucXYH << 4) & 0xf00);
    // if(iDX & 0x800)    iDX |= 0xfffff000;//16bit resolution
    iDY = ucYL | ((ucXYH << 8) & 0xf00);
    // if(iDY & 0x800)    iDY |= 0xfffff000;//16bit resolution

    if (iDX & 0x800)
      iDX -= 4096;
    if (iDY & 0x800)
      iDY -= 4096;

    pat9125_x += iDX;
    pat9125_y -= iDY;
  }
}

void PAT9125::pat9125_update_y()
{
  ucMotion = read_reg(PAT9125_MOTION);
  if (ucMotion & 0x80)
  {
    ucYL = read_reg(PAT9125_DELTA_YL);
    ucXYH = read_reg(PAT9125_DELTA_XYH);
    iDY = ucYL | ((ucXYH << 8) & 0xf00);
    if (iDY & 0x800)
      iDY -= 4096;
    pat9125_y -= iDY;
  }
}

void PAT9125::pat9125_update_y2()
{
  ucMotion = read_reg(PAT9125_MOTION);
  if (ucMotion & 0x80)
  {
    dy = read_reg(PAT9125_DELTA_YL);
    pat9125_y2 -= dy;
  }
}

void PAT9125::pat9125_update_x()
{
  ucMotion = read_reg(PAT9125_MOTION);
  if (ucMotion & 0x80)
  {
    ucXL = read_reg(PAT9125_DELTA_XL);
    ucXYH = read_reg(PAT9125_DELTA_XYH);
    iDX = ucXL | ((ucXYH << 4) & 0xf00);
    if (iDX & 0x800)
      iDX -= 4096;
    pat9125_x += iDX;
  }
}

void PAT9125::pat9125_update_x2()
{
  ucMotion = read_reg(PAT9125_MOTION);
  if (ucMotion & 0x80)
  {
    dx = read_reg(PAT9125_DELTA_XL);
    pat9125_x2 += dx;
  }
}

//----PRIVATE----
void PAT9125::write_reg(uint8_t reg, uint8_t _data)
{
  uint8_t _reg = reg;
  _wire->beginTransmission(_addr);
  _wire->write(_reg);
  _wire->write(_data);
  _wire->endTransmission();
  delay(1);
}

uint16_t PAT9125::read_reg(uint8_t reg)
{
  uint8_t _reg = reg;
  uint8_t val2 = 4;
  _wire->beginTransmission(_addr);
  _wire->write(_reg);
  _wire->endTransmission();
  _wire->requestFrom(_addr, val2);
  uint8_t c = _wire->read();
  _wire->endTransmission();
  delay(1);
  return c;
}
