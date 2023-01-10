#include "stm32f4xx.h"
#include "cmsis_os2.h"
#include <stdio.h>
#include "Driver_SPI.h"
#define LEN 4
extern ARM_DRIVER_SPI Driver_SPI3,Driver_SPI1;

osThreadId_t tid;
////////////////////////////////////////////////////////////////
char buff_A[LEN]="saba",buff_B[LEN];
int lenA,lenB;

void liteSend(void *arg){
	ARM_DRIVER_SPI *drv=(ARM_DRIVER_SPI*)arg;
	drv->Initialize(NULL);
	drv->PowerControl(ARM_POWER_FULL);
	drv->Control(ARM_SPI_MODE_MASTER|ARM_SPI_DATA_BITS(8),1000000);
	lenA=0;
	while(1){
		drv->Send(buff_A,LEN);
		osDelay(1000);
		lenA+=drv->GetDataCount();
	}
}


void liteReceive(void *arg){
	ARM_DRIVER_SPI *drv=(ARM_DRIVER_SPI*)arg;
	drv->Initialize(NULL);
	drv->PowerControl(ARM_POWER_FULL);
	drv->Control(ARM_SPI_MODE_SLAVE|ARM_SPI_DATA_BITS(8)|ARM_SPI_SS_SLAVE_SW,0);
	drv->Control(ARM_SPI_CONTROL_SS,ARM_SPI_SS_ACTIVE);
	lenB=0;
	while(1){
		drv->Receive(buff_B,LEN);
		osDelay(1000);
		lenB+=drv->GetDataCount();		
	}
}


//////////////////////////////////////////////////////////////

static unsigned int sec,min,hour,tick;
void clock(void *arg){
		tick++;
		sec++;
		if(sec==60){
			sec=0;
			min++;
			if(min==60){
				hour++;
				if(hour==24)
					hour=0;
			}
		}
	osThreadFlagsSet(tid,1);
}	



extern void stdout_goxy(int x,int y);
extern void stdout_write(char *data);
extern int stdout_putchar(int ch);


void oled(void){
	unsigned int inner_tick;
	osDelay(100);
	stdout_goxy(0,0);
	printf("\n\n\n\n");
	stdout_goxy(0,0);
	printf("Lnx Embroidery\n");
	inner_tick=1;
	tick=0;
	min=0;
	hour=0;
	sec=0;
	osTimerStart(osTimerNew(clock,osTimerPeriodic,NULL,NULL),1000);
	osThreadNew(liteReceive,&Driver_SPI3,NULL);
	osThreadNew(liteSend,&Driver_SPI1,NULL);

	while(1){
		
		if(inner_tick!=tick){
			stdout_goxy(1,0);
			printf("%.2d:%.2d:%.2d\n",hour,min,sec);
			inner_tick=tick;
			stdout_goxy(2,0);
			printf(">> %2d\n",lenA);
			stdout_goxy(3,0);
			printf("<< %2d %c%c%c%c\n",lenB,buff_B[0],buff_B[1],buff_B[2],buff_B[3]);
		}
		osThreadFlagsWait(1,osFlagsWaitAny,2000);
		osThreadFlagsClear(1);
	}
}



void led(void *arg){
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOCEN;
	GPIOC->MODER|=GPIO_MODER_MODE13_0;
	GPIOC->OTYPER&=~(GPIO_OTYPER_OT13);
	GPIOC->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR13_1;
	GPIOC->PUPDR|=GPIO_PUPDR_PUPD13_0;

	while(1){
		GPIOC->ODR=1<<13;
		osDelay(70);
		GPIOC->ODR=1<<29;
		osDelay(50);
	}
}



void main_app(void *arg){
	osThreadNew(led,NULL,NULL);
	tid=osThreadGetId();
	oled();
	osDelay(osWaitForever);
}
