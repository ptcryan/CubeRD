/*
 CubeRD 1.0 Library. 
 
 Copyright (c) 2013 David Ryan

 This library heavily reuses the Rainbowduino and Cube4 libraries from
 Seeed Technology Inc. and Freetronics Inc.
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*
This library heavily makes use of line, circle and other shape drawing algorithms and code presented
in paper "The Beauty of Bresenham's Algorithm" by Alois Zingl, Vienna, Austria. The original author
has kindly released these example programs with no copyright.
*/

#include "CubeRD.h"

// If defined then include the code to add serial port function.
// However, it uses a lot of memory (~9KB), so comment out if you don't need it.
#define USE_SERIAL_PORT

void send16bitData(unsigned int data);
void clearDisplay(void);
void latchData(void);
void switchOnDrive(unsigned char line);

void cubeAll(rgb_t rgb);
void cubeFillPlaneZ(byte z, rgb_t rgb);
void cubeSet(unsigned char x,
                           unsigned char y,
                           unsigned char z,
                           rgb_t rgb);
void cubeShift(byte axis, byte direction);
void cubeNext(rgb_t rgb);
void cubeCopyplane(byte axis, byte position, byte destination);
void cubeMoveplane(byte axis, byte position, byte destination, rgb_t rgb);
void cubeSetplane(byte axis, byte offset, rgb_t rgb);
void cubeSetPixelXY(unsigned char x,
                              unsigned char y,
                              unsigned char colorR,
                              unsigned char colorG,
                              unsigned char colorB);
rgb_t getPixelZXY(unsigned char z,
                  unsigned char x,
                  unsigned char y);

#ifdef USE_SERIAL_PORT
extern void serialBegin(byte serialPort, uint32_t baudRate);
extern void serialHandler(void);
extern bool hasReceivedSerialCommand(void);
extern bool printHelp(void);
#endif  // USE_SERIAL_PORT

// Frame Buffer -- placed in RAM
unsigned char frameBuffer[3][8][8]= {
// blue
        {{0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0}},
// green
        {{0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0}},
// red
        {{0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0}}
};

// Matrix to Cube mapping
unsigned char ZX[4][4] = {
  {7, 7, 0, 0},
  {6, 6, 1, 1},
  {5, 5, 2, 2},
  {4, 4, 3, 3},
};

unsigned char YX[4][4] = {
  {0, 7, 0, 7},
  {1, 6, 1, 6},
  {2, 5, 2, 5},
  {3, 4, 3, 4}
};

unsigned char cursorX, cursorY, cursorZ;

unsigned char lineDrive = 0;  // used within ISR

void CubeRD::begin(byte serialPort, uint32_t baudRate) {
#ifdef USE_SERIAL_PORT
  serialBegin(serialPort, baudRate);
#endif  // USE_SERIAL_PORT

  init();
  all(COLOR_BLACK);
}

// Init the Port lines and invoke timer 1 configuration
void CubeRD::init() {
  DDR_Lines |= BIT_Lines;
  PORT_Lines &= ~BIT_Lines;

  DDRD |= 0x04;

  DDR_Data |= BIT_Data;
  DDR_Clk |= BIT_Clk;
  PORT_Data &= ~BIT_Data;
  PORT_Clk &= ~BIT_Clk;

  DDRB |= 0x20;

  clearDisplay();
  init_timer1();  // Configure interrupt
}

// Configure Timer 1 ISR for periodic 100uS interrupts
// initialize Timer1 to overflow every  100uS
void CubeRD::init_timer1(void) {
  TCCR1A = 0;           // clear control register A
  TCCR1B = _BV(WGM13);  // set mode as phase and frequency
                          // correct pwm, stop the timer
  ICR1 = 10000;         // (XTAL * microseconds) / 2000000  1mS cycles
  TIMSK1 = _BV(TOIE1);
  TCNT1 = 0;
  TCCR1B |= _BV(CS10);
  sei();                // enable global interrupt
}

// Routine to send 16bit data to MY9221 driver chips

