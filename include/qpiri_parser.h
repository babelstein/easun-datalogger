#ifndef QPIRI_PARSER_H
#define QPIRI_PARSER_H

#include <Arduino.h>

/**
 * @brief Structure to hold parsed QPIRI message data
 * 
 * QPIRI (Get Configuration) command response contains inverter configuration settings
 */
struct QPIRIData {
    // 1. Rated AC input voltage [V]
    float ratedACInputVoltage;
    
    // 2. Rated AC input current [A]
    float ratedACInputCurrent;
    
    // 3. Rated AC output voltage [V]
    float ratedACOutputVoltage;
    
    // 4. Rated output frequency [Hz]
    float ratedOutputFrequency;
    
    // 5. Rated AC output current [A]
    float ratedACOutputCurrent;
    
    // 6. Rated apparent output power [VA]
    float ratedApparentOutputPower;
    
    // 7. Rated active output power [W]
    float ratedActiveOutputPower;
    
    // 8. Rated battery voltage [V]
    float ratedBatteryVoltage;
    
    // 9. Re-charge voltage [V]
    float rechargeVoltage;
    
    // 10. Cut-off voltage (Under-voltage / Cut-off) [V]
    float cutOffVoltage;
    
    // 11. Bulk charging voltage [V]
    float bulkChargingVoltage;
    
    // 12. Float charging voltage [V]
    float floatChargingVoltage;
    
    // 13. Battery type (probably User-defined or AGM)
    int batteryType;
    
    // 14. Maximum AC charging current [A] (from mains)
    float maximumACChargingCurrent;
    
    // 15. Maximum total charging current [A] (AC + PV)
    float maximumTotalChargingCurrent;
    
    // 16. Input voltage range (0 = Appliance / 1 = UPS)
    int inputVoltageRange;
    
    // 17. Output source (0 = USB, 1 = SUB, 2 = SBU)
    int outputSource;
    
    // 18. Charger priority (0 = Solar First, 1 = Solar & Utility, 2 = Only Solar)
    int chargerPriority;
    
    // 19. Number of parallel inverters (Parallel number)
    int parallelInverters;
    
    // 20. Machine type (Grid-tie / Off-grid)
    int machineType;
    
    // 21. Topology (Transformerless / Transformer)
    int topology;
    
    // 22. Output mode (Single phase / Parallel / 3-phase)
    int outputMode;
    
    // 23. Battery re-discharge voltage [V]
    float batteryReDischargeVoltage;
    
    // 24. PV OK (panel connection status)
    int pvOK;
    
    // 25. PV power status (PV Power Balance)
    int pvPowerStatus;
    
    // 26. Low DC cut-off voltage [V]
    float lowDCCutOffVoltage;
    
    // 27. Max PV charging current [A] (or additional field)
    float maxPVChargingCurrent;
};

/**
 * @brief Parse QPIRI message string into QPIRIData structure
 * 
 * @param message The QPIRI response string (without parentheses)
 * @param data Pointer to QPIRIData structure to populate
 * @return true if parsing was successful, false otherwise
 * 
 * Example input: "230.0 26.9 230.0 50.0 26.9 6200 6200 48.0 44.0 42.0 56.4 54.0 2 030 060 0 2 1 1 01 0 0 52.0 0 1 44.0 10 44.0l"
 */
bool parseQPIRIMessage(const String& message, QPIRIData* data);

/**
 * @brief Parse QPIRI message string into QPIRIData structure (convenience function)
 * 
 * @param message The QPIRI response string (without parentheses)
 * @return QPIRIData populated with parsed values
 */
QPIRIData parseQPIRIMessage(const String& message);

#endif // QPIRI_PARSER_H
