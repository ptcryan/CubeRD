/*

 Rainbowduino v3.0 Library examples : American Flag

*/

#include <Rainbowduino.h>

unsigned int z,x,y;
uint32_t buffer[4][4][4];

void setup()
{

  Rb.init(); //initialize Rainbowduino driver
  
  for (x = 0; x < 4; x++) {
    for (y = 0; y < 4; y++) {
      for (z = 0; z < 4; z++) {
        if (z % 2 == 1) {
          Rb.setPixelZXY(z, x, y, 0xff0000);
        } else {
          Rb.setPixelZXY(z, x, y, 0xffffff);
        }
      }
    }
  }
  
  Rb.setPixelZXY(3, 0, 0, 0x0000ff);
  Rb.setPixelZXY(2, 0, 0, 0x0000ff);
  Rb.setPixelZXY(3, 1, 0, 0x0000ff);
  Rb.setPixelZXY(2, 1, 0, 0x0000ff);
  Rb.setPixelZXY(3, 0, 1, 0x0000ff);
  Rb.setPixelZXY(2, 0, 1, 0x0000ff);
  Rb.setPixelZXY(3, 1, 1, 0x0000ff);
  Rb.setPixelZXY(2, 1, 1, 0x0000ff);
}

void loop() {}

