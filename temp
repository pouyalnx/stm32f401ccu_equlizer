/*-----------------------------------------------------------------------------
 * Name:    stdout_user.c
 * Purpose: STDOUT User Template
 * Rev.:    1.0.0
 *-----------------------------------------------------------------------------*/
 
/* Copyright (c) 2013 - 2015 ARM LIMITED
 
   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/
#include "driver_spi.h" 
#include "gpio_lpc17xx.h" 
#include "font.h" 
 
/**
  Put a character to the stdout
 
  \param[in]   ch  Character to output
  \return          The character written, or -1 on write error.
*/




/*********************************************************************//**
 * @brief            user functions
 * @param[in]    
 * @return         
 **********************************************************************/
extern void LCD_INIT(ARM_DRIVER_SPI *drv);
extern void LCD_text_colour(unsigned short colour);
/*********************************************************************//**
 * @brief            
 * @param[in]    
 * @return         
 **********************************************************************/


struct lcd_controller{
	unsigned char x;
	unsigned char y;
	unsigned short colour_text;
}lcd_control_write;

/*********************************************************************//**
 * @brief            
 * @param[in]    
 * @return         
 **********************************************************************/

ARM_DRIVER_SPI *spi_lcd;



void lcd_data_cmd(int mode){
	if(mode) GPIO_PinWrite(1,0,1);
	else     GPIO_PinWrite(1,0,0);
}



void transfer_end(void){
ARM_SPI_STATUS line_stat;
do{
	line_stat=spi_lcd->GetStatus();
}while(line_stat.busy);	
}


void send_cmd(unsigned char cmd,unsigned char *data,int data_length){
int stat=0;
spi_lcd->Control(ARM_SPI_CONTROL_SS,1);		

lcd_data_cmd(0);

spi_lcd->Send(&cmd,1);
transfer_end();
	
lcd_data_cmd(1);	

if(data_length){	
spi_lcd->Send(data,data_length);
transfer_end();
}

spi_lcd->Control(ARM_SPI_CONTROL_SS,0);	
}



/*********************************************************************//**
 * @brief            
 * @param[in]    
 * @return         
 **********************************************************************/
#define LCD_template_colour 0xff00

void LCD_select_page(unsigned char x_s,unsigned char x_size,unsigned char y_s,unsigned char y_size){
	unsigned char data[4];
	data[0]=0;
	data[1]=x_s;
	data[2]=0;
	data[3]=x_s+x_size;
	send_cmd(0x2a,data,4);
	
	data[0]=0;
	data[1]=y_s;
	data[2]=0;
	data[3]=y_s+y_size;
	send_cmd(0x2b,data,4);	
}


void LCD_cls(void){
	unsigned short data[128];
	int gpv0;
	unsigned char cmd;
	for(gpv0=0;gpv0<128;gpv0++)
	data[gpv0]=LCD_template_colour;
	
	LCD_select_page(0,127,0,127);
	spi_lcd->Control(ARM_SPI_CONTROL_SS,1);
	lcd_data_cmd(0);
	cmd=0x2c;
	spi_lcd->Send(&cmd,1);
	
	transfer_end();
	lcd_data_cmd(1);
	for(gpv0=0;gpv0<128;gpv0++){
	spi_lcd->Send(data,256);
	transfer_end();		
	}
	cmd=0;
	spi_lcd->Send(&cmd,1);
	transfer_end();
	
	spi_lcd->Control(ARM_SPI_CONTROL_SS,0);	
}


/*********************************************************************//**
 * @brief            
 * @param[in]    
 * @return         
 **********************************************************************/
