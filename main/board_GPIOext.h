#ifndef MAIN_BOARD_GPIOEXT_H_
#define MAIN_BOARD_GPIOEXT_H_

#include "esp_err.h"
esp_err_t eBoardGPIOextInit(void);
esp_err_t eBoard_GPIOextDisplayCTPresetLevel(const bool bLevel);
esp_err_t eBoard_GPIOextDisplayBackLight(const bool bEnable);
esp_err_t eBoard_GPIOextDisplayLCDresetLevel(const bool bLevel);
esp_err_t eBoard_GPIOextSDCSlevel(const bool bLevel);
esp_err_t eBoard_GPIOextSelectCAN_nUSB(const bool bSelectCAN);
esp_err_t eBoard_GPIOextDisplayLCDpower(const bool bEnable);

#endif /* MAIN_BOARD_GPIOEXT_H_ */
