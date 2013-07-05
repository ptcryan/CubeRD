/*
 * File:    serial.h
 * Version: 0.0
 * Author:  Andy Gelme (@geekscape)
 * License: GPLv3
 */

#ifndef SERIAL_h
#define SERIAL_h

#include <Arduino.h>
#include <avr/pgmspace.h>
//#include "color.h"
//#include "engine.h"
#include "parser.h"

static const uint32_t MESSAGE_TIMEOUT = 5000;  // milliseconds
static const uint32_t SERIAL_HANDLER_PERIOD = 5;  // milliseconds

static const byte NUL =   0x00;  // Null character
static const byte STX =   0x02;  // Start of TeXt
static const byte ETX =   0x03;  // End of TeXt
static const byte HT =    0x09;  // Horizontal Tab
static const byte LF =    0x0A;  // Line Feed
static const byte CR =    0x0D;  // Carriage Return
static const byte SPACE = 0x20;  // Space bar
static const byte LBRAC = 0x28;  // Left bracket '('
static const byte RBRAC = 0x29;  // Right bracket ')'
static const byte COMMA = 0x2c;  // Comma ','
static const byte COLON = 0x3a;  // Colon ':'
static const byte SEMIC = 0x3b;  // Semicolon ';'

char message[32];
byte messageLength = 0;

// Set to true the first time the sketch receives a serial command
bool receivedSerialCommand = false;

void serialBegin(byte serialPort, uint32_t baudRate);
void serialHandler(void);
boolean hasReceivedSerialCommand(void);
boolean readMessage(void);

extern Stream *serial;

#endif  // SERIAL_h
