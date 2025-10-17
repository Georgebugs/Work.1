#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/semphr.h"
#include "board_i2c.h"
#include "board_GPIOext.h"
#include "driver/i2c_master.h"
#include "esp_err.h"

typedef enum
{
    bGPIO_CTPreset = (1 << 1),
    bGPIO_BackLightEn = (1 << 2),
    bGPIO_LCD_Nreset = (1 << 3),
    bGPIO_SDCS = (1 << 4),
    bGPIO_SelectCAN = (1 << 5),
    bGPIO_LCDpower = (1 << 6),
} tBoardExtGPIO;

#define ccBOARD_MAX_WAIT_TIME_I2C_MS (200)

#define ccBOARD_GPIOEXT_DEFAULT_OUTPUTS (0 * bGPIO_CTPreset | 0 * bGPIO_BackLightEn | 0 * bGPIO_LCD_Nreset | 0 * bGPIO_SDCS | 0 * bGPIO_SelectCAN | 0 * bGPIO_LCDpower)
static uint8_t ucOutputsState = ccBOARD_GPIOEXT_DEFAULT_OUTPUTS;

static SemaphoreHandle_t xMutexOutputs = NULL;
static StaticSemaphore_t xMutexOutputsBuffer;

static i2c_master_dev_handle_t xI2CdevConfigHandle = NULL;
static i2c_master_dev_handle_t xI2CdevOutputsHandle = NULL;

#define ccBOARD_GPIOEXT_I2C_ADDRESS_CONFIG (0x24)
#define ccBOARD_GPIOEXT_I2C_ADDRESS_OUTPUTS (0x38)
#define ccBOARD_GPIOEXT_MAX_ERRORS (5)
#define ccBOARD_GPIOEXT_WAIT_TIME_MS (200)

static esp_err_t eBoardGPIOset(const uint8_t ucOutputs)
{
    uint8_t ucWriteBuffer = ucOutputs;
    return i2c_master_transmit(xI2CdevOutputsHandle, &ucWriteBuffer, 1, ccBOARD_GPIOEXT_WAIT_TIME_MS);
}
static esp_err_t eBoardConfigSet(const uint8_t ucConfig)
{
    uint8_t ucWriteBuffer = ucConfig;
    return i2c_master_transmit(xI2CdevConfigHandle, &ucWriteBuffer, 1, ccBOARD_GPIOEXT_WAIT_TIME_MS);
}
static esp_err_t eBoardExpanderModifyAndWriteData(const tBoardExtGPIO xGPIO, const bool bEnable)
{
    esp_err_t eError = ESP_FAIL;

    do
    {
        if(pdTRUE != xSemaphoreTake(xMutexOutputs, pdMS_TO_TICKS(ccBOARD_MAX_WAIT_TIME_I2C_MS)))
            break;

        if(bEnable)
            ucOutputsState |= xGPIO;
        else
            ucOutputsState &= ~xGPIO;

        if(ESP_OK != eBoardGPIOset(ucOutputsState))
            break;

        eError = ESP_OK;
    } while(0);

    xSemaphoreGive(xMutexOutputs);
    return eError;
}

esp_err_t eBoard_GPIOextDisplayCTPresetLevel(const bool bLevel)
{
    return eBoardExpanderModifyAndWriteData(bGPIO_CTPreset, bLevel);
}
esp_err_t eBoard_GPIOextDisplayBackLight(const bool bEnable)
{
    return eBoardExpanderModifyAndWriteData(bGPIO_BackLightEn, bEnable);
}
esp_err_t eBoard_GPIOextDisplayLCDresetLevel(const bool bLevel)
{
    return eBoardExpanderModifyAndWriteData(bGPIO_LCD_Nreset, bLevel);
}
esp_err_t eBoard_GPIOextSDCSlevel(const bool bLevel)
{
    return eBoardExpanderModifyAndWriteData(bGPIO_SDCS, bLevel);
}
esp_err_t eBoard_GPIOextSelectCAN_nUSB(const bool bSelectCAN)
{
    return eBoardExpanderModifyAndWriteData(bGPIO_SelectCAN, bSelectCAN);
}
esp_err_t eBoard_GPIOextDisplayLCDpower(const bool bEnable)
{
    return eBoardExpanderModifyAndWriteData(bGPIO_LCDpower, bEnable);
}

esp_err_t eBoardGPIOextInit(void)
{
    const i2c_device_config_t xConfigI2Cdev = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = ccBOARD_GPIOEXT_I2C_ADDRESS_CONFIG,
        .scl_speed_hz = 400000,
    };
    const i2c_device_config_t xOutputsI2Cdev = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = ccBOARD_GPIOEXT_I2C_ADDRESS_OUTPUTS,
        .scl_speed_hz = 400000,
    };

    esp_err_t eError = ESP_FAIL;

    xMutexOutputs = xSemaphoreCreateMutexStatic(&xMutexOutputsBuffer);
    if(NULL == xMutexOutputs)
        return ESP_FAIL;
    if(pdTRUE != xSemaphoreTake(xMutexOutputs, pdMS_TO_TICKS(ccBOARD_MAX_WAIT_TIME_I2C_MS)))
        return ESP_FAIL;


    ucOutputsState = ccBOARD_GPIOEXT_DEFAULT_OUTPUTS;

    for(int i = 0; i < ccBOARD_GPIOEXT_MAX_ERRORS; i++)
    {
        if(ESP_OK != i2c_master_probe(xBoardI2C_handle(), ccBOARD_GPIOEXT_I2C_ADDRESS_CONFIG, ccBOARD_GPIOEXT_WAIT_TIME_MS))
            continue;
        if(ESP_OK != i2c_master_bus_add_device(xBoardI2C_handle(), &xConfigI2Cdev, &xI2CdevConfigHandle))
            continue;
        if(ESP_OK != i2c_master_bus_add_device(xBoardI2C_handle(), &xOutputsI2Cdev, &xI2CdevOutputsHandle))
            continue;
        if(ESP_OK != eBoardGPIOset(ucOutputsState))
            continue;
        if(ESP_OK != eBoardConfigSet(0x01))
            continue;

        eError = ESP_OK;
        break;
    }

    xSemaphoreGive(xMutexOutputs);
    return eError;
}