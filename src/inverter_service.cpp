#include "inverter_service.h"
#include "inverter_commands.h"

InverterService::InverterService(Stream& serial) : _serial(serial), _delay(MILISECONDS_DELAY), _maxRetries(MAX_RETRIES) {
}

InverterService::~InverterService() {
}

uint16_t InverterService::calculateCRC16XMODEM(const byte* command, size_t length) {
    uint16_t crc = 0;
    
    for (size_t i = 0; i < length; i++) {
        crc ^= (command[i] << 8);
        
        for (int j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0xA001;
            } else {
                crc = crc << 1;
            }
        }
    }
    
    return crc;
}

String InverterService::readResult() {
    String commandResult;
    
    if (_serial.available()) {
        while (_serial.available()) {
            char c = _serial.read();
            
            if (c == '(') {
                continue;
            }
            else if (c == ')') {
                continue;
            }
            else if (c == '\r') {
                return commandResult;
            } 
            else if (c >= 32 && c <= 126) {
                commandResult += c;
            }
            yield();
        }
    }
    _serial.flush();
    return commandResult;
}

AllCommandResults InverterService::sendAllCommands() {
    AllCommandResults results;
    
    // Send QPIGS command and parse result
    _serial.write(ALL_COMMANDS[0], CMD_SIZES[0]); // cmd_QPIGS
    delay(_delay);
    String qpigsResult = readResult();
    parseQPIGSMessage(qpigsResult, &results.qpigsData);
    
    // Send QMOD command and parse result
    _serial.write(ALL_COMMANDS[1], CMD_SIZES[1]); // cmd_QMOD
    delay(_delay);
    String qmodResult = readResult();
    parseQMODMessage(qmodResult, &results.qmodData);
    
    // Send QPIRI command and parse result
    _serial.write(ALL_COMMANDS[2], CMD_SIZES[2]); // cmd_QPIRI
    delay(_delay);
    String qpiriResult = readResult();
    parseQPIRIMessage(qpiriResult, &results.qpiriData);

    return results;
}

CommandResult InverterService::sendCommand(const char* commandName, const String& parameter) {
    CommandResult result;
    
    // Build command array: command name + parameter only (no padding)
    byte cmdBuffer[20];
    size_t cmdLength = 0;
    
    // Add command name characters
    for (size_t i = 0; commandName[i] != '\0'; i++) {
        cmdBuffer[cmdLength++] = static_cast<byte>(commandName[i]);
    }
    
    // Add parameter characters
    for (size_t i = 0; i < parameter.length(); i++) {
        char c = parameter.charAt(i);
        if (c >= 32 && c <= 126) {
            cmdBuffer[cmdLength++] = static_cast<byte>(c);
        }
    }
    
    // Calculate CRC on command name + parameter
    uint16_t crc = calculateCRC16XMODEM(cmdBuffer, cmdLength);
    
    // Build final command with CRC and carriage return
    byte finalCommand[20];
    size_t finalLength = 0;
    
    // Add command name and parameter
    for (size_t i = 0; i < cmdLength; i++) {
        finalCommand[finalLength++] = cmdBuffer[i];
    }
    
    // Add CRC bytes (low byte first, then high byte)
    finalCommand[finalLength++] = static_cast<byte>(crc & 0xFF);
    finalCommand[finalLength++] = static_cast<byte>((crc >> 8) & 0xFF);
    
    // Add carriage return
    finalCommand[finalLength++] = 0x0D;
    
    // Send command
    _serial.write(finalCommand, finalLength);
    delay(_delay * 2);
    
    // Read result
    result.data = readResult();
    
    // Set acknowledged based on whether we received a result
    result.acknowledged = !result.data.isEmpty();
    
    return result;
}

