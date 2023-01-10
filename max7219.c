

extern void segWrite(char id,char byte);
extern void segWriteRaw(char *byte_8);

extern void segWriteDig(char id,char dig);
extern void segWriteDigRaw(char *dig);


#include "Driver_SPI.h"
#include "stm32f4xx.h"
#include "cmsis_os2.h"

#define SEG_A	64
#define SEG_B	32
#define SEG_C	16
#define SEG_D	8
#define SEG_E	4
#define SEG_F	2
#define SEG_G	1
#define SEG_P	128

extern ARM_DRIVER_SPI Driver_SPI1;
static ARM_DRIVER_SPI *drv;

static int isInited=0;

static void drvSend(char cmd,char dat){
	char data[2]={cmd,dat};
	drv->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_ACTIVE);
	drv->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_ACTIVE);
	drv->Send(data,2);
	while(drv->GetStatus().busy);	
	drv->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);
	drv->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);
}

static  void segInit(void){
	drv=&Driver_SPI1;
	drv->Initialize(NULL);
	drv->PowerControl(ARM_POWER_FULL);
	drv->Control(ARM_SPI_MODE_MASTER|ARM_SPI_DATA_BITS(8)|ARM_SPI_SS_MASTER_SW,1000000);

	drvSend(0xc,1);
	drvSend(0xb,7);
	drvSend(0xa,0xf);
	drvSend(0x9,0);
	drvSend(0xf,0);
	isInited=1;
}

void segWrite(char id,char byte){
	if(isInited==0) segInit();
	drvSend(8-id&0x7,byte);
}

void segWriteRaw(char *byte){
	if(isInited==0) segInit();
	for(int i=0;i<8;i++){
		drvSend(8-(char)i,byte[i]);
	}
}


static const char segmentMap[] = {
    126, 48, 109, 121, 51, 91, 95, 112, // 0-7
    127, 123, 119, 31, 78, 61, 79, 71, // 8-9, A-F
    0x00
};



void segWriteDig(char id,char dig){
	if(isInited==0) segInit();
	drvSend(8-id&0x7,segmentMap[dig&0xf]);
}

void segWriteDigRaw(char *dig){
	if(isInited==0) segInit();
	for(int i=0;i<8;i++){
		drvSend(8-(char)i,segmentMap[dig[i]&0xf]);
	}
}

