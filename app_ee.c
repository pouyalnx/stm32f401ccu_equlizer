#include "stm32f4xx.h"
#include "cmsis_os2.h"
#include <stdio.h>


const char msg[]="LnxEmbroidery10/26/2022,Day40#mahsaamini,EntireIranFightForFreedom,ImpoyaIWorkOnForexAndTextile,PriceOfBtcAbout20k$,ethAbout1k$,IlikeToKnowhowSaba,IhateImagineItKillLife,BestWayForMakeBetterOfUrSelfDetectingErrorAndSolve,1401Revolotion";

osThreadId_t tid;

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


extern void lcdWrite(char *data,unsigned int size);
extern void lcdInit(void);
extern void lcdSelectBlock(int x,int xsize,int y,int ysize);


extern int eeRead(int addr);
extern int eeWrite(int addr,char val);
extern void eeInit(void);

uint16_t data[64];

void oled(void){
	unsigned int inner_tick;
	int ptr=0;
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

	lcdInit();
	eeInit();
	int x,y;
	x=0;
	y=0;
		
	ptr=0;
//		for(int i=0;i<sizeof(msg);i++)	{eeWrite(ptr++,msg[i]);osDelay(10);}
//		eeWrite(ptr++,0);
//		ptr=0;
//		osDelay(1000);
	
	stdout_goxy(2,0);
	for(int i=0;i<12;i++){
		printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
		eeRead(ptr++),
		eeRead(ptr++),
		eeRead(ptr++),
		eeRead(ptr++),
		eeRead(ptr++),
		eeRead(ptr++),
		eeRead(ptr++),
		eeRead(ptr++),
		eeRead(ptr++),
		eeRead(ptr++),
		eeRead(ptr++),
		eeRead(ptr++),
		eeRead(ptr++),
		eeRead(ptr++),
		eeRead(ptr++),
		eeRead(ptr++),
		eeRead(ptr++),
		eeRead(ptr++));
	}
	while(1){
		
		if(inner_tick!=tick){
			stdout_goxy(1,0);
			printf("%.2d:%.2d:%.2d\n",hour,min,sec);
			inner_tick=tick;


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
