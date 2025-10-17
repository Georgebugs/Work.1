#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "communication.h"
#include "mainboardcommands.h"

typedef union __attribute__((packed))
{
	float f;
	uint32_t ul;
	uint16_t us[2];
} uFloatUint32;

void vMainBoardCommandSetSendorClosedState(const uint8_t ucSensor, const bool bOpen)
{
	eCommunicationSendCommand(eCommandCloseSensor, ucSensor, bOpen);
}

void vMainBoardCommandStartGoingToPressure(void)
{
	eCommunicationSendCommand(eCommandStartGoingToPressure, 0, 0);
}
void vMainBoardCommandStopGoingToPressure(void)
{
	eCommunicationSendCommand(eCommandStopGoingToPressure, 0, 0);
}
void vMainBoardCommandVentilation(void)
{
	eCommunicationSendCommand(eCommandVentilation, 0, 0);
}
void vMainBoardSetTargetPressure(const float fNewTarget)
{
	uFloatUint32 uTemp = {.f = fNewTarget	};
	
	eCommunicationSendCommand(eCommandSetTargetPressure, uTemp.ul, 0);
}

float fMainBoardGetCurrentTargetPressure(void)
{
	return xpCommunicationGetLogicMapCpointer()->ufTargetPressure;	
}