# Easun Datalogger - Moduł komunikacji z inwerterem

## 📋 Opis projektu

Projekt ESP8266/ESP32 do komunikacji z inwerterem Easun poprzez moduł loggera. Urządzenie pobiera dane z inwertera za pomocą protokołu UART i wysyła je do serwera API za pomocą HTTPS z uwierzytelnianiem SSL oraz publikuje dane przez MQTT.

### Architektura systemu

```
┌─────────────────┐      ┌─────────────────┐     ┌─────────────────┐
│   Inverter      │────▶│  EASun Logger   │────▶│   ESP8266/ESP32 │
│   (UART)        │      │                 │     │   (WiFi/MQTT)   │
└─────────────────┘      └─────────────────┘     └─────────────────┘
                                                         │
                                                         ▼
                                               ┌─────────────────┐
                                               │   MQTT Broker   │
                                               └─────────────────┘
                                                         │
                                                         ▼
                                               ┌─────────────────┐
                                               │   API Server    │
                                               │   (HTTPS)       │
                                               └─────────────────┘
```

## ✨ Funkcje

### Komunikacja z inwerterem
- ✅ Komunikacja z inwerterem przez SoftwareSerial (ESP8266) lub UART1 (ESP32) - zalecane ESP32
- ✅ Obsługa trzech komend: QPIGS, QMOD, QPIRI
- ✅ Obliczanie CRC16 XMODEM do weryfikacji integralności danych
- ✅ Automatyczne ponawianie prób przy błędach CRC

### Komunikacja z HomeAssistant
- ✅ Publikacja danych przez wiadomości do brokera MQTT
- ✅ Przykładowy plik `mqtt.yaml` do konwersji danych z wiadomości na encje w HomeAssistant

### Komunikacja z API
- ✅ HTTPS z certyfikatem root CA
- ✅ Uwierzytelnianie tokenem (Authorization header)
- ✅ Retry mechanism z exponential backoff (3 próby)

### Formatowanie danych
- ✅ Konwersja struktur komend do JSON (qpigsDataToJson, qmodDataToJson, qpiriDataToJson)
- ✅ Formatowanie odpowiedzi API jako tablica obiektów JSON

## 🛠️ Wymagania

### Mikrokontroler
- Zalecane ESP32 lub ESP8266

### Środowisko developmentowe
- Arduino IDE 2.0+
- PlatformIO

### Biblioteki

#### Dla ESP8266
- ESP8266WiFi
- ESP8266HTTPClient
- PubSubClient

#### Dla ESP32
- WiFi
- HTTPClient
- WiFiClientSecure
- PubSubClient

## 📦 Instalacja

1. **Klonowanie repozytorium:**
   ```bash
   git clone https://github.twój-username/easun-datalogger.git
   cd easun-datalogger
   ```

2. **Konfiguracja:**
   - Uwórz plik `src/secrets.h`
   - Ustaw dane WiFi (SSID, password)
   - Ustaw `auth_token` dla uwierzytelnienia
   - Ustaw `caCert` z certyfikatem root CA
   - Skonfiguruj dane MQTT (opcjonalnie)

3. **Kompilacja:**
   ```bash
   # W Arduino IDE:
   Tools > Board > ESP8266 NodeMCU 1.0 (lub ESP32 Dev Module)
   Tools > Upload Speed > 115200
   Tools > Flash Mode > QIO (ESP8266)
   Tools > Flash Size > 4MB
   Upload
   
   # Lub używając PlatformIO:
   pio run -t upload
   ```

## ⚙️ Konfiguracja

### Plik `src/secrets.h`

```cpp
// Dane WiFi
#define WIFI_SSID "Twoj-SSID"
#define WIFI_PASSWORD "Twoje-hasło"

// Konfiguracja API
#define API_URL "https://twoj-serwer.eu/sh-api-dev/inverter"
#define AUTH_TOKEN "twoj-token-here"

// Konfiguracja MQTT
#define MQTT_SERVER "<mqtt-ip-address>"
#define MQTT_PORT 1883
#define MQTT_USER "<mqtt_user>"
#define MQTT_PASSWORD "<mqtt_password>"
#define MQTT_TOPIC "inverter/topic-name"

// Certyfikat root CA (dla połączenia z API)
const char* caCert = "-----BEGIN CERTIFICATE-----\n...\n-----END CERTIFICATE-----";
```

