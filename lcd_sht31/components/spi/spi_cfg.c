#include "spi_cfg.h"





void spi_init(int mosi_pin, int sclk_pin, int max_transfer_sz)
{

    esp_err_t ret;

    gpio_config_t cfg = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = ((1ULL << PIN_NUM_DC) | (1ULL << PIN_NUM_RST) ),
        .pull_up_en = true,
    };
    gpio_config(&cfg);


    spi_bus_config_t bus_cfg = {
        .mosi_io_num = mosi_pin,
        .miso_io_num = -1,
        .sclk_io_num = sclk_pin,
        .quadhd_io_num = -1,
        .quadwp_io_num = -1,
        .max_transfer_sz = max_transfer_sz,
        
    };

    ret = spi_bus_initialize(SPI2_HOST, &bus_cfg, SPI_DMA_CH_AUTO);
    if (ret == ESP_OK)
    {
        /* code */
        ESP_LOGI("SPI", "Init is finished");
    }

}


void spi_dev_init(int cs_pin, int sclk_speed, spi_device_handle_t *dev_handle)
{

    esp_err_t ret;

    spi_device_interface_config_t dev_cfg = {
        .mode = 3,
        .clock_speed_hz = sclk_speed,
        .spics_io_num = cs_pin,
        .queue_size = 7,
        .pre_cb = spi_pre_transfer_callback,
        .clock_source = SPI_CLK_SRC_DEFAULT,

    };

    ret = spi_bus_add_device(SPI2_HOST, &dev_cfg, dev_handle);
    if (ret == ESP_OK)
    {
        /* code */
        ESP_LOGI("SPI_Dev", "Add device is done");
    }  

}

void spi_pre_transfer_callback(spi_transaction_t *t)
{
    int dc = (int)t->user;
    gpio_set_level(PIN_NUM_DC, dc);
}


void spi_cmd(spi_device_handle_t *spi_dev_handle, const uint8_t cmd)
{
    esp_err_t ret;

    spi_transaction_t t;
    memset(&t, 0, sizeof(t));       //Zero out the transaction

    t.length = 8;                   //Command is 8 bits
    t.tx_buffer = &cmd;             //The data is the cmd itself
    t.user = (void*)0;              //D/C needs to be set to 0
    ret = spi_device_polling_transmit(*spi_dev_handle, &t); //Transmit!
    assert(ret == ESP_OK);          //Should have had no issues.
}

void spi_data(spi_device_handle_t *spi_dev_handle, const uint8_t data)
{
    esp_err_t ret;

    spi_transaction_t t;
    memset(&t, 0, sizeof(t));       //Zero out the transaction

    t.length = 8;             //Len is in bytes, transaction length is in bits.
    t.tx_buffer = &data;             //Data
    t.user = (void*)1;              //D/C needs to be set to 1
    ret = spi_device_polling_transmit(*spi_dev_handle, &t); //Transmit!
    assert(ret == ESP_OK);          //Should have had no issues.
}


