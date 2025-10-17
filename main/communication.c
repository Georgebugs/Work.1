#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "driver/uart.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "communication.h"
#include "portmacro.h"

static const char* TAG = "COMM";
#define cCOMM_TASK_STACK_SIZE (8192)
static TaskHandle_t xCommTaskHandle = NULL;
static StackType_t xCommStack[cCOMM_TASK_STACK_SIZE];
static StaticTask_t xCommTaskdef;
static EventGroupHandle_t xEventGroupHandle = NULL;
static StaticEventGroup_t xEventGroupBuffer;

static const int RX_BUF_SIZE = 128;

#define cCOMMAND_QUEUE_LENGTH (10)
static StaticQueue_t xCommandQueue;
static uint8_t ucCommandQueueStorageArea[cCOMMAND_QUEUE_LENGTH * sizeof(tCommandStruct)];
static QueueHandle_t xCommandQueueHandle = NULL;

spinlock_t xLockMutex = portMUX_INITIALIZER_UNLOCKED;

#define MB_PORT_NUM (UART_NUM_1)
#define MB_DEV_SPEED (115200)
#define MASTER_MAX_RETRY (30)
#define POLL_TIMEOUT_MS (10)

#define MB_DEVICE_ADDR1 (1)

typedef union 
{
	struct
	{
		tLogicMap xLogicData;
		uint32_t ulCRC;
	} sDataAndCRC;
	uint8_t ucBytes[sizeof(tLogicMap) + sizeof(uint32_t)];
}uReceiveBuf;

uReceiveBuf xRegisters = {.ucBytes = { 0 } };
uReceiveBuf xReceiveDataShadowCopy = { .ucBytes = { UINT8_MAX } };
typedef union 
{
	struct
	{
		tCommandStruct xCommand;
		uint32_t ulCRC;
	} sCommandAndCRC;
	uint8_t ucBytes[sizeof(tCommandStruct) + sizeof(uint32_t)];
}uTransmitBuf;
uTransmitBuf uxTransmitStructure = {.ucBytes = { 0 }};

static uint16_t usCRC16(const uint8_t *ucpData, const uint16_t usLength)
{
#define POLY 	(0x1021)
		
	uint16_t usCRC = 0;
	for (int i = 0; i < usLength; i++)
	{
		usCRC = usCRC ^ ((uint16_t) ucpData[i] << 8);
		for (int j = 0; j < 8; j++)
		{
			if (usCRC & 0x8000)
				usCRC = (usCRC << j) ^ POLY;
			else
				usCRC <<= 1;
		}
	}			
		
	return usCRC;
}
	

