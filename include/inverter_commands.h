#ifndef INVERTER_COMMANDS_H
#define INVERTER_COMMANDS_H

#include <Arduino.h>

const unsigned long MILISECONDS_DELAY = 650;
const int MAX_RETRIES = 3;

// Definicje gotowych ramek z obliczonym CRC (CRC-16/XMODEM) i znakiem \r
static const byte cmd_QPIGS[] = {0x51, 0x50, 0x49, 0x47, 0x53, 0xB7, 0xA9, 0x0D}; 
static const byte cmd_QMOD[]  = {0x51, 0x4D, 0x4F, 0x44, 0x49, 0xC1, 0x0D};
static const byte cmd_QPIRI[] = {0x51, 0x50, 0x49, 0x52, 0x49, 0xF8, 0x54, 0x0D};

// Tabela wskaźników do komend i ich rozmiarów
static const byte* ALL_COMMANDS[] = {cmd_QPIGS, cmd_QMOD, cmd_QPIRI};
static const int CMD_SIZES[] = {8, 7, 8};
static const char* CMD_NAMES[] = {"QPIGS", "QMOD", "QPIRI"};
static const int CMD_COUNT = 3;

#endif // INVERTER_COMMANDS_H