void spi_lcd_init(spi_device_handle_t *spi_device_handle_t)
{

    //Reset the display
    gpio_set_level(PIN_NUM_RST, 0);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    gpio_set_level(PIN_NUM_RST, 1);
    vTaskDelay(100 / portTICK_PERIOD_MS);


    spi_cmd(spi_device_handle_t, 0xD6); //NVM Load Control 
    spi_data(spi_device_handle_t, 0X13); 
    spi_data(spi_device_handle_t, 0X02);

    spi_cmd(spi_device_handle_t, 0xD1); //Booster Enable 
    spi_data(spi_device_handle_t, 0X01); 

    spi_cmd(spi_device_handle_t, 0xC0); //Gate Voltage Setting 
    spi_data(spi_device_handle_t, 0X12); //VGH 00:8V  04:10V  08:12V   0E:15V   12:17V
    spi_data(spi_device_handle_t, 0X0A); //VGL 00:-5V   04:-7V   0A:-10V

    // VLC=3.6V (12/-5)(delta Vp=0.6V)		
    spi_cmd(spi_device_handle_t, 0xC1); //VSHP Setting (4.8V)	
    spi_data(spi_device_handle_t, 0X3C); //VSHP1 	
    spi_data(spi_device_handle_t, 0X3E); //VSHP2 	
    spi_data(spi_device_handle_t, 0X3C); //VSHP3 	
    spi_data(spi_device_handle_t, 0X3C); //VSHP4	

    spi_cmd(spi_device_handle_t, 0xC2); //VSLP Setting (0.98V)	
    spi_data(spi_device_handle_t, 0X23); //VSLP1 	
    spi_data(spi_device_handle_t, 0X21); //VSLP2 	
    spi_data(spi_device_handle_t, 0X23); //VSLP3 	
    spi_data(spi_device_handle_t, 0X23); //VSLP4 	

    spi_cmd(spi_device_handle_t, 0xC4); //VSHN Setting (-3.6V)	
    spi_data(spi_device_handle_t, 0X5A); //VSHN1	
    spi_data(spi_device_handle_t, 0X5C); //VSHN2 	
    spi_data(spi_device_handle_t, 0X5A); //VSHN3 	
    spi_data(spi_device_handle_t, 0X5A); //VSHN4 	

    spi_cmd(spi_device_handle_t, 0xC5); //VSLN Setting (0.22V)	
    spi_data(spi_device_handle_t, 0X37); //VSLN1 	
    spi_data(spi_device_handle_t, 0X35); //VSLN2 	
    spi_data(spi_device_handle_t, 0X37); //VSLN3 	
    spi_data(spi_device_handle_t, 0X37); //VSLN4

    spi_cmd(spi_device_handle_t, 0xD8); //OSC Setting                                                                                                                                                                              
    spi_data(spi_device_handle_t, 0XA6); //Enable OSC, HPM Frame Rate Max = 32hZ
    spi_data(spi_device_handle_t, 0XE9); 

    /*-- HPM=32hz ; LPM=> 0x15=8Hz 0x14=4Hz 0x13=2Hz 0x12=1Hz 0x11=0.5Hz 0x10=0.25Hz---*/
    spi_cmd(spi_device_handle_t, 0xB2); //Frame Rate Control 
    spi_data(spi_device_handle_t, 0X12); //HPM=32hz ; LPM=1hz 

    spi_cmd(spi_device_handle_t, 0xB3); //Update Period Gate EQ Control in HPM 
    spi_data(spi_device_handle_t, 0XE5); 
    spi_data(spi_device_handle_t, 0XF6); 
    spi_data(spi_device_handle_t, 0X17);
    spi_data(spi_device_handle_t, 0X77); 
    spi_data(spi_device_handle_t, 0X77); 
    spi_data(spi_device_handle_t, 0X77); 
    spi_data(spi_device_handle_t, 0X77); 
    spi_data(spi_device_handle_t, 0X77); 
    spi_data(spi_device_handle_t, 0X77); 
    spi_data(spi_device_handle_t, 0X71); 

    spi_cmd(spi_device_handle_t, 0xB4); //Update Period Gate EQ Control in LPM 
    spi_data(spi_device_handle_t, 0X05); //LPM EQ Control 
    spi_data(spi_device_handle_t, 0X46); 
    spi_data(spi_device_handle_t, 0X77); 
    spi_data(spi_device_handle_t, 0X77); 
    spi_data(spi_device_handle_t, 0X77); 
    spi_data(spi_device_handle_t, 0X77); 
    spi_data(spi_device_handle_t, 0X76); 
    spi_data(spi_device_handle_t, 0X45); 

    spi_cmd(spi_device_handle_t, 0x62); //Gate Timing Control
    spi_data(spi_device_handle_t, 0X32);
    spi_data(spi_device_handle_t, 0X03);
    spi_data(spi_device_handle_t, 0X1F);

    spi_cmd(spi_device_handle_t, 0xB7); //Source EQ Enable 
    spi_data(spi_device_handle_t, 0X13); 

    spi_cmd(spi_device_handle_t, 0xB0); //Gate Line Setting 
    spi_data(spi_device_handle_t, 0X60); //384 line 

    spi_cmd(spi_device_handle_t, 0x11); //Sleep out 
    vTaskDelay(10 / portTICK_PERIOD_MS);

    spi_cmd(spi_device_handle_t, 0xC9); //Source Voltage Select  
    spi_data(spi_device_handle_t, 0X00); //VSHP1; VSLP1 ; VSHN1 ; VSLN1

    spi_cmd(spi_device_handle_t, 0x36); //Memory Data Access Control
    // spi_data(spi_device_handle_t, 0X00); //Memory Data Access Control: MX=0 ; DO=0 
    //spi_data(spi_device_handle_t, 0X48); //MX=1 ; DO=1 
    spi_data(spi_device_handle_t, 0X00); //MX=1 ; DO=1 GS=1

    spi_cmd(spi_device_handle_t, 0x3A); //Data Format Select 
    spi_data(spi_device_handle_t, 0X11); //10:4write for 24bit ; 11: 3write for 24bit

    spi_cmd(spi_device_handle_t, 0xB9); //Gamma Mode Setting 
    spi_data(spi_device_handle_t, 0X20); //20: Mono 00:4GS  

    spi_cmd(spi_device_handle_t, 0xB8); //Panel Setting 
    spi_data(spi_device_handle_t, 0x29); // Panel Setting: 0x29: 1-Dot inversion, Frame inversion, One Line Interlace

    //WRITE RAM 168*384
    spi_cmd(spi_device_handle_t, 0x2A); //Column Address Setting 
    spi_data(spi_device_handle_t, 0X17); 
    spi_data(spi_device_handle_t, 0X24); // 0X24-0X17=14 // 14*12=168

    spi_cmd(spi_device_handle_t, 0x2B); //Row Address Setting 
    spi_data(spi_device_handle_t, 0X00); 
    spi_data(spi_device_handle_t, 0XBF); // 192*2=384
    /*
    spi_cmd(spi_device_handle_t, 0x72); //de-stress off 
    spi_data(spi_device_handle_t, 0X13);
    */
    spi_cmd(spi_device_handle_t, 0x35); //TE
    spi_data(spi_device_handle_t, 0X00); //

    spi_cmd(spi_device_handle_t, 0xD0); //Auto power dowb OFF
    // spi_data(spi_device_handle_t, 0X7F); //Auto power dowb OFF
    spi_data(spi_device_handle_t, 0XFF); //Auto power dowb ON


    spi_cmd(spi_device_handle_t, 0x39); //LPM:Low Power Mode ON
    // spi_cmd(spi_device_handle_t, 0x38); //HPM:high Power Mode ON


    spi_cmd(spi_device_handle_t, 0x29); //DISPLAY ON  
    // spi_cmd(spi_device_handle_t, 0x28); //DISPLAY OFF  

    // spi_cmd(spi_device_handle_t, 0x21); //Display Inversion On 
    spi_cmd(spi_device_handle_t, 0x20); //Display Inversion Off 

    spi_cmd(spi_device_handle_t, 0xBB); // Enable Clear RAM
    spi_data(spi_device_handle_t, 0x4F); // CLR=0 ; Enable Clear RAM,clear RAM to 0


}



