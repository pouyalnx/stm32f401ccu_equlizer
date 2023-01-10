#include "stm32f4xx.h"
#include "cmsis_os2.h"
#include "Driver_SPI.h"
extern ARM_DRIVER_SPI Driver_SPI1;

const static unsigned char fontpd[95][4]={
		  {0x00, 0x00, 0x00},          // Code for char 32 
        {0x00, 0x17, 0x00},           // Code for char !
        {0x03, 0x00, 0x03},           // Code for char "
        {0x1F, 0x0A, 0x1F},            // Code for char #
        {0x14, 0x3F, 0x0A},            // Code for char $
        {0x09, 0x04, 0x12},          // Code for char %
        {0x1A, 0x15, 0x1E},            // Code for char &
        {0x03, 0x01, 0x00},           // Code for char '
        {0x0E, 0x11, 0x00},            // Code for char (
        {0x11, 0x0E, 0x00},            // Code for char )
        {0x15, 0x0E, 0x15},            // Code for char *
        {0x04, 0x0E, 0x04},           // Code for char +
        {0x30, 0x10, 0x00},            // Code for char ,
        {0x04, 0x04, 0x04},           // Code for char -
        {0x00, 0x10, 0x00},            // Code for char .
        {0x18, 0x04, 0x03},            // Code for char /
        {0x1E, 0x11, 0x0F},            // Code for char 0
        {0x12, 0x1F, 0x10},            // Code for char 1
        {0x19, 0x15, 0x12},            // Code for char 2
        {0x11, 0x15, 0x0A},            // Code for char 3
        {0x06, 0x04, 0x1F},            // Code for char 4
        {0x17, 0x15, 0x09},            // Code for char 5
        {0x0E, 0x15, 0x08},            // Code for char 6
        {0x01, 0x1D, 0x07},            // Code for char 7
        {0x0A, 0x15, 0x0A},            // Code for char 8
        {0x02, 0x15, 0x0E},            // Code for char 9
        {0x00, 0x14, 0x00},            // Code for char :
        {0x30, 0x14, 0x00},            // Code for char ;
        {0x04, 0x0A, 0x11},            // Code for char <
        {0x14, 0x14, 0x14},            // Code for char =
        {0x11, 0x0A, 0x04},            // Code for char >
        {0x01, 0x15, 0x02},            // Code for char ?
        {0x1F, 0x11, 0x17},            // Code for char @
        {0x1E, 0x05, 0x1E},            // Code for char A
        {0x1F, 0x15, 0x0A},            // Code for char B
        {0x0E, 0x11, 0x11},            // Code for char C
        {0x1F, 0x11, 0x0E},            // Code for char D
        {0x1F, 0x15, 0x11},            // Code for char E
        {0x1F, 0x05, 0x01},            // Code for char F
        {0x0E, 0x11, 0x1D},            // Code for char G
        {0x1F, 0x04, 0x1F},            // Code for char H
        {0x11, 0x1F, 0x11},            // Code for char I
        {0x08, 0x10, 0x0F},            // Code for char J
        {0x1F, 0x04, 0x1B},            // Code for char K
        {0x1F, 0x10, 0x10},            // Code for char L
        {0x1F, 0x06, 0x1F},            // Code for char M
        {0x1F, 0x02, 0x1F},            // Code for char N
        {0x0E, 0x11, 0x0E},            // Code for char O
        {0x1F, 0x05, 0x02},            // Code for char P
        {0x0E, 0x19, 0x1E},            // Code for char Q
        {0x1F, 0x05, 0x1A},            // Code for char R
        {0x16, 0x15, 0x0D},            // Code for char S
        {0x01, 0x1F, 0x01},            // Code for char T
        {0x1F, 0x10, 0x1F},            // Code for char U
        {0x0F, 0x10, 0x0F},            // Code for char V
        {0x1F, 0x0C, 0x1F},            // Code for char W
        {0x1B, 0x04, 0x1B},            // Code for char X
        {0x03, 0x1C, 0x03},            // Code for char Y
        {0x19, 0x15, 0x13},            // Code for char Z
        {0x1F, 0x11, 0x00},            // Code for char [
        {0x03, 0x04, 0x18},            // Code for char BackSlash
        {0x11, 0x1F, 0x00},            // Code for char ]
        {0x02, 0x01, 0x02},            // Code for char ^
        {0x20, 0x20, 0x20},            // Code for char _
        {0x01, 0x03, 0x00},            // Code for char `
        {0x18, 0x14, 0x1C},            // Code for char a
        {0x1F, 0x14, 0x08},            // Code for char b
        {0x08, 0x14, 0x14},            // Code for char c
        {0x08, 0x14, 0x1F},            // Code for char d
        {0x0C, 0x1C, 0x14},            // Code for char e
        {0x04, 0x1E, 0x05},            // Code for char f
        {0x2C, 0x24, 0x3C},            // Code for char g
        {0x1F, 0x04, 0x18},            // Code for char h
        {0x00, 0x1D, 0x00},            // Code for char i
        {0x20, 0x3D, 0x00},            // Code for char j
        {0x1F, 0x08, 0x14},            // Code for char k
        {0x00, 0x1F, 0x00},            // Code for char l
        {0x1C, 0x0C, 0x1C},            // Code for char m
        {0x1C, 0x04, 0x18},            // Code for char n
        {0x08, 0x14, 0x08},            // Code for char o
        {0x3C, 0x14, 0x08},            // Code for char p
        {0x08, 0x14, 0x3C},            // Code for char q
        {0x1C, 0x04, 0x00},            // Code for char r
        {0x10, 0x1C, 0x04},            // Code for char s
        {0x04, 0x1E, 0x14},            // Code for char t
        {0x1C, 0x10, 0x1C},            // Code for char u
        {0x0C, 0x10, 0x0C},            // Code for char v
        {0x1C, 0x18, 0x1C},            // Code for char w
        {0x14, 0x08, 0x14},            // Code for char x
        {0x2C, 0x10, 0x0C},            // Code for char y
        {0x04, 0x1C, 0x10},            // Code for char z
        {0x04, 0x1F, 0x11},            // Code for char {
        {0x00, 0x1F, 0x00},            // Code for char |
        {0x11, 0x1F, 0x04},            // Code for char }
        {0x01, 0x02, 0x01}             // Code for char ~ 126
        };

	
