#include "web_cfg.h"
#include "esp_log.h"
#include "esp_http_server.h"
#include "nvs_flash.h"
#include "nvs.h"

static const char *TAG = "web_config";
static httpd_handle_t server = NULL;
static wifi_config_save_cb_t config_callback = NULL;

// HTML页面模板
static const char HTML_TEMPLATE[] = 
"<html>"
"<head>"
  "<title>WiFi Config</title>"
  "<meta name='viewport' content='width=device-width, initial-scale=1'>"
  "<style>"
    "body {font-family: Arial; margin: 20px;}"
    "form {max-width: 400px; margin: 0 auto;}"
    "input {width: 100%; padding: 8px; margin: 5px 0;}"
    "button {background: #0066cc; color: white; border: none; padding: 10px;}"
  "</style>"
"</head>"
"<body>"
  "<h2>WiFi Configuration</h2>"
  "<form method='post' action='/save'>"
    "<input type='text' name='ssid' placeholder='SSID' required><br>"
    "<input type='password' name='password' placeholder='Password' required><br>"
    "<button type='submit'>Connect</button>"
  "</form>"
"</body>"
"</html>";

// GET请求处理
static esp_err_t root_get_handler(httpd_req_t *req) {
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, HTML_TEMPLATE, sizeof(HTML_TEMPLATE));
    return ESP_OK;
}

// POST请求处理
static esp_err_t save_post_handler(httpd_req_t *req) {
    char content[256];
    int ret;

    // 读取POST数据
    if ((ret = httpd_req_recv(req, content, sizeof(content))) <= 0) {
        return ESP_FAIL;
    }
    content[ret] = '\0';

    // 解析参数
    char ssid[32] = {0};
    char password[64] = {0};
    httpd_query_key_value(content, "ssid", ssid, sizeof(ssid));
    httpd_query_key_value(content, "password", password, sizeof(password));

    // 调用回调函数
    if (config_callback) {
        config_callback(ssid, password);
    }

    httpd_resp_send(req, "<h3>Configuration saved!</h3>", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

void web_config_init(wifi_config_save_cb_t save_cb) {
    config_callback = save_cb;
}

bool web_config_start(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    
    httpd_uri_t root_uri = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = root_get_handler
    };

    httpd_uri_t save_uri = {
        .uri = "/save",
        .method = HTTP_POST,
        .handler = save_post_handler
    };

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_register_uri_handler(server, &root_uri);
        httpd_register_uri_handler(server, &save_uri);
        ESP_LOGI(TAG, "Web server started on port: %d", config.server_port);
        return true;
    }
    return false;
}

void web_config_stop(void) {
    if (server) {
        httpd_stop(server);
        server = NULL;
        ESP_LOGI(TAG, "Web server stopped");
    }
}