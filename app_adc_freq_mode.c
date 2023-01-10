#include "stm32f4xx.h"
#include "cmsis_os2.h"
#include <stdio.h>
#include "arm_math.h"
#include "arm_const_structs.h"

extern void stdout_goxy(int x,int y);
extern int stdout_putchar(int ch);
///////////////////////////////////////////////////////////////////////////////////////////
static void pulse_set_freq(int freq){
	RCC->APB1ENR|=RCC_APB1ENR_TIM2EN;
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOAEN;
	
	TIM2->CR1=TIM_CR1_DIR;
	TIM2->PSC=84-1;
	TIM2->ARR=(uint32_t)(1000000/freq)-1;
	TIM2->CCR1=TIM2->ARR/2;
	TIM2->EGR=TIM_EGR_UG;
	TIM2->CCMR1=TIM_CCMR1_OC1M_2|TIM_CCMR1_OC1M_1;
	TIM2->CCER=TIM_CCER_CC1E;
	
	GPIOA->MODER|=GPIO_MODER_MODE5_1;
	GPIOA->OTYPER&=~(GPIO_OTYPER_OT5);
	GPIOA->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR5_1;
	GPIOA->PUPDR|=GPIO_PUPDR_PUPD5_0;
	GPIOA->AFR[0]&=~GPIO_AFRL_AFRL5;
	GPIOA->AFR[0]|=GPIO_AFRL_AFRL5_0;

	TIM2->CR1|=TIM_CR1_CEN;
}

////////////////////////////////////////////////////////////////////////////////////////////
static int *adc_buffer;
static int	adc_sample_req_cnt;
static int  adc_sample_done_cnt;

void TIM4_IRQHandler (void){
	TIM4->SR=0;
	adc_buffer[adc_sample_done_cnt++]=(int)ADC1->DR;
	if(adc_sample_req_cnt<=adc_sample_done_cnt){
		NVIC_EnableIRQ(TIM4_IRQn);
		TIM4->CR1=0;		
	}
	else{
		ADC1->SR=0;
		ADC1->CR2|=ADC_CR2_SWSTART;	
	}
}



static int adc_is_busy(void){
	if(adc_sample_done_cnt==adc_sample_req_cnt)	return 0;
	return 1;
}

static void adc_start(int freq,int cnt,int *buffer){
	if(adc_is_busy()!=0) return; 
	adc_buffer=buffer;
	adc_sample_done_cnt=0;
	adc_sample_req_cnt=cnt;
	
	RCC->APB1ENR|=RCC_APB1ENR_TIM4EN;
	RCC->APB2ENR|=RCC_APB2ENR_ADC1EN;
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOAEN;
	
	TIM4->CR1=TIM_CR1_DIR;
	TIM4->PSC=84-1;
	TIM4->ARR=(uint32_t)(1000000/freq)-1;
	TIM4->EGR=TIM_EGR_UG;
	TIM4->CR2=TIM_CR2_MMS_1;
	TIM4->DIER|=TIM_DIER_UIE;
	TIM4->SR=0;
	NVIC_EnableIRQ(TIM4_IRQn);
	ADC->CCR|=ADC_CCR_TSVREFE|ADC_CCR_ADCPRE_1;
	ADC1->CR2=1;
	GPIOA->MODER|=GPIO_MODER_MODE0;	
	ADC1->SR=0;
	ADC1->SQR3=0;
	
	ADC1->CR2|=ADC_CR2_SWSTART;	
	TIM4->CR1|=TIM_CR1_CEN;
}
///////////////////////////////////////////////////////////////////////////////////////////
#define L	1024
#define F   4096
#define D	16384.0
static int data[L];
static float32_t   x_data[2*L];
static float32_t   X_data[2*L];

static float32_t get_power_range(int f1,int f2,int Fs,int Len,float32_t *buffer){
	int n1,n2;
	float32_t power=0;
	n1=(f1*Len)/(Fs);
	n2=(f2*Len)/(Fs);
	for(int gpv0=n1;gpv0<n2;gpv0++){
		power+=buffer[gpv0];
	}
	return power;
}

static int point_to_freq(int n,int Fs,int Len){
	return (n*Fs)/Len;
}


static void main_app_sub(void){
	int gpv0,f1,f2,gpv1;
	osDelay(100);
	int f;
	pulse_set_freq(450);
	printf("lnxEbm2023\n");
	gpv1=0;
	f=50;
	while(1){
		gpv1++;
		if(gpv1==20){
			gpv1=0;
			f+=40;
			if(f>=800) f=50;
			pulse_set_freq(f);
		}
		
		adc_start(F,L,data);	
		for(gpv0=0;gpv0<L;gpv0++){
			x_data[gpv0<<1]=(float32_t)data[gpv0]/D;
			x_data[(gpv0<<1)+1]=0;
		}
		
		

		arm_cfft_f32(&arm_cfft_sR_f32_len1024,x_data,0,1);
		arm_cmplx_mag_f32(x_data,X_data,L);
		stdout_goxy(0,0);
		f1=10;
		f2=100;
		printf("%d-%d=%.0f\n",f1,f2,get_power_range(f1,f2,F,L,X_data));
		f1=100;
		f2=200;
		printf("%d-%d=%.0f\n",f1,f2,get_power_range(f1,f2,F,L,X_data));
		f1=200;
		f2=300;
		printf("%d-%d=%.0f\n",f1,f2,get_power_range(f1,f2,F,L,X_data));
		f1=300;
		f2=400;
		printf("%d-%d=%.0f\n",f1,f2,get_power_range(f1,f2,F,L,X_data));
		f1=400;
		f2=500;
		printf("%d-%d=%.0f\n",f1,f2,get_power_range(f1,f2,F,L,X_data));
		f1=500;
		f2=600;
		printf("%d-%d=%.0f\n",f1,f2,get_power_range(f1,f2,F,L,X_data));
		f1=600;
		f2=700;
		printf("%d-%d=%.0f\n",f1,f2,get_power_range(f1,f2,F,L,X_data));
		f1=700;
		f2=800;
		printf("%d-%d=%.0f\n",f1,f2,get_power_range(f1,f2,F,L,X_data));

		while(adc_is_busy()!=0) osDelay(0);	
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
static void led(void *arg){
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOCEN;
	GPIOC->MODER|=GPIO_MODER_MODE13_0;
	GPIOC->OTYPER&=~(GPIO_OTYPER_OT13);
	GPIOC->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR13_1;
	GPIOC->PUPDR|=GPIO_PUPDR_PUPD13_0;
	while(1){
		osDelay(10);
		GPIOC->ODR=1<<13;
		osDelay(100);
		GPIOC->ODR=1<<29;
	}
}



void main_app(void *arg){
	osThreadNew(led,NULL,NULL);
	main_app_sub();
	osDelay(osWaitForever);
}
