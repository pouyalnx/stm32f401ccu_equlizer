#include "cmsis_os2.h"
#include "Driver_I2C.h"

extern int gyRead(int channel);
extern void gyInit(void);

extern ARM_DRIVER_I2C Driver_I2C1;
static ARM_DRIVER_I2C *drv;
#define GY273_ADDR  0x1e


static void send_data(void *data,int size,int continue_req){
  drv->MasterTransmit(GY273_ADDR,data,size,continue_req);	
  while(drv->GetStatus().busy);  //wait for last transfer
}

static void read_data(void *data,int size,int continue_req){
  drv->MasterReceive(GY273_ADDR,data,size,continue_req);	
  while(drv->GetStatus().busy);  //wait for last transfer
}


int gyRead(int channel){
	unsigned char data[2];
	data[0]=channel*2+3;
	send_data(data,1,true);
	read_data(data,2,false);
	return data[0]|data[1]<<8;	
}


void gyInit(void){
unsigned char data[4];	
drv=&Driver_I2C1;
drv->Initialize(NULL);
drv->PowerControl(ARM_POWER_FULL);	
drv->Control(ARM_I2C_BUS_SPEED,ARM_I2C_BUS_SPEED_FAST);	
data[0]=0;
data[1]=0x70;
send_data(data,2,true);	

data[0]=1;
data[1]=0x20;
send_data(data,2,true);	

data[0]=2;
data[1]=0x00;
send_data(data,2,true);		
}

