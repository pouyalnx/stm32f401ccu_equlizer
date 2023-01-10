#include "stm32f4xx.h"
#include "cmsis_os2.h"

extern int keyRead(void);
extern int keyGetChar(void);
extern int keyGetCharRelase(void);
extern void keyInit(void);


//int stdin_getchar(void){return keyGetChar();}
int stdin_getchar(void){return keyGetCharRelase();}

#define U_read (GPIOA->IDR & GPIO_IDR_ID11)==0
#define C_read (GPIOA->IDR & GPIO_IDR_ID10)==0
#define L_read (GPIOA->IDR & GPIO_IDR_ID9)==0
#define D_read (GPIOA->IDR & GPIO_IDR_ID8)==0
#define R_read (GPIOB->IDR & GPIO_IDR_ID15)==0
static int isInit=0;


int keyRead(void){
	if(isInit==0) keyInit();
	if(U_read) return 'U';
	if(C_read) return 'C';
	if(L_read) return 'L';
	if(D_read) return 'D';
	if(R_read) return 'R';
	return 0;
}

int keyGetChar(void){
	if(isInit==0) keyInit();
	while(1){
		if(U_read) return 'U';
		if(C_read) return 'C';
		if(L_read) return 'L';
		if(D_read) return 'D';
		if(R_read) return 'R';
		osDelay(2);
	}
}



int keyGetCharRelase(void){
	if(isInit==0) keyInit();
	while(1){
		if(U_read){
			osDelay(10);
			if(U_read){
				while(U_read) osDelay(2);
				return '<';
			}
		}
		if(C_read){
			osDelay(10);
			if(C_read){
				while(C_read) osDelay(2);
				return 'C';
			}
		}
		if(L_read){
			osDelay(10);
			if(L_read){
				while(L_read) osDelay(2);
				return '^';
			}
		}
		if(D_read){
			osDelay(10);
			if(D_read){
				while(D_read) osDelay(2);
				return '>';
			}
		}
		if(R_read){
			osDelay(10);
			if(R_read){
				while(R_read) osDelay(2);
				return 'v';
			}
		}
		osDelay(2);
	}
}





void keyInit(void){
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOAEN;
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOBEN;
	
	GPIOA->MODER&=~(GPIO_MODER_MODE11);
	GPIOA->OTYPER&=~(GPIO_OTYPER_OT11);
	GPIOA->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR11_1;
	GPIOA->PUPDR|=GPIO_PUPDR_PUPD11_0;
	
	GPIOA->MODER&=~(GPIO_MODER_MODE10);
	GPIOA->OTYPER&=~(GPIO_OTYPER_OT10);
	GPIOA->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR10_1;
	GPIOA->PUPDR|=GPIO_PUPDR_PUPD10_0;
	
	GPIOA->MODER&=~(GPIO_MODER_MODE9);
	GPIOA->OTYPER&=~(GPIO_OTYPER_OT9);
	GPIOA->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR9_1;
	GPIOA->PUPDR|=GPIO_PUPDR_PUPD9_0;
	
	GPIOA->MODER&=~(GPIO_MODER_MODE8);
	GPIOA->OTYPER&=~(GPIO_OTYPER_OT8);
	GPIOA->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR8_1;
	GPIOA->PUPDR|=GPIO_PUPDR_PUPD8_0;

	GPIOB->MODER&=~(GPIO_MODER_MODE15);
	GPIOB->OTYPER&=~(GPIO_OTYPER_OT15);
	GPIOB->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR15_1;
	GPIOB->PUPDR|=GPIO_PUPDR_PUPD15_0;


	GPIOA->MODER|=GPIO_MODER_MODE12_0;
	GPIOA->OTYPER&=~(GPIO_OTYPER_OT12);
	GPIOA->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR12_1;
	GPIOA->PUPDR|=GPIO_PUPDR_PUPD12_0;
	GPIOA->ODR=1<<12;


	GPIOB->MODER|=GPIO_MODER_MODE14_0;
	GPIOB->OTYPER&=~(GPIO_OTYPER_OT14);
	GPIOB->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR14_1;
	GPIOB->PUPDR|=GPIO_PUPDR_PUPD14_0;
	GPIOB->ODR=1<<30;

	isInit=1;
}






