#include "i2c_cfg.h"


void i2c_init(i2c_port_t i2c_port, int scl_pin, int sda_pin, i2c_master_bus_handle_t *bus_handle)
{

    i2c_master_bus_config_t bus_cfg = {
        .i2c_port = i2c_port,
        .scl_io_num = scl_pin,
        .sda_io_num = sda_pin,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_cfg, bus_handle));

    ESP_LOGI("I2C", "Init is finished");

}

void i2c_dev_init(i2c_master_bus_handle_t *bus_handle, i2c_master_dev_handle_t *dev_handle, uint16_t address, uint32_t scl_speed, i2c_addr_bit_len_t addr_len)
{

    i2c_device_config_t dev_cfg = {
        .device_address = address,
        .scl_speed_hz = scl_speed,
        .dev_addr_length = addr_len,
    };
    
    ESP_ERROR_CHECK(i2c_master_bus_add_device(*bus_handle, &dev_cfg, dev_handle));

    ESP_LOGI("SHT31", "Init is finished");
}


esp_err_t i2c_dev_probe(i2c_master_bus_handle_t *bus_handle, uint16_t address)
{
    esp_err_t ret;

    ret = i2c_master_probe(*bus_handle, address, -1);

    if (ret == ESP_OK)
    {
        ESP_LOGI("I2C", "Find device, address: %x", address);
    }

    return ret;
    
}