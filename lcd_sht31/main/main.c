#include "main.h"



/*bsp init handle*/
i2c_master_bus_handle_t i2c_bus_handle;
i2c_master_dev_handle_t i2c_dev_handle;

spi_device_handle_t spi_dev_handle;




/*task handle*/
TaskHandle_t task_start_handle;

TaskHandle_t single_shot_handle;
TaskHandle_t find_sht31_handle;
TaskHandle_t lcd_show_sht31_handle;


 
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

    spi_dev_init(PIN_NUM_CS, 15000000, &spi_dev_handle);

    spi_lcd_init(&spi_dev_handle);

    LCD_Refresh(&spi_dev_handle);

    LCD_Fill(&spi_dev_handle, BACK_COLOR_WHITE);

    LCD_ShowString(0, 0, "Temp:", 32, 0);

    LCD_ShowString(0, 34, "Humi:", 32, 0);

    LCD_Refresh(&spi_dev_handle);







}


void freeRTOS_start()
{

    xTaskCreatePinnedToCore(Task_Start, "Task_Start", 3072, NULL, 12, &task_start_handle, 1);

}


void Task_Start()
{

    xTaskCreatePinnedToCore(Task_SHT31_Single_Shot, "SHT31_Single_Shot", 3072, NULL, 12, &single_shot_handle, 1);
    
    
    xTaskCreatePinnedToCore(Task_LCD_Show_SHT31, "LCD_show_SHT31", 3072, NULL, 11, &lcd_show_sht31_handle, 1);

    vTaskDelete(NULL);
}


void Task_SHT31_Single_Shot()
{

    uint8_t data[6];

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

        ESP_LOGI("SHT31", "Temp: %0.2f℃  Hum: %0.2f%%", temp, humi);

        xTaskNotifyGive(lcd_show_sht31_handle);
        
        vTaskDelayUntil(&pxPreviousWakeTime, 1000);

    }
    
}


void Task_LCD_Show_SHT31()
{

    while (1)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        LCD_ShowFloatNum1(84, 0, temp, 4, 32, 2, 0);

        LCD_ShowFloatNum1(84, 34, humi, 4, 32, 2, 0);
        LCD_ShowString(170, 34, "%", 32, 0);

        LCD_Refresh(&spi_dev_handle);

    }

}




















