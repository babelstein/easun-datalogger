#include "qmod_parser.h"

/**
 * @brief Parse QMOD message string into QMODData structure
 * 
 * QMOD response is a single character: P, S, L, B, F, H, D
 */
bool parseQMODMessage(const String& message, QMODData* data) {
    if (!data) {
        return false;
    }
    
    // Reset data
    data->modeChar = ' ';
    data->mode = QMODMode::UNKNOWN;
    data->modeName = "";
    
    // Get the first character (skip any whitespace)
    if (message.length() > 0) {
        data->modeChar = message[0];
        
        // Map character to enum and name
        switch (data->modeChar) {
            case 'P':
                data->mode = QMODMode::POWER_ON;
                data->modeName = "POWER_ON";
                break;
            case 'S':
                data->mode = QMODMode::STANDBY;
                data->modeName = "STANDBY";
                break;
            case 'L':
                data->mode = QMODMode::LINE;
                data->modeName = "LINE";
                break;
            case 'B':
                data->mode = QMODMode::BATTERY;
                data->modeName = "BATTERY";
                break;
            case 'F':
                data->mode = QMODMode::FAULT;
                data->modeName = "FAULT";
                break;
            case 'H':
                data->mode = QMODMode::POWER_SAVING;
                data->modeName = "POWER_SAVING";
                break;
            case 'D':
                data->mode = QMODMode::SHUTDOWN;
                data->modeName = "SHUTDOWN";
                break;
            default:
                return false;
        }
    }
    
    return true;
}

/**
 * @brief Parse QMOD message string into QMODData structure (convenience function)
 */
QMODData parseQMODMessage(const String& message) {
    QMODData data;
    parseQMODMessage(message, &data);
    return data;
}

/**
 * @brief Convert QMODMode enum to string
 */
String qmodModeToString(QMODMode mode) {
    switch (mode) {
        case QMODMode::POWER_ON:
            return "POWER_ON";
        case QMODMode::STANDBY:
            return "STANDBY";
        case QMODMode::LINE:
            return "LINE";
        case QMODMode::BATTERY:
            return "BATTERY";
        case QMODMode::FAULT:
            return "FAULT";
        case QMODMode::POWER_SAVING:
            return "POWER_SAVING";
        case QMODMode::SHUTDOWN:
            return "SHUTDOWN";
        default:
            return "UNKNOWN";
    }
}

/**
 * @brief Convert QMODMode enum to character
 */
char qmodModeToChar(QMODMode mode) {
    switch (mode) {
        case QMODMode::POWER_ON:
            return 'P';
        case QMODMode::STANDBY:
            return 'S';
        case QMODMode::LINE:
            return 'L';
        case QMODMode::BATTERY:
            return 'B';
        case QMODMode::FAULT:
            return 'F';
        case QMODMode::POWER_SAVING:
            return 'H';
        case QMODMode::SHUTDOWN:
            return 'D';
        default:
            return ' ';
    }
}
