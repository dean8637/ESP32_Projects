#pragma once

#include "stdint.h"
#include "esp_err.h"
#include "driver/i2c_master.h"
#include "i2c_cfg.h"
#include "esp_log.h"
#include "esp_check.h"


#define PIN_NUM_SCL 4
#define PIN_NUM_SDA 5

#define SHT31_I2C_ADDR_DELF 0x44