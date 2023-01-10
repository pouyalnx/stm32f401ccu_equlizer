#include "stm32f4xx.h"
#include "cmsis_os2.h"
#include <stdio.h>


#define SEG_A	64
#define SEG_B	32
#define SEG_C	16
#define SEG_D	8
#define SEG_E	4
#define SEG_F	2
#define SEG_G	1
#define SEG_P	128

extern void segWrite(char id,char byte);
extern void segWriteRaw(char *byte_8);
extern void segWriteDig(char id,char dig);
extern void segWriteDigRaw(char *dig);


static const char segmentMap[] = {
    126, 48, 109, 121, 51, 91, 95, 112, // 0-7
    127, 123, 119, 31, 78, 61, 79, 71, // 8-9, A-F
    0x00
};


static void floatWrite(float inp){
	int real,img;
	char dat[8];
	dat[0]=0;
	if(inp<0){
		dat[0]=SEG_G;
		inp=-inp;
	}
	real=(int)inp;
	
	if(real>9999999){
		if(dat[0]==0){
			dat[1]=SEG_A;
			dat[2]=SEG_A;
			dat[3]=SEG_A;
			dat[4]=SEG_A;
			dat[5]=SEG_A;
			dat[6]=SEG_A;
			dat[7]=SEG_A;
		}
		else{
			dat[1]=SEG_D;
			dat[2]=SEG_D;
			dat[3]=SEG_D;
			dat[4]=SEG_D;
			dat[5]=SEG_D;
			dat[6]=SEG_D;
			dat[7]=SEG_D;
		}
	}
	else if(real>999999){	
		dat[7]=segmentMap[real%10]|SEG_P;
		real/=10;
		dat[6]=segmentMap[real%10];
		real/=10;
		dat[5]=segmentMap[real%10];
		real/=10;
		dat[4]=segmentMap[real%10];
		real/=10;
		dat[3]=segmentMap[real%10];
		real/=10;
		dat[2]=segmentMap[real%10];
		real/=10;
		dat[1]=segmentMap[real%10];
	}
	else if(real>99999){	
		img=(int)(10*(inp-(float)real));
		dat[7]=segmentMap[img%10];

		dat[6]=segmentMap[real%10]|SEG_P;
		real/=10;
		dat[5]=segmentMap[real%10];
		real/=10;
		dat[4]=segmentMap[real%10];
		real/=10;
		dat[3]=segmentMap[real%10];
		real/=10;
		dat[2]=segmentMap[real%10];
		real/=10;
		dat[1]=segmentMap[real%10];
	}
	else if(real>9999){	
		img=(int)(100*(inp-(float)real));
		dat[7]=segmentMap[img%10];
		img/=10;
		dat[6]=segmentMap[img%10];
		
		dat[5]=segmentMap[real%10]|SEG_P;
		real/=10;
		dat[4]=segmentMap[real%10];
		real/=10;
		dat[3]=segmentMap[real%10];
		real/=10;
		dat[2]=segmentMap[real%10];
		real/=10;
		dat[1]=segmentMap[real%10];
	}
	else if(real>999){	
		img=(int)(1000*(inp-(float)real));
		dat[7]=segmentMap[img%10];
		img/=10;
		dat[6]=segmentMap[img%10];
		img/=10;
		dat[5]=segmentMap[img%10];

		dat[4]=segmentMap[real%10]|SEG_P;
		real/=10;
		dat[3]=segmentMap[real%10];
		real/=10;
		dat[2]=segmentMap[real%10];
		real/=10;
		dat[1]=segmentMap[real%10];
	}
	else if(real>99){	
		img=(int)(10000*(inp-(float)real));
		dat[7]=segmentMap[img%10];
		img/=10;
		dat[6]=segmentMap[img%10];
		img/=10;
		dat[5]=segmentMap[img%10];
		img/=10;
		dat[4]=segmentMap[img%10];

		dat[3]=segmentMap[real%10]|SEG_P;
		real/=10;
		dat[2]=segmentMap[real%10];
		real/=10;
		dat[1]=segmentMap[real%10];
	}
	else if(real>9){	
		img=(int)(100000*(inp-(float)real));
		dat[7]=segmentMap[img%10];
		img/=10;
		dat[6]=segmentMap[img%10];
		img/=10;
		dat[5]=segmentMap[img%10];
		img/=10;
		dat[4]=segmentMap[img%10];
		img/=10;
		dat[3]=segmentMap[img%10];
		
		dat[2]=segmentMap[real%10]|SEG_P;
		real/=10;
		dat[1]=segmentMap[real%10];
	}
	else{	
		img=(int)(1000000*(inp-(float)real));
		dat[7]=segmentMap[img%10];
		img/=10;
		dat[6]=segmentMap[img%10];
		img/=10;
		dat[5]=segmentMap[img%10];
		img/=10;
		dat[4]=segmentMap[img%10];
		img/=10;
		dat[3]=segmentMap[img%10];
		img/=10;
		dat[2]=segmentMap[img%10];
		
		dat[1]=segmentMap[real%10]|SEG_P;
	}
	segWriteRaw(dat);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern int adcSample(void);
float adcTemp(void);
#define VREF	(float)3.3*(float)11

void oled(void){

	while(1){
		floatWrite((float)(VREF*(float)adcSample()/(float)4096)+(float)0.199);
		osDelay(100);
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
