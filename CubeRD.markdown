This Library mixes together the Rainbowduino Rainbow Cube library from
Seeed Technologies and Cube4 from Freetronics.

The intent is to extend the Rainbowduino Library from Seeed to include the
library calls that Cube4 provides. This makes it possible to run the examples
from either library.

Rainbowduino is a Arduino compatible HW platform that provides capability
to drive 8x8 or 4x4x4 RGB LEDs. It is very similar to the Freetronics cube
platform with the major differences being the order in which the LEDs are
driven, and serial support built into the Freetronics library.

Notes:
There is a #define called USE_SERIAL_PORT that allows the built in serial
capability of the Cube4 library to be turned on or off. By default it is
enabled. If it is disabled sketch sizes are reduced considerably (~9KB).
Sketches that use the serial port will still compile, but will not have
serial port functionality.

Known bugs:
The TestPattern sketch does not cycle through the test cases. Serial port
function still works.

Serial1 is not defined in SerialBegin(). It's commented out and left to
the user to do whatever with it. The intent was to bind it to a serial
port alternative to USB serial.

Licenses and Copyright. This library heavily reuses the Rainbowduino and
Cube4 libraries from Seeed Technology Inc. and Freetronics Inc.

Rainbowduino library
/*
 Rainbowduino v3.0 Library. 
 
 2011 Copyright (c) Seeed Technology Inc.
 
 Authors: Albert.Miao, Visweswara R

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
http://www.seeedstudio.com

Cube library
/*
 * Cube4 Library
 *
 * Author:  Andy Gelme (@geekscape) and Marc Alexander (@mtronic)
 * License: GPLv3
 */
http://www.freetronics.com 
