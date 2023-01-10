#include "Driver_I2C.h"
#include "cmsis_os2.h"




extern int stdin_getchar (void);
extern int stdin_read(void);
extern void stdin_init(void);



//////////////////////////////////////////////////////////////////////////////;

#define mpr121_address 0X5A 

extern ARM_DRIVER_I2C Driver_I2C1;
ARM_DRIVER_I2C *mpr121;

//////////////////////////////////////////////////////////////////////////////
#define TS1		0x00 
#define TS2 		0x01 
#define OORS1 		0x02 
#define OORS2 		0x03 

/* filtered data */
#define E0FDL 		0x04 
#define E0FDH 		0x05 
#define E1FDL 		0x06 
#define E1FDH 		0x07 
#define E2FDL 		0x08 
#define E2FDH 		0x09 
#define E3FDL 		0x0A 
#define E3FDH 		0x0B 
#define E4FDL 		0x0C 
#define E4FDH 		0x0D 
#define E5FDL 		0x0E 
#define E5FDH 		0x0F 
#define E6FDL 		0x10 
#define E6FDH 		0x11 
#define E7FDL 		0x12 
#define E7FDH 		0x13 
#define E8FDL 		0x14 
#define E8FDH 		0x15 
#define E9FDL 		0x16 
#define E9FDH 		0x17 
#define E10FDL 		0x18 
#define E10FDH 		0x19 
#define E11FDL 		0x1A 
#define E11FDH 		0x1B 
#define E12FDL 		0x1C 
#define E12FDH 		0x1D 

/* baseline values */
#define E0BV 		0x1E 
#define E1BV 		0x1F 
#define E2BV 		0x20 
#define E3BV 		0x21 
#define E4BV 		0x22 
#define E5BV 		0x23 
#define E6BV 		0x24 
#define E7BV 		0x25 
#define E8BV 		0x26 
#define E9BV 		0x27 
#define E10BV 		0x28 
#define E11BV 		0x29 
#define E12BV 		0x2A 

/* general electrode touch sense baseline filters */
/* rising filter */
#define MHDR 		0x2B 
#define NHDR 		0x2C 
#define NCLR 		0x2D 
#define FDLR 		0x2E 

/* falling filter */
#define MHDF 		0x2F 
#define NHDF 		0x30 
#define NCLF 		0x31 
#define FDLF 		0x32 

/* touched filter */
#define NHDT 		0x33 
#define NCLT 		0x34 
#define FDLT 		0x35 

/* proximity electrode touch sense baseline filters */
/* rising filter */
#define MHDPROXR 	0x36 
#define NHDPROXR 	0x37 
#define NCLPROXR 	0x38 
#define FDLPROXR 	0x39 

/* falling filter */
#define MHDPROXF 	0x3A 
#define NHDPROXF 	0x3B 
#define NCLPROXF 	0x3C 
#define FDLPROXF 	0x3D 

/* touched filter */
#define NHDPROXT 	0x3E 
#define NCLPROXT 	0x3F 
#define FDLPROXT 	0x40 

/* electrode touch and release thresholds */
#define E0TTH 		0x41 
#define E0RTH 		0x42 
#define E1TTH 		0x43 
#define E1RTH 		0x44 
#define E2TTH 		0x45 
#define E2RTH 		0x46 
#define E3TTH 		0x47 
#define E3RTH 		0x48 
#define E4TTH 		0x49 
#define E4RTH 		0x4A 
#define E5TTH 		0x4B 
#define E5RTH 		0x4C 
#define E6TTH 		0x4D 
#define E6RTH 		0x4E 
#define E7TTH 		0x4F 
#define E7RTH 		0x50 
#define E8TTH 		0x51 
#define E8RTH 		0x52 
#define E9TTH 		0x53 
#define E9RTH 		0x54 
#define E10TTH 		0x55 
#define E10RTH 		0x56 
#define E11TTH 		0x57 
#define E11RTH 		0x58 
#define E12TTH 		0x59 
#define E12RTH 		0x5A 

/* debounce settings */
#define DTR 		0x5B 

/* configuration registers */
#define AFE1 		0x5C 
#define AFE2 		0x5D 
#define ECR 		0x5E 

/* electrode currents */
#define CDC0		0x5F
#define CDC1 		0x60
#define CDC2 		0x61
#define CDC3 		0x62
#define CDC4 		0x63
#define CDC5 		0x64
#define CDC6 		0x65
#define CDC7 		0x66
#define CDC8 		0x67
#define CDC9 		0x68
#define CDC10 		0x69
#define CDC11 		0x6A
#define CDC12 		0x6B

/* electrode charge times */
#define CDT01 		0x6C
#define CDT23 		0x6D
#define CDT45 		0x6E
#define CDT67 		0x6F
#define CDT89 		0x70
#define CDT1011 	0x71
#define CDT11 		0x72