uint8_t LCD_GRAM[8064];

uint32_t LCD_pow(uint8_t m,uint8_t n)
{
	uint32_t result=1;
	while(n--) result*=m;
	return result;
}

void LCD_Refresh(spi_device_handle_t *spi_dev_handle)
{
	uint16_t i,j,k;
	if(USE_HORIZONTAL==0)
	{
        spi_cmd(spi_dev_handle,  0x2A);
        spi_data(spi_dev_handle, 0x17);
        spi_data(spi_dev_handle, 0x24);
        spi_cmd(spi_dev_handle,  0x2B);
        spi_data(spi_dev_handle, 0x00);
        spi_data(spi_dev_handle, 0xBF);
        spi_cmd(spi_dev_handle,  0x2C);

	}
	else if(USE_HORIZONTAL==1)
	{
        // spi_cmd(spi_dev_handle, 0x2A);
        // spi_data(spi_dev_handle, 0x19);
        // spi_data(spi_dev_handle, 0x23);
        // spi_cmd(spi_dev_handle,  0x2B);
        // spi_data(spi_dev_handle, 0x4B);
        // spi_data(spi_dev_handle, 0xC7);
        // spi_cmd(spi_dev_handle,  0x2C);		
	}
	for(i=0;i<14*3;i++)
	{
		for(j=0;j<192;j++)
		{
			 spi_data(spi_dev_handle, LCD_GRAM[k]);
			 k++;
		 }
		}
}

