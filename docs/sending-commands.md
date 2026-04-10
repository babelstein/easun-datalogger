# EAsun / Voltronic Inverter Communication Protocol (PI30)

This document describes how to send configuration commands to EAsun/Voltronic-compatible inverters using an ESP32.

## 1. Communication Frame Structure

Every command sent to the inverter must follow this specific format:
`[Command][CRC_High][CRC_Low][CR]`

- **Command**: ASCII string (e.g., `POP02`).
- **CRC**: 16-bit CRC XMODEM checksum (2 bytes).
- **CR**: Carriage Return (`0x0D` or `\r`).

---

## 2. Common Setting Commands
### 2.1. Output Source Priority (POP)
Defines which energy source has priority to power the connected loads.


| Command | Mode | Description |
|:--- |:--- |:--- |
| **`POP00`** | **Utility** | Utility provides power to loads first. Solar/Battery used only during outages. |
| **`POP01`** | **Solar** | Solar provides power first. If insufficient, Battery is used. |
| **`POP02`** | **SBU** | Solar first, then Battery. Utility is the last resort (Back-to-Grid). |

---

### 2.2. Charger Source Priority (PCP)
Defines which energy source is allowed to charge the batteries.


| Command | Mode | Description |
|:--- |:--- |:--- |
| **`PCP00`** | **Solar First** | Solar charges battery as first priority. Utility charges only if Solar is unavailable. |
| **`PCP01`** | **Solar & Utility** | Both Solar and Utility charge the battery simultaneously (SNU). |
| **`PCP02`** | **Solar Only** | Only Solar energy charges the battery, regardless of Utility status. |

---

### 2.3. Charging Current Limits (MNCHGC / MUCHGC)
Sets the maximum current for battery charging. Value `nnn` must be **3 digits**.


| Command | Description | Example (30A) |
|:--- |:--- |:--- |
| **`MNCHGCnnn`** | **Max Total Charge** | `MNCHGC030` (Combined Solar + Utility limit) |
| **`MUCHGCnnn`** | **Max Utility Charge** | `MUCHGC010` (Limit for AC/Grid charging only) |

---

### 2.4. Operating Voltages (Battery)
Sets the thresholds for switching between Battery and Utility modes. Value `nn.n` must be **4 characters** (including the dot).


| Command | Description | Example (48.0V) |
|:--- |:--- |:--- |
| **`PBCVnn.n`** | **Back to Grid** | `PBCV44.0` (Switch to Utility when battery is low) |
| **`PBDVnn.n`** | **Back to Battery** | `PBDV54.0` (Switch back to Battery when charged) |
| **`PSDVnn.n`** | **Cut-off Voltage** | `PSDV42.0` (Inverter turns off to protect battery) |

---

## 3. ESP32 Implementation (C++ / Arduino)

Below is a snippet for calculating the CRC and sending the command via `Serial2`.

```cpp
#include <Arduino.h>

#define RXD2 16
#define TXD2 17

/**
 * @brief Oblicza CRC-16 (XMODEM) dla ciągu znaków.
 */
uint16_t calculate_crc(const char *pin, uint8_t len) {
    uint16_t crc = 0;
    for (uint8_t i = 0; i < len; i++) {
        crc = (uint16_t)((crc << 8) ^ 0x1021 * (uint16_t)(crc >> 8 ^ pin[i]));
    }
    return crc;
}

/**
 * @brief Weryfikuje sumę kontrolną odpowiedzi z falownika.
 * Zakłada, że odpowiedź kończy się na: [DANE][CRC_H][CRC_L][\r]
 */
bool verifyResponseCRC(String resp) {
    if (resp.length() < 3) return false;

    // Wyodrębnij dane (wszystko oprócz ostatnich 2 bajtów CRC)
    String data = resp.substring(0, resp.length() - 2);
    
    // Wyodrębnij CRC z odpowiedzi
    uint8_t received_crc_h = resp[resp.length() - 2];
    uint8_t received_crc_l = resp[resp.length() - 1];
    uint16_t received_crc = (received_crc_h << 8) | received_crc_l;

    // Oblicz CRC dla otrzymanych danych
    uint16_t calculated_crc = calculate_crc(data.c_str(), data.length());

    return (calculated_crc == received_crc);
}

/**
 * @brief Wysyła komendę z CRC i obsługuje odpowiedź.
 */
void sendInverterCommand(const char* command) {
    uint16_t crc = calculate_crc(command, strlen(command));
    
    Serial2.print(command);
    Serial2.write((uint8_t)(crc >> 8));
    Serial2.write((uint8_t)(crc & 0xFF));
    Serial2.write(0x0D);

    // Czekaj na odpowiedź (timeout 1s)
    unsigned long start = millis();
    while (!Serial2.available() && millis() - start < 1000);

    if (Serial2.available()) {
        String response = Serial2.readStringUntil('\r');
        
        if (verifyResponseCRC(response)) {
            // Usuń bajty CRC z tekstu dla czytelności logów
            String cleanResponse = response.substring(0, response.length() - 2);
            Serial.print("Inverter Response: ");
            Serial.println(cleanResponse);

            if (cleanResponse.startsWith("(ACK")) {
                Serial.println("Result: OK (Command Accepted)");
            } else if (cleanResponse.startsWith("(NAK")) {
                Serial.println("Result: ERROR (Command Rejected)");
            }
        } else {
            Serial.println("Result: CRC Error (Checksum Mismatch)");
        }
    } else {
        Serial.println("Result: Timeout (No response)");
    }
}

void setup() {
    Serial.begin(115200);
    // Prędkość 2400 jest standardem dla większości EAsun
    Serial2.begin(2400, SERIAL_8N1, RXD2, TXD2);
}

void loop() {
    // Przykład: Odczyt parametrów (QPIGS) co 5 sekund
    sendInverterCommand("QPIGS");
    delay(5000);
}

```
## 4. Inverter Responses

- `(ACK`: Command accepted and executed.
- `(NAK`: Command rejected (wrong syntax, out of range, or mode not supported).

# Inverter Configuration Commands (PI30 Protocol)

This section details the commands used to modify the operational parameters of EAsun/Voltronic inverters via ESP32.

---

## Important Technical Notes

1.  **CRC Requirement**: Every command **must** be followed by a 2-byte CRC (XMODEM) and a Carriage Return (`\r`). Without a valid CRC, the inverter will ignore the command.
2.  **Format Sensitivity**: Commands are strictly formatted. For example, charging current must be `030`, not `30`. Voltages must include one decimal place (e.g., `52.0`).
3.  **ACK/NAK Response**: After sending a command, always wait for the response:
    *   `(ACK`: Success. The setting has been saved.
    *   `(NAK`: Rejected. Usually due to invalid value range or incorrect mode (e.g., trying to set Grid voltage while in Battery-only mode).
4.  **Hardware Levels**: ESP32 operates at **3.3V TTL**. Most inverters use **RS232** levels (+/- 12V). An RS232-to-TTL converter (like MAX3232) is mandatory to avoid damaging the ESP32.
5.  **EEPROM Wear**: Frequent writing of settings (e.g., every few seconds) may wear out the inverter's internal flash memory. It is recommended to change settings only when necessary.