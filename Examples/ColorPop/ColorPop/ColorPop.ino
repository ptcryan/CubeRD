/*
 * File:    ColorPop.ino
 * Version: 1.0
 * Author:  David Ryan (@ptcryan)
 * License: GPLv3
 */

#include <Rainbowduino.h>

CubeRD cube;

rgb_t colorArray[] = {COLOR_RED, COLOR_GREEN, COLOR_BLUE, COLOR_YELLOW};
rgb_t LED[4][4][4];

void setup(void) {
  byte xPos, yPos, zPos;

  cube.init();

  for (xPos = 0; xPos < 4; xPos++) {
    for (yPos = 0; yPos < 4; yPos++) {
      for (zPos = 0; zPos < 4; zPos++) {
        LED[xPos][yPos][zPos] = colorArray[random(0, 4)];
        cube.set(xPos, yPos, zPos, LED[xPos][yPos][zPos]);
      }
    }
  }
}

void loop(void) {
  UpdateLEDs();
  RandomFade();
  delay(50);

/*
  rgb_t color = COLOR_RED;
  cube.all(color);

  while (color.color[0] > 5) {
    cube.all(color);
    color.color[0] -= 5;
    delay(100);
  }
*/
}

void UpdateLEDs(void) {
  byte xPos, yPos, zPos;

  for (xPos = 0; xPos < 4; xPos++) {
    for (yPos = 0; yPos < 4; yPos++) {
      for (zPos = 0; zPos < 4; zPos++) {
        for (int i = 0; i < 3; i++) {
          if (LED[xPos][yPos][zPos].color[i] <= 1) {
            LED[xPos][yPos][zPos] = colorArray[random(0, 4)];
          }
        }
        cube.set(xPos, yPos, zPos, LED[xPos][yPos][zPos]);
      }
    }
  }
}

void RandomFade(void) {
  byte xPos, yPos, zPos;

  for (xPos = 0; xPos < 4; xPos++) {
    for (yPos = 0; yPos < 4; yPos++) {
      for (zPos = 0; zPos < 4; zPos++) {
        for (int i = 0; i < 3; i++) {
          if (LED[xPos][yPos][zPos].color[i] )

  xPos = random(0, 4);
  yPos = random(0, 4);
  zPos = random(0, 4);            

  LED[random(0, 4)][random(0, 4)][random(0, 4)].color -= 5;
}