void send16bitData(unsigned int data) {
  for (unsigned char i = 0; i < 16; i++) {
    if (data & 0x8000) {
        PORT_Data |= BIT_Data;
    } else {
        PORT_Data &= ~BIT_Data;
    }

    PORT_Clk ^= BIT_Clk;
    data <<= 1;
  }
}

// latch routine for MY9221 data exchange

void latchData(void) {
  PORT_Data &= ~BIT_Data;

  delayMicroseconds(10);
  switchOffDrive;
  for (unsigned char i = 0; i < 8; i++) {
    PORT_Data ^= BIT_Data;
  }
}

void switchOnDrive(unsigned char line) {
  unsigned char LineBits = ((line) << 4);
  PORT_Lines &= ~BIT_Lines;
  PORT_Lines |= LineBits;
  PORT_Lines |= 0x80;
}

// clear MY9221 lines. Internally used for avoiding flicker.
// This is not the same as blank disply.

void clearDisplay(void) {
  unsigned char i = 0;
  unsigned char f = 0;

  send16bitData(CmdMode);
  PORT_Data &= ~BIT_Data;
  for (i = 0; i < 192; i++) {
    PORT_Clk ^= BIT_Clk;
  }

  send16bitData(CmdMode);
  PORT_Data &= ~BIT_Data;
  for (i = 0; i < 192; i++) {
    PORT_Clk ^= BIT_Clk;
  }
  latchData();
}

// blank all pixels
void CubeRD::blankDisplay(void) {
  for (unsigned char x = 0; x <= 7; x++) {
    for (unsigned char y = 0; y <= 7; y++) {
      frameBuffer[0][x][y] = 0x00;
      frameBuffer[1][x][y] = 0x00;
      frameBuffer[2][x][y] = 0x00;
    }
  }
}

// set the pixel (X,Y) of RGB matrix with colour 24-bit RGB Colour
void CubeRD::setPixelXY(unsigned char x,
                        unsigned char y,
                        uint32_t colorRGB /*24-bit RGB Color*/) {
  if (x > 7 || y > 7) {
  // Do nothing.
  // This check is used to avoid writing to out-of-bound pixels by graphics
  // function. But this might slow down setting pixels (remove this check
  // if fast disply is desired)
  } else {
    colorRGB = (colorRGB & 0x00FFFFFF);
    frameBuffer[0][x][y] = (colorRGB & 0x0000FF);  // channel Blue
    colorRGB = (colorRGB >> 8);
    frameBuffer[1][x][y]=(colorRGB & 0x0000FF);  // channel Green
    colorRGB = (colorRGB >> 8);
    frameBuffer[2][x][y]=(colorRGB & 0x0000FF);  // channel Red
    }
}

// set the pixel (X,Y) of RGB matrix with colours R,G,B
void CubeRD::setPixelXY(unsigned char x,
                        unsigned char y,
                        unsigned char colorR,
                        unsigned char colorG,
                        unsigned char colorB) {
  frameBuffer[0][x][y] = colorB;  // channel Blue
  frameBuffer[1][x][y] = colorG;  // channel Green
  frameBuffer[2][x][y] = colorR;  // channel Red
}

// set the pixel (Z,X,Y) of RGB Cube with colour 24-bit RGB Colour
void CubeRD::setPixelZXY(unsigned char z,
                         unsigned char x,
                         unsigned char y,
                         uint32_t colorRGB /*24-bit RGB Color*/) {
  setPixelXY(ZX[z][x], YX[y][x], colorRGB);
}

// set the pixel (Z,X,Y) of RGB Cube with colours R,G,B
void CubeRD::setPixelZXY(unsigned char z,
                         unsigned char x,
                         unsigned char y,
                         unsigned char colorR,
                         unsigned char colorG,
                         unsigned char colorB) {
  setPixelXY(ZX[z][x], YX[y][x], colorR, colorG, colorB);
}

