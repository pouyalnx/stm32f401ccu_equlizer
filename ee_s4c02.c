#include "Driver_I2C.h"

/*
	extern int eeRead(int addr);
	extern int eeWrite(int addr,char val);
	extern void eeInit(void);

*/


#define I2C_DRVIER			 Driver_I2C1 //PLEASE CHANGE HERE FOR USING DIFRENT
#define I2C_ADDR				 0x50
extern ARM_DRIVER_I2C 		 I2C_DRVIER;

static ARM_DRIVER_I2C *drv;
static unsigned char buffer[4];


int eeRead(int addr){
	buffer[0]=addr&0xff;
	drv->MasterTransmit(I2C_ADDR,buffer,1,true);
	while(drv->GetStatus().busy);	
	drv->MasterReceive(I2C_ADDR,buffer,1,false);
	while(drv->GetStatus().busy);	
	if(drv->GetStatus().bus_error) return 0x100;
	return buffer[0];
}
int eeWrite(int addr,char val){
	buffer[0]=addr&0xff;
	buffer[1]=val;
	drv->MasterTransmit(I2C_ADDR,buffer,2,false);
	while(drv->GetStatus().busy);
	if(drv->GetStatus().bus_error) return 0x100;
	return val;
}


void eeInit(void){
	drv=&I2C_DRVIER;
	drv->Initialize(NULL);
	drv->PowerControl(ARM_POWER_FULL);	
	drv->Control(ARM_I2C_BUS_SPEED,ARM_I2C_BUS_SPEED_FAST);	
}