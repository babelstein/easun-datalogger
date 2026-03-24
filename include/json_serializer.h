#ifndef JSON_SERIALIZER_H
#define JSON_SERIALIZER_H

#include <Arduino.h>
#include "qpigs_parser.h"
#include "qpiri_parser.h"
#include "qmod_parser.h"

/**
 * @brief Convert QPIGSData to JSON string
 * 
 * @param data The QPIGSData structure to convert
 * @return String JSON representation of the data
 */
String qpigsDataToJson(const QPIGSData& data);

/**
 * @brief Convert QPIRIData to JSON string
 * 
 * @param data The QPIRIData structure to convert
 * @return String JSON representation of the data
 */
String qpiriDataToJson(const QPIRIData& data);

/**
 * @brief Convert QMODData to JSON string
 * 
 * @param data The QMODData structure to convert
 * @return String JSON representation of the data
 */
String qmodDataToJson(const QMODData& data);

#endif // JSON_SERIALIZER_H