// fill the frame buffer starting from 'start'
// to ending at 'end' with values in colorRGB array.
// Pixels range is from 0,1,2,.....61,62
void CubeRD::setPixelXY(unsigned char start,
                              unsigned char end,
                              uint32_t *colorRGB) {
  unsigned char ci = 0;
  for (unsigned char i = start; i <= end; i++) {
    setPixelXY(i / 8, i % 8, colorRGB[ci]);
    ci++;
  }
}

rgb_t getPixelZXY(unsigned char z,
                  unsigned char x,
                  unsigned char y) {
  return RGB(frameBuffer[COLOR_PLANE_RED]   [ZX[z][x]] [YX[y][x]],
             frameBuffer[COLOR_PLANE_GREEN] [ZX[z][x]] [YX[y][x]],
             frameBuffer[COLOR_PLANE_BLUE]  [ZX[z][x]] [YX[y][x]]);
}

void CubeRD::set(unsigned char x,
                 unsigned char y,
                 unsigned char z,
                 rgb_t colorRGB) {
  cubeSet(x, y, z, colorRGB);
}

void cubeSet(unsigned char x,
             unsigned char y,
             unsigned char z,
             rgb_t rgb) {
  cubeSetPixelXY(ZX[z][x],
             YX[y][x],
             rgb.color[red],
             rgb.color[green],
             rgb.color[blue]);

  cursorX = x;
  cursorY = y;
  cursorZ = z;
}

void cubeSetPixelXY(unsigned char x,
                    unsigned char y,
                    unsigned char colorR,
                    unsigned char colorG,
                    unsigned char colorB) {
  frameBuffer[0][x][y] = colorB;  // channel Blue
  frameBuffer[1][x][y] = colorG;  // channel Green
  frameBuffer[2][x][y] = colorR;  // channel Red
}

void CubeRD::all(rgb_t rgb) {
    cubeAll(rgb);
}

void cubeAll(rgb_t rgb) {
  for (byte z = 0;  z < CUBE_SIZE;  z++) cubeFillPlaneZ(z, rgb);
}

void cubeFillPlaneZ(byte  z, rgb_t rgb) {
  for (byte y = 0;  y < CUBE_SIZE;  y++) {
    for (byte x = 0;  x < CUBE_SIZE;  x++) {
      cubeSet(x, y, z, rgb);
    }
  }
}

void CubeRD::setplane(byte axis, byte offset, rgb_t rgb) {
  cubeSetplane(axis, offset, rgb);
}

void cubeSetplane(byte axis, byte offset, rgb_t rgb) {
  if (axis == X) {
    byte y = 0;
    byte z = 0;
    for (byte z = 0;  z < CUBE_SIZE;  z++) {
      for (byte y = 0;  y < CUBE_SIZE;  y++) {
        cubeSet(offset, y, z, rgb);
      }
    }
  }

  if (axis == Y) {
    byte x = 0;
    byte z = 0;
    for (byte z = 0;  z < CUBE_SIZE;  z++) {
      for (byte x = 0;  x < CUBE_SIZE;  x++) {
        cubeSet(x, offset, z, rgb);
      }
    }
  }

  if (axis == Z) {
    byte x = 0;
    byte y = 0;
    for (byte y = 0;  y < CUBE_SIZE;  y++) {
      for (byte x = 0;  x < CUBE_SIZE;  x++) {
        cubeSet(x, y, offset, rgb);
      }
    }
  }
}

void CubeRD::next(rgb_t rgb) {
  cubeNext(rgb);
}

void cubeNext(rgb_t rgb) {
  cursorX++;
  if (cursorX > CUBE_SIZE - 1) {
    cursorX = 0;
    cursorY++;
    if (cursorY > CUBE_SIZE - 1) {
      cursorY = 0;
      cursorZ++;
      if (cursorZ > CUBE_SIZE - 1) {
        cursorZ = 0;
      }
    }
  }

  cubeSet(cursorX, cursorY, cursorZ, rgb);
}

void CubeRD::shift(byte axis, byte direction) {
  cubeShift(axis, direction);
}