### Plik `src/main.cpp`

#### Stałe globalne
- `MILISECONDS_DELAY` - opóźnienie między komendami (domyślnie 10000ms)
- `MAX_RETRIES` - maksymalna liczba prób (domyślnie 3)
- `CMD_COUNT` - liczba komend (3: QPIGS, QMOD, QPIRI)
- `CMD_NAMES[]` - nazwy komend

#### Pinowanie (ESP32)
- `RX2_PIN` - pin RX dla UART1 (domyślnie 16)
- `TX2_PIN` - pin TX dla UART1 (domyślnie 17)

#### Debugowanie
- Zmiana zmiennej `debug` na `true` w `main.cpp` włącza logowanie do Serial

## 🔌 Komendy inwertera

### Komenda QPIGS - Pobierz dane ogólne
Pobiera aktualne dane z inwertera: napięcia, częstotliwości, moc, temperatury, status baterii.

**Format odpowiedzi:**
```
(241.6 49.9 241.6 49.9 0000 0000 000 319 00.30 000.0 00.30 00000 00010000 00 00 00000 010.Z)
```

**Pola danych:**
| Pole | Jednostka | Opis |
|------|-----------|------|
| inputACVoltage | V | Napięcie wejściowe AC |
| gridFrequency | Hz | Częstotliwość sieci |
| acOutputVoltage | V | Napięcie wyjściowe AC |
| outputFrequency | Hz | Częstotliwość wyjściowa |
| apparentOutputPower | VA | Moc pozorna |
| activeOutputPower | W | Moc czynna |
| percentLoad | % | Obciążenie |
| internalBusVoltage | V | Napięcie bus wewnętrzne |
| batteryVoltage | V | Napięcie baterii |
| batteryChargingCurrent | A | Prąd ładowania baterii |
| batteryChargeLevel | % | Poziom naładowania baterii |
| heatsinkTemperature | °C | Temperatura chłodnicy |
| pvInputCurrent | A | Prąd z paneli PV |
| pvInputVoltage | V | Napięcie z paneli PV |
| batteryVoltageFromSCC | V | Napięcie baterii z SCC |
| batteryDischargeCurrent | A | Prąd rozładowania baterii |
| deviceStatusFlags | int | Flagi statusu urządzenia |
| pvProduction | W | Produkcja PV |

### Komenda QMOD - Pobierz tryb pracy
Pobiera aktualny tryb pracy inwertera.

**Format odpowiedzi:**
```
(P) lub (S) lub (L) lub (B) lub (F) lub (H) lub (D)
```

**Możliwe wartości:**
| Wartość | Opis |
|---------|------|
| P | POWER_ON (uruchomiony) |
| S | STANDBY (tryb czuwania) |
| L | LINE (sieć) |
| B | BATTERY (bateria) |
| F | FAULT (błąd) |
| H | POWER_SAVING (oszczędzanie energii) |
| D | SHUTDOWN (wyłączony) |

### Komenda QPIRI - Pobierz dane szczegółowe (RI)
Pobiera dane szczegółowe dotyczące parametrów inwertera.

**Format odpowiedzi:**
```
(230.0 26.9 230.0 50.0 26.9 6200 6200 48.0 44.0 42.0 56.4 54.0 2 030 060 0 2 1 1 01 0 0 52.0 0 1 44.0 10 44.0l)
```

