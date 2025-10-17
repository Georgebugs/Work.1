#ifndef _PRESSURE_UNITS_H
#define _PRESSURE_UNITS_H

#define cPRESSURE_UNITS_MAX_MMHG		(300.0)
#define cPRESSURE_UNITS_MIN_MMHG		(0.0)

float fConvertFromKPaToMmHg(const float fKPa);
float fConvertFromMMHgToKPa(const float fMmHg);
float fConvertFromString(const char* cBuf, const uint16_t usLengthMax);

#endif // _PRESSURE_UNITS_H