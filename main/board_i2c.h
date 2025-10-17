#ifndef MAIN_BOARD_I2C_H_
#define MAIN_BOARD_I2C_H_

#include "driver/i2c_master.h"
i2c_master_bus_handle_t xBoardI2C_handle(void);

#define ccBOARD_I2C_NUM (I2C_NUM_0)
#define ccBOARD_I2C_MASTER_SCL_IO (GPIO_NUM_9)
#define ccBOARD_I2C_MASTER_SDA_IO (GPIO_NUM_8)


#endif /* MAIN_BOARD_I2C_H_ */
