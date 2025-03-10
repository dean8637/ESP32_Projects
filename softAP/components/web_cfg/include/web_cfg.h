#pragma once
#include <stdbool.h>
#include "esp_http_server.h"

// 配置保存回调函数类型
typedef void (*wifi_config_save_cb_t)(const char* ssid, const char* password);

/**
 * @brief 初始化Web配置服务
 * @param save_cb 配置保存回调函数
 */
void web_config_init(wifi_config_save_cb_t save_cb);

/**
 * @brief 启动Web服务器
 * @return true 启动成功，false 失败
 */
bool web_config_start(void);

/**
 * @brief 停止Web服务器
 */
void web_config_stop(void);


