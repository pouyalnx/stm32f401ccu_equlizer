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

#define SIG_ADD	SEG_G
#define SIG_DEC	SEG_G

extern void segWrite(char id,char byte);
extern void segWriteRaw(char *byte_8);
extern void segWriteDig(char id,char dig);
extern void segWriteDigRaw(char *dig);

static const char segmentMap[] = {
    126, 48, 109, 121, 51, 91, 95, 112, // 0-7
    127, 123, 119, 31, 78, 61, 79, 71, // 8-9, A-F
    0x00
};
///////////////////////////////////////////////////////////////
static char password[4];
static char password_shadow[4];
///////////////////////////////////////////////////////////////
static int quartSecondFlag;
static void quartSecondTimer(void *arg){
	quartSecondFlag++;
	if(quartSecondFlag==4) quartSecondFlag=0;
}

///////////////////////////////////////////////////////////////
#define startup_pattern_len  20
const char startup_pattern[startup_pattern_len][8]={
	{SEG_A,0,0,SEG_A,0,0,0,0},
	{0,SEG_A,0,0,SEG_A,0,0,0},
	{0,0,SEG_A,0,0,SEG_A,0,0},	
	{0,0,0,SEG_A,0,0,SEG_A,0},
	{0,0,0,0,SEG_A,0,0,SEG_A},
	{SEG_A,0,0,0,0,SEG_A,0,0},
	{0,SEG_A,0,0,0,0,SEG_A,0},
	{0,0,SEG_A,0,0,0,0,SEG_A},
	{0,0,0,SEG_A,0,0,0,SEG_B},
	{0,0,0,0,SEG_A,0,0,SEG_C},
	{0,0,0,SEG_A,0,0,0,SEG_D},
	{0,0,SEG_A,0,0,0,SEG_D,0},
	{0,SEG_A,0,0,0,SEG_D,0,0},
	{SEG_A,0,0,0,SEG_D,0,0,0},
	{0,0,0,SEG_D,0,0,0,SEG_A},
	{0,0,SEG_D,0,0,0,SEG_A,0},	
	{0,SEG_D,0,0,0,SEG_A,0,0},
	{SEG_D,0,0,0,SEG_A,0,0,0},
	{SEG_E,0,0,SEG_A,0,0,0,0},
	{SEG_F,0,SEG_A,0,0,0,0,0}
};
/////////////////////////////////////////////////////////////////
static char seg_buffer[8];
static void seg_update(void){   //used for all operations
	segWriteRaw(seg_buffer);
}


/////////////////////////////////////////////////////////////////
extern int keyGetCharRelase(void);
static int keydir_key;
/////////////////////////////////////////////////////////////////
#define STAT_LOCK 1
#define STAT_OPEN 2
#define STAT_WAIT 3


#define LOCK_IO_ON	GPIOB->ODR=1<<7
#define LOCK_IO_OFF	GPIOB->ODR=1<<20

//////////////////////////////////////////////////////////////////
static void get_password(char *dat){
	dat[0]=RTC->BKP0R&0xff;
	dat[1]=RTC->BKP1R&0xff;
	dat[2]=RTC->BKP2R&0xff;
	dat[3]=RTC->BKP3R&0xff;
}

static void set_password(char *dat){
	PWR->CR|=PWR_CR_DBP;
	RTC->WPR=0xCA;
	RTC->WPR=0x53;
	RTC->BKP0R=dat[0];
	RTC->BKP1R=dat[1];
	RTC->BKP2R=dat[2];
	RTC->BKP3R=dat[3];
	PWR->CR&=~PWR_CR_DBP;
}


//////////////////////////////////////////////////////////////////

