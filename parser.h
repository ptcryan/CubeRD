/*
 * File:    parser.h
 * Version: 0.0
 * Author:  Andy Gelme (@geekscape)
 * License: GPLv3
 */

#ifndef PARSER_h
#define PARSER_h

#include <Arduino.h>
#include <avr/pgmspace.h>
#include "cubeRD.h"
#include "engine.h"

bool printHelp(void);
byte parser(char *message, byte length, bytecode_t *bytecode);

static const byte NUL_   = 0x00;  // Null character
static const byte SPACE_ = 0x20;  // Space bar
static const byte RBRAC_ = 0x29;  // Right bracket ')'

typedef struct command_s {
  char  *name;
  byte (*parser) (
    char             *message,
    byte              length,
    byte             *position,
    struct command_s *command,
    bytecode_t       *bytecode);
  byte (*executer)(bytecode_t bytecode);
} command_t;

#endif  // PARSER_h
