#include "stm32f4xx.h"
#include "cmsis_os2.h"
#include "Driver_SPI.h"

////////////////////////////////////////////////
//	user fucntion for application like display
	extern void lcdWrite(char *data,unsigned int size);
	extern void lcdSelectBlock(int x,int xsize,int y,int ysize);
	extern void lcdInit(void);
	
//	user function like text display
	extern void stdout_goxy_st(int line,int pos);
	extern int stdout_putchar_st(int ch);

//for use as stdout lcd uncommnet bellow line and define if u dont have font
//#define __font_sst
int stdout_putchar(int ch){return stdout_putchar_st(ch);}
void stdout_goxy(int x,int y){stdout_goxy_st(x,y);}

////////////////////////////////////////////////
extern ARM_DRIVER_SPI Driver_SPI1;
static ARM_DRIVER_SPI *drv;

#define CS_SET		GPIOB->BSRR=GPIO_BSRR_BS2
#define CS_CLR		GPIOB->BSRR=GPIO_BSRR_BR2

#define RST_SET	GPIOB->BSRR=GPIO_BSRR_BS1
#define RST_CLR	GPIOB->BSRR=GPIO_BSRR_BR1		  
		  
#define DC_SET		GPIOB->BSRR=GPIO_BSRR_BS0
#define DC_CLR		GPIOB->BSRR=GPIO_BSRR_BR0

#define Y 	128
#define X	128
#define TEXT_COLOR	0xffff
#define BACK_COLOR	0x3f00




static char buff[16];
#define bus_init 				unsigned int ptr=0
#define bus_start				ptr=0
#define bus_write(byte)		buff[ptr++]=byte
#define bus_end_cmd			lcdCmd(buff,ptr)
#define bus_end_data		   lcdData(buff,ptr)

static bool isInitated=false;
static int lcdx,lcdy;
extern const unsigned char font[];
static unsigned short color_ram[64];

////////////////////////////////////////////////
static void lcdData(char *data,unsigned int len){
	DC_SET;
	drv->Send(data,len);
	while(drv->GetStatus().busy);	
}

static void lcdCmd(char *cmd,unsigned int len){
	DC_CLR;
	drv->Send(cmd,len);
	while(drv->GetStatus().busy);		
}
//////////////////////////////////////////////////////////
void lcdWrite(char *data,unsigned int size){
	bus_init; 
	bus_start;
	bus_write(0x2c);
	bus_end_cmd;
	lcdData(data,size);
}

void lcdSelectBlock(int x,int xsize,int y,int ysize){
	bus_init; 
	bus_start;
	bus_write(0x2a);	
	bus_end_cmd;
	bus_start;
	bus_write(0);
	bus_write(y&0xff);
	bus_write(0);
	bus_write((y+ysize-1)&0xff);
	bus_end_data;

	bus_start;
	bus_write(0x2b);	
	bus_end_cmd;
	bus_start;
	bus_write(0);
	bus_write(x&0xff);
	bus_write(0);
	bus_write((x+xsize-1)&0xff);
	bus_end_data;
}

