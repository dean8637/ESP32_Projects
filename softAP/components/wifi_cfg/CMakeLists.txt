idf_component_register(SRCS "src/wifi_cfg.c"
                    INCLUDE_DIRS "include"
                    REQUIRES esp_netif esp_wifi lwip nvs_flash esp_http_server)