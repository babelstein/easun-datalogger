#ifndef QPIGS_PARSER_H
#define QPIGS_PARSER_H

#include <Arduino.h>

/**
 * @brief Structure to hold parsed QPIGS message data
 * 
 * QPIGS (Get Status) command response contains inverter status information
 */
struct QPIGSData {
    // 1. Input AC voltage [V]
    float inputACVoltage;
    
    // 2. Grid Frequency [Hz]
    float gridFrequency;
    
    // 3. AC Output Voltage [V]
    float acOutputVoltage;
    
    // 4. Output Frequency [Hz]
    float outputFrequency;
    
    // 5. Apparent Output Power [W]
    float apparentOutputPower;
    
    // 6. Active Output Power [W]
    float activeOutputPower;
    
    // 7. Percent Load [%]
    float percentLoad;
    
    // 8. Internal Bus Voltage [V]
    float internalBusVoltage;
    
    // 9. Battery Voltage [V]
    float batteryVoltage;
    
    // 10. Battery Charging Current [A]
    float batteryChargingCurrent;
    
    // 11. Battery Charge Level [%]
    float batteryChargeLevel;
    
    // 12. Heatsink Temperature [°C]
    float heatsinkTemperature;
    
    // 13. PV Input Current [A]
    float pvInputCurrent;
    
    // 14. PV Input Voltage [V]
    float pvInputVoltage;
    
    // 15. Battery Voltage from SCC Controller [V]
    float batteryVoltageFromSCC;
    
    // 16. Battery Discharge Current [A]
    float batteryDischargeCurrent;
    
    // 17. Device Status Flags (raw string)
    String deviceStatusFlags;
    
    // 18. Device Status Flags - Bit 0: SBU Priority
    bool sbuPriority;
    
    // 19. Device Status Flags - Bit 1: Configuration changed
    bool configChanged;
    
    // 20. Device Status Flags - Bit 2: SCC Firmware updated
    bool sccFirmwareUpdated;
    
    // 21. Device Status Flags - Bit 3: Load status
    bool loadStatus;
    
    // 22. Device Status Flags - Bit 4: Battery voltage steady
    bool batteryVoltageSteady;
    
    // 23. Device Status Flags - Bit 5: Charging status
    bool chargingStatus;
    
    // 24. Device Status Flags - Bit 6: SCC charging status
    bool sccChargingStatus;
    
    // 25. Device Status Flags - Bit 7: AC charging status
    bool acChargingStatus;
    
    // 26. Additional Data 1/ Reserve
    int additionalData1;
    
    // 27. Additional Data 2/ Reserve
    int additionalData2;
    
    // 28. PV Production in Watts [W]
    float pvProduction;
    
    // 29. Additional Flags
    int additionalFlags;
};

/**
 * @brief Parse QPIGS message string into QPIGSData structure
 * 
 * @param message The QPIGS response string (without parentheses)
 * @param data Pointer to QPIGSData structure to populate
 * @return true if parsing was successful, false otherwise
 * 
 * Example input: "241.6 49.9 241.6 49.9 0000 0000 000 319 00.30 000 000 0029 00.0 000.0 00.30 00000 00010000 00 00 00000 010.Z"
 */
bool parseQPIGSMessage(const String& message, QPIGSData* data);

/**
 * @brief Parse QPIGS message string into QPIGSData structure (convenience function)
 * 
 * @param message The QPIGS response string (without parentheses)
 * @return QPIGSData populated with parsed values
 */
QPIGSData parseQPIGSMessage(const String& message);

#endif // QPIGS_PARSER_H
