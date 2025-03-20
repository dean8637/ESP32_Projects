#include "main.h"





static const char *TAG1 = "wifi station";
static int s_retry_num = 0;





/*bsp init handle*/
i2c_master_bus_handle_t i2c_bus_handle;
i2c_master_dev_handle_t i2c_dev_handle;

spi_device_handle_t spi_dev_handle;




/*task handle*/
TaskHandle_t task_start_handle;

TaskHandle_t single_shot_handle;
TaskHandle_t find_sht31_handle;

TaskHandle_t wifi_con_handle;
TaskHandle_t wifi_disc_handle;
TaskHandle_t wifi_reconnect_handle;


TaskHandle_t mqtt_start_handle;
TaskHandle_t pub_temp_handle;
TaskHandle_t offline_handle;


esp_mqtt_client_handle_t client;

 
/*sht31 data*/
volatile float temp;
volatile float humi;







void app_main(void)
{

    bsp_init();

    freeRTOS_start();

}


void bsp_init()
{
    i2c_init(-1, PIN_NUM_SCL, PIN_NUM_SDA, &i2c_bus_handle);

    i2c_dev_init(&i2c_bus_handle, &i2c_dev_handle, SHT31_I2C_ADDR_DELF, 400000, I2C_ADDR_BIT_LEN_7);

    i2c_dev_probe(&i2c_bus_handle, SHT31_I2C_ADDR_DELF);

    spi_init(PIN_NUM_MOSI, PIN_NUM_CLK, 4096);

    spi_dev_init(PIN_NUM_CS, 20000000, &spi_dev_handle);

    spi_lcd_init(&spi_dev_handle);

    LCD_Refresh(&spi_dev_handle);

    LCD_Fill(&spi_dev_handle, BACK_COLOR_WHITE);

    LCD_ShowString(0, 0, "TEMP", 72, DISPLAY_MODE);
    LCD_ShowString(144, 0, ":", 72, DISPLAY_MODE);

    LCD_ShowString(0, 62, "HUMI:", 72, DISPLAY_MODE);
    LCD_ShowString(144, 62, ":", 72, DISPLAY_MODE);

    LCD_DrawRectangle(0, 135, 168, 384);


    LCD_ShowPicture(0, 135, 384, 32, rect, 1);

    LCD_ShowPicture(34, 136, 64, 32, offline, 1);
    // LCD_ShowPicture(165, 136, 64, 32, online, 1);



    LCD_Refresh(&spi_dev_handle);



    wifi_init_sta(event_handler);













}


void freeRTOS_start()
{

    xTaskCreatePinnedToCore(Task_Start, "Task_Start", 3072, NULL, 12, &task_start_handle, 1);

}


void Task_Start()
{

    xTaskCreatePinnedToCore(Task_SHT31_Single_Shot, "SHT31_Single_Shot", 3072, NULL, 12, &single_shot_handle, 1);
    
    xTaskCreatePinnedToCore(Task_WiFi_Con, "wifi_con", 2048, NULL, 10, &wifi_con_handle, 1);
    xTaskCreatePinnedToCore(Task_WiFi_Disc, "wifi_disc", 2048, NULL, 10, &wifi_disc_handle, 1);
    xTaskCreatePinnedToCore(Task_WiFi_Reconnect, "wifi_reconnect", 2048, NULL, 10, &wifi_reconnect_handle, 1);

    xTaskCreatePinnedToCore(Task_MQTT_Start, "mqtt_start", 2048, NULL, 9, &mqtt_start_handle, 1);
    xTaskCreatePinnedToCore(Task_Pub_Temp, "pub_temp", 2048, NULL, 8, &pub_temp_handle, 1);
    xTaskCreatePinnedToCore(Task_Offline, "offline", 2048, NULL, 8, &offline_handle, 1);
    vTaskSuspend(pub_temp_handle);
    vTaskSuspend(wifi_reconnect_handle);
    

    vTaskDelete(NULL);
}


