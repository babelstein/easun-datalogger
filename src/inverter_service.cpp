#include "inverter_service.h"
#include "inverter_commands.h"

InverterService::InverterService(Stream &serial) : _serial(serial), _delay(MILISECONDS_DELAY), _maxRetries(MAX_RETRIES)
{
}

InverterService::~InverterService()
{
}

uint16_t calculateCRC16XMODEM(const String &data)
{
    uint16_t crc = 0x0000;
    uint16_t polynomial = 0x1021;
    
    for (unsigned char b : data)
    {
        crc ^= (static_cast<uint16_t>(b) << 8);
        for (int i = 0; i < 8; i++)
        {
            if (crc & 0x8000)
            {
                crc = (crc << 1) ^ polynomial;
            }
            else
            {
                crc <<= 1;
            }
        }
    }
    return crc;
}

/**
 * @brief Verify CRC of received response
 * @param response The full response string from inverter (includes '(' + data + CRC + \r)
 * @return true if CRC is valid, false otherwise
 */
CRCResult InverterService::verifyCRC(const String& response)
{
    CRCResult result;

    if (response.length() < 3) {
        return result;
    }
    
    // Extract CRC from response (last 2 bytes)
    uint8_t crc_high = static_cast<uint8_t>(response[response.length() - 3]);
    uint8_t crc_low = static_cast<uint8_t>(response[response.length() - 2]);
    uint16_t received_crc = (static_cast<uint16_t>(crc_high) << 8) | crc_low;
    
    // Extract data part (everything before last 3 bytes)
    String data_part = response.substring(0, response.length() - 3);
    
    // Calculate CRC using XMODEM - convert String to byte array
    size_t cmdLength = data_part.length();
    byte cmdBuffer[cmdLength];

    for (size_t i = 0; i < cmdLength; i++) {
        cmdBuffer[i] = static_cast<byte>(data_part.charAt(i));
    }
    
    result.calculated_crc = calculateCRC16XMODEM(data_part);
    result.isValid = result.calculated_crc == received_crc;
    
    return result;
}

/**
 * @brief Check if response contains NAKss (command not received properly)
 * @param response The response string from inverter
 * @return true if NAKss is present, false otherwise
 */
bool InverterService::isNegativeAcknowledgeResponse(const String& response)
{
    return response.indexOf("(NAKss") >= 0;
}

String InverterService::readResult()
{
    String commandResult;
    bool dataStarted = false;
    
    if (_serial.available())
    {
        while (_serial.available())
        {
            char c = _serial.read();
            if(c != '(' && !dataStarted){
                continue;
            }
            if(c == '(') {
                dataStarted = true;
                commandResult += c;
                continue;
            }
            if (c == '\r')
            {
                commandResult += c;
                break; // End of response
            } else {
                commandResult += c;
            }
            yield();
        }
    }
    _serial.flush();
    return commandResult;
}

AllCommandResults InverterService::sendAllCommands()
{
    AllCommandResults results;
    CRCResult crcResult;
    String result = "";
    
    result = "";
    do {
        _serial.write(ALL_COMMANDS[0], CMD_SIZES[0]); // cmd_QPIGS
        delay(_delay);
        result = readResult();
        crcResult = verifyCRC(result);
    } while (!crcResult.isValid || isNegativeAcknowledgeResponse(result));
    results.qpigs = result;

    result = "";
    do {
        _serial.write(ALL_COMMANDS[1], CMD_SIZES[1]); // cmd_QMOD
        delay(_delay);
        result = readResult();
        crcResult = verifyCRC(result);
    } while (!crcResult.isValid || isNegativeAcknowledgeResponse(result));
    results.qmod = result;

    result = "";
    do {
        _serial.write(ALL_COMMANDS[2], CMD_SIZES[2]); // cmd_QPIRI
        delay(_delay);
        result = readResult();
        crcResult = verifyCRC(result);
    } while (!crcResult.isValid || isNegativeAcknowledgeResponse(result));
    results.qpiri = result;
    
    return results;
}

AllCommandResults InverterService::parseAllCommands(AllCommandResults &results)
{
parseQPIGSMessage(results.qpigs, &results.qpigsData);
parseQMODMessage(results.qmod, &results.qmodData);
parseQPIRIMessage(results.qpiri, &results.qpiriData);

return results;
}

CommandResult InverterService::sendCommand(const char *commandName, const String &parameter)
{
    CommandResult result;

    // Build command array: command name + parameter only (no padding)
    byte cmdBuffer[20];
    size_t cmdLength = 0;

    // Add command name characters
    for (size_t i = 0; commandName[i] != '\0'; i++)
    {
        cmdBuffer[cmdLength++] = static_cast<byte>(commandName[i]);
    }

    // Add parameter characters
    for (size_t i = 0; i < parameter.length(); i++)
    {
        char c = parameter.charAt(i);
        if (c >= 32 && c <= 126)
        {
            cmdBuffer[cmdLength++] = static_cast<byte>(c);
        }
    }

    // Calculate CRC on command name + parameter
    uint16_t crc = calculateCRC16XMODEM(String((char *)cmdBuffer));

    // Build final command with CRC and carriage return
    byte finalCommand[20];
    size_t finalLength = 0;

    // Add command name and parameter
    for (size_t i = 0; i < cmdLength; i++)
    {
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

    // Check for Negative Acknowledge response - command not received properly by inverter
    if (isNegativeAcknowledgeResponse(result.data)) {
        result.acknowledged = false;
        result.data = ""; // Clear data to indicate failure
    }
    // Set acknowledged based on whether we received a valid result (not NAKss)
    else {
        result.acknowledged = !result.data.isEmpty();
    }

    return result;
}