void LCD_Fill(spi_device_handle_t *spi_dev_handle, uint8_t dat)
{
	uint16_t i,j;
	uint32_t k;
	for(i=0;i<14*3;i++)
	{
		for(j=0;j<192;j++)       //384/2=192
		{
			k=j+i*192;
			LCD_GRAM[k]=dat;
		}
	}
	LCD_Refresh(spi_dev_handle);
}


void LCD_DrawPoint(uint16_t x,uint16_t y,uint8_t mode)
{
	uint16_t x1,y1,n,y2;
	x1=x/2;
	y1=y/4;
	
	y2=y-y1*4;
	n=LCD_pow(4,(3-y2));
	if(x-x1*2==0)n*=2;
	if(mode==0)
	{
	    LCD_GRAM[x1*0x2A+y1]|=n;
	}
	else
	{
        LCD_GRAM[x1*0x2A+y1]|=n;
        LCD_GRAM[x1*0x2A+y1]=~LCD_GRAM[x1*0x2A+y1];
        LCD_GRAM[x1*0x2A+y1]|=n;
        LCD_GRAM[x1*0x2A+y1]=~LCD_GRAM[x1*0x2A+y1];		
	}
}


void LCD_ShowChar(uint16_t x,uint16_t y,uint8_t num,uint8_t sizey,uint8_t mode)
{
	uint8_t temp,sizex,t;
	uint16_t i,TypefaceNum;
	uint16_t x0=x;
	sizex=sizey/2;
	TypefaceNum=(sizex/8+((sizex%8)?1:0))*sizey;
	num=num-' ';    
	for(i=0;i<TypefaceNum;i++)
	{ 
		if(sizey==12)temp=ascii_1206[num][i];		       
		else if(sizey==24)temp=ascii_2412[num][i];		 
		else if(sizey==32)temp=ascii_3216[num][i];		 
		else return;
		for(t=0;t<8;t++)
		{
				if(temp&(0x01<<t))LCD_DrawPoint(x,y,mode);
				else LCD_DrawPoint(x,y,!mode);
				x++;
				if((x-x0)==sizex)
				{
					x=x0;
					y++;
					break;
				}
		}
	}   	 	  
}

void LCD_ShowString(uint16_t x,uint16_t y, const char *p, uint8_t sizey, uint8_t mode)
{         
	while(*p!='\0')
	{       
		LCD_ShowChar(x,y,*p,sizey,mode);
		x+=sizey/2;
		p++;
	}  
}



void LCD_ShowFloatNum1(uint16_t x, uint16_t y,  float num, uint8_t len, uint8_t sizey, uint8_t pre, uint8_t mode)
{         	
	uint8_t t,temp,sizex;
	uint16_t num1;
	sizex=sizey/2;
	num1=num*LCD_pow(10,pre);
	for(t=0;t<len;t++)
	{
		temp=(num1/LCD_pow(10,len-t-1))%10;
		if(t==(len-pre))
		{
			LCD_ShowChar(x+(len-pre)*sizex,y,'.',sizey,mode);
			t++;
			len+=1;
		}
	 	LCD_ShowChar(x+t*sizex,y,temp+48,sizey,mode);
	}
}







