void lcdInit(void){	
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOBEN;
	drv=&Driver_SPI1;
	
	drv->Initialize(NULL);
	drv->PowerControl(ARM_POWER_FULL);
	drv->Control(ARM_SPI_MODE_MASTER|ARM_SPI_DATA_BITS(8),8000000);

	GPIOB->MODER&=~(GPIO_MODER_MODE2);
	GPIOB->MODER|=GPIO_MODER_MODE2_0;

	GPIOB->MODER&=~(GPIO_MODER_MODE1);
	GPIOB->MODER|=GPIO_MODER_MODE1_0;
	
	GPIOB->MODER&=~(GPIO_MODER_MODE0);
	GPIOB->MODER|=GPIO_MODER_MODE0_0;


	CS_CLR;
	osDelay(50);	
	RST_CLR;
	osDelay(50);
	RST_SET;
	osDelay(50);
	
	bus_init; 

	bus_start;
	bus_write(0x1);	
	bus_end_cmd;
	osDelay(10);
	bus_start;
	bus_write(0x11);	
	bus_end_cmd;
	osDelay(10);


	bus_start;
	bus_write(0xb1);	
	bus_end_cmd;
	bus_start;
	bus_write(1);
	bus_write(0x2c);
	bus_write(0x2d);
	bus_end_data;

	bus_start;
	bus_write(0xb2);	
	bus_end_cmd;
	bus_start;
	bus_write(1);
	bus_write(0x2c);
	bus_write(0x2d);
	bus_end_data;


	bus_start;
	bus_write(0xb3);	
	bus_end_cmd;
	bus_start;
	bus_write(1);
	bus_write(0x2c);
	bus_write(0x2d);
	bus_write(1);
	bus_write(0x2c);
	bus_write(0x2d);
	bus_end_data;

	bus_start;
	bus_write(0xb4);	
	bus_end_cmd;
	bus_start;
	bus_write(0x7);
	bus_end_data;


	bus_start;
	bus_write(0xc0);	
	bus_end_cmd;
	bus_start;
	bus_write(0xa2);
	bus_write(0x2);
	bus_write(0x84);
	bus_end_data;


	bus_start;
	bus_write(0xc1);	
	bus_end_cmd;
	bus_start;
	bus_write(0xc5);
	bus_end_data;

	bus_start;
	bus_write(0xc2);	
	bus_end_cmd;
	bus_start;
	bus_write(0xa);
	bus_write(0x0);
	bus_end_data;
	
	bus_start;
	bus_write(0xc3);	
	bus_end_cmd;
	bus_start;
	bus_write(0x8a);
	bus_write(0x2a);
	bus_end_data;	

	bus_start;
	bus_write(0xc4);	
	bus_end_cmd;
	bus_start;
	bus_write(0x8a);
	bus_write(0xee);
	bus_end_data;	

	bus_start;
	bus_write(0xc1);	
	bus_end_cmd;
	bus_start;
	bus_write(0xe);
	bus_end_data;

	bus_start;
	bus_write(0x20);	
	bus_end_cmd;

	bus_start;
	bus_write(0x36);	
	bus_end_cmd;
	bus_start;
	bus_write(0x8);
	bus_end_data;


	bus_start;
	bus_write(0x3a);	
	bus_end_cmd;
	bus_start;
	bus_write(0x5);
	bus_end_data;

	bus_start;
	bus_write(0x0);	
	bus_end_cmd;


	bus_start;
	bus_write(0x2a);	
	bus_end_cmd;
	bus_start;
	bus_write(0);
	bus_write(0);
	bus_write(0);
	bus_write(127);
	bus_end_data;

	bus_start;
	bus_write(0x2b);	
	bus_end_cmd;
	bus_start;
	bus_write(0);
	bus_write(0);
	bus_write(0);
	bus_write(127);
	bus_end_data;

	bus_start;
	bus_write(0x0);	
	bus_end_cmd;
	
	bus_start;
	bus_write(0xe0);	
	bus_end_cmd;
	bus_start;
	bus_write(0x2);
	bus_write(0x1c);
	bus_write(0x7);
	bus_write(0x12);
	bus_write(0x37);
	bus_write(0x32);
	bus_write(0x29);
	bus_write(0x2d);	
	bus_write(0x29);
	bus_write(0x25);
	bus_write(0x2b);
	bus_write(0x39);
	bus_write(0x0);
	bus_write(0x1);
	bus_write(0x3);
	bus_write(0x10);
	bus_end_data;

	bus_start;
	bus_write(0x0);	
	bus_end_cmd;

	bus_start;
	bus_write(0xe1);	
	bus_end_cmd;
	bus_start;
	bus_write(0x3);
	bus_write(0x1d);
	bus_write(0x7);
	bus_write(0x6);
	bus_write(0x2e);
	bus_write(0x2c);
	bus_write(0x29);
	bus_write(0x2d);
	bus_write(0x2e);
	bus_write(0x3e);
	bus_write(0x37);
	bus_write(0x3f);
	bus_write(0x0);
	bus_write(0x0);
	bus_write(0x2);
	bus_write(0x10);
	bus_end_data;

	bus_start;
	bus_write(0x0);	
	bus_end_cmd;

	bus_start;
	bus_write(0x29);	
	bus_end_cmd;
	osDelay(10);
	bus_start;
	bus_write(0x13);	
	bus_end_cmd;
	osDelay(10);
}
///////////////////////////////////////////////////////////////////////////////////////////////
// like ssd1306 action
void stdout_goxy_st(int line,int pos){
	lcdx=(line*font[3])%X;
	lcdy=(pos*font[2])%Y;
}