void Task_SHT31_Single_Shot()
{

    uint8_t data[6];
    uint16_t temp_int;
    uint16_t humi_int;

    static uint8_t single_shot[2] = {0x2c, 0x06};

    TickType_t pxPreviousWakeTime = xTaskGetTickCount();


    while (1)
    {

        i2c_master_transmit(i2c_dev_handle, single_shot, sizeof(single_shot), -1);

        vTaskDelay(pdMS_TO_TICKS(15));

        i2c_master_receive(i2c_dev_handle, data, sizeof(data), -1);

        uint16_t raw_temp = (data[0] << 8) | data[1];

        uint16_t raw_hum = (data[3] << 8) | data[4];

        temp = -45.0f + 175.0f * (raw_temp / 65535.0f);
        humi = 100.0f * (raw_hum / 65535.0f);

        temp_int = (uint16_t)(temp*10);
        humi_int = (uint16_t)(humi*10);
        

        ESP_LOGI("SHT31", "Temp: %0.2f℃  Hum: %0.2f%%", temp, humi);



        LCD_ShowIntNum(165, 0, temp_int/10, 2, 72, DISPLAY_MODE);
        LCD_ShowString(237, 0, ".", 72, DISPLAY_MODE);
        LCD_ShowIntNum(253, 0, temp_int%10, 1, 72, DISPLAY_MODE);
        LCD_ShowString(289, 0, "C", 72, DISPLAY_MODE);

        LCD_ShowIntNum(165, 62, humi_int/10, 2, 72, DISPLAY_MODE);
        LCD_ShowString(237, 62, ".", 72, DISPLAY_MODE);
        LCD_ShowIntNum(253, 62, humi_int%10, 1, 72, DISPLAY_MODE);
        LCD_ShowString(289, 62, "%", 72, DISPLAY_MODE);
        xTaskNotifyGive(pub_temp_handle);


        

        LCD_Refresh(&spi_dev_handle);
        
        vTaskDelayUntil(&pxPreviousWakeTime, 1000);

    }
    
}



void Task_WiFi_Con()
{
    while (1)
    {
        /* code */
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        LCD_ShowPicture(0, 136, 32, 32, wifi_logo1, 1);

    }
    
}

void Task_WiFi_Disc()
{

    while (1)
    {
        /* code */

        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        LCD_ShowPicture(0, 136, 32, 32, wifi_logo2, 1);

    }
    
}


void Task_WiFi_Reconnect()
{

    static int recon_cnt = 0;

    while (1)
    {
        /* code */
        if (recon_cnt == 15)
        {
            /* code */
            esp_wifi_connect();
            
            recon_cnt = 0;
        }
        
        recon_cnt++;



        vTaskDelay(1000);

    }
    
}



void Task_Pub_Temp()
{

    char sensor[60];
    while (1)
    {
        /* code */
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        sprintf(sensor, "{\"temp\" : %.2f, \"humi\" : %.2f}", temp, humi);
        LCD_ShowPicture(34, 136, 64, 32, pub, 1);
        esp_mqtt_client_publish(client, "Home/Sensor/Apartment/Temp&Humi", sensor, 0, 1, 0);

    }
    
}

void Task_Offline()
{


    while (1)
    {
        /* code */
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        LCD_ShowPicture(34, 136, 64, 32, offline, 1);
    }
    
}










void event_handler(void* arg, esp_event_base_t event_base,
    int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
    esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {

        xTaskNotifyGive(wifi_disc_handle);
        if (s_retry_num < 10) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG1, "retry to connect to the AP");
        } 
        else if (s_retry_num == 10)
        {
            /* code */
            vTaskResume(wifi_reconnect_handle);
        }
        
        ESP_LOGI(TAG1,"connect to the AP fail");

        

    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG1, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xTaskNotifyGive(wifi_con_handle);
        xTaskNotifyGive(mqtt_start_handle);

        vTaskSuspend(wifi_reconnect_handle);

    }
}


void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI("MQTT", "MQTT_EVENT_CONNECTED");
        vTaskResume(pub_temp_handle);

        // msg_id = esp_mqtt_client_subscribe(client, "Home/Sensor/Apartment/Humi", 1);
        // ESP_LOGI("MQTT", "sent subscribe successful, msg_id=%d", msg_id);

        // msg_id = esp_mqtt_client_subscribe(client, "/topic/qos1", 1);
        // ESP_LOGI("MQTT", "sent subscribe successful, msg_id=%d", msg_id);

        // msg_id = esp_mqtt_client_unsubscribe(client, "/topic/qos1");
        // ESP_LOGI("MQTT", "sent unsubscribe successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI("MQTT", "MQTT_EVENT_DISCONNECTED");
        xTaskNotifyGive(offline_handle);
        vTaskSuspend(pub_temp_handle);


        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI("MQTT", "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        // msg_id = esp_mqtt_client_publish(client, "Home/Sensor/Apartment/Humi", "data", 0, 1, 0);
        // ESP_LOGI("MQTT", "sent publish successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI("MQTT", "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        //ESP_LOGI("MQTT", "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI("MQTT", "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        break;
    // case MQTT_EVENT_ERROR:
    //     ESP_LOGI("MQTT", "MQTT_EVENT_ERROR");
    //     if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
    //         log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
    //         log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
    //         log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
    //         ESP_LOGI("MQTT", "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));

    //     }
    //     break;
    default:
        ESP_LOGI("MQTT", "Other event id:%d", event->event_id);
        break;
    }
}

void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = "mqtt://broker-cn.emqx.io",
    };


    client = esp_mqtt_client_init(&mqtt_cfg);
    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}

void Task_MQTT_Start()
{

    while (1)
    {
        /* code */
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        mqtt_app_start();

        vTaskDelete(NULL);

    }
    
}










