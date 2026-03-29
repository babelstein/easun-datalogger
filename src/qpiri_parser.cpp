#include "qpiri_parser.h"

/**
 * @brief Parse QPIRI message string into QPIRIData structure
 * 
 * QPIRI response format: "230.0 26.9 230.0 50.0 26.9 6200 6200 48.0 44.0 42.0 56.4 54.0 2 030 060 0 2 1 1 01 0 0 52.0 0 1 44.0 10 44.0l"
 * 
 * Fields:
 * 1. Rated AC input voltage [V]
 * 2. Rated AC input current [A]
 * 3. Rated AC output voltage [V]
 * 4. Rated output frequency [Hz]
 * 5. Rated AC output current [A]
 * 6. Rated apparent output power [VA]
 * 7. Rated active output power [W]
 * 8. Rated battery voltage [V]
 * 9. Re-charge voltage [V]
 * 10. Cut-off voltage [V]
 * 11. Bulk charging voltage [V]
 * 12. Float charging voltage [V]
 * 13. Battery type
 * 14. Maximum AC charging current [A]
 * 15. Maximum total charging current [A]
 * 16. Input voltage range
 * 17. Output source
 * 18. Charger priority
 * 19. Number of parallel inverters
 * 20. Machine type
 * 21. Topology
 * 22. Output mode
 * 23. Battery re-discharge voltage [V]
 * 24. PV OK
 * 25. PV power status
 * 26. Low DC cut-off voltage [V]
 * 27. Max PV charging current [A]
 */
bool parseQPIRIMessage(const String& message, QPIRIData* data) {
    if (!data) {
        return false;
    }
    
    // Reset data
    data->ratedACInputVoltage = 0.0;
    data->ratedACInputCurrent = 0.0;
    data->ratedACOutputVoltage = 0.0;
    data->ratedOutputFrequency = 0.0;
    data->ratedACOutputCurrent = 0.0;
    data->ratedApparentOutputPower = 0.0;
    data->ratedActiveOutputPower = 0.0;
    data->ratedBatteryVoltage = 0.0;
    data->rechargeVoltage = 0.0;
    data->cutOffVoltage = 0.0;
    data->bulkChargingVoltage = 0.0;
    data->floatChargingVoltage = 0.0;
    data->batteryType = 0;
    data->maximumACChargingCurrent = 0.0;
    data->maximumTotalChargingCurrent = 0.0;
    data->inputVoltageRange = 0;
    data->outputSource = 0;
    data->chargerPriority = 0;
    data->parallelInverters = 0;
    data->machineType = 0;
    data->topology = 0;
    data->outputMode = 0;
    data->batteryReDischargeVoltage = 0.0;
    data->pvOK = 0;
    data->pvPowerStatus = 0;
    data->lowDCCutOffVoltage = 0.0;
    data->maxPVChargingCurrent = 0.0;
    
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
    String values[] = {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""};
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
        data->ratedACInputVoltage = atof(values[0].c_str());
    }
    if (valueCount >= 2) {
        data->ratedACInputCurrent = atof(values[1].c_str());
    }
    if (valueCount >= 3) {
        data->ratedACOutputVoltage = atof(values[2].c_str());
    }
    if (valueCount >= 4) {
        data->ratedOutputFrequency = atof(values[3].c_str());
    }
    if (valueCount >= 5) {
        data->ratedACOutputCurrent = atof(values[4].c_str());
    }
    if (valueCount >= 6) {
        data->ratedApparentOutputPower = atof(values[5].c_str());
    }
    if (valueCount >= 7) {
        data->ratedActiveOutputPower = atof(values[6].c_str());
    }
    if (valueCount >= 8) {
        data->ratedBatteryVoltage = atof(values[7].c_str());
    }
    if (valueCount >= 9) {
        data->rechargeVoltage = atof(values[8].c_str());
    }
    if (valueCount >= 10) {
        data->cutOffVoltage = atof(values[9].c_str());
    }
    if (valueCount >= 11) {
        data->bulkChargingVoltage = atof(values[10].c_str());
    }
    if (valueCount >= 12) {
        data->floatChargingVoltage = atof(values[11].c_str());
    }
    if (valueCount >= 13) {
        data->batteryType = atoi(values[12].c_str());
    }
    if (valueCount >= 14) {
        data->maximumACChargingCurrent = atof(values[13].c_str());
    }
    if (valueCount >= 15) {
        data->maximumTotalChargingCurrent = atof(values[14].c_str());
    }
    if (valueCount >= 16) {
        data->inputVoltageRange = atoi(values[15].c_str());
    }
    if (valueCount >= 17) {
        data->outputSource = atoi(values[16].c_str());
    }
    if (valueCount >= 18) {
        data->chargerPriority = atoi(values[17].c_str());
    }
    if (valueCount >= 19) {
        data->parallelInverters = atoi(values[18].c_str());
    }
    if (valueCount >= 20) {
        data->machineType = atoi(values[19].c_str());
    }
    if (valueCount >= 21) {
        data->topology = atoi(values[20].c_str());
    }
    if (valueCount >= 22) {
        data->outputMode = atoi(values[21].c_str());
    }
    if (valueCount >= 23) {
        data->batteryReDischargeVoltage = atof(values[22].c_str());
    }
    if (valueCount >= 24) {
        data->pvOK = atoi(values[23].c_str());
    }
    if (valueCount >= 25) {
        data->pvPowerStatus = atoi(values[24].c_str());
    }
    if (valueCount >= 26) {
        data->lowDCCutOffVoltage = atof(values[25].c_str());
    }
    if (valueCount >= 27) {
        data->maxPVChargingCurrent = atof(values[26].c_str());
    }
    
    return true;
}

/**
 * @brief Parse QPIRI message string into QPIRIData structure (convenience function)
 */
QPIRIData parseQPIRIMessage(const String& message) {
    QPIRIData data;
    parseQPIRIMessage(message, &data);
    return data;
}
