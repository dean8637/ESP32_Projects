idf_component_register(
    SRCS "wifi_cfg.c"
    INCLUDE_DIRS .
    REQUIRES esp_wifi esp_netif lwip nvs_flash spi
)