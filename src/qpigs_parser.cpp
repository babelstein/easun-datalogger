#include "qpigs_parser.h"

/**
 * @brief Parse QPIGS message string into QPIGSData structure
 * 
 * QPIGS response format: "241.6 49.9 241.6 49.9 0000 0000 000 319 00.30 000 000 0029 00.0 000.0 00.30 00000 00010000 00 00 00000 010.Z"
 * 
 * Fields:
 * 1. Input AC voltage [V]
 * 2. Grid Frequency [Hz]
 * 3. AC Output Voltage [V]
 * 4. Output Frequency [Hz]
 * 5. Apparent Output Power [W]
 * 6. Active Output Power [W]
 * 7. Percent Load [%]
 * 8. Internal Bus Voltage [V]
 * 9. Battery Voltage [V]
 * 10. Battery Charging Current [A]
 * 11. Battery Charge Level [%]
 * 12. Heatsink Temperature [°C]
 * 13. PV Input Current [A]
 * 14. PV Input Voltage [V]
 * 15. Battery Voltage from SCC Controller [V]
 * 16. Battery Discharge Current [A]
 * 17. Device Status Flags
 * 18. Additional Data 1/ Reserve
 * 19. Additional Data 2/ Reserve
 * 20. PV Production in Watts [W]
 * 21. Additional Flags
 */
bool parseQPIGSMessage(const String& message, QPIGSData* data) {
    if (!data) {
        return false;
    }
    
    // Reset data
    data->inputACVoltage = 0.0;
    data->gridFrequency = 0.0;
    data->acOutputVoltage = 0.0;
    data->outputFrequency = 0.0;
    data->apparentOutputPower = 0.0;
    data->activeOutputPower = 0.0;
    data->percentLoad = 0.0;
    data->internalBusVoltage = 0.0;
    data->batteryVoltage = 0.0;
    data->batteryChargingCurrent = 0.0;
    data->batteryChargeLevel = 0.0;
    data->heatsinkTemperature = 0.0;
    data->pvInputCurrent = 0.0;
    data->pvInputVoltage = 0.0;
    data->batteryVoltageFromSCC = 0.0;
    data->batteryDischargeCurrent = 0.0;
    data->deviceStatusFlags = 0;
    data->additionalData1 = 0;
    data->additionalData2 = 0;
    data->pvProduction = 0.0;
    data->additionalFlags = 0;
    
    // Remove '(' at the beginning and last 3 bytes (CRC + \r)
    String temp = message;
    if (temp.length() > 3) {
        // Remove '(' at the beginning if present
        if (temp.charAt(0) == '(') {
            temp = temp.substring(1);
        }
        // Remove last 3 bytes (CRC + \r)
        temp = temp.substring(0, temp.length() - 3);
    }
    
    // Split message by spaces
    String values[] = {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""};
    int valueCount = 0;
    
    // Simple space-separated parsing
    int start = 0;
    
    for (int i = 0; i < temp.length(); i++) {
        if (temp[i] == ' ') {
            if (i > start) {
                values[valueCount++] = temp.substring(start, i);
            }
            start = i + 1;
        }
    }
    if (start < temp.length()) {
        values[valueCount++] = temp.substring(start);
    }
    
    // Parse each field
    if (valueCount >= 1) {
        data->inputACVoltage = atof(values[0].c_str());
    }
    if (valueCount >= 2) {
        data->gridFrequency = atof(values[1].c_str());
    }
    if (valueCount >= 3) {
        data->acOutputVoltage = atof(values[2].c_str());
    }
    if (valueCount >= 4) {
        data->outputFrequency = atof(values[3].c_str());
    }
    if (valueCount >= 5) {
        data->apparentOutputPower = atof(values[4].c_str());
    }
    if (valueCount >= 6) {
        data->activeOutputPower = atof(values[5].c_str());
    }
    if (valueCount >= 7) {
        data->percentLoad = atof(values[6].c_str());
    }
    if (valueCount >= 8) {
        data->internalBusVoltage = atof(values[7].c_str());
    }
    if (valueCount >= 9) {
        data->batteryVoltage = atof(values[8].c_str());
    }
    if (valueCount >= 10) {
        data->batteryChargingCurrent = atof(values[9].c_str());
    }
    if (valueCount >= 11) {
        data->batteryChargeLevel = atof(values[10].c_str());
    }
    if (valueCount >= 12) {
        data->heatsinkTemperature = atof(values[11].c_str());
    }
    if (valueCount >= 13) {
        data->pvInputCurrent = atof(values[12].c_str());
    }
    if (valueCount >= 14) {
        data->pvInputVoltage = atof(values[13].c_str());
    }
    if (valueCount >= 15) {
        data->batteryVoltageFromSCC = atof(values[14].c_str());
    }
    if (valueCount >= 16) {
        data->batteryDischargeCurrent = atof(values[15].c_str());
    }
    if (valueCount >= 17) {
        data->deviceStatusFlags = atoi(values[16].c_str());
    }
    if (valueCount >= 18) {
        data->additionalData1 = atoi(values[17].c_str());
    }
    if (valueCount >= 19) {
        data->additionalData2 = atoi(values[18].c_str());
    }
    if (valueCount >= 20) {
        data->pvProduction = atof(values[19].c_str());
    }
    if (valueCount >= 21) {
        data->additionalFlags = atoi(values[20].c_str());
    }
    
    return true;
}

/**
 * @brief Parse QPIGS message string into QPIGSData structure (convenience function)
 */
QPIGSData parseQPIGSMessage(const String& message) {
    QPIGSData data;
    parseQPIGSMessage(message, &data);
    return data;
}