void cubeShift(byte axis, byte direction) {
  if (direction == '+') {
    for (byte i = CUBE_SIZE - 1; i > 0; i--) {
      cubeCopyplane(axis, i - 1, i);
    }
    cubeSetplane(axis, 0, COLOR_BLACK);
  }
  if (direction == '-') {
    for (byte i = 0; i < CUBE_SIZE - 1; i++) {
      cubeCopyplane(axis, i + 1, i);
    }
    cubeSetplane(axis, 3, COLOR_BLACK);
  }
}

void CubeRD::copyplane(byte axis, byte position, byte destination) {
  cubeCopyplane(axis, position, destination);
}

void cubeCopyplane(byte axis, byte position, byte destination) {
  if (axis == X) {
    byte y = 0;
    byte z = 0;
    for (byte z = 0;  z < CUBE_SIZE;  z++) {
      for (byte y = 0;  y < CUBE_SIZE;  y++) {
        cubeSet(destination, y, z, getPixelZXY(position, y, z));
      }
    }
  }
  if (axis == Y) {
    byte x = 0;
    byte z = 0;
    for (byte z = 0;  z < CUBE_SIZE;  z++) {
      for (byte x = 0;  x < CUBE_SIZE;  x++) {
        cubeSet(x, destination, z, getPixelZXY(x, position, z));
      }
    }
  }
  if (axis == Z) {
    byte x = 0;
    byte y = 0;
    for (byte y = 0;  y < CUBE_SIZE;  y++) {
      for (byte x = 0;  x < CUBE_SIZE;  x++) {
        cubeSet(x, y, destination, getPixelZXY(x, y, position));
      }
    }
  }
}

void CubeRD::moveplane(byte axis,
                       byte position,
                       byte destination,
                       rgb_t rgb) {
  cubeMoveplane(axis, position, destination, rgb);
}

void cubeMoveplane(byte axis,
                   byte position,
                   byte destination,
                   rgb_t rgb) {
  cubeCopyplane(axis, position, destination);
  cubeSetplane(axis, position, rgb);
}

bool CubeRD::hasReceivedSerialCommand(void) {
#ifdef USE_SERIAL_PORT
  return(hasReceivedSerialCommand());
#endif  // USE_SERIAL_PORT
#ifndef USE_SERIAL_PORT  // !USE_SERIAL_PORT
  return(false);
#endif  // !USE_SERIAL_PORT
}

void CubeRD::drawCircle(int poX, int poY, int r, uint32_t color) {
  int x = -r, y = 0, err = 2-2*r, e2;
  do {
    setPixelXY(poX - x, poY + y, color);
    setPixelXY(poX + x, poY + y, color);
    setPixelXY(poX + x, poY - y, color);
    setPixelXY(poX - x, poY - y, color);
    e2 = err;
    if (e2 <= y) {
      err += ++y * 2 + 1;
      if (-x == y && e2 <= x) e2 = 0;
    }
      if (e2 > x) err += ++x * 2 + 1;
  } while (x <= 0);
}

void CubeRD::fillCircle(int poX, int poY, int r, uint32_t color) {
  int x = -r, y = 0, err = 2 - 2 * r, e2;
  do {
    drawVerticalLine(poX - x, poY - y, 2 * y, color);
    drawVerticalLine(poX + x, poY - y, 2 * y, color);

    e2 = err;
    if (e2 <= y) {
      err += ++y * 2 + 1;
      if (-x == y && e2 <= x) e2 = 0;
    }
      if (e2 > x) err += ++x * 2 + 1;
  } while (x <= 0);
}

void CubeRD::drawLine(unsigned int x0,
                      unsigned int y0,
                      unsigned int x1,
                      unsigned int y1,
                      uint32_t color) {
  int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
  int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
  int err = dx+dy, e2; /* error value e_xy */
  for (;;) {  /* loop */
    setPixelXY(x0, y0, color);
    e2 = 2 * err;
    if (e2 >= dy) { /* e_xy+e_x > 0 */
      if (x0 == x1) break;
      err += dy;
      x0 += sx;
    }
    if (e2 <= dx) { /* e_xy+e_y < 0 */
      if (y0 == y1) break;
      err += dx;
      y0 += sy;
    }
  }
}

