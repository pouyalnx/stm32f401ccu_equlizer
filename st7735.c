#include "stm32f4xx.h"
#include "cmsis_os2.h"
#include "Driver_SPI.h"
extern ARM_DRIVER_SPI Driver_SPI1;
static ARM_DRIVER_SPI *drv;

#define CS_SET		GPIOB->BSRR=GPIO_BSRR_BS2
#define CS_CLR		GPIOB->BSRR=GPIO_BSRR_BR2

#define RST_SET	GPIOB->BSRR=GPIO_BSRR_BS1
#define RST_CLR	GPIOB->BSRR=GPIO_BSRR_BR1		  
		  
#define DC_SET		GPIOB->BSRR=GPIO_BSRR_BS0
#define DC_CLR		GPIOB->BSRR=GPIO_BSRR_BR0



////////////////////////////////////////////////
static char buff[16];
#define bus_init 				unsigned int ptr=0
#define bus_start				ptr=0
#define bus_write(byte)		buff[ptr++]=byte
#define bus_end_cmd			lcdCmd(buff,ptr)
#define bus_end_data		   lcdData(buff,ptr)

////////////////////////////////////////////////
static void lcdData(char *data,unsigned int len){
	DC_SET;
	drv->Send(data,len);
	while(drv->GetStatus().busy);	
}

static void lcdCmd(char *cmd,unsigned int len){
	DC_CLR;
	drv->Send(cmd,len);
	while(drv->GetStatus().busy);		
}
//////////////////////////////////////////////////////////
void lcdWrite(char *data,unsigned int size){
	bus_init; 
	bus_start;
	bus_write(0x2c);
	bus_end_cmd;
	lcdData(data,size);
}

void lcdSelectBlock(int x,int xsize,int y,int ysize){
	bus_init; 
	bus_start;
	bus_write(0x2a);	
	bus_end_cmd;
	bus_start;
	bus_write(0);
	bus_write(x&0xff);
	bus_write(0);
	bus_write((x+xsize)&0xff);
	bus_end_data;

	bus_start;
	bus_write(0x2b);	
	bus_end_cmd;
	bus_start;
	bus_write(0);
	bus_write(y&0xff);
	bus_write(0);
	bus_write((y+ysize)&0xff);
	bus_end_data;
}

void lcdInit(void){	
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOBEN;
	drv=&Driver_SPI1;
	
	drv->Initialize(NULL);
	drv->PowerControl(ARM_POWER_FULL);
	drv->Control(ARM_SPI_MODE_MASTER|ARM_SPI_DATA_BITS(8),8000000);

	GPIOB->MODER&=~(GPIO_MODER_MODE2);
	GPIOB->MODER|=GPIO_MODER_MODE2_0;

	GPIOB->MODER&=~(GPIO_MODER_MODE1);
	GPIOB->MODER|=GPIO_MODER_MODE1_0;
	
	GPIOB->MODER&=~(GPIO_MODER_MODE0);
	GPIOB->MODER|=GPIO_MODER_MODE0_0;


	CS_CLR;
	osDelay(50);	
	RST_CLR;
	osDelay(50);
	RST_SET;
	osDelay(50);
	
	bus_init; 

	bus_start;
	bus_write(0x1);	
	bus_end_cmd;
	osDelay(10);
	bus_start;
	bus_write(0x11);	
	bus_end_cmd;
	osDelay(10);


	bus_start;
	bus_write(0xb1);	
	bus_end_cmd;
	bus_start;
	bus_write(1);
	bus_write(0x2c);
	bus_write(0x2d);
	bus_end_data;

	bus_start;
	bus_write(0xb2);	
	bus_end_cmd;
	bus_start;
	bus_write(1);
	bus_write(0x2c);
	bus_write(0x2d);
	bus_end_data;


	bus_start;
	bus_write(0xb3);	
	bus_end_cmd;
	bus_start;
	bus_write(1);
	bus_write(0x2c);
	bus_write(0x2d);
	bus_write(1);
	bus_write(0x2c);
	bus_write(0x2d);
	bus_end_data;

	bus_start;
	bus_write(0xb4);	
	bus_end_cmd;
	bus_start;
	bus_write(0x7);
	bus_end_data;


	bus_start;
	bus_write(0xc0);	
	bus_end_cmd;
	bus_start;
	bus_write(0xa2);
	bus_write(0x2);
	bus_write(0x84);
	bus_end_data;


	bus_start;
	bus_write(0xc1);	
	bus_end_cmd;
	bus_start;
	bus_write(0xc5);
	bus_end_data;

	bus_start;
	bus_write(0xc2);	
	bus_end_cmd;
	bus_start;
	bus_write(0xa);
	bus_write(0x0);
	bus_end_data;
	
	bus_start;
	bus_write(0xc3);	
	bus_end_cmd;
	bus_start;
	bus_write(0x8a);
	bus_write(0x2a);
	bus_end_data;	

	bus_start;
	bus_write(0xc4);	
	bus_end_cmd;
	bus_start;
	bus_write(0x8a);
	bus_write(0xee);
	bus_end_data;	

	bus_start;
	bus_write(0xc1);	
	bus_end_cmd;
	bus_start;
	bus_write(0xe);
	bus_end_data;

	bus_start;
	bus_write(0x20);	
	bus_end_cmd;

	bus_start;
	bus_write(0x36);	
	bus_end_cmd;
	bus_start;
	bus_write(0x8);
	bus_end_data;


	bus_start;
	bus_write(0x3a);	
	bus_end_cmd;
	bus_start;
	bus_write(0x5);
	bus_end_data;

	bus_start;
	bus_write(0x0);	
	bus_end_cmd;


	bus_start;
	bus_write(0x2a);	
	bus_end_cmd;
	bus_start;
	bus_write(0);
	bus_write(0);
	bus_write(0);
	bus_write(127);
	bus_end_data;

	bus_start;
	bus_write(0x2b);	
	bus_end_cmd;
	bus_start;
	bus_write(0);
	bus_write(0);
	bus_write(0);
	bus_write(127);
	bus_end_data;

	bus_start;
	bus_write(0x0);	
	bus_end_cmd;
	
	bus_start;
	bus_write(0xe0);	
	bus_end_cmd;
	bus_start;
	bus_write(0x2);
	bus_write(0x1c);
	bus_write(0x7);
	bus_write(0x12);
	bus_write(0x37);
	bus_write(0x32);
	bus_write(0x29);
	bus_write(0x2d);	
	bus_write(0x29);
	bus_write(0x25);
	bus_write(0x2b);
	bus_write(0x39);
	bus_write(0x0);
	bus_write(0x1);
	bus_write(0x3);
	bus_write(0x10);
	bus_end_data;

	bus_start;
	bus_write(0x0);	
	bus_end_cmd;

	bus_start;
	bus_write(0xe0);	
	bus_end_cmd;
	bus_start;
	bus_write(0x3);
	bus_write(0x1d);
	bus_write(0x7);
	bus_write(0x6);
	bus_write(0x2e);
	bus_write(0x2c);
	bus_write(0x29);
	bus_write(0x2d);
	bus_write(0x2e);
	bus_write(0x3e);
	bus_write(0x37);
	bus_write(0x3f);
	bus_write(0x0);
	bus_write(0x0);
	bus_write(0x2);
	bus_write(0x10);
	bus_end_data;

	bus_start;
	bus_write(0x0);	
	bus_end_cmd;

	bus_start;
	bus_write(0x29);	
	bus_end_cmd;
	osDelay(10);
	bus_start;
	bus_write(0x13);	
	bus_end_cmd;
	osDelay(10);
}
