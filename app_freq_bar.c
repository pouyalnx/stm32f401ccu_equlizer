#include "stm32f4xx.h"
#include "cmsis_os2.h"
#include "arm_math.h"
#include "arm_const_structs.h"

extern void LCD_UPDATE(unsigned char *bfr);


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
#define F   8192
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

//////////////////////////////////////////////////////////////////////////////////////////
static char lcd_buffer[1025];
static char *lcd; 
static void clr_lcd_ram(void){
	for(int gpv0=0;gpv0<1024;gpv0++) lcd[gpv0]=0;
}

static void set_pixel_ram(int x,int y){
	int l=y%8,r=y/8;
	
	
	if(r==1)			r=7;
	else if(r==2)	r=6;
	else if(r==3)	r=5;
	else if(r==4)	r=4;
	else if(r==5)	r=3;
	else if(r==6)	r=2;
	else if(r==7)	r=1;
	x+=128*r;
	
	
	if(l==0) 	  lcd[x]|=128;
	else if(l==1) lcd[x]|=64;
	else if(l==2) lcd[x]|=32;
	else if(l==3) lcd[x]|=16;
	else if(l==4) lcd[x]|=8;
	else if(l==5) lcd[x]|=4;
	else if(l==6) lcd[x]|=2;
	else if(l==7) lcd[x]|=1;
}

static void clr_pixel_ram(int x,int y){
	int l=y%8,r=y/8;
	
	
	if(r==1)			r=7;
	else if(r==2)	r=6;
	else if(r==3)	r=5;
	else if(r==4)	r=4;
	else if(r==5)	r=3;
	else if(r==6)	r=2;
	else if(r==7)	r=1;
	x+=128*r;
	
	
	if(l==0) 	  lcd[x]&=127;
	else if(l==1) lcd[x]&=191;
	else if(l==2) lcd[x]&=223;
	else if(l==3) lcd[x]&=239;
	else if(l==4) lcd[x]&=247;
	else if(l==5) lcd[x]&=251;
	else if(l==6) lcd[x]&=253;
	else if(l==7) lcd[x]&=254;
}

static void lcd_set_page(int x,int r, char val){	
	if(r==1)			r=7;
	else if(r==2)	r=6;
	else if(r==3)	r=5;
	else if(r==4)	r=4;
	else if(r==5)	r=3;
	else if(r==6)	r=2;
	else if(r==7)	r=1;
	x+=128*r;
	lcd[x]=val;
}	
///////////////////////////////////////////////////////////////////////////////////////
const char bar_under_pattern_off[8]={0,126,66,66,66,66,126,0};
const char bar_under_pattern_on[8]={0,126,66,80,80,66,126,0};
const char bar_under_pattern_seg_0[8]={0,127,64,64,64,64,127,0};
const char bar_under_pattern_seg_1_6[8]={0,255,0,0,0,0,255,0};
const char bar_under_pattern_seg_7[8]={0,254,2,2,2,2,254,0};
///////////////////////////////////////////////////////////////////////////////////////
static void bar_down_phase(int p){
	int ix=0;
	if(p){
		for(int gpv0=0;gpv0<16;gpv0++){
			for(int gpv1=0;gpv1<8;gpv1++){
				lcd_set_page(ix++,7,bar_under_pattern_on[gpv1]);
			}
		}
	}
	else{
		for(int gpv0=0;gpv0<16;gpv0++){
			for(int gpv1=0;gpv1<8;gpv1++){
				lcd_set_page(ix++,7,bar_under_pattern_off[gpv1]);
			}
		}		
	}
}

static void bar_init(void){
	int ix=0;
	bar_down_phase(0);
	for(int gpv0=0;gpv0<16;gpv0++){
		for(int gpv1=0;gpv1<8;gpv1++){
			lcd_set_page(ix,0,bar_under_pattern_seg_0[gpv1]);
			lcd_set_page(ix,1,bar_under_pattern_seg_1_6[gpv1]);
			lcd_set_page(ix,2,bar_under_pattern_seg_1_6[gpv1]);
			lcd_set_page(ix,3,bar_under_pattern_seg_1_6[gpv1]);
			lcd_set_page(ix,4,bar_under_pattern_seg_1_6[gpv1]);
			lcd_set_page(ix,5,bar_under_pattern_seg_1_6[gpv1]);
			lcd_set_page(ix,6,bar_under_pattern_seg_7[gpv1]);
			ix++;
		}
	}
}


static bar_set_val(int ix,int val){
	int v=52-val/2;
	ix=ix*8+3;
	for(int gpv0=2;gpv0<53;gpv0++){
		set_pixel_ram(ix,gpv0);
		set_pixel_ram(ix+1,gpv0);
	}
	
	for(int gpv0=2;gpv0<v;gpv0++){
		clr_pixel_ram(ix,gpv0);
		clr_pixel_ram(ix+1,gpv0);
	}
}

///////////////////////////////////////////////////////////////////////////////////////
int 		 val_data[16];
float32_t val_data_fl[16];
static void main_app_sub(void){
	unsigned int gpv0,gpv1,gpv2,gpv3;
	float32_t temp;
	osDelay(100);
	lcd_buffer[0]=0x40;
	lcd=&lcd_buffer[1];
	int f;
	pulse_set_freq(450);
	gpv1=0;
	gpv2=0;
	f=50;
	bar_init();
	LCD_UPDATE(lcd_buffer);
	while(1){
		f+=20;
		if(f>5000) f=100;
		pulse_set_freq(f);
		
		adc_start(F,L,data);
		gpv1++;
		bar_down_phase(gpv1&1);
		
		
		for(gpv0=0;gpv0<L;gpv0++){
			x_data[gpv0<<1]=(float32_t)data[gpv0]/D;
			x_data[(gpv0<<1)+1]=0;
		}
		
		arm_cfft_f32(&arm_cfft_sR_f32_len1024,x_data,0,1);
		arm_cmplx_mag_f32(x_data,X_data,L);
		

		gpv3=32;
		for(gpv0=0;gpv0<16;gpv0++){
			temp=0;
				for(gpv2=0;gpv2<24;gpv2++){
					temp+=X_data[gpv3++];
				}
			val_data_fl[gpv0]=temp;	
		}
		
		arm_max_f32(val_data_fl,16,&temp,&gpv3);
		for(gpv2=0;gpv2<16;gpv2++){
			val_data[gpv2]=(int)100*(val_data_fl[gpv2]/temp);
		}
		for(gpv2=0;gpv2<16;gpv2++){
			bar_set_val(gpv2,val_data[gpv2]);
		}
		
		LCD_UPDATE(lcd_buffer);
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
