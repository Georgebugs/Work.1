#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_touch_gt911.h"
#include "esp_log.h"
#include "esp_lvgl_port.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal/gpio_types.h"
#include "indev/lv_indev.h"
#include "lvgl.h"
#include "esp_lvgl_port_touch.h"
#include "board_GPIOext.h"
#include "board_i2c.h"
#include "soc/gpio_num.h"
#include "display.h"

#define ccDISPLAY_DRAW_BUFF_HEIGHT (120)

#define ccDISPLAY_TOUCH_INT_LINE (GPIO_NUM_4)

#define ccDISPLAY_PIXEL_CLOCK_HZ (13 * 1000 * 1000)
#define PIN_NUM_HSYNC 46
#define PIN_NUM_VSYNC 3
#define PIN_NUM_DE 5
#define PIN_NUM_PCLK 7
#define PIN_NUM_DATA0 14 // B3
#define PIN_NUM_DATA1 38 // B4
#define PIN_NUM_DATA2 18 // B5
#define PIN_NUM_DATA3 17 // B6
#define PIN_NUM_DATA4 10 // B7
#define PIN_NUM_DATA5 39 // G2
#define PIN_NUM_DATA6 0 // G3
#define PIN_NUM_DATA7 45 // G4
#define PIN_NUM_DATA8 48 // G5
#define PIN_NUM_DATA9 47 // G6
#define PIN_NUM_DATA10 21 // G7
#define PIN_NUM_DATA11 1 // R3
#define PIN_NUM_DATA12 2 // R4
#define PIN_NUM_DATA13 42 // R5
#define PIN_NUM_DATA14 41 // R6
#define PIN_NUM_DATA15 40 // R7
#define PIN_NUM_DISP_EN -1

#define ccDISPLAY_H_RES (800)
#define ccDISPLAY_V_RES (480)

#define ccDISPLAY_TOUCH_FREQ_HZ (400000)

static const char* TAG = "DISPLAY";

static esp_lcd_panel_handle_t xLCDpanelHandle = NULL;
static esp_lcd_touch_handle_t xLCDtouchHandle = NULL;
static lv_display_t* xLVGLdisplay = NULL;
static lv_indev_t* xLVGLtouchIndev = NULL;

esp_err_t eDisplayInitLCD(void)
{
    esp_err_t eError = ESP_FAIL;

    esp_lcd_rgb_panel_config_t panel_conf = {
      .clk_src = LCD_CLK_SRC_PLL240M,
      .timings = {.pclk_hz = ccDISPLAY_PIXEL_CLOCK_HZ,
                  .h_res = ccDISPLAY_H_RES,
                  .v_res = ccDISPLAY_V_RES,
                  .hsync_back_porch = 8,
                  .hsync_front_porch = 8,
                  .hsync_pulse_width = 4,
                  .vsync_back_porch = 8,
                  .vsync_front_porch = 8,
                  .vsync_pulse_width = 4,
                  .flags = {.hsync_idle_low = false,
                            .vsync_idle_low = false,
                            .de_idle_high = false,
                            .pclk_active_neg = true,
                            .pclk_idle_high = false}},
      .data_width = 16,
      .bits_per_pixel = 16,
      .num_fbs = 2,
      .bounce_buffer_size_px = 0,
      .psram_trans_align = 64,

      .hsync_gpio_num = PIN_NUM_HSYNC,
      .vsync_gpio_num = PIN_NUM_VSYNC,
      .de_gpio_num = PIN_NUM_DE,
      .pclk_gpio_num = PIN_NUM_PCLK,
      .disp_gpio_num = PIN_NUM_DISP_EN,
      .data_gpio_nums =
          {
              PIN_NUM_DATA0,
              PIN_NUM_DATA1,
              PIN_NUM_DATA2,
              PIN_NUM_DATA3,
              PIN_NUM_DATA4,
              PIN_NUM_DATA5,
              PIN_NUM_DATA6,
              PIN_NUM_DATA7,
              PIN_NUM_DATA8,
              PIN_NUM_DATA9,
              PIN_NUM_DATA10,
              PIN_NUM_DATA11,
              PIN_NUM_DATA12,
              PIN_NUM_DATA13,
              PIN_NUM_DATA14,
              PIN_NUM_DATA15,
          },
      .flags = {.disp_active_low = 0,
                .refresh_on_demand = 0,
                .fb_in_psram = true,
                .double_fb = true,
                .no_fb = 0,
                .bb_invalidate_cache = 0}};

    do
    {
        if(ESP_OK != eBoardGPIOextInit())
            break;
        vTaskDelay(pdMS_TO_TICKS(50));
        if(ESP_OK != eBoard_GPIOextDisplayLCDpower(true))
            break;
        vTaskDelay(pdMS_TO_TICKS(100));
        if(ESP_OK != eBoard_GPIOextDisplayLCDresetLevel(false))
            break;
        vTaskDelay(pdMS_TO_TICKS(2));
        if(ESP_OK != eBoard_GPIOextDisplayLCDresetLevel(true))
            break;
        vTaskDelay(pdMS_TO_TICKS(5));
        if(ESP_OK != esp_lcd_new_rgb_panel(&panel_conf, &xLCDpanelHandle))
            break;
        if(ESP_OK != esp_lcd_panel_init(xLCDpanelHandle))
            break;
        eError = ESP_OK;
    } while(0);

    if(ESP_OK != eError)
    {
        if(NULL != xLCDpanelHandle)
            esp_lcd_panel_del(xLCDpanelHandle);
        ESP_LOGE(TAG, "LCD init fail!");
    }

    return eError;
}

