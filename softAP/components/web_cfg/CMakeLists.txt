idf_component_register(SRCS "src/web_cfg.c"
                    INCLUDE_DIRS "include"
                    REQUIRES esp_http_server nvs_flash)