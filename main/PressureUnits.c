#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "PressureUnits.h"

float fConvertFromKPaToMmHg(const float fKPa)
{
	return fKPa * 7.50062;
}

float fConvertFromMMHgToKPa(const float fMmHg)
{
	return fMmHg / 7.50062;
}

typedef enum
{
	fpParseSign = 0,
	fpParseFirstPart = 1,
	fpParseSecondPart = 2,
	fpCollectTogether = 3,
	fpError = 4,
	fpFinished = 5,
	fpMAX
}eFloatParserState;

float fConvertFromString(const char* cBuf, const uint16_t usLengthMax)
{
	eFloatParserState xState = fpParseSign;
	bool bSign = true;
	uint32_t ulFirstPart = 0;
	uint32_t ulFirstPartDigitCount = 0;
	uint32_t ulSecondPartDigitCount = 0;
	uint32_t ulSecondPart = 0;
	float fResult = NAN;
	int i = 0;
	
	for (i = 0; i < usLengthMax; i++)
	{
		switch (xState)
		{
		case fpParseSign:
			{
				switch (cBuf[i])
				{
				case 0:
					xState = fpError;
					break;
				case '-':
					xState = fpParseFirstPart;
					bSign = false;
					break;
				case '+':
					xState = fpParseFirstPart;					
					break;
				case '0' ... '9':
					ulFirstPart = cBuf[i] - '0';
					ulFirstPartDigitCount++;
					xState = fpParseFirstPart;			
					break;
				case '.':
					xState = fpParseSecondPart;				
					break;				
				default:
					xState = fpError;
					break;
				}
			}
			break;
		case fpParseFirstPart:
			{
				switch (cBuf[i])
				{
				case 0:
					if (ulFirstPartDigitCount > 0) 
						xState = fpCollectTogether;
					else
						xState = fpError;
					break;
				case '-':
				case '+':
					xState = fpError;
					break;
				case '0' ... '9':
					ulFirstPart *= 10;
					ulFirstPart += cBuf[i] - '0';					
					ulFirstPartDigitCount++;
					break;
				case '.':
					xState = fpParseSecondPart;			
					break;
				default:
					xState = fpError;
					break;
				}
			}
			break;
		case fpParseSecondPart:
			{
				switch (cBuf[i])
				{
				case 0:
					xState = fpCollectTogether;
					break;
				case '-':
				case '+':
				case '.':
					xState = fpError;
					break;
				case '0' ... '9':
					ulSecondPart *= 10;
					ulSecondPart += cBuf[i] - '0';					
					ulSecondPartDigitCount++;
					break;
				default:
					xState = fpError;
					break;
				}
			}
			break;
		case fpCollectTogether:
			{
				fResult = ulFirstPart;
				float fTemp = ulSecondPart;
				for (; ulSecondPartDigitCount > 0; ulSecondPartDigitCount--)
					fTemp /= 10;
				fResult += fTemp;
				if (!bSign) fResult = -fResult;
				xState = fpFinished;
			}break;
		default:
			break;
		}
		
		if (xState >= fpError) break;
	}
	
	return fResult;
}
