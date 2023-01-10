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
char buff[64];
#define bus_init 				int ptr=0
#define bus_start				ptr=0
#define bus_write(byte)		buff[ptr++]=byte
#define bus_end				lcdCmd(buff,ptr)


////////////////////////////////////////////////
void lcdData(char *data,int len){
	DC_SET;
	drv->Send(data,len);
	while(drv->GetStatus().busy);	
}

void lcdCmd(char *cmd,int len){
	DC_CLR;
	drv->Send(cmd,len);
	while(drv->GetStatus().busy);		
}



void lcdGoXY(int x,int y){
	bus_init; 
	bus_start;	
	bus_write(0xb0|(y&0xf));
	bus_write(x&0xf);
	bus_write(0x10|((x>>4)&0xf));
	bus_end;
}

void lcdInit(void){	
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOBEN;
	drv=&Driver_SPI1;
	
	drv->Initialize(NULL);
	drv->PowerControl(ARM_POWER_FULL);
	drv->Control(ARM_SPI_MODE_MASTER|ARM_SPI_DATA_BITS(8),4000000);

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
	bus_write(0xAE);
	bus_write(0xd5);
	bus_write(0x80);
	bus_write(0xa8);
	bus_write(0x3f);
	bus_write(0x00);
	bus_write(0x40);
	bus_write(0x8d);
	bus_write(0x14);
	bus_write(0x20);
	bus_write(0x01);
	bus_write(0xa6);
	bus_write(0xa4);
	bus_write(0xa1);
	bus_write(0xc8);
	bus_write(0xda);
	bus_write(0x22);
	bus_write(0x81);
	bus_write(0xff);
	bus_write(0xd9);
	bus_write(0xF1);
	bus_write(0xdb);
	bus_write(0x40);
	bus_write(0x22);
	bus_write(0);
	bus_write(127);
	bus_write(0x22);
	bus_write(0);
	bus_write(0x7);
	bus_write(0x20);
	bus_write(0x01);
	bus_write(0xaf);
	osDelay(100);
	bus_end;
	bus_start;
	bus_write(0x81);
	bus_write(0xff);
	bus_write(0x22);
	bus_end;
}