static ARM_DRIVER_SPI *drv;

#define CS_SET		GPIOB->BSRR=GPIO_BSRR_BS2
#define CS_CLR		GPIOB->BSRR=GPIO_BSRR_BR2

#define RST_SET	GPIOB->BSRR=GPIO_BSRR_BS1
#define RST_CLR	GPIOB->BSRR=GPIO_BSRR_BR1		  
		  
#define DC_SET		GPIOB->BSRR=GPIO_BSRR_BS0
#define DC_CLR		GPIOB->BSRR=GPIO_BSRR_BR0
		  

void dat(unsigned char p){
	DC_SET;
	drv->Send(&p,1);
	osDelay(2);
	while(drv->GetStatus().busy);
}
void ins(unsigned char p){
	DC_CLR;	
	drv->Send(&p,1);
	osDelay(2);
	while(drv->GetStatus().busy);
}

void send(char i){
int j;
//dat(0);	
for(j=1;j<7;j++)
dat(fontpd[i-32][j]);	
}
void send0(char *c){
int k=0;
while( *(c+k)!='\0') send(*(c+k++));	
}

void clear(void){
int j;
for(j=0;j<1024;j++){	
dat(0x00);	
}
}
struct disp{
unsigned no:1;
unsigned num:1;
}disp_conf;

