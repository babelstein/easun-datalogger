/**
 * @file modbus_registers.h
 * @brief Konfiguracja rejestrów Modbus RTU dla SMG II
 * 
 * @author ESP8266 Datalogger Team
 * @date 2026-03-20
 * @version 1.0
 */

#ifndef MODBUS_REGISTERS_H
#define MODBUS_REGISTERS_H

#include <stdint.h>
// ============================================================================
// Rejestry Odczytu (Read - R)
// ============================================================================

// Tablica rejestrów do odczytu (z długością)
const struct {
    int address;           // Adres rejestru
    const char* name;      // Nazwa rejestru
    const char* unit;      // Jednostka
    float scale;           // Skalowanie (np. ×0.1 dla napięcia)
    float offset;          // Offset (np. -273.15 dla temperatury)
    const char* description; // Opis
    int length;            // Długość rejestru w bajtach
} readRegisters[] = {
    // Kod awarii i ostrzeżeń
    {100, "fault_code", "-", 1.0, 0.0, "Kod awarii", 2},
    {108, "warning_code", "-", 1.0, 0.0, "Kod ostrzeżenia", 2},
    
    // Numer seryjny (ASCII)
    {186, "serial_number", "ASCII", 1.0, 0.0, "Numer seryjny (ASCII)", 12},
    
    // Tryb pracy
    {201, "operation_mode", "-", 1.0, 0.0, "Tryb pracy (0: Power On, 1: Standby, 2: Mains, 3: Off-Grid, 4: Bypass, 5: Charging, 6: Fault)", 1},
    
    // Napięcie sieci
    {202, "mains_voltage", "V", 0.1, 0.0, "Napięcie sieci", 1},
    {203, "mains_frequency", "Hz", 0.01, 0.0, "Częstotliwość sieci", 1},
    {204, "mains_power", "W", 1.0, 0.0, "Średnia moc sieci", 1},
    
    // Napięcie inwertera
    {205, "inverter_voltage", "V", 0.1, 0.0, "Napięcie inwertera", 1},
    {206, "inverter_current", "A", 0.1, 0.0, "Prąd inwertera", 1},
    {207, "inverter_frequency", "Hz", 0.01, 0.0, "Częstotliwość inwertera", 1},
    {208, "inverter_power", "W", 1.0, 0.0, "Średnia moc inwertera", 1},
    {209, "inverter_charging_power", "W", 1.0, 0.0, "Moc ładowania inwertera", 1},
    
    // Napięcie wyjściowe
    {210, "output_voltage", "V", 0.1, 0.0, "Napięcie wyjściowe", 1},
    {211, "output_current", "A", 0.1, 0.0, "Prąd wyjściowy", 1},
    {212, "output_frequency", "Hz", 0.01, 0.0, "Częstotliwość wyjściowa", 1},
    {213, "output_active_power", "W", 1.0, 0.0, "Moc wyjściowa", 1},
    {214, "output_apparent_power", "VA", 1.0, 0.0, "Moc pozorna wyjściowa", 1},
    
    // Napięcie baterii
    {215, "battery_voltage", "V", 0.1, 0.0, "Średnie napięcie baterii", 1},
    {216, "battery_current", "A", 0.1, 0.0, "Średni prąd baterii", 1},
    {217, "battery_power", "W", 1.0, 0.0, "Średnia moc baterii", 1},
    
    // Napięcie PV
    {219, "pv_voltage", "V", 0.1, 0.0, "Średnie napięcie PV", 1},
    {220, "pv_current", "A", 0.1, 0.0, "Średni prąd PV", 1},
    {223, "pv_power", "W", 1.0, 0.0, "Średnia moc PV", 1},
    {224, "pv_charging_power", "W", 1.0, 0.0, "Średnia moc ładowania PV", 1},
    
    // Obciążenie
    {225, "load_percentage", "%", 1.0, 0.0, "Procent obciążenia", 1},
    
    // Temperatura
    {226, "dcdc_temperature", "°C", 1.0, 0.0, "Temperatura DCDC", 1},
    {227, "inverter_temperature", "°C", 1.0, 0.0, "Temperatura inwertera", 1},
    
    // Stan baterii
    {229, "battery_soc", "%", 1.0, 0.0, "Stan naładowania baterii", 1},
    
    // Prądy
    {232, "battery_avg_current", "A", 0.1, 0.0, "Średni prąd baterii", 1},
    {233, "inverter_charging_current", "A", 0.1, 0.0, "Średni prąd ładowania inwertera", 1},
    {234, "pv_charging_current", "A", 0.1, 0.0, "Średni prąd ładowania PV", 1},
    
    // Efektywność
    {240, "inverter_efficiency", "%", 1.0, 0.0, "Efektywność inwertera", 1},
    
    // Status
    {241, "inverter_status", "-", 1.0, 0.0, "Status inwertera", 1},
    {242, "error_code", "-", 1.0, 0.0, "Kod błędu", 1},
    {243, "firmware_version", "-", 1.0, 0.0, "Wersja firmware", 1},
    {244, "hardware_version", "-", 1.0, 0.0, "Wersja hardware", 1},
    {245, "serial_number", "-", 1.0, 0.0, "Numer seryjny", 1},
    {246, "production_date", "-", 1.0, 0.0, "Data produkcji", 1},
    {247, "warranty_status", "-", 1.0, 0.0, "Status gwarancji", 1},
    {248, "maintenance_status", "-", 1.0, 0.0, "Status serwisu", 1},
    {249, "calibration_status", "-", 1.0, 0.0, "Status kalibracji", 1},
    
    // Ustawienia
    {250, "factory_settings", "-", 1.0, 0.0, "Ustawienia fabryczne", 1},
    {251, "user_settings", "-", 1.0, 0.0, "Ustawienia użytkownika", 1},
    {252, "communication_settings", "-", 1.0, 0.0, "Ustawienia komunikacji", 1},
    {253, "security_settings", "-", 1.0, 0.0, "Ustawienia bezpieczeństwa", 1},
    {254, "network_settings", "-", 1.0, 0.0, "Ustawienia sieciowe", 1},
    {255, "system_settings", "-", 1.0, 0.0, "Ustawienia systemowe", 1},
    
    // Moc znamionowa
    {643, "rated_power", "W", 1.0, 0.0, "Moc znamionowa", 1}
};

