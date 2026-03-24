#ifndef INVERTER_COMMANDS_H
#define INVERTER_COMMANDS_H

#include <Arduino.h>

const unsigned long MILISECONDS_DELAY = 1000;
const int MAX_RETRIES = 3;

// Definicje gotowych ramek z obliczonym CRC (CRC-16/XMODEM) i znakiem \r
const byte cmd_QPIGS[] = {0x51, 0x50, 0x49, 0x47, 0x53, 0xB7, 0xA9, 0x0D}; 
const byte cmd_QMOD[]  = {0x51, 0x4D, 0x4F, 0x44, 0x49, 0xC1, 0x0D};
const byte cmd_QPIRI[] = {0x51, 0x50, 0x49, 0x52, 0x49, 0xF8, 0x54, 0x0D};
const byte cmd_QID[]   = {0x51, 0x49, 0x44, 0xD6, 0xEA, 0x0D};

// Tabela wskaźników do komend i ich rozmiarów
const byte* allCommands[] = {cmd_QPIGS, cmd_QMOD, cmd_QPIRI, cmd_QID};
const int cmdSizes[] = {8, 7, 8, 6};
const char* cmdNames[] = {"QPIGS", "QMOD", "QPIRI", "QID"};
const int numCommands = 4;

#endif // INVERTER_COMMANDS_H
