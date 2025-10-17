#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "esp_lvgl_port.h"
#include "display.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "portmacro.h"
#include "communication.h"
#include "PressureUnits.h"
#include "ui.h"
#include "gui.h"
#include "screens.c"

#include "vars.h"

static const char* TAG = "GUI";
#define cGUI_TASK_STACK_SIZE (8192)

static TaskHandle_t xGuiTaskHandle = NULL;
static StackType_t xGuiStack[cGUI_TASK_STACK_SIZE];
static StaticTask_t xGuiTaskdef;

void vGuiTask(void* pvParam)
{
    lvgl_port_lock(0);
    ui_init();
    lvgl_port_unlock();

    while(1)
    {
        lvgl_port_lock(0);
        ui_tick();
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(25));

        EventBits_t bits = xEventGroupWaitBits(xCommunicationGetEvenGroupHandle(), eNewData,
        pdTRUE, // Clear bits on exit
        pdFALSE, // Don't wait for all bits
        0);
        if (0 != (bits & eNewData))
		{
    		char cpTemp[16];
    		snprintf(cpTemp, sizeof(cpTemp), "%.2f", fConvertFromKPaToMmHg(xpCommunicationGetLogicMapCpointer()->ufCurrentPressure));    
    		set_var_cp_current_pressure(cpTemp);
    
    
    		ui_set_pressure_value(cpTemp);
    
    		snprintf(cpTemp, sizeof(cpTemp), "%.2f", fConvertFromKPaToMmHg(xpCommunicationGetLogicMapCpointer()->ufTargetPressure));    
    		set_var_cp_target_pressure(cpTemp);
    
    		uint32_t ulTemp = xpCommunicationGetLogicMapCpointer()->ulOutputs;
    		set_var_b_switch1_checked_Directly((ulTemp & (1 << 0)) != 0);
    		set_var_b_switch2_checked_Directly((ulTemp & (1 << 1)) != 0);
    		set_var_b_switch3_checked_Directly((ulTemp & (1 << 2)) != 0);
    		set_var_b_switch4_checked_Directly((ulTemp & (1 << 3)) != 0);          
		}  
    }
}


esp_err_t eGuiInitDisplay(const UBaseType_t uxGuiTaskPriority)
{
    uint32_t ulErrorCounter = 0;
    esp_err_t eError = ESP_FAIL;

    do
    {
        // --- 1. Инициализация LCD ---
        for (ulErrorCounter = 0; ulErrorCounter < 5; ulErrorCounter++) {
            if (ESP_OK == eDisplayInitLCD()) break;
            vTaskDelay(pdMS_TO_TICKS(100));
        }
        if (ulErrorCounter > 3) {
            ESP_LOGE(TAG, "LCD init failed!");
            break;
        }

        // --- 2. Попробуем инициализировать тач, но без зависания ---
        esp_err_t touch_res = ESP_FAIL;
        for (ulErrorCounter = 0; ulErrorCounter < 3; ulErrorCounter++) {
            touch_res = sDisplayInitTouch();
            if (touch_res == ESP_OK) {
                ESP_LOGI(TAG, "Touch initialized successfully");
                break;
            }
            ESP_LOGW(TAG, "Touch init failed, retry %u", (unsigned int)(ulErrorCounter + 1));
            vTaskDelay(pdMS_TO_TICKS(100));
        }
        if (touch_res != ESP_OK) {
            ESP_LOGW(TAG, "⚠️ Touchscreen not responding — continuing without touch");
            // Не прерываем инициализацию, просто идём дальше
        }

        // --- 3. LVGL ---
        for (ulErrorCounter = 0; ulErrorCounter < 5; ulErrorCounter++) {
            if (ESP_OK == eDisplayInitLVGL()) break;
            vTaskDelay(pdMS_TO_TICKS(100));
        }
        if (ulErrorCounter > 3) {
            ESP_LOGE(TAG, "LVGL init failed!");
            break;
        }

        // --- 4. GUI task ---
        xGuiTaskHandle = xTaskCreateStatic(&vGuiTask, TAG, cGUI_TASK_STACK_SIZE, NULL, uxGuiTaskPriority, xGuiStack, &xGuiTaskdef);
        if (NULL == xGuiTaskHandle) {
            ESP_LOGE(TAG, "Failed to create GUI task!");
            break;
        }

        // --- 5. Подсветка ---
        if (ESP_OK != eDisplayControlBacklight(true)) {
            ESP_LOGW(TAG, "Backlight control failed");
        }

        eError = ESP_OK;

    } while (0);

    return eError;
}