**Pola danych:**
| Pole | Jednostka | Opis |
|------|-----------|------|
| ratedACInputVoltage | V | Napięcie wejściowe AC (nominalne) |
| ratedACInputCurrent | A | Prąd wejściowy AC (nominalny) |
| ratedACOutputVoltage | V | Napięcie wyjściowe AC (nominalne) |
| ratedOutputFrequency | Hz | Częstotliwość wyjściowa (nominalna) |
| ratedACOutputCurrent | A | Prąd wyjściowy AC (nominalny) |
| ratedApparentOutputPower | VA | Moc pozorna (nominalna) |
| ratedActiveOutputPower | W | Moc czynna (nominalna) |
| ratedBatteryVoltage | V | Napięcie baterii (nominalne) |
| rechargeVoltage | V | Napięcie ładowania |
| cutOffVoltage | V | Napięcie odcięcia |
| bulkChargingVoltage | V | Napięcie ładowania bulk |
| floatChargingVoltage | V | Napięcie ładowania float |
| batteryType | int | Typ baterii |
| maximumACChargingCurrent | A | Maksymalny prąd ładowania AC |
| maximumTotalChargingCurrent | A | Maksymalny całkowity prąd ładowania |
| inputVoltageRange | int | Zakres napięcia wejściowego |
| outputSource | int | Źródło wyjścia |
| chargerPriority | int | Priorytet ładowania |
| parallelInverters | int | Liczba inwerterów równoległych |
| machineType | int | Typ maszyny |
| topology | int | Topologia |
| outputMode | int | Tryb wyjścia |
| batteryReDischargeVoltage | V | Napięcie rozładowania baterii |
| pvOK | int | Status PV |
| pvPowerStatus | int | Status mocy PV |
| lowDCCutOffVoltage | V | Napięcie odcięcia DC |
| maxPVChargingCurrent | A | Maksymalny prąd ładowania PV |

## 📡 Format żądań API

### POST - Wysyłanie danych

**Endpoint:** `/inverter`

**Headers:**
```
Content-Type: application/json
Authorization: Bearer twoj-token
```

**Body:**
```json
[
  {
    "operation": "QPIGS",
    "message": "{\"inputACVoltage\": 241.6, \"gridFrequency\": 49.9, ...}"
  },
  {
    "operation": "QMOD",
    "message": "{\"modeChar\": \"P\", \"mode\": \"POWER_ON\", \"modeValue\": 0}"
  },
  {
    "operation": "QPIRI",
    "message": "{\"ratedACInputVoltage\": 230.0, ...}"
  }
]
```

## 📊 Odpowiedzi z falownika w formacie JSON

### QPIGSData
```json
{
  "inputACVoltage": 241.6,
  "gridFrequency": 49.9,
  "acOutputVoltage": 241.6,
  "outputFrequency": 49.9,
  "apparentOutputPower": 0,
  "activeOutputPower": 0,
  "percentLoad": 0,
  "internalBusVoltage": 319,
  "batteryVoltage": 0.3,
  "batteryChargingCurrent": 0,
  "batteryChargeLevel": 30,
  "heatsinkTemperature": 0,
  "pvInputCurrent": 0,
  "pvInputVoltage": 0,
  "batteryVoltageFromSCC": 0,
  "batteryDischargeCurrent": 0,
  "deviceStatusFlags": "0",
  "sbuPriority": false,
  "configChanged": false,
  "sccFirmwareUpdated": false,
  "loadStatus": false,
  "batteryVoltageSteady": false,
  "chargingStatus": false,
  "sccChargingStatus": false,
  "acChargingStatus": false,
  "additionalData1": 0,
  "additionalData2": 0,
  "pvProduction": 0,
  "additionalFlags": 16
}
```

### QMODData
```json
{
  "modeChar": "P",
  "mode": "POWER_ON",
  "modeValue": 0
}
```

