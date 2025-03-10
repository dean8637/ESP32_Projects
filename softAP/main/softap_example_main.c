/*  WiFi softAP Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "nvs_flash.h"
#include "nvs.h"

#include "wifi_cfg.h"
#include "web_cfg.h"


























void app_main(void)
{



    wifi_init_softap();


   // 初始化Web配置
   web_config_init(save_config_callback);






    // 启动Web服务器
    if (web_config_start()) {
      ESP_LOGI("MAIN", "Web configuration available at http://192.168.4.1");
  }








   // 主循环
   while (1) {
   vTaskDelay(pdMS_TO_TICKS(1000));
   }
}
