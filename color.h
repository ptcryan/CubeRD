/*
 * File:    color.h
 * Version: 0.0
 * Author:  Andy Gelme (@geekscape)
 * License: GPLv3
 */

#ifndef COLOR_h
#define COLOR_h

typedef struct {
  byte color[3];
}
  rgb_t;

#define red 0
#define green 1
#define blue 2

#define RGB(red, green, blue) (rgb_t) { red, green, blue }

#define COLOR_BLACK  RGB(0x00, 0x00, 0x00)
#define COLOR_BLUE   RGB(0x00, 0x00, 0xff)
#define COLOR_GREEN  RGB(0x00, 0xff, 0x00)
#define COLOR_ORANGE RGB(0xff, 0x45, 0x00)
#define COLOR_PINK   RGB(0xff, 0x14, 0x44)
#define COLOR_PURPLE RGB(0xff, 0x00, 0xff)
#define COLOR_RED    RGB(0xff, 0x00, 0x00)
#define COLOR_WHITE  RGB(0xff, 0xff, 0xff)
#define COLOR_YELLOW RGB(0xff, 0xff, 0x00)

#endif  // COLOR_h