### QPIRIData
```json
{
  "ratedACInputVoltage": 230.0,
  "ratedACInputCurrent": 26.9,
  "ratedACOutputVoltage": 230.0,
  "ratedOutputFrequency": 50.0,
  "ratedACOutputCurrent": 26.9,
  "ratedApparentOutputPower": 6200,
  "ratedActiveOutputPower": 6200,
  "ratedBatteryVoltage": 48.0,
  "rechargeVoltage": 44.0,
  "cutOffVoltage": 42.0,
  "bulkChargingVoltage": 56.4,
  "floatChargingVoltage": 54.0,
  "batteryType": 2,
  "maximumACChargingCurrent": 30,
  "maximumTotalChargingCurrent": 60,
  "inputVoltageRange": 0,
  "outputSource": 2,
  "chargerPriority": 1,
  "parallelInverters": 1,
  "machineType": 1,
  "topology": 1,
  "outputMode": 1,
  "batteryReDischargeVoltage": 0,
  "pvOK": 0,
  "pvPowerStatus": 0,
  "lowDCCutOffVoltage": 52.0,
  "maxPVChargingCurrent": 44.0
}
```

## 📡 MQTT

### Konfiguracja MQTT

Dane MQTT są definiowane w `src/secrets.h` tak jak opisano w sekcji `⚙️ Konfiguracja`

### Format wiadomości MQTT

Wiadomości są publikowane na temacie `MQTT_TOPIC` w formacie JSON:

```json
{
  "command": "QPIGS",
  "result": {
    "inputACVoltage": 241.6,
    ...
  }
}
```

### Callback dla komend 

System obsługuje odbieranie komend przez MQTT. Komendy są przetwarzane w kolejności FIFO:

```json
{"command":"QPIGS","parameter":"100"}
```

## 🔍 Logowanie

Urządzenie loguje:
- ✅ Sukcesowe połączenia WiFi
- ✅ Sukcesowe połączenia MQTT
- ✅ Sukcesowe żądania HTTP (200)
- ❌ Błędy HTTP (4xx, 5xx)
- ❌ Próby retry (1/3, 2/3, 3/3)
- ❌ Osiągnięcie maksymalnych prób
- ✅ Wyniki komend inwertera

### Przykładowe logi

```
Initializing system...
Connecting to WiFi... OK
WiFi connected, IP: 192.168.2.100
Connecting to MQTT... OK
MQTT connected, topic: inverters/easun_1
=== Parsed Command Results ===
QPIGS: (241.6 49.9 ...)
QMOD: (P)
QPIRI: (230.0 26.9 ...)
=== JSON Results ===
QPIGS JSON: {"inputACVoltage": 241.6, ...}
QMOD JSON: {"modeChar": "P", ...}
QPIRI JSON: {"ratedACInputVoltage": 230.0, ...}
✅ POST request successful with code: 200
Server response: [{"operation":"QPIGS","message":...}]
```

## 🔍 Debugowanie

### Włączenie trybu debug

Edytuj plik `src/main.cpp`:

```cpp
const bool debug = true;  // Zmień na true
```

### Sprawdzenie połączenia WiFi

```
Connecting to WiFi... OK
WiFi connected, IP: 192.168.2.100
```

### Sprawdzenie logów HTTP

```
✅ POST request successful with code: 200
Server response: [{"operation":"QPIGS","message":...}]
```

### Rozwiązywanie problemów

| Problem | Rozwiązanie |
|---------|-------------|
| ❌ Błąd 401 Unauthorized | Sprawdź `auth_token` w `secrets.h` |
| ❌ Błąd 400 Bad Request | Sprawdź format JSON w odpowiedziach |
| ❌ Błąd 5xx Server Error | Retry mechanism powinien zadziałać automatycznie |
| ❌ Timeout | Sprawdź połączenie internetowe i serwer API |
| ❌ WiFi nie łączy się | Sprawdź SSID i hasło w `secrets.h` |
| ❌ MQTT nie łączy się | Sprawdź dane MQTT w `secrets.h` |
| ❌ CRC błąd | Sprawdź połączenie UART z inwerterem |
| ❌ UART nie działa | Sprawdź pinowanie UART w `main.cpp` |

### Rozwiązanie problemu z programowaniem ESP32U w PlatformIO

Jeśli Twoja płytka ESP32 (wersja U z anteną zewnętrzną) jest widoczna w Menedżerze Urządzeń jako **"Nieznane urządzenie: CP2102 USB to UART Bridge"**, wykonaj poniższe kroki.

