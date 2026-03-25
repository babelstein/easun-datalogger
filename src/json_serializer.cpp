#include "json_serializer.h"

/**
 * @brief Convert QPIGSData to JSON string
 */
String qpigsDataToJson(const QPIGSData& data) {
    String json = "{";
    json += "\"inputACVoltage\": " + String(data.inputACVoltage, 1) + ", ";
    json += "\"gridFrequency\": " + String(data.gridFrequency, 1) + ", ";
    json += "\"acOutputVoltage\": " + String(data.acOutputVoltage, 1) + ", ";
    json += "\"outputFrequency\": " + String(data.outputFrequency, 1) + ", ";
    json += "\"apparentOutputPower\": " + String(data.apparentOutputPower, 1) + ", ";
    json += "\"activeOutputPower\": " + String(data.activeOutputPower, 1) + ", ";
    json += "\"percentLoad\": " + String(data.percentLoad, 1) + ", ";
    json += "\"internalBusVoltage\": " + String(data.internalBusVoltage, 1) + ", ";
    json += "\"batteryVoltage\": " + String(data.batteryVoltage, 1) + ", ";
    json += "\"batteryChargingCurrent\": " + String(data.batteryChargingCurrent, 1) + ", ";
    json += "\"batteryChargeLevel\": " + String(data.batteryChargeLevel, 1) + ", ";
    json += "\"heatsinkTemperature\": " + String(data.heatsinkTemperature, 1) + ", ";
    json += "\"pvInputCurrent\": " + String(data.pvInputCurrent, 1) + ", ";
    json += "\"pvInputVoltage\": " + String(data.pvInputVoltage, 1) + ", ";
    json += "\"batteryVoltageFromSCC\": " + String(data.batteryVoltageFromSCC, 1) + ", ";
    json += "\"batteryDischargeCurrent\": " + String(data.batteryDischargeCurrent, 1) + ", ";
    json += "\"deviceStatusFlags\": " + String(data.deviceStatusFlags) + ", ";
    json += "\"additionalData1\": " + String(data.additionalData1) + ", ";
    json += "\"additionalData2\": " + String(data.additionalData2) + ", ";
    json += "\"pvProduction\": " + String(data.pvProduction, 1) + ", ";
    json += "\"additionalFlags\": " + String(data.additionalFlags);
    json += "}";
    return json;
}

/**
 * @brief Convert QPIRIData to JSON string
 */
String qpiriDataToJson(const QPIRIData& data) {
    String json = "{";
    json += "\"ratedACInputVoltage\": " + String(data.ratedACInputVoltage, 1) + ", ";
    json += "\"ratedACInputCurrent\": " + String(data.ratedACInputCurrent, 1) + ", ";
    json += "\"ratedACOutputVoltage\": " + String(data.ratedACOutputVoltage, 1) + ", ";
    json += "\"ratedOutputFrequency\": " + String(data.ratedOutputFrequency, 1) + ", ";
    json += "\"ratedACOutputCurrent\": " + String(data.ratedACOutputCurrent, 1) + ", ";
    json += "\"ratedApparentOutputPower\": " + String(data.ratedApparentOutputPower, 1) + ", ";
    json += "\"ratedActiveOutputPower\": " + String(data.ratedActiveOutputPower, 1) + ", ";
    json += "\"ratedBatteryVoltage\": " + String(data.ratedBatteryVoltage, 1) + ", ";
    json += "\"rechargeVoltage\": " + String(data.rechargeVoltage, 1) + ", ";
    json += "\"cutOffVoltage\": " + String(data.cutOffVoltage, 1) + ", ";
    json += "\"bulkChargingVoltage\": " + String(data.bulkChargingVoltage, 1) + ", ";
    json += "\"floatChargingVoltage\": " + String(data.floatChargingVoltage, 1) + ", ";
    json += "\"batteryType\": " + String(data.batteryType) + ", ";
    json += "\"maximumACChargingCurrent\": " + String(data.maximumACChargingCurrent, 1) + ", ";
    json += "\"maximumTotalChargingCurrent\": " + String(data.maximumTotalChargingCurrent, 1) + ", ";
    json += "\"inputVoltageRange\": " + String(data.inputVoltageRange) + ", ";
    json += "\"outputSource\": " + String(data.outputSource) + ", ";
    json += "\"chargerPriority\": " + String(data.chargerPriority) + ", ";
    json += "\"parallelInverters\": " + String(data.parallelInverters) + ", ";
    json += "\"machineType\": " + String(data.machineType) + ", ";
    json += "\"topology\": " + String(data.topology) + ", ";
    json += "\"outputMode\": " + String(data.outputMode) + ", ";
    json += "\"batteryReDischargeVoltage\": " + String(data.batteryReDischargeVoltage, 1) + ", ";
    json += "\"pvOK\": " + String(data.pvOK) + ", ";
    json += "\"pvPowerStatus\": " + String(data.pvPowerStatus) + ", ";
    json += "\"lowDCCutOffVoltage\": " + String(data.lowDCCutOffVoltage, 1) + ", ";
    json += "\"maxPVChargingCurrent\": " + String(data.maxPVChargingCurrent, 1);
    json += "}";
    return json;
}

/**
 * @brief Convert QMODData to JSON string
 */
String qmodDataToJson(const QMODData& data) {
    String json = "{";
    json += "\"modeChar\": \"" + String(data.modeChar) + "\", ";
    json += "\"mode\": \"" + String(data.modeName) + "\", ";
    json += "\"modeValue\": " + String((int)data.mode);
    json += "}";
    return json;
}