#include "cmsis_os2.h"
#include "stm32f4xx.h"
#include <stdio.h>




static void SystemCoreClockInit(void){
	RCC->CR|=RCC_CR_HSEON;
	while(!(RCC->CR & RCC_CR_HSERDY));
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;									// Enable PWR clock
	
	RCC->CFGR|=RCC_CFGR_HPRE_DIV1;	
	RCC->CFGR|=RCC_CFGR_PPRE1_DIV2;
	RCC->CFGR|=RCC_CFGR_PPRE2_DIV1;
	
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM;									// Reset PLLM
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLN;									// Reset PLLN
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLP;                 			// Reset PLLP	

    //using HSI
     
    RCC->PLLCFGR=RCC_PLLCFGR_PLLM_4		                                    //fin=16 n=336 m=16 p=4 q=8
				|RCC_PLLCFGR_PLLN_4|RCC_PLLCFGR_PLLN_6|RCC_PLLCFGR_PLLN_8
				|RCC_PLLCFGR_PLLP_0
				|RCC_PLLCFGR_PLLQ_3;
    
   //using HSE
   /*
	RCC->PLLCFGR=RCC_PLLCFGR_PLLM_0|RCC_PLLCFGR_PLLM_3|RCC_PLLCFGR_PLLM_4		//fin=25 n=336 m=25 p=4 q=8
					|RCC_PLLCFGR_PLLN_4|RCC_PLLCFGR_PLLN_6|RCC_PLLCFGR_PLLN_8
					|RCC_PLLCFGR_PLLSRC_HSE
					|RCC_PLLCFGR_PLLP_0
					|RCC_PLLCFGR_PLLQ_3;
    */
	RCC->CR |= RCC_CR_PLLON;                      	  		   // Enable PLL
	while((RCC->CR & RCC_CR_PLLRDY) == 0){}    

	FLASH->ACR = FLASH_ACR_PRFTEN | 
	FLASH_ACR_ICEN | FLASH_ACR_DCEN |								// Config Flash
	FLASH_ACR_LATENCY_5WS;
	
	RCC->CFGR &= ~RCC_CFGR_SW;                   		    	// Reset SW
	RCC->CFGR |= RCC_CFGR_SW_PLL;                 				// SYSCLK = PLL					
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {} // Wait PLL-SYSCLK flag		
	SystemCoreClockUpdate();
}
/////////////////////////////////////////////////////////////////////////////////////////

extern void main_app(void *arg);

extern void app(void);
int main(){
	SystemCoreClockInit();
	osKernelInitialize();
	osThreadNew(main_app,NULL,NULL);
	osKernelStart();
}

	
