
extern void LCD_UPDATE(unsigned char *bfr);

//////////////////////////////////////////////////////////////////////////////////
#include "Driver_I2C.h"
#define I2C_DRVIER			 Driver_I2C1 //PLEASE CHANGE HERE FOR USING DIFRENT
////////////////////////////////////////////////////////
extern ARM_DRIVER_I2C 		 I2C_DRVIER;
static ARM_DRIVER_I2C 		*drv;
static int isInited=0;
/////////////////////////////////////////////////////////
static char Lcd_ram[16];
#define i2c_init					unsigned int i 
#define i2c_start					i=0
#define i2c_write(byte)			Lcd_ram[i++]=byte
#define i2c_stop(flg)			drv_write(Lcd_ram,i,flg)

static void drv_write(void *data,unsigned int size,bool continue_req){
  drv->MasterTransmit(0X78>>1,data,size,continue_req);	
  while(drv->GetStatus().busy);  //wait for last transfer
}

/////////////////////////////////////////////////////////////////////////////////
static void LCD_Init(void) {
	i2c_init;
	drv=&I2C_DRVIER;
	drv->Initialize(NULL);
	drv->PowerControl(ARM_POWER_FULL);	
	drv->Control(ARM_I2C_BUS_SPEED,ARM_I2C_BUS_SPEED_FAST);
	
	//delay 100ms
/*
	i2c_start;
	i2c_write(0x80);
	i2c_write(0xae);
	i2c_stop(true);

	i2c_start;
	i2c_write(0);
   i2c_write(0x20);
	i2c_write(0);
	i2c_stop(true);

	i2c_start;
	i2c_write(0);
	i2c_write(0x21);
	i2c_write(0);
	i2c_write(127);
	i2c_stop(true);

	i2c_start;
	i2c_write(0);
	i2c_write(0x22);
	i2c_write(0);
	i2c_write(7);  //3 for 128*32
	i2c_stop(true);

	i2c_start;
	i2c_write(0x80);
	i2c_write(0xb0);
	i2c_stop(true);

	i2c_start;
	i2c_write(0x80);
	i2c_write(0xc0); //this one also needed to change for oled 128*64
	i2c_stop(true);

	i2c_start;
	i2c_write(0x80);
	i2c_write(0x0);
	i2c_stop(true);

	i2c_start;
	i2c_write(0x80);
	i2c_write(0x10);
	i2c_stop(true);

	i2c_start;
	i2c_write(0x80);
	i2c_write(0x7f);
	i2c_stop(true);


	i2c_start;
	i2c_write(0x80);
	i2c_write(0xa1);
	i2c_stop(true);

	i2c_start;
	i2c_write(0x80);
	i2c_write(0xa6);
	i2c_stop(true);

	i2c_start;
	i2c_write(0x80);
	i2c_write(0xa4);
	i2c_stop(true);
///////////////////////////////////////////////////////
	i2c_start;
	i2c_write(0);
	i2c_write(0xd3);
	i2c_write(0x0);
	i2c_stop(true);

	i2c_start;
	i2c_write(0);
	i2c_write(0xa8);
	i2c_write(64);
	i2c_stop(true);

	i2c_start;
	i2c_write(0);
	i2c_write(0xd5);
	i2c_write(0xf0);
	i2c_stop(true);

	i2c_start;
	i2c_write(0);
	i2c_write(0xd9);
	i2c_write(0xf1);
	i2c_stop(true);

	i2c_start;
	i2c_write(0);
	i2c_write(0xda);
	i2c_write(0x12); //0x22 for 128*32 0x12)
	i2c_stop(true);

	i2c_start;
	i2c_write(0);
	i2c_write(0xdb);
	i2c_write(0x40);
	i2c_stop(true);


	i2c_start;
	i2c_write(0);
	i2c_write(0x8d);
	i2c_write(0x14);
	i2c_stop(true);

	i2c_start;
	i2c_write(0x80);
	i2c_write(0xaf);
	i2c_stop(true);

	i2c_start;
	i2c_write(0);
	i2c_write(0x81);
	i2c_write(0xff);
	i2c_stop(false);
	*/
	isInited=1;
	/////
	//new config
	i2c_start;
	i2c_write(0x80);
	i2c_write(0xae);
	i2c_stop(true);	

	i2c_start;
	i2c_write(0);
   i2c_write(0x20);
	i2c_write(0);
	i2c_stop(true);


	i2c_start;
	i2c_write(0x80);
	i2c_write(0xb0);
	i2c_stop(true);	


	i2c_start;
	i2c_write(0x80);
	i2c_write(0xc0); //c8 mirror vertically
	i2c_stop(true);	

	i2c_start;
	i2c_write(0x80);
	i2c_write(0x00);
	i2c_stop(true);	

	i2c_start;
	i2c_write(0x80);
	i2c_write(0x10);
	i2c_stop(true);	

	i2c_start;
	i2c_write(0x80);
	i2c_write(0x40);
	i2c_stop(true);	

	i2c_start;
	i2c_write(0);
	i2c_write(0x81);
	i2c_write(0xff);
	i2c_stop(false);
	
	i2c_start;
	i2c_write(0x80);
	i2c_write(0xa1); //0xa0 mirror horz
	i2c_stop(true);
	

	i2c_start;
	i2c_write(0x80);
	i2c_write(0xa6); // 0xa7 inverse colors
	i2c_stop(true);
	
	i2c_start;
	i2c_write(0x80);
	i2c_write(0xa8);
	i2c_stop(true);
	
	i2c_start;
	i2c_write(0x80);
	i2c_write(0x3f);
	i2c_stop(true);

	i2c_start;
	i2c_write(0x80);
	i2c_write(0xa4);
	i2c_stop(true);
	
	i2c_start;
	i2c_write(0x0);
	i2c_write(0xd3);
	i2c_write(0);
	i2c_stop(true);
	
	i2c_start;
	i2c_write(0x0);
	i2c_write(0xd5);
	i2c_write(0xf0);
	i2c_stop(true);
	
	i2c_start;
	i2c_write(0x0);
	i2c_write(0xd9);
	i2c_write(0x22);
	i2c_stop(true);

	i2c_start;
	i2c_write(0x0);
	i2c_write(0xda);
	i2c_write(0x12);
	i2c_stop(true);
	
	i2c_start;
	i2c_write(0x0);
	i2c_write(0xdb);
	i2c_write(0x20);
	i2c_stop(true);

	i2c_start;
	i2c_write(0x0);
	i2c_write(0x8d);
	i2c_write(0x14);
	i2c_stop(true);
	
	i2c_start;
	i2c_write(0x80);
	i2c_write(0xaf);
	i2c_stop(true);
	/////
}

//////////////////////////////////////////////////////////

void LCD_UPDATE(uint8_t *bfr){
	if(isInited==0) LCD_Init();
	drv_write(bfr,1025,false);
}