int stdout_putchar_st(int ch){	
	int start_ptr,gpv0,gpv1,zero_cnt,ram_ptr,lcdy_start;
	if(!isInitated){
		lcdInit();
		isInitated=true;
	}
	if(ch=='\b'){
		if(lcdy!=0){
			lcdy-=font[2];
			stdout_putchar_st(' ');
			lcdy-=font[2];
		}
		else{
		   lcdy=(Y/font[2]-1)*font[2];
			if(lcdx==0) lcdx=(X/font[3]-1)*font[3];
			else			lcdx-=font[3];
			stdout_putchar_st(' ');
			lcdy=(Y/font[2]-1)*font[2];
			if(lcdx==0) lcdx=(X/font[3]-1)*font[3];
			else			lcdx-=font[3];
		}
		return ch;
	}
	if(ch=='\n' || ch=='\r'){
		start_ptr=lcdx;
		while(start_ptr==lcdx) stdout_putchar_st(' ');
		return ch;
	}
	if(ch=='\t'){
		zero_cnt=4-(lcdy/font[2])%4;
		while(zero_cnt--) stdout_putchar_st(' ');
		return ch;
	}
	if(ch<font[4] || ch>(font[4]+font[5])) ch='?';
	
	lcdSelectBlock(lcdx,font[3],lcdy,font[2]);

	zero_cnt=0;
	lcdy+=font[2];
	if((Y-lcdy)<font[2]){
		zero_cnt=Y-lcdy;
		lcdy_start=lcdy;
		lcdy=0;
		lcdx+=font[3];
		if(lcdx>=X) lcdx=0;
	}
	
	ram_ptr=0;
	start_ptr=6+(ch-font[4])*font[2];
	
	for(gpv1=1;gpv1<256;gpv1<<=1)
		for(gpv0=0;gpv0<font[2];gpv0++)
			if(font[gpv0+start_ptr] & gpv1) color_ram[ram_ptr++]=TEXT_COLOR;
			else									  color_ram[ram_ptr++]=BACK_COLOR;

	lcdWrite((char *)color_ram,(font[2]*font[3])<<1);
	if(zero_cnt){
		for(gpv0=0;gpv0<font[3];gpv0++)  color_ram[gpv0]=BACK_COLOR;
		while(zero_cnt--){
			lcdSelectBlock(lcdx,font[3],lcdy,1);
			lcdWrite((char *)color_ram,font[3]);
		}
	}
	
	return ch;
}

////////////////////////////////////////////////////////////////////////////////


