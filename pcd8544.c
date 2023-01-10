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
char buff[16];
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
	bus_write(0x20);
	bus_write(0xc);
	bus_write(0x40+(char)(y%6));
	bus_write(0x80+(char)(x%84));
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
	bus_write(0x21);
	bus_write(0x5);
	bus_write(0x13);
	bus_write(0xc2);
	bus_end;
	
	bus_write(0x20);
	bus_write(0xc);
	bus_write(0x40);
	bus_write(0x80);
	bus_end;

	bus_write(0x20);
	bus_write(0xc);
	bus_write(0x44);
	bus_write(0x80);
	bus_end;
}
