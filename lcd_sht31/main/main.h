#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "i2c_cfg.h"
#include "sht31.h"

#include "spi_cfg.h"

#include "wifi_cfg.h"

#include "mqtt_client.h"

// #define ESP_WIFI_SSID      "Dean"
// #define ESP_WIFI_PASS      "l17612722442."
// #define ESP_MAXIMUM_RETRY  5




#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

#define BACK_COLOR_WHITE   0x00
#define BACK_COLOR_BLACK    0xff

#define FONT_WHITE   0
#define FONT_BLACK    1
#define DISPLAY_MODE   FONT_BLACK

void bsp_init();


void event_handler(void* arg, esp_event_base_t event_base,
    int32_t event_id, void* event_data);

void freeRTOS_start();
void Task_Start();

void Task_SHT31_Single_Shot();

void Task_WiFi_Con();

void Task_WiFi_Disc();

void Task_WiFi_Reconnect();

void Task_MQTT_Start();

void Task_Pub_Temp();
void Task_Offline();



void mqtt_app_start(void);

void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);