void disp(char in[],unsigned int gpv0,unsigned int gpv1,unsigned char gpv2){
	unsigned char gpv3,gpv6,gpv7;
	unsigned int gpv4,gpv5,cont;
	disp_conf.no=0;
	disp_conf.num=0;
	ins(0x20);
	ins(2);
	ins(0x10);
	ins(0x80);
	ins(0x40+gpv2);
	cont=0;
for(gpv3=0;*(in+gpv3);gpv3++){
	if(*(in+gpv3)=='!'){ disp_conf.no=~disp_conf.no;
	gpv3++;
	}
if(*(in+gpv3)=='@'){
  if(disp_conf.num==0){
	gpv5=gpv0;
	gpv4=1;
	while(gpv5>0){
	gpv4*=10;
	gpv5/=10;	
	}
	gpv4/=10;
	gpv5=gpv0;
	while(gpv4>0){
	gpv7=gpv5/gpv4;
	if(disp_conf.no)	
    for(gpv6=0;gpv6<4;gpv6++){
		dat(~fontpd[gpv7+16][gpv6]);
		cont++;
		}
    else
    for(gpv6=0;gpv6<4;gpv6++){
		dat(~fontpd[gpv7+16][gpv6]);
		cont++;
		}
	gpv5%=gpv4;
	gpv4/=10;	
	}
	}
	else{
	gpv5=gpv1;
	gpv4=1;
	while(gpv5>0){	
	gpv4*=10;
	gpv5/=10;	
	}
	gpv4/=10;
	gpv5=gpv1;
	while(gpv4>0){
	gpv7=gpv5/gpv4;
	if(disp_conf.no)	
    for(gpv6=0;gpv6<4;gpv6++){
		dat(~fontpd[gpv7+16][gpv6]);
		cont++;
		}
    else
    for(gpv6=0;gpv6<4;gpv6++){
		dat(~fontpd[gpv7+16][gpv6]);
		cont++;
		}
	gpv5%=gpv4;
	gpv4/=10;	
	}
	}		
	disp_conf.num=1;
	}
	else{
	if(disp_conf.no)	
    for(gpv6=0;gpv6<4;gpv6++){	
		dat(~fontpd[*(in+gpv3)-32][gpv6]);
		cont++;
		}
    else
    for(gpv6=0;gpv6<4;gpv6++){	
		dat(~fontpd[*(in+gpv3)-32][gpv6]);
		cont++;
		}
	}	
	}
	if(disp_conf.no)
	while(cont<84){
	dat(0xff);
cont++;
}	
	else
	while(cont<84){
	dat(0);
cont++;
}	
}	



int lcdInit(){	
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOBEN;

	drv=&Driver_SPI1;
	drv->Initialize(NULL);
	drv->PowerControl(ARM_POWER_FULL);
	drv->Control(ARM_SPI_MODE_MASTER|ARM_SPI_DATA_BITS(8),4000000);

	GPIOB->MODER&=~(GPIO_MODER_MODE2);
	GPIOB->MODER|=GPIO_MODER_MODE2_0;

	GPIOB->MODER&=~(GPIO_MODER_MODE1);
	GPIOB->MODER|=GPIO_MODER_MODE1_0;
	GPIOB->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR1_1;
	GPIOB->OTYPER&=~(GPIO_OTYPER_OT1);
	GPIOB->PUPDR|=GPIO_PUPDR_PUPD1_0;
	
	GPIOB->MODER&=~(GPIO_MODER_MODE0);
	GPIOB->MODER|=GPIO_MODER_MODE0_0;
	GPIOB->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR0_1;
	GPIOB->OTYPER&=~(GPIO_OTYPER_OT0);
	GPIOB->PUPDR|=GPIO_PUPDR_PUPD0_0;

	CS_CLR;
	osDelay(100);	
	RST_CLR;
	osDelay(100);
	RST_SET;
	osDelay(100);
	
	ins(0x21);
	ins(0x5);
	ins(0x13);
	ins(0xc2);
	
	ins(0x20);
	ins(0xc);
	ins(0x40);
	ins(0x80);
	osDelay(50);

	ins(0x20);
	ins(0xc);
	ins(0x44);
	ins(0x80);
	osDelay(50);
	disp(" @ @ ",12,22,4);
	disp("!pouya",0,0,2);
	disp("!hello world! ",0,0,3);
	disp("you !@! ",18,0,5);
	disp("TRUTST TO !GOD! ",0,0,1);

}