static void main_app_sub(void){

	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOBEN;
	GPIOB->MODER|=GPIO_MODER_MODE7_0;
	GPIOB->OTYPER&=~(GPIO_OTYPER_OT7);
	GPIOB->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR7_1;
	GPIOB->PUPDR|=GPIO_PUPDR_PUPD7_0;
	LOCK_IO_OFF;
	osDelay(100);
	int ix=0,repeat=40,error_count_input=0,chng_digit=-1;
	keydir_key=0;
	osTimerStart(osTimerNew(quartSecondTimer,osTimerPeriodic,0,0),250);
	while(repeat--){
		segWriteRaw(startup_pattern[ix]);
		ix=(ix+1)%startup_pattern_len;
		osDelay(50);
	}
	int stat=STAT_LOCK;
	int wait_seconds=0;
	int inner_quard_flag=-1;
	int screen_update_req=0;
	
	get_password(password_shadow);
	password[0]=password_shadow[0];
	password[1]=password_shadow[1];
	password[2]=password_shadow[2];
	password[3]=password_shadow[3];
	
	while(1){
		///////////////////////////////////////////////////////////
		//KeyScan & logical
		if(keydir_key){
			if(keydir_key=='C'){
				if(chng_digit!=-1){
					chng_digit=-1;
					if(stat==STAT_OPEN){
						password_shadow[0]=password[0];
						password_shadow[1]=password[1];
						password_shadow[2]=password[2];
						password_shadow[3]=password[3];
						set_password(password_shadow);
					}
				}
				else if(stat==STAT_OPEN){
					error_count_input=0;
					stat=STAT_LOCK;
					LOCK_IO_OFF;
				}
				else if(stat==STAT_LOCK){ 
					if(password[0]==password_shadow[0] && password[1]==password_shadow[1] && password[2]==password_shadow[2] && password[3]==password_shadow[3]){
						stat=STAT_OPEN;
						LOCK_IO_ON;
					}
					else{
						error_count_input++;
						if(error_count_input==4){
							error_count_input=0;
							stat=STAT_WAIT;
							wait_seconds=15;
						}
					}
				}
			}
			if(keydir_key=='<'){
				if(stat==STAT_LOCK || stat==STAT_OPEN){
					chng_digit++;
					if(chng_digit==4) chng_digit=0;
				}
			}
			if(keydir_key=='>'){
				if(stat==STAT_LOCK || stat==STAT_OPEN){
					chng_digit--;
					if(chng_digit<0) chng_digit=3;
				}
			}
			if(keydir_key=='v'){
				if(chng_digit!=-1){
					password[chng_digit]=(password[chng_digit]-1)%10;
				}
			}
			if(keydir_key=='^'){
				if(chng_digit!=-1){
					password[chng_digit]=(password[chng_digit]+1)%10;
				}
			}
			keydir_key=0;
		}
		//////////////////////////////////////////////////////////
		///Display Area
		if(stat==STAT_WAIT && inner_quard_flag!=quartSecondFlag){
			seg_buffer[7]=SEG_A|SEG_D; // wait stat
			seg_buffer[6]=0;
			if(quartSecondFlag==0)			seg_buffer[6]=SEG_A;
			else if(quartSecondFlag==1)	seg_buffer[6]=SEG_B|SEG_C;
			else if(quartSecondFlag==2)	seg_buffer[6]=SEG_D;
			else if(quartSecondFlag==3)	seg_buffer[6]=SEG_E|SEG_F;

			if(quartSecondFlag==1)			seg_buffer[5]=SEG_A;
			else if(quartSecondFlag==2)	seg_buffer[5]=SEG_B|SEG_C;
			else if(quartSecondFlag==3)	seg_buffer[5]=SEG_D;
			else if(quartSecondFlag==0)	seg_buffer[5]=SEG_E|SEG_F;
			
			if(quartSecondFlag==2)			seg_buffer[4]=SEG_A;
			else if(quartSecondFlag==3)	seg_buffer[4]=SEG_B|SEG_C;
			else if(quartSecondFlag==0)	seg_buffer[4]=SEG_D;
			else if(quartSecondFlag==1)	seg_buffer[4]=SEG_E|SEG_F;
			
			if(quartSecondFlag==3) wait_seconds--;
			ix=wait_seconds;
			seg_buffer[3]=segmentMap[ix%10];
			ix/=10;
			seg_buffer[2]=segmentMap[ix%10];
			ix/=10;
			seg_buffer[1]=segmentMap[ix%10];
			ix/=10;
			seg_buffer[0]=segmentMap[ix%10];
			screen_update_req=1;
			inner_quard_flag=quartSecondFlag;
			if(wait_seconds==-1)	{
				stat=STAT_LOCK;
				inner_quard_flag=-1;
			}
		}
		if(stat==STAT_LOCK  && inner_quard_flag!=quartSecondFlag){
			seg_buffer[7]=SEG_A; // wait Lock
			
			if(chng_digit==-1){
				seg_buffer[6]=0;
				seg_buffer[5]=segmentMap[(int)password[3]];
				seg_buffer[4]=segmentMap[(int)password[2]];
				seg_buffer[3]=segmentMap[(int)password[1]];
				seg_buffer[2]=segmentMap[(int)password[0]];
				seg_buffer[1]=0;
			}
			else if(chng_digit==3){
				seg_buffer[6]=SIG_DEC;
				seg_buffer[5]=segmentMap[(int)password[3]];
				seg_buffer[4]=SIG_ADD;
				seg_buffer[3]=segmentMap[(int)password[2]];
				seg_buffer[2]=segmentMap[(int)password[1]];
				seg_buffer[1]=segmentMap[(int)password[0]];
			}
			else if(chng_digit==2){
				seg_buffer[6]=segmentMap[(int)password[3]];
				seg_buffer[5]=SIG_DEC;
				seg_buffer[4]=segmentMap[(int)password[2]];
				seg_buffer[3]=SIG_ADD;
				seg_buffer[2]=segmentMap[(int)password[1]];
				seg_buffer[1]=segmentMap[(int)password[0]];
			}		
			else if(chng_digit==1){
				seg_buffer[6]=segmentMap[(int)password[3]];
				seg_buffer[5]=segmentMap[(int)password[2]];
				seg_buffer[4]=SIG_DEC;
				seg_buffer[3]=segmentMap[(int)password[1]];
				seg_buffer[2]=SIG_ADD;
				seg_buffer[1]=segmentMap[(int)password[0]];
			}	
			else if(chng_digit==0){
				seg_buffer[6]=segmentMap[(int)password[3]];
				seg_buffer[5]=segmentMap[(int)password[2]];
				seg_buffer[4]=segmentMap[(int)password[1]];
				seg_buffer[3]=SIG_DEC;
				seg_buffer[2]=segmentMap[(int)password[0]];
				seg_buffer[1]=SIG_ADD;
			}						
			if(quartSecondFlag==0)			seg_buffer[0]=SEG_A;
			else if(quartSecondFlag==1)	seg_buffer[0]=SEG_B|SEG_C;
			else if(quartSecondFlag==2)	seg_buffer[0]=SEG_D;
			else if(quartSecondFlag==3)	seg_buffer[0]=SEG_E|SEG_F;
			screen_update_req=1;
			inner_quard_flag=quartSecondFlag;
		}

		if(stat==STAT_OPEN  && inner_quard_flag!=quartSecondFlag){
			seg_buffer[7]=SEG_G; // wait Lock
			if(chng_digit==-1){
				seg_buffer[6]=0;
				seg_buffer[5]=segmentMap[(int)password[3]];
				seg_buffer[4]=segmentMap[(int)password[2]];
				seg_buffer[3]=segmentMap[(int)password[1]];
				seg_buffer[2]=segmentMap[(int)password[0]];
				seg_buffer[1]=0;
			}
			else if(chng_digit==3){
				seg_buffer[6]=SIG_DEC;
				seg_buffer[5]=segmentMap[(int)password[3]];
				seg_buffer[4]=SIG_ADD;
				seg_buffer[3]=segmentMap[(int)password[2]];
				seg_buffer[2]=segmentMap[(int)password[1]];
				seg_buffer[1]=segmentMap[(int)password[0]];
			}
			else if(chng_digit==2){
				seg_buffer[6]=segmentMap[(int)password[3]];
				seg_buffer[5]=SIG_DEC;
				seg_buffer[4]=segmentMap[(int)password[2]];
				seg_buffer[3]=SIG_ADD;
				seg_buffer[2]=segmentMap[(int)password[1]];
				seg_buffer[1]=segmentMap[(int)password[0]];
			}		
			else if(chng_digit==1){
				seg_buffer[6]=segmentMap[(int)password[3]];
				seg_buffer[5]=segmentMap[(int)password[2]];
				seg_buffer[4]=SIG_DEC;
				seg_buffer[3]=segmentMap[(int)password[1]];
				seg_buffer[2]=SIG_ADD;
				seg_buffer[1]=segmentMap[(int)password[0]];
			}	
			else if(chng_digit==0){
				seg_buffer[6]=segmentMap[(int)password[3]];
				seg_buffer[5]=segmentMap[(int)password[2]];
				seg_buffer[4]=segmentMap[(int)password[1]];
				seg_buffer[3]=SIG_DEC;
				seg_buffer[2]=segmentMap[(int)password[0]];
				seg_buffer[1]=SIG_ADD;
			}	
			if(quartSecondFlag==0)			seg_buffer[0]=SEG_A;
			else if(quartSecondFlag==1)	seg_buffer[0]=SEG_B|SEG_C;
			else if(quartSecondFlag==2)	seg_buffer[0]=SEG_D;
			else if(quartSecondFlag==3)	seg_buffer[0]=SEG_E|SEG_F;
			screen_update_req=1;
			inner_quard_flag=quartSecondFlag;
		}
		///////////////////////////////////////////////////////////////////////////////
		//Update Center
		if(screen_update_req){
			seg_update();
			screen_update_req=0;
		}
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
		keydir_key=keyGetCharRelase();
		while(keydir_key!=0) osDelay(2);
		GPIOC->ODR=1<<13;
		keydir_key=keyGetCharRelase();
		while(keydir_key!=0) osDelay(2);
		GPIOC->ODR=1<<29;
	}
}



void main_app(void *arg){
	osThreadNew(led,NULL,NULL);
	main_app_sub();
	osDelay(osWaitForever);
}
