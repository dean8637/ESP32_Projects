#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "i2c_cfg.h"
#include "sht31.h"

#include "spi_cfg.h"



#define BACK_COLOR_WHITE   0x00
#define BACK_COLOR_BLACK    0xff

#define FONT_WHITE   0
#define FONT_BLACK    1
#define DISPLAY_MODE   FONT_BLACK

void bsp_init();

void freeRTOS_start();
void Task_Start();

void Task_SHT31_Single_Shot();


