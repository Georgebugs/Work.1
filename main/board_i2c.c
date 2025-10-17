#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "driver/i2c_master.h"
#include "board_i2c.h"

i2c_master_bus_handle_t xBoardI2C_handle(void)
{
    static i2c_master_bus_handle_t bus_handle = NULL;

    i2c_master_bus_config_t i2c_bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = ccBOARD_I2C_NUM,
        .scl_io_num = ccBOARD_I2C_MASTER_SCL_IO,
        .sda_io_num = ccBOARD_I2C_MASTER_SDA_IO,
        .glitch_ignore_cnt = 7,
    };

    if(NULL == bus_handle)
        ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_config, &bus_handle));

    return bus_handle;
}
