#include "stm32f4xx.h"
#include "cmsis_os2.h"
#include <stdio.h>

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

extern void stdout_goxy_st(int line,int pos);
extern int stdout_putchar_st(int ch);

uint16_t data[64];

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

	lcdInit();

	int x,y;
	x=0;
	y=0;
	stdout_goxy_st(0,0);
	//stdout_putchar_st('\n');
//	stdout_putchar_st('\n');
//	stdout_putchar_st('\n');
//	stdout_putchar_st('\n');
//	stdout_putchar_st('\n');
//	stdout_putchar_st('\n');
	while(1){
		
		if(inner_tick!=tick){
			stdout_goxy(1,0);
			printf("%.2d:%.2d:%.2d\n",hour,min,sec);
			inner_tick=tick;
			stdout_goxy(2,0);
			stdout_goxy(3,0);
			//stdout_putchar_st('S');
			/*
			lcdSelectBlock(x,8-1,y,8-1);
			lcdWrite((char *)data,128);
			for(int i=0;i<64;i++) data[i]=0xffff;
			x+=8;
			if(x==128){
				x=0;
				y+=8;
				if(y==128){
					y=0;
					
				}
			}
			*/
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
