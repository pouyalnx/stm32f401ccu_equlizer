#include "stm32f4xx.h"
#include "cmsis_os2.h"
#include <stdio.h>

/*************************************************************/
int rotpos;
int rotneg;

void EXTI0_IRQHandler(void){
	if(GPIOB->IDR&GPIO_IDR_ID1) rotpos++;
	else								 rotneg--;
	EXTI->PR|=EXTI_PR_PR0;
}


void RotaryInit(void){
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOBEN;
	RCC->APB2ENR|=RCC_APB2ENR_SYSCFGEN;
	
	GPIOB->MODER&=~GPIO_MODER_MODE0;
	GPIOB->OTYPER&=~(GPIO_OTYPER_OT0);
	GPIOB->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR0_1;
	GPIOB->PUPDR|=GPIO_PUPDR_PUPD0_0;

	
	GPIOB->MODER&=~GPIO_MODER_MODE1;
	GPIOB->OTYPER&=~(GPIO_OTYPER_OT1);
	GPIOB->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR1_1;
	GPIOB->PUPDR|=GPIO_PUPDR_PUPD1_0;

	EXTI->PR|=EXTI_PR_PR0;
	EXTI->FTSR|=EXTI_FTSR_TR0;
	EXTI->IMR|=EXTI_IMR_IM0;


	*(SYSCFG->EXTICR)&=~SYSCFG_EXTICR1_EXTI0_Msk;
	*(SYSCFG->EXTICR)|=SYSCFG_EXTICR1_EXTI0_PB;
	rotneg=0;
	rotpos=0;
	
	NVIC_EnableIRQ(EXTI0_IRQn);	
}







/*************************************************************/
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

char lcdpat[6]={0xff,0x83,0xbd,0xbd,0xc1,0xff,0};

extern void lcdData(char *data,int len);
extern void lcdInit(void);
extern void lcdGoXY(int x,int y);



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

	RotaryInit();
	
	while(1){
		
		if(inner_tick!=tick){
			stdout_goxy(1,0);
			printf("%.2d:%.2d:%.2d\n",hour,min,sec);
			inner_tick=tick;
			stdout_goxy(2,0);
			printf(">> %d\n",rotneg);
			stdout_goxy(3,0);
			printf("<< %d\n",rotpos);		
			
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
