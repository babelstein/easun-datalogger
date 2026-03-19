# Easun Datalogger - Moduł komunikacji z inwerterem

## 📋 Opis projektu

Projekt ESP8266 do komunikacji z inwerterem poprzez moduł loggera. Urządzenie wysyła dane z inwertera do serwera API za pomocą HTTPS z uwierzytelnianiem SSL.

## ✨ Funkcje

- ✅ Komunikacja z inwerterem przez SoftwareSerial
- ✅ HTTPS z certyfikatem root CA
- ✅ Uwierzytelnianie tokenem (Authorization header)
- ✅ Retry mechanism z exponential backoff (3 próby)
- ✅ Odczyt danych z loggera
- ✅ Zapis danych do bazy danych MySQL

## 🛠️ Wymagania

- ESP8266 mikrokontroler
- Arduino IDE 2.0+
- Biblioteki:
  - ESP8266WiFi
  - ESP8266HTTPClient
  - SoftwareSerial

## 📦 Instalacja

1. Klonowanie repozytorium:
   git clone https://github.twój-username/easun-datalogger.git
   cd easun-datalogger

2. Konfiguracja:
   - Edytuj plik src/secrets.h
   - Ustaw dane WiFi (SSID, password)
   - Ustaw auth_token dla uwierzytelnienia
   - Ustaw caCert z certyfikatem root CA

3. Kompilacja:
   # W Arduino IDE:
   Tools > Board > ESP8266 NodeMCU 1.0
   Tools > Upload Speed > 115200
   Tools > Flash Mode > QIO
   Tools > Flash Size > 4MB
   Upload

## ⚙️ Konfiguracja

### Plik src/secrets.h

const char* ssid = "Twoj-SSID";
const char* password = "Twoje-hasło";
const char* api_url = "https://adres-url.com/do/endpointa";
const char* auth_token = "token-uwierzytelniający";

### Plik src/main.cpp

- MILISECONDS_DELAY - opóźnienie między komendami (10000ms)
- MAX_RETRIES - maksymalna liczba prób (3)
- cmdNames[] - nazwy komend (QPIGS, QMOD, QPI, QPIRI, QVIRI)

## 🔌 Komendy inwertera

| Komenda | Opis |
|---------|------|
| QPIGS | Pobierz dane z inwertera |
| QMOD | Pobierz moduł |
| QPI | Pobierz dane inwertera |
| QPIRI | Pobierz dane inwertera (RI) |
| QVIRI | Pobierz dane inwertera (VI) |

## 📡 Format żądań API

### GET - Health Check

GET /inverter HTTP/1.1
Host: twoj-serwer.eu

### POST - Wysyłanie danych

POST /inverter HTTP/1.1
Host: twoj-serwer.eu
Content-Type: application/json
Authorization: twoj-token-here

{
  "operation": "QPIGS",
  "message": "Dane z komendy QPIGS"
}

## 📊 Logowanie

Urządzenie loguje:
- ✅ Sukcesowe żądania HTTP (200)
- ❌ Błędy HTTP (4xx, 5xx)
- ❌ Próby retry (1/3, 2/3, 3/3)
- ❌ Osiągnięcie maksymalnych prób

## 🔍 Debugowanie

### Sprawdzenie połączenia WiFi

Wi-Fi connected

### Sprawdzenie logów HTTP

✅ POST request successful with code: 200
Server response: {"operation": "QPIGS", "message": "Komenda: QPIGS"}

### Rozwiązywanie problemów

| Problem | Rozwiązanie |
|---------|------------|
| ❌ Błąd 401 | Sprawdź auth_token w secrets.h |
| ❌ Błąd 400 | Sprawdź format JSON |
| ❌ Błąd 5xx | Retry mechanism powinien zadziałać |
| ❌ Timeout | Sprawdź połączenie internetowe |

## 📝 Licencja

MIT License - używaj swobodnie!

## 🙏 Podziękowania

- ESP8266 Arduino Core
- ESP8266HTTPClient Library

---

Wersja: 1.0
Data: 2026-03-19