// Liczba rejestrów do odczytu
const int numReadRegisters = sizeof(readRegisters) / sizeof(readRegisters[0]);

// ============================================================================
// Rejestry Odczytu/Zapisu (Read/Write - R/W)
// ============================================================================

// Tablica rejestrów do odczytu i zapisu (z długością)
const struct {
    int address;           // Adres rejestru
    const char* name;      // Nazwa rejestru
    const char* unit;      // Jednostka
    float scale;           // Skalowanie
    float offset;          // Offset
    const char* description; // Opis
    int length;            // Długość rejestru w bajtach
} rwRegisters[] = {
    // Tryb wyjścia
    {300, "output_mode", "-", 1.0, 0.0, "Tryb wyjścia (0: Single, 1: Parallel, 2: 3 Phase-P1, 3: 3 Phase-P2, 4: 3 Phase-P3)", 1},
    {301, "output_priority", "-", 1.0, 0.0, "Priorytet wyjścia (0: Utility-PV-Battery, 1: PV-Utility-Battery, 2: PV-Battery-Utility)", 1},
    {302, "input_voltage_range", "-", 1.0, 0.0, "Zakres napięcia wejściowego (0: Wide range, 1: Narrow range)", 1},
    {303, "buzzer_mode", "-", 1.0, 0.0, "Tryb dzwonka (0: Mute, 1: Sound on change/warning/fault, 2: Sound on warning/fault, 3: Sound on fault)", 1},
    {305, "lcd_backlight", "-", 1.0, 0.0, "Podświetlenie LCD (0: Timed off, 1: Always on)", 1},
    {306, "lcd_auto_return", "-", 1.0, 0.0, "Automatyczny powrót do strony głównej (0: Do not return, 1: Return after 1 minute)", 1},
    {307, "energy_saving_mode", "-", 1.0, 0.0, "Tryb oszczędzania energii (0: Off, 1: On)", 1},
    {308, "overload_auto_restart", "-", 1.0, 0.0, "Automatyczny restart przy przeciążeniu (0: No restart, 1: Automatic restart)", 1},
    {309, "over_temp_auto_restart", "-", 1.0, 0.0, "Automatyczny restart przy przegrzaniu (0: No restart, 1: Automatic restart)", 1},
    {310, "overload_bypass_enabled", "-", 1.0, 0.0, "Przekierowanie do bypass przy przeciążeniu (0: Disable, 1: Enable)", 1},
    {313, "battery_eq_mode_enabled", "-", 1.0, 0.0, "Tryb równoważenia baterii (0: Disable, 1: Enable)", 1},
    
    // Tryb włączania
    {406, "turn_on_mode", "-", 1.0, 0.0, "Tryb włączania (0: Local/Remote, 1: Only local, 2: Only remote)", 1},
    
    // Zdalne włączanie/wyłączanie
    {420, "remote_switch", "-", 1.0, 0.0, "Zdalne włączanie/wyłączanie (0: Remote shutdown, 1: Remote turn-on)", 1},
    
    // Priorytet ładowania
    {331, "battery_charging_priority", "-", 1.0, 0.0, "Priorytet ładowania baterii (0: Utility, 1: PV, 2: PV=Utility, 3: PV only)", 1},
    
    // Maksymalny prąd ładowania
    {332, "max_charging_current", "A", 1.0, 0.0, "Maksymalny prąd ładowania", 1},
    {333, "max_mains_charging_current", "A", 1.0, 0.0, "Maksymalny prąd ładowania z sieci", 1},
    
    // Czas równoważenia
    {335, "eq_charging_time", "min", 1.0, 0.0, "Czas równoważenia baterii (0~900 min)", 1},
    {336, "eq_timeout_exit", "min", 1.0, 0.0, "Czas wygaśnięcia równoważenia (0~900 min)", 1},
    {337, "eq_intervals", "day", 1.0, 0.0, "Liczba cykli równoważenia (1~90)", 1},
    
    // Napięcie wyjściowe (wymaga restartu)
    {320, "output_voltage", "V", 1.0, 0.0, "Napięcie wyjściowe (wymaga restartu)", 1},
    {321, "output_frequency", "Hz", 1.0, 0.0, "Częstotliwość wyjściowa (wymaga restartu)", 1},
    
    // Ochrona napięcia
    {323, "battery_overvoltage", "V", 1.0, 0.0, "Punkt ochrony przed nadmiernym napięciem", 1},
    {324, "max_charging_voltage", "V", 1.0, 0.0, "Maksymalne napięcie ładowania", 1},
    {325, "floating_charging_voltage", "V", 1.0, 0.0, "Napięcie ładowania float", 1},
    {326, "battery_discharge_recovery", "V", 1.0, 0.0, "Punkt odzyskiwania rozładowania", 1},
    {327, "battery_low_voltage_mains", "V", 1.0, 0.0, "Ochrona przed niskim napięciem (sieć)", 1},
    {329, "battery_low_voltage_offgrid", "V", 1.0, 0.0, "Ochrona przed niskim napięciem (off-grid)", 1},
    {334, "eq_charging_voltage", "V", 1.0, 0.0, "Napięcie ładowania równoważenia", 1}
};

// Liczba rejestrów do odczytu i zapisu
const int numRwRegisters = sizeof(rwRegisters) / sizeof(rwRegisters[0]);

// ============================================================================
// Rejestry Tylko Zapisu (Write Only - W)
// ============================================================================

// Tablica rejestrów do zapisu tylko
const struct {
    int address;           // Adres rejestru
    const char* name;      // Nazwa rejestru
    const char* unit;      // Jednostka
    float scale;           // Skalowanie
    float offset;          // Offset
    const char* description; // Opis
} writeOnlyRegisters[] = {
    // Wyjście z trybu awarii
    {426, "exit_fault_mode", "-", 1.0, 0.0, "Wyjście z trybu awarii (1: Exit fault state)"}
};

// Liczba rejestrów tylko do zapisu
const int numWriteOnlyRegisters = sizeof(writeOnlyRegisters) / sizeof(writeOnlyRegisters[0]);

#endif // MODBUS_REGISTERS_H