### 1. Instalacja Brakujących Sterowników
System Windows nie zawsze posiada wbudowane sterowniki dla konwertera CP2102.
1. Pobierz sterowniki ze strony producenta: [Silicon Labs CP210x VCP Drivers](https://silabs.com).
2. Wybierz paczkę: **CP210x Universal Windows Driver**.
3. Rozpakuj archiwum, kliknij prawym przyciskiem myszy na plik `silabser.inf` i wybierz **Zainstaluj**.
4. Odłącz i podłącz płytkę – w Menedżerze Urządzeń powinien pojawić się port, np. `Silicon Labs CP210x USB to UART Bridge (COM3)`.


### Struktura projektu

```
easun-datalogger/
├── .gitignore
├── platformio.ini
├── readme.md
├── docs/
│   ├── mqtt.yaml
│   └── sending-commands.md
├── include/
│   ├── inverter_commands.h
│   ├── inverter_service.h
│   ├── json_serializer.h
│   ├── mqtt_service.h
│   ├── qmod_parser.h
│   ├── qpigs_parser.h
│   └── qpiri_parser.h
├── lib/
├── src/
│   ├── inverter_service.cpp
│   ├── json_serializer.cpp
│   ├── main.cpp
│   ├── mqtt_service.cpp
│   ├── qmod_parser.cpp
│   ├── qpigs_parser.cpp
│   ├── qpiri_parser.cpp
│   └── secrets.h
└── test/
```

### Pliki źródłowe

#### `src/main.cpp`
Główny plik programu zawierający:
- Inicjalizację WiFi i MQTT
- Obsługę komend MQTT
- Pętli głównej (`loop()`)
- Funkcję `sendDataToApi()` do wysyłania danych do serwera

#### `src/inverter_service.cpp`
Obsługa komunikacji z inwerterem:
- `InverterService` - klasa obsługująca komunikację UART
- `sendAllCommands()` - wysyła wszystkie komendy (QPIGS, QMOD, QPIRI)
- `parseAllCommands()` - parsuje odpowiedzi z inwertera
- `sendCommand()` - wysyła pojedynczą komendę
- `verifyCRC()` - weryfikuje CRC odpowiedzi
- `readResult()` - odczytuje odpowiedź z inwertera

#### `src/json_serializer.cpp`
Konwersja struktur do JSON:
- `qpigsDataToJson()` - konwersja QPIGSData
- `qmodDataToJson()` - konwersja QMODData
- `qpiriDataToJson()` - konwersja QPIRIData

#### `src/mqtt_service.cpp`
Obsługa MQTT:
- `sendMessage()` - publikacja wiadomości
- `setCallback()` - ustawienie callback dla wiadomości MQTT

#### `src/qpigs_parser.cpp`
Parsowanie danych QPIGS:
- `parseQPIGSMessage()` - parsowanie odpowiedzi QPIGS
- `parseQPIGSMessage()` (convenience) - wersja zwracająca obiekt

#### `src/qmod_parser.cpp`
Parsowanie danych QMOD:
- `parseQMODMessage()` - parsowanie odpowiedzi QMOD
- `qmodModeToString()` - konwersja enum do stringa
- `qmodModeToChar()` - konwersja enum do znaku

#### `src/qpiri_parser.cpp`
Parsowanie danych QPIRI:
- `parseQPIRIMessage()` - parsowanie odpowiedzi QPIRI
- `parseQPIRIMessage()` (convenience) - wersja zwracająca obiekt

#### `src/secrets.h`
Plik konfiguracyjny z sekretami:
- Dane WiFi
- Dane API (URL, token)
- Dane MQTT
- Certyfikat root CA

**⚠️ UWAGA:** Ten plik NIE powinien być w repozytorium Git!

## 📝 Licencja

MIT License - używaj swobodnie!

## 🙏 Podziękowania

- ESP8266 Arduino Core
- ESP32 Arduino Core
- ESP8266HTTPClient Library
- PubSubClient Library
- Easun Inverter Documentation

---

## Wersja: 1.0
## Data: 2026-04-10
