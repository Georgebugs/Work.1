#ifndef MAIN_COMMUNICATION_H_
#define MAIN_COMMUNICATION_H_

#include "esp_err.h"
#include "portmacro.h"

typedef union
{
	float fValue;
	uint16_t usValues[2];
}uFloatUint16;

typedef enum
{
    eCommandCode = 0,
    eCommandParamA = 1,
    eCommandParamB = 2,
    eCommandID = 3,
    eCommandLength = 4,
    eFirstDataAddressForPolling = 5,
} tCommRegularPoll_IDs;

typedef enum
{
	status_Alive               = (1 << 0),
	status_DAC_OK              = (1 << 1),
	status_PressureSensor_OK   = (1 << 2),
	status_InProgress          = (1 << 3),
	status_Failure             = (1 << 4),
	status_PressureLocked      = (1 << 5),
	status_VentilationFinished = (1 << 6),
	status_PressureInRange     = (1 << 7),
	status_ALL                 = 0xFFFF,
} tPressureStatusBits;

#pragma pack(push, 1)
typedef struct __attribute__((packed)) 
{
	uint32_t usLastCommandNumber;
	uint32_t usPressureStatus;
	uint32_t usMainStatus;
	float ufTargetPressure;
	float ufCurrentPressure;
	uint32_t ulOutputs;
} tLogicMap;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct __attribute__((packed)) 
{
	uint32_t ulCommandCode;
	uint32_t usCommandNumber;
	uint32_t ulParamA;
	uint32_t ulParamB;
} tCommandStruct;
#pragma pack(pop)

typedef enum
{
    eNewData = (1 << 0),
} tCommEvents;

esp_err_t eCommunicationInit(const UBaseType_t uxCommTaskPriority);
esp_err_t eCommunicationSendCommand(const uint32_t usCommand, const uint32_t usParamA, const uint32_t usParamB);
EventGroupHandle_t xCommunicationGetEvenGroupHandle(void);
const tLogicMap* xpCommunicationGetLogicMapCpointer(void);

#endif /* MAIN_COMMUNICATION_H_ */
