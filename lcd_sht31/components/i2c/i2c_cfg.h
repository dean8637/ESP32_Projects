#pragma once

#include "driver/i2c_master.h"
#include "esp_err.h"
#include "esp_log.h"




void i2c_init(i2c_port_t i2c_port, int scl_pin, int sda_pin, i2c_master_bus_handle_t *bus_handle);


void i2c_dev_init(i2c_master_bus_handle_t *bus_handle, i2c_master_dev_handle_t *dev_handle, uint16_t address, uint32_t scl_speed, i2c_addr_bit_len_t addr_len);


esp_err_t i2c_dev_probe(i2c_master_bus_handle_t *bus_handle, uint16_t address);