/* GPIO */
#define CTL0		0x73
#define CTL1 		0x74
#define DAT 		0x75
#define DIR 		0x76
#define EN 		0x77
#define SET 		0x78
#define CLR 		0x79
#define TOG 		0x7A

/* auto-config */
#define ACCR0		0x7B
#define ACCR1		0x7C
#define USL		0x7D
#define LSL		0x7E
#define TL		0x7F

/* soft reset */
#define SRST		0x80

/* PWM */
#define PWM0		0x81
#define PWM1		0x82
#define PWM2		0x83
#define PWM3 		0x84
//////////////////////////////////////////////////////////////////////////////////////////


static void mpr121_send(unsigned char *in,int size,int continue_req){
	mpr121->MasterTransmit(mpr121_address,in,size,continue_req);
	while(mpr121->GetStatus().busy);
}

static void mpr121_read(unsigned char *in,int size,int continue_req){
	mpr121->MasterReceive(mpr121_address,in,size,continue_req);
	while(mpr121->GetStatus().busy);
}


void stdin_init(void){

	unsigned int gpv0;
	unsigned char send_buffer[4];
	mpr121=&Driver_I2C1;
	
	mpr121->Initialize(NULL);
   mpr121->PowerControl(ARM_POWER_FULL);	
   mpr121->Control(ARM_I2C_BUS_SPEED,ARM_I2C_BUS_SPEED_FAST);	

	
      #define write_register(x,y) send_buffer[0]=x;send_buffer[1]=y;mpr121_send(send_buffer,2,false)
	
      #define NUM_OF_ELECTRODES	13



     /* turn off all electrodes to stop */
		write_register(ECR, 0x00); 

		/* write register with initial values */
		write_register(MHDR, 0x01);
		write_register(NHDR, 0x01);
		write_register(NCLR, 0x10);
		write_register(FDLR, 0x20);
		write_register(MHDF, 0x01);
		write_register(NHDF, 0x01);
		write_register(NCLF, 0x10);
		write_register(FDLF, 0x20);
		write_register(NHDT, 0x01);
		write_register(NCLT, 0x10);
		write_register(FDLT, 0xFF);
		write_register(MHDPROXR, 0x0F);
		write_register(NHDPROXR, 0x0F);
		write_register(NCLPROXR, 0x00);
		write_register(FDLPROXR, 0x00);
		write_register(MHDPROXF, 0x01);
		write_register(NHDPROXF, 0x01);
		write_register(NCLPROXF, 0xFF);
		write_register(FDLPROXF, 0xFF);
		write_register(NHDPROXT, 0x00);
		write_register(NCLPROXT, 0x00);
		write_register(FDLPROXT, 0x00);
	   write_register(DTR, 0x33);
		

		write_register(AFE1,0x20);//*((unsigned char*)&afe1));  
		write_register(AFE2,0x10);//*((unsigned char*)&afe2));
		
		write_register(ACCR0, 0x00);
		write_register(ACCR1, 0x00);
		write_register(USL, 0x00); 
		write_register(LSL, 0x00); 
		write_register(TL, 0x00); 
		

		/* apply next setting for all electrodes */
		for (gpv0 = 0; gpv0 < NUM_OF_ELECTRODES; gpv0++) {
			write_register((E0TTH + (gpv0<<1)), 40); 
			write_register((E0RTH + (gpv0<<1)), 40); 
		}

   		write_register(ECR, 0xcf);

}


static int mpr121_read_touch(void){
	unsigned char buffer[4];
	buffer[0]=0;
	buffer[1]=0;
	mpr121_send(buffer,1,true);
	mpr121_read(buffer,2,false);
	return buffer[1]<<8|buffer[0];
}


int stdin_getchar (void) {
  int stat;
	while(1){
	stat=mpr121_read_touch();

		if(stat & 1) return '0';
	if(stat & 2) return '1';
	if(stat & 4) return '2';
	if(stat & 8) return '3';
	if(stat & 16) return '4';
	if(stat & 32) return '5';
	if(stat & 64) return '6';
	if(stat & 128) return '7';
	if(stat & 256) return '8';
	if(stat & 512) return '9';	
	if(stat & 1024) return '\n';
	if(stat & 2048) return '\b';

	osDelay(2);
	}
	
}

int stdin_read(void){
  int stat;
	stat=mpr121_read_touch();
	if(stat & 1) return '0';
	if(stat & 2) return '1';
	if(stat & 4) return '2';
	if(stat & 8) return '3';
	if(stat & 16) return '4';
	if(stat & 32) return '5';
	if(stat & 64) return '6';
	if(stat & 128) return '7';
	if(stat & 256) return '8';
	if(stat & 512) return '9';	
	if(stat & 1024) return '\n';
	if(stat & 2048) return '\b';
	return 0;
}
