#include "stm32f4xx.h"
#include "cmsis_os2.h"
#include <stdio.h>

extern void segWriteByte(char id,char byte);
extern void segWriteByteRaw(char *byte_8);
extern void segWriteDig(char id,char dig);
extern void segWriteDigRaw(char *dig);



void oled(void){
	osDelay(100);
	int i=0,j=0xfffffff;
	char dat[8]={0,1,2,3,4,5,6,7};

	//segWriteByte(0,0xf);
//	segWriteDig(0,1);
//	segWriteDig(1,1);
//	segWriteDig(2,1);
//	segWriteDig(3,1);
	segWriteDigRaw(dat);
	osDelay(200);
	while(1){
		/*
		i++;
		j--;
		dat[0]=i&0xff;
		dat[1]=(i>>8)&0xff;
		dat[2]=(i>>16)&0xff;
		dat[3]=(i>>24)&0xff;
		dat[4]=j&0xff;
		dat[5]=(j>>8)&0xff;
		dat[6]=(j>>16)&0xff;
		dat[7]=(j>>24)&0xff;
		segWriteByteRaw(dat);
*/
		
		dat[0]=(dat[0]+1)%10;
		dat[1]=(dat[1]+1)%10;
		dat[2]=(dat[2]+1)%10;
		dat[3]=(dat[3]+1)%10;
		dat[4]=(dat[4]+1)%10;
		dat[5]=(dat[5]+1)%10;
		dat[6]=(dat[6]+1)%10;
		dat[7]=(dat[7]+1)%10;
		segWriteDigRaw(dat);
		osDelay(200);
		
		osDelay(200);
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
	oled();
	osDelay(osWaitForever);
}
