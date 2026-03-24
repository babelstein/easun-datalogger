#ifndef QMOD_PARSER_H
#define QMOD_PARSER_H

#include <Arduino.h>

/**
 * @brief Enumeration for QMOD power modes
 */
enum class QMODMode {
    UNKNOWN = -1,
    POWER_ON = 'P',   // Power On Mode - Falownik sprawdza system, ale jeszcze nie podaje napięcia
    STANDBY = 'S',    // Standby Mode - Urządzenie jest włączone, ale wyjście AC jest wyłączone
    LINE = 'L',       // Line Mode - Tryb sieciowy (Bypass)
    BATTERY = 'B',    // Battery Mode - Tryb bateryjny
    FAULT = 'F',      // Fault Mode - Tryb awaryjny
    POWER_SAVING = 'H', // Power Saving Mode - Tryb oszczędzania energii
    SHUTDOWN = 'D'    // Shutdown Mode - Tryb wyłączania
};

/**
 * @brief Structure to hold parsed QMOD message data
 */
struct QMODData {
    // Mode character (P, S, L, B, F, H, D)
    char modeChar;
    
    // Enum value for the mode
    QMODMode mode;
    
    // Human-readable mode name
    String modeName;
};

/**
 * @brief Parse QMOD message string into QMODData structure
 * 
 * @param message The QMOD response string (single character: P, S, L, B, F, H, D)
 * @param data Pointer to QMODData structure to populate
 * @return true if parsing was successful, false otherwise
 * 
 * Example input: "P" or "L"
 */
bool parseQMODMessage(const String& message, QMODData* data);

/**
 * @brief Parse QMOD message string into QMODData structure (convenience function)
 * 
 * @param message The QMOD response string (single character: P, S, L, B, F, H, D)
 * @return QMODData populated with parsed values
 */
QMODData parseQMODMessage(const String& message);

/**
 * @brief Convert QMODMode enum to string
 * 
 * @param mode The QMODMode value
 * @return String representation of the mode
 */
String qmodModeToString(QMODMode mode);

/**
 * @brief Convert QMODMode enum to character
 * 
 * @param mode The QMODMode value
 * @return Character representation of the mode
 */
char qmodModeToChar(QMODMode mode);

#endif // QMOD_PARSER_H