void CubeRD::drawVerticalLine(unsigned int poX,
                              unsigned int poY,
                              unsigned int length,
                              uint32_t color) {
  drawLine(poX, poY, poX, poY + length - 1, color);
}

void CubeRD::drawHorizontalLine(unsigned int poX,
                                unsigned int poY,
                                unsigned int length,
                                uint32_t color) {
  drawLine(poX, poY, poX + length - 1, poY, color);
}

void CubeRD::drawRectangle(unsigned int poX,
                           unsigned int poY,
                           unsigned int length,
                           unsigned int width,
                           uint32_t color) {
  drawHorizontalLine(poX, poY, length, color);
  drawHorizontalLine(poX, poY + width - 1, length, color);
  drawVerticalLine(poX, poY, width, color);
  drawVerticalLine(poX + length - 1, poY, width, color);
}

void CubeRD::fillRectangle(unsigned int poX,
                           unsigned int poY,
                           unsigned int length,
                           unsigned int width,
                           uint32_t color) {
  for (unsigned int i = 0; i < width; i++) {
    drawHorizontalLine(poX, poY+i, length, color);
  }
}

void CubeRD::drawChar(unsigned char ascii,
                      unsigned int poX,
                      unsigned int poY,
                      uint32_t colorRGB) {
  if ((ascii < 0x20) || (ascii > 0x7e)) {  // Unsupported char.
    ascii = '?';
  }
  for (unsigned char i = 0; i < 8; i++) {
    unsigned char temp = pgm_read_byte(&simpleFont[ascii - 0x20][i]);
    for (unsigned char f = 0; f < 8; f++) {
      if ((temp >> f) & 0x01) {
        setPixelXY(poY + f, poX + i, colorRGB);
      }
    }
  }
}

// Timer1 Interrupt Service Routine
// All frameBuffer data exchange happens here

ISR(TIMER1_OVF_vect) {
  // An Ideal ISR has to be short and not make any function calls
  // But, in this case only data exchange happens.

  clearDisplay();  // clear current display to avoid the next line dim light

  unsigned char line = (lineDrive & 0x07);

  send16bitData(CmdMode);

  send16bitData(frameBuffer[0][line][7]);
  send16bitData(frameBuffer[0][line][6]);
  send16bitData(frameBuffer[0][line][5]);
  send16bitData(frameBuffer[0][line][4]);
  send16bitData(frameBuffer[0][line][3]);
  send16bitData(frameBuffer[0][line][2]);
  send16bitData(frameBuffer[0][line][1]);
  send16bitData(frameBuffer[0][line][0]);

  send16bitData(frameBuffer[1][line][7]);
  send16bitData(frameBuffer[1][line][6]);
  send16bitData(frameBuffer[1][line][5]);
  send16bitData(frameBuffer[1][line][4]);

  send16bitData(CmdMode);

  send16bitData(frameBuffer[1][line][3]);
  send16bitData(frameBuffer[1][line][2]);
  send16bitData(frameBuffer[1][line][1]);
  send16bitData(frameBuffer[1][line][0]);

  send16bitData(frameBuffer[2][line][7]);
  send16bitData(frameBuffer[2][line][6]);
  send16bitData(frameBuffer[2][line][5]);
  send16bitData(frameBuffer[2][line][4]);
  send16bitData(frameBuffer[2][line][3]);
  send16bitData(frameBuffer[2][line][2]);
  send16bitData(frameBuffer[2][line][1]);
  send16bitData(frameBuffer[2][line][0]);

  latchData();
  switchOnDrive(line);
  lineDrive++;

  PORTD &= ~0x04;

#ifdef USE_SERIAL_PORT
  // check for serial data coming in
  serialHandler();

  // now enable interrupts and if necessary print help
  sei();
  printHelp();
#endif  // USE_SERIAL_PORT
}