void LCD_INIT(ARM_DRIVER_SPI *drv){
unsigned char data[16];	
unsigned int delay;
	
spi_lcd=drv;
spi_lcd->Initialize(NULL);	
spi_lcd->PowerControl(ARM_POWER_FULL);
spi_lcd->Control(ARM_SPI_DATA_BITS(8)|ARM_SPI_MODE_MASTER|ARM_SPI_SS_MASTER_SW,16000000);

	

GPIO_SetDir(1,0,GPIO_DIR_OUTPUT); //act as data cmd
GPIO_SetDir(1,1,GPIO_DIR_OUTPUT); //act as reset	//cs designed by hardware

GPIO_PinWrite(1,1,1);
for(delay=0;delay<1000000;delay++);	
GPIO_PinWrite(1,1,0);	
for(delay=0;delay<1000000;delay++);	
GPIO_PinWrite(1,1,1);		
for(delay=0;delay<1000000;delay++);	


send_cmd(1,0,0);
for(delay=0;delay<1000000;delay++);
send_cmd(0x11,0,0);
for(delay=0;delay<1000000;delay++);

data[0]=1;
data[1]=0x2c;
data[2]=0x2d;
send_cmd(0xb1,data,3);


data[0]=1;
data[1]=0x2c;
data[2]=0x2d;
send_cmd(0xb2,data,3);

data[0]=1;
data[1]=0x2c;
data[2]=0x2d;
data[3]=1;
data[4]=0x2c;
data[5]=0x2d;
send_cmd(0xb3,data,6);

data[0]=0x7;
send_cmd(0xb4,data,1);

data[0]=0xa2;
data[1]=0x2;
data[2]=0x84;
send_cmd(0xc0,data,3);

data[0]=0xc5;
send_cmd(0xc1,data,1);

data[0]=0xa;
data[1]=0;
send_cmd(0xc2,data,2);

data[0]=0x8a;
data[1]=0x2a;
send_cmd(0xc3,data,2);

data[0]=0x8a;
data[1]=0xee;
send_cmd(0xc4,data,2);

data[0]=0xe;
send_cmd(0xc5,data,1);


send_cmd(0x20,data,0);

data[0]=0x8;
send_cmd(0x36,data,1);


data[0]=5;
send_cmd(0x3a,data,1);


send_cmd(0,data,0);

data[0]=0;
data[1]=0;
data[2]=0;
data[3]=127;
send_cmd(0x2a,data,4);


data[0]=0;
data[1]=0;
data[2]=0;
data[3]=127;
send_cmd(0x2b,data,4);



send_cmd(0,data,0);

data[0]=0x2;
data[1]=0x1c;
data[2]=0x7;
data[3]=0x12;
data[4]=0x37;
data[5]=0x32;
data[6]=0x29;
data[7]=0x2d;
data[8]=0x29;
data[9]=0x25;
data[10]=0x2b;
data[11]=0x39;
data[12]=0x0;
data[13]=0x1;
data[14]=0x3;
data[15]=0x10;
send_cmd(0xe0,data,16);
send_cmd(0,data,0);

data[0]=0x3;
data[1]=0x1d;
data[2]=0x7;
data[3]=0x6;
data[4]=0x2e;
data[5]=0x2c;
data[6]=0x29;
data[7]=0x2d;
data[8]=0x2e;
data[9]=0x2e;
data[10]=0x37;
data[11]=0x3f;
data[12]=0x0;
data[13]=0x0;
data[14]=0x2;
data[15]=0x10;
send_cmd(0xe1,data,16);
send_cmd(0,data,0);

send_cmd(0x29,data,0);
for(delay=0;delay<1000000;delay++);
send_cmd(0x13,data,0);
for(delay=0;delay<1000000;delay++);
send_cmd(0,data,0);

lcd_control_write.x=0;
lcd_control_write.y=0;
lcd_control_write.colour_text=0xffff;
}


/*********************************************************************//**
 * @brief            
 * @param[in]    
 * @return         
 **********************************************************************/
void LCD_text_colour(unsigned short colour){
lcd_control_write.colour_text=colour;	
}


int LCD_char_t(unsigned char x,unsigned char y,char ch,unsigned short text_colour){
unsigned short plate[88];
int gpv0,gpv1,gpv2,template_font;
unsigned char write_cmd;	
gpv1=0;
	
gpv2=11*(ch-0x20);
for(gpv0=0;gpv0<11;gpv0++){
template_font=consolas_bold[gpv0+gpv2];	

if(template_font & 1) plate[gpv1]=text_colour;
else                  plate[gpv1]=LCD_template_colour;
gpv1++;

if(template_font & 2) plate[gpv1]=text_colour;
else                  plate[gpv1]=LCD_template_colour;
gpv1++;

if(template_font & 4) plate[gpv1]=text_colour;
else                  plate[gpv1]=LCD_template_colour;
gpv1++;

if(template_font & 8) plate[gpv1]=text_colour;
else                  plate[gpv1]=LCD_template_colour;
gpv1++;	
	
if(template_font & 16) plate[gpv1]=text_colour;
else                   plate[gpv1]=LCD_template_colour;
gpv1++;	

if(template_font & 32) plate[gpv1]=text_colour;
else                   plate[gpv1]=LCD_template_colour;
gpv1++;

if(template_font & 64) plate[gpv1]=text_colour;
else                   plate[gpv1]=LCD_template_colour;
gpv1++;

if(template_font & 128) plate[gpv1]=text_colour;
else                    plate[gpv1]=LCD_template_colour;
gpv1++;

}

LCD_select_page(x,7,y,11);

spi_lcd->Control(ARM_SPI_CONTROL_SS,1);	
lcd_data_cmd(0);
write_cmd=0x2c;
spi_lcd->Send(&write_cmd,1);
transfer_end();

lcd_data_cmd(1);
spi_lcd->Send(plate,176);
transfer_end();

lcd_data_cmd(0);
write_cmd=0;
spi_lcd->Send(&write_cmd,1);
transfer_end();
	spi_lcd->Control(ARM_SPI_CONTROL_SS,0);	
return ch;	
}





/*********************************************************************//**
 * @brief            
 * @param[in]    
 * @return         
 **********************************************************************/


void LCD_chat_t(char ch){
	
	if(lcd_control_write.y>120){
			lcd_control_write.x=0;
			lcd_control_write.y=0;		
	    LCD_cls();
	}
	
	if(ch>0x19 && ch<0x7f){
		LCD_char_t(lcd_control_write.x,lcd_control_write.y,ch,lcd_control_write.colour_text);
	  lcd_control_write.x+=8;
		if(lcd_control_write.x>120){
			lcd_control_write.x=0;
			lcd_control_write.y+=11;
		}
	}
	else{
		switch(ch){
			case '\n':
      case '\t':
			lcd_control_write.x=0;
			lcd_control_write.y+=11;
      break;			
		}
	}	
}

/*********************************************************************//**
 * @brief            
 * @param[in]    
 * @return         
 **********************************************************************/

int stdout_putchar (int ch) {
  LCD_chat_t(ch);
  return (ch);
}
 
