#pragma once

#include "stdio.h"
#include "string.h"



#include "esp_err.h"
#include "esp_event.h"




void wifi_event_handler(void* arg, esp_event_base_t event_base,
    int32_t event_id, void* event_data);


  
void wifi_init_softap(void);


void save_config_callback(const char* ssid, const char* password);



























