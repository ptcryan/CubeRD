/*
 * File:    serial.cpp
 * Version: 0.0
 * Author:  Andy Gelme (@geekscape)
 * License: GPLv3
 *
 * Low-level Cube serial communications.
 *
 * ToDo
 * ~~~~
 * - None, yet.
 */

#include "serial.h"

boolean readMessage(void);

uint32_t messageTimer = 0;
uint32_t serialTimer = 0;
Stream *serial;

// Set to true the first time the sketch receives a serial command
// using volatile since it's modified in an ISR.
volatile bool receivedSerialCommand = false;

void serialBegin(byte serialPort, uint32_t baudRate) {
  serial = NULL;

  switch (serialPort) {
    case 0:
      Serial.begin(baudRate);
      serial = & Serial;
      break;

    case 1:
      // FIXME: Where is Serial1 declared?
//      Serial1.begin(baudRate);
      Serial.begin(baudRate);
//      serial = & Serial1;
      serial = & Serial;
      break;
  }

  if (serial) {
    serial->println("[Cube 1.0]");
    serial->println("Type 'help;'");
  }
}

void serialHandler(void) {
  if (serial) {
    uint32_t timeNow = millis();

    if (messageLength > 0) {
      if (timeNow >= messageTimer) messageLength = 0;
    }

    if (timeNow >= serialTimer) {
      serialTimer = timeNow + SERIAL_HANDLER_PERIOD;

      if (readMessage()) {
        receivedSerialCommand = true;
        bytecode_t bytecode = {};
        byte errorCode = parser(message, messageLength, & bytecode);
        messageLength = 0;
      }
    }
  }
}

bool hasReceivedSerialCommand() {
  return(receivedSerialCommand);
}

boolean readMessage() {
  while (serial->available()) {
    messageTimer = millis() + MESSAGE_TIMEOUT;

    char data = serial->read();

    switch (data) {
      case CR:
      case SEMIC:
        return(true);  // complete message ready

      case HT:
      case LF:
        data = SPACE;

      case SPACE:
        if (messageLength < 1) break;
        if (message[messageLength - 1] == SPACE) break;

      default:
        if (data == LBRAC) messageLength = 0;

        if (messageLength < (sizeof(message) - 1)) {
          message[messageLength ++] = data;
          message[messageLength] = 0x00;
          if (data == RBRAC) return(true);  // complete message ready
        }
        break;
    }
  }

  return(false);  // partial message only
}
