#pragma once


#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "driver/spi_master.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"

#include "font.h"
#include "font2.h"
#include "font3.h"





#define LCD_HOST    SPI2_HOST

#define PIN_NUM_CS   10
#define PIN_NUM_CLK  12
#define PIN_NUM_MOSI 11
#define PIN_NUM_MISO 13



#define PIN_NUM_DC   14
#define PIN_NUM_RST  9


extern  uint16_t BACK_COLOR;

#define USE_HORIZONTAL 0  

#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 168
#define LCD_H 384

#else
#define LCD_W 320
#define LCD_H 170
#endif

#define WHITE         	 0xFF
#define BLACK         	 0x00




void spi_init(int mosi_pin, int sclk_pin, int max_transfer_sz);

void spi_dev_init(int cs_pin, int sclk_speed, spi_device_handle_t *dev_handle);

void spi_pre_transfer_callback(spi_transaction_t *t);

void spi_cmd(spi_device_handle_t *spi_dev_handle, const uint8_t cmd);

void spi_data(spi_device_handle_t *spi_dev_handle, const uint8_t data);

void spi_lcd_init(spi_device_handle_t *spi_device_handle_t);



void LCD_Refresh(spi_device_handle_t *spi_dev_handle);

void LCD_Fill(spi_device_handle_t *spi_dev_handle, uint8_t dat);

uint32_t LCD_pow(uint8_t m,uint8_t n);

void LCD_DrawPoint(uint16_t x,uint16_t y,uint8_t mode);


void LCD_ShowChar(uint16_t x,uint16_t y,uint8_t num,uint8_t sizey,uint8_t mode);


void LCD_ShowString(uint16_t x,uint16_t y, const char *p, uint8_t sizey, uint8_t mode);

void LCD_ShowIntNum(uint16_t x, uint16_t y, uint16_t num, uint8_t len, uint8_t sizey, uint8_t mode);

void LCD_ShowFloatNum1(uint16_t x, uint16_t y,  float num, uint8_t len, uint8_t sizey, uint8_t pre, uint8_t mode);

//void LCD_TempSymbol(uint16_t x,uint16_t y,uint8_t sizey,uint8_t mode);