#ifdef __font_sst
const unsigned char font[]={
    0x0, 0x1, // size of one indicates fixed font width and no pixel padding
    0x08, // width
    0x08, // height
    0x20, // first char
    0x5f, // char count

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  // <space>
0x00,0x00,0x00,0x00,0x5F,0x00,0x00,0x00,  // !
0x00,0x00,0x00,0x03,0x00,0x03,0x00,0x00,  // "
0x00,0x24,0x7E,0x24,0x24,0x7E,0x24,0x00,  // #
0x00,0x2E,0x2A,0x7F,0x2A,0x3A,0x00,0x00,  // $
0x00,0x46,0x26,0x10,0x08,0x64,0x62,0x00,  // %
0x00,0x20,0x54,0x4A,0x54,0x20,0x50,0x00,  // &
0x00,0x00,0x00,0x04,0x02,0x00,0x00,0x00,  // '
0x00,0x00,0x00,0x3C,0x42,0x00,0x00,0x00,  // (
0x00,0x00,0x00,0x42,0x3C,0x00,0x00,0x00,  // )
0x00,0x10,0x54,0x38,0x54,0x10,0x00,0x00,  // *
0x00,0x10,0x10,0x7C,0x10,0x10,0x00,0x00,  // +
0x00,0x00,0x00,0x80,0x60,0x00,0x00,0x00,  // ,
0x00,0x10,0x10,0x10,0x10,0x10,0x00,0x00,  // -
0x00,0x00,0x00,0x60,0x60,0x00,0x00,0x00,  // .
0x00,0x40,0x20,0x10,0x08,0x04,0x00,0x00,  // /

0x3C,0x62,0x52,0x4A,0x46,0x3C,0x00,0x00,  // 0
0x44,0x42,0x7E,0x40,0x40,0x00,0x00,0x00,  // 1
0x64,0x52,0x52,0x52,0x52,0x4C,0x00,0x00,  // 2
0x24,0x42,0x42,0x4A,0x4A,0x34,0x00,0x00,  // 3
0x30,0x28,0x24,0x7E,0x20,0x20,0x00,0x00,  // 4
0x2E,0x4A,0x4A,0x4A,0x4A,0x32,0x00,0x00,  // 5
0x3C,0x4A,0x4A,0x4A,0x4A,0x30,0x00,0x00,  // 6
0x02,0x02,0x62,0x12,0x0A,0x06,0x00,0x00,  // 7
0x34,0x4A,0x4A,0x4A,0x4A,0x34,0x00,0x00,  // 8
0x0C,0x52,0x52,0x52,0x52,0x3C,0x00,0x00,  // 9
0x00,0x00,0x00,0x48,0x00,0x00,0x00,0x00,  // :
0x00,0x00,0x80,0x64,0x00,0x00,0x00,0x00,  // ;
0x00,0x00,0x10,0x28,0x44,0x00,0x00,0x00,  // <
0x00,0x28,0x28,0x28,0x28,0x28,0x00,0x00,  // =
0x00,0x00,0x44,0x28,0x10,0x00,0x00,0x00,  // >
0x00,0x04,0x02,0x02,0x52,0x0A,0x04,0x00,  // ?

0x00,0x3C,0x42,0x5A,0x56,0x5A,0x1C,0x00,  // @
0x7C,0x12,0x12,0x12,0x12,0x7C,0x00,0x00,  // A
0x7E,0x4A,0x4A,0x4A,0x4A,0x34,0x00,0x00,  // B
0x3C,0x42,0x42,0x42,0x42,0x24,0x00,0x00,  // C
0x7E,0x42,0x42,0x42,0x24,0x18,0x00,0x00,  // D
0x7E,0x4A,0x4A,0x4A,0x4A,0x42,0x00,0x00,  // E
0x7E,0x0A,0x0A,0x0A,0x0A,0x02,0x00,0x00,  // F
0x3C,0x42,0x42,0x52,0x52,0x34,0x00,0x00,  // G
0x7E,0x08,0x08,0x08,0x08,0x7E,0x00,0x00,  // H
0x00,0x42,0x42,0x7E,0x42,0x42,0x00,0x00,  // I
0x30,0x40,0x40,0x40,0x40,0x3E,0x00,0x00,  // J
0x7E,0x08,0x08,0x14,0x22,0x40,0x00,0x00,  // K
0x7E,0x40,0x40,0x40,0x40,0x40,0x00,0x00,  // L
0x7E,0x04,0x08,0x08,0x04,0x7E,0x00,0x00,  // M
0x7E,0x04,0x08,0x10,0x20,0x7E,0x00,0x00,  // N
0x3C,0x42,0x42,0x42,0x42,0x3C,0x00,0x00,  // O

0x7E,0x12,0x12,0x12,0x12,0x0C,0x00,0x00,  // P
0x3C,0x42,0x52,0x62,0x42,0x3C,0x00,0x00,  // Q
0x7E,0x12,0x12,0x12,0x32,0x4C,0x00,0x00,  // R
0x24,0x4A,0x4A,0x4A,0x4A,0x30,0x00,0x00,  // S
0x02,0x02,0x02,0x7E,0x02,0x02,0x02,0x00,  // T
0x3E,0x40,0x40,0x40,0x40,0x3E,0x00,0x00,  // U
0x1E,0x20,0x40,0x40,0x20,0x1E,0x00,0x00,  // V
0x3E,0x40,0x20,0x20,0x40,0x3E,0x00,0x00,  // W
0x42,0x24,0x18,0x18,0x24,0x42,0x00,0x00,  // X
0x02,0x04,0x08,0x70,0x08,0x04,0x02,0x00,  // Y
0x42,0x62,0x52,0x4A,0x46,0x42,0x00,0x00,  // Z
0x00,0x00,0x7E,0x42,0x42,0x00,0x00,0x00,  // [
0x00,0x04,0x08,0x10,0x20,0x40,0x00,0x00,  // <backslash>
0x00,0x00,0x42,0x42,0x7E,0x00,0x00,0x00,  // ]
0x00,0x08,0x04,0x7E,0x04,0x08,0x00,0x00,  // ^
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,  // _

0x3C,0x42,0x99,0xA5,0xA5,0x81,0x42,0x3C,  // `
0x00,0x20,0x54,0x54,0x54,0x78,0x00,0x00,  // a
0x00,0x7E,0x48,0x48,0x48,0x30,0x00,0x00,  // b
0x00,0x00,0x38,0x44,0x44,0x44,0x00,0x00,  // c
0x00,0x30,0x48,0x48,0x48,0x7E,0x00,0x00,  // d
0x00,0x38,0x54,0x54,0x54,0x48,0x00,0x00,  // e
0x00,0x00,0x00,0x7C,0x0A,0x02,0x00,0x00,  // f
0x00,0x18,0xA4,0xA4,0xA4,0xA4,0x7C,0x00,  // g
0x00,0x7E,0x08,0x08,0x08,0x70,0x00,0x00,  // h
0x00,0x00,0x00,0x48,0x7A,0x40,0x00,0x00,  // i
0x00,0x00,0x40,0x80,0x80,0x7A,0x00,0x00,  // j
0x00,0x7E,0x18,0x24,0x40,0x00,0x00,0x00,  // k
0x00,0x00,0x00,0x3E,0x40,0x40,0x00,0x00,  // l
0x00,0x7C,0x04,0x78,0x04,0x78,0x00,0x00,  // m
0x00,0x7C,0x04,0x04,0x04,0x78,0x00,0x00,  // n
0x00,0x38,0x44,0x44,0x44,0x38,0x00,0x00,  // o

0x00,0xFC,0x24,0x24,0x24,0x18,0x00,0x00,  // p
0x00,0x18,0x24,0x24,0x24,0xFC,0x80,0x00,  // q
0x00,0x00,0x78,0x04,0x04,0x04,0x00,0x00,  // r
0x00,0x48,0x54,0x54,0x54,0x20,0x00,0x00,  // s
0x00,0x00,0x04,0x3E,0x44,0x40,0x00,0x00,  // t
0x00,0x3C,0x40,0x40,0x40,0x3C,0x00,0x00,  // u
0x00,0x0C,0x30,0x40,0x30,0x0C,0x00,0x00,  // v
0x00,0x3C,0x40,0x38,0x40,0x3C,0x00,0x00,  // w
0x00,0x44,0x28,0x10,0x28,0x44,0x00,0x00,  // x
0x00,0x1C,0xA0,0xA0,0xA0,0x7C,0x00,0x00,  // y
0x00,0x44,0x64,0x54,0x4C,0x44,0x00,0x00,  // z
0x00,0x08,0x08,0x76,0x42,0x42,0x00,0x00,  // {
0x00,0x00,0x00,0x7E,0x00,0x00,0x00,0x00,  // |
0x00,0x42,0x42,0x76,0x08,0x08,0x00,0x00,  // }
0x00,0x00,0x04,0x02,0x04,0x02,0x00,0x00,  // ~
};
#endif