void vCommTask(void* pvParam)
{		
	const uart_config_t uart_config = {
		.baud_rate = MB_DEV_SPEED,
		.data_bits = UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
		.source_clk = UART_SCLK_DEFAULT,
		.rx_flow_ctrl_thresh = 0,
		//.flags.allow_pd = 0,
		.flags.backup_before_sleep = 0,
	};
	typedef enum
	{
		dm_Init            = 0,
		dm_SendRequest     = 1,
		dm_WaitForRegularResponce = 2,
		dm_WaitForNewCommandResponce = 3,
		dm_MAX,
	} tMessagingState;
	static tMessagingState xState = dm_Init;
	uint32_t ulErrorCounter = 0;
	tCommandStruct xNewCommand = { 0 };
	static uint16_t usMessageNumber = UINT16_MAX / 2;
	int rxBytes = 0;
		
	while (1)
	{							
		switch (xState)
		{		
		case dm_Init:
			{
				ESP_LOGI(TAG, "Starting communication task...");    
				esp_err_t err = uart_driver_install(MB_PORT_NUM, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
				ESP_ERROR_CHECK(err != ESP_OK);
				err = uart_param_config(MB_PORT_NUM, &uart_config);				    
				ESP_ERROR_CHECK(err != ESP_OK);
				err = uart_set_pin(MB_PORT_NUM, GPIO_NUM_16, GPIO_NUM_15, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
				ESP_ERROR_CHECK(err != ESP_OK);
				err = uart_set_mode(MB_PORT_NUM, UART_MODE_RS485_HALF_DUPLEX);
				ESP_ERROR_CHECK(err != ESP_OK);
		
				xState = dm_SendRequest;				
			}break;
		case dm_SendRequest:
			{				
				bool bSendNewCommand = false;				
				if  (pdTRUE == xQueuePeek(xCommandQueueHandle, &xNewCommand, 0))
				{
					uxTransmitStructure.sCommandAndCRC.xCommand.ulCommandCode = xNewCommand.ulCommandCode;	        
					uxTransmitStructure.sCommandAndCRC.xCommand.ulParamA = xNewCommand.ulParamA; 
					uxTransmitStructure.sCommandAndCRC.xCommand.ulParamB = xNewCommand.ulParamB;
					uxTransmitStructure.sCommandAndCRC.xCommand.usCommandNumber = usMessageNumber;    
					
					bSendNewCommand = true;
				}
				uint32_t ulTemp = usCRC16(uxTransmitStructure.ucBytes, sizeof(tCommandStruct));				
				uxTransmitStructure.sCommandAndCRC.ulCRC = ((~ulTemp) << 16) | ulTemp;
			
				rxBytes = uart_read_bytes(UART_NUM_1, &(xReceiveDataShadowCopy.ucBytes[0]), sizeof(xReceiveDataShadowCopy), pdMS_TO_TICKS(POLL_TIMEOUT_MS));				
				
				const int txBytes = uart_write_bytes(MB_PORT_NUM, (void*)&uxTransmitStructure.ucBytes, sizeof(uxTransmitStructure));
				if (txBytes == sizeof(uxTransmitStructure)) 									
					xState = bSendNewCommand ? dm_WaitForNewCommandResponce : dm_WaitForRegularResponce;		
				
				vTaskDelay(pdMS_TO_TICKS(1));
			}break;;
		case dm_WaitForRegularResponce:
		case dm_WaitForNewCommandResponce:
			{				
				if (rxBytes == sizeof(xReceiveDataShadowCopy))
				{
					uint32_t ulTemp = usCRC16(xReceiveDataShadowCopy.ucBytes, sizeof(tLogicMap));				
					ulTemp = ((~ulTemp) << 16) | ulTemp;
					
					if (xReceiveDataShadowCopy.sDataAndCRC.ulCRC == ulTemp)
					{						
						if (xState == dm_WaitForNewCommandResponce)
						{																
							if (usMessageNumber == xRegisters.sDataAndCRC.xLogicData.usLastCommandNumber)
							{
								usMessageNumber++;
								ulErrorCounter = 0;
								xQueueReceive(xCommandQueueHandle, &xNewCommand, 0);							
							}
							else
							{
								if (++ulErrorCounter > 10) 
								{
									ulErrorCounter = 0;
									xQueueReceive(xCommandQueueHandle, &xNewCommand, 0);
									ESP_LOGE(TAG, "Can't transmit data. Dropped!");
								}
							}
						}
					
						taskENTER_CRITICAL(&xLockMutex);
						if (0 != memcmp(&(xRegisters.ucBytes), xReceiveDataShadowCopy.ucBytes, sizeof(xReceiveDataShadowCopy)))
						{													                
							memcpy(&(xRegisters.ucBytes), xReceiveDataShadowCopy.ucBytes, sizeof(xReceiveDataShadowCopy));
							xEventGroupSetBits(xEventGroupHandle, eNewData);
						}
						taskEXIT_CRITICAL(&xLockMutex);
					}
				}
				
				xState = dm_SendRequest;					
				break;
			default:
				break;
			}
		}
	}		
}

esp_err_t eCommunicationSendCommand(const uint32_t usCommand, const uint32_t usParamA, const uint32_t usParamB)
{
	tCommandStruct xTempCommand = { 0 };

	xTempCommand.ulCommandCode = usCommand;
	xTempCommand.ulParamA = usParamA;
	xTempCommand.ulParamB = usParamB;

	if (pdTRUE == xQueueSend(xCommandQueueHandle, &xTempCommand, 0))
		return ESP_OK;

	return ESP_FAIL;
}

esp_err_t eCommunicationInit(const UBaseType_t uxCommTaskPriority)
{
	xCommandQueueHandle = xQueueCreateStatic(cCOMMAND_QUEUE_LENGTH, sizeof(tCommandStruct), ucCommandQueueStorageArea, &xCommandQueue);
	ESP_ERROR_CHECK(NULL == xCommandQueueHandle);
	xCommTaskHandle = xTaskCreateStatic(&vCommTask, TAG, cCOMM_TASK_STACK_SIZE, NULL, uxCommTaskPriority, xCommStack, &xCommTaskdef);
	ESP_ERROR_CHECK(NULL == xCommTaskHandle);
	xEventGroupHandle = xEventGroupCreateStatic(&xEventGroupBuffer);
	ESP_ERROR_CHECK(NULL == xEventGroupHandle);

	return ESP_OK;
}

EventGroupHandle_t xCommunicationGetEvenGroupHandle(void)
{
	return xEventGroupHandle;		
}

uint16_t usCommunicationGetRegister(const uint16_t usAddress)
{
	return 0;
}

const tLogicMap* xpCommunicationGetLogicMapCpointer(void)
{
	return &(xRegisters.sDataAndCRC.xLogicData);		
}