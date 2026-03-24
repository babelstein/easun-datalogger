#ifndef INVERTER_SERVICE_H
#define INVERTER_SERVICE_H

#include <Arduino.h>
#include "inverter_commands.h"
#include "qpigs_parser.h"
#include "qpiri_parser.h"
#include "qmod_parser.h"

// Structure to hold command result
struct CommandResult {
    String data;
    bool acknowledged;
};

// Structure to hold all command results with parsed data
struct AllCommandResults {
    QPIGSData qpigsData;
    QMODData qmodData;
    QPIRIData qpiriData;
    String serialNumber;
};

class InverterService {
public:
    InverterService(Stream& serial);
    ~InverterService();

    // Send all defined commands and return their results as parsed structs
    AllCommandResults sendAllCommands();

    // Send a single command with parameters and return result
    CommandResult sendCommand(const char* commandName, const String& parameter);

    // Calculate CRC-16/XMODEM for a command byte array
    uint16_t calculateCRC16XMODEM(const byte* command, size_t length);

private:
    Stream& _serial;
    unsigned long _delay;
    int _maxRetries;

    // Read result from serial after sending command
    String readResult();
};

#endif // INVERTER_SERVICE_H