esp_err_t sDisplayInitTouch(void)
{
    const esp_lcd_touch_config_t tp_cfg = {
      .x_max = ccDISPLAY_H_RES,
      .y_max = ccDISPLAY_V_RES,
      .rst_gpio_num = GPIO_NUM_NC,
      .int_gpio_num = ccDISPLAY_TOUCH_INT_LINE,
      .levels =
          {
              .reset = 0,
              .interrupt = 0,
          },
      .interrupt_callback = NULL,
      .process_coordinates = NULL,
      .flags =
          {
              .swap_xy = 0,
              .mirror_x = 0,
              .mirror_y = 0,
          },
      .user_data = NULL,
      .driver_data = NULL,
  };

    static esp_lcd_panel_io_handle_t tp_io_handle = NULL;
    static const esp_lcd_panel_io_i2c_config_t tp_io_config = { .dev_addr = ESP_LCD_TOUCH_IO_I2C_GT911_ADDRESS,
        .scl_speed_hz = ccDISPLAY_TOUCH_FREQ_HZ,
        .control_phase_bytes = 1,
        .dc_bit_offset = 0,
        .lcd_cmd_bits = 16,
        .flags = {
        .dc_low_on_data = 0,
        .disable_control_phase = 1,
        } };

    esp_err_t eError = ESP_FAIL;

    do
    {
        if(ESP_OK != eBoard_GPIOextDisplayCTPresetLevel(false))
            break;
        gpio_set_direction(ccDISPLAY_TOUCH_INT_LINE, GPIO_MODE_OUTPUT);
        gpio_set_level(ccDISPLAY_TOUCH_INT_LINE, 0); // To force I2C address of GT911 to the default value
        vTaskDelay(pdMS_TO_TICKS(2));
        if(ESP_OK != eBoard_GPIOextDisplayCTPresetLevel(true))
            break;
        vTaskDelay(pdMS_TO_TICKS(2));

        int lProbes;
        for(lProbes = 0; lProbes < 5; lProbes++)
        {
            if(ESP_OK == i2c_master_probe(xBoardI2C_handle(), ESP_LCD_TOUCH_IO_I2C_GT911_ADDRESS, 5))
                break;
            vTaskDelay(pdMS_TO_TICKS(2));
        }
        if(lProbes > 3)
            break;

        if(ESP_OK != esp_lcd_new_panel_io_i2c(xBoardI2C_handle(), &tp_io_config, &tp_io_handle))
            break;
        gpio_set_direction(ccDISPLAY_TOUCH_INT_LINE, GPIO_MODE_INPUT);
        if(ESP_OK != esp_lcd_touch_new_i2c_gt911(tp_io_handle, &tp_cfg, &xLCDtouchHandle))
            break;

        eError = ESP_OK;
    } while(0);

    if(ESP_OK != eError)
        ESP_LOGE(TAG, "Touch init fail!");

    return eError;
}

esp_err_t eDisplayInitLVGL(void)
{
    const lvgl_port_cfg_t xLVGLconfig = {
        .task_priority = 4, /* LVGL task priority */
        .task_stack = 8192, /* LVGL task stack size */
        .task_affinity = -1, /* LVGL task pinned to core (-1 is no affinity) */
        .task_max_sleep_ms = 500, /* Maximum sleep in LVGL task */
        .timer_period_ms = 5 /* LVGL timer tick period in ms */		    
    };
    const lvgl_port_display_cfg_t xLVGLdisplayConfig = {
      .io_handle = NULL,
      .panel_handle = xLCDpanelHandle,
      .control_handle = NULL,
      .buffer_size = ccDISPLAY_H_RES * ccDISPLAY_DRAW_BUFF_HEIGHT,
      .double_buffer = true,
      .trans_size = 0,

      .hres = ccDISPLAY_H_RES,
      .vres = ccDISPLAY_V_RES,
      .monochrome = false,

      .rotation =
          {
              .swap_xy = false,
              .mirror_x = false,
              .mirror_y = false,
          },
      .color_format = LV_COLOR_FORMAT_RGB565,
      .flags = {
          .buff_dma = false,
          .buff_spiram = true,
          .sw_rotate = false,
          .swap_bytes = false,
          .full_refresh = false,
          .direct_mode = false,
      }};
    const lvgl_port_display_rgb_cfg_t xLVGL_RGB_DisplayConfig = { .flags = {
                                                                  .bb_mode = true,
                                                                  .avoid_tearing = false,
                                                                  } };
    lvgl_port_touch_cfg_t xLVGLtouchConfig = {
        .disp = NULL,
        .handle = xLCDtouchHandle,
    };

    esp_err_t eError = ESP_FAIL;

    do
    {
        if(ESP_OK != lvgl_port_init(&xLVGLconfig))
            break;
        if(!lvgl_port_lock(0))
            break;
        xLVGLdisplay = lvgl_port_add_disp_rgb(&xLVGLdisplayConfig, &xLVGL_RGB_DisplayConfig);
        if(NULL == xLVGLdisplay)
            break;
        xLVGLtouchConfig.disp = xLVGLdisplay;
        xLVGLtouchIndev = lvgl_port_add_touch(&xLVGLtouchConfig);
        if(NULL == xLVGLtouchIndev)
            break;

        eError = ESP_OK;
    } while(0);

    lvgl_port_unlock();

    if(ESP_OK != eError)
        ESP_LOGE(TAG, "LVGL init fail!");

    return eError;
}

esp_err_t eDisplayControlBacklight(const bool bEnable)
{
    return eBoard_GPIOextDisplayBackLight(bEnable);
}
