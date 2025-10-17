#ifndef MAINBOARDCOMMANDS_H_
#define MAINBOARDCOMMANDS_H_

typedef enum
{
	eCommandNone        = 0,
	eCommandCloseSensor = 1,	
	eCommandSetTargetPressure = 2,
	eCommandStartGoingToPressure = 3,
	eCommandStopGoingToPressure = 4,
	eCommandVentilation = 5,
	eCommanMAX
}tCommCommand;

void vMainBoardCommandSetSendorClosedState(const uint8_t ucSensor, const bool bOpen);
void vMainBoardCommandStartGoingToPressure(void);
void vMainBoardCommandStopGoingToPressure(void);
void vMainBoardCommandVentilation(void);
void vMainBoardSetTargetPressure(const float fNewTarget);
float fMainBoardGetCurrentTargetPressure(void);
#endif // MAINBOARDCOMMANDS_H_


