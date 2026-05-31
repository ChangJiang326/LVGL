#include "spi.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"

unsigned char CCLK; // LT758 Core Clock
unsigned char MCLK; // SDRAM Clock
unsigned char SCLK; // LCD Scan Clock
 unsigned long Bcolor;
unsigned long Fcolor;


unsigned long Current_Layer,Draw_Layer;
unsigned char Lcd_Fre=35;
unsigned char Flash_Type=0;
volatile unsigned long lcd_init_stage=0;

void IOSET(uchar io)
{
	 GPIO_InitTypeDef GPIO_Initure;

	
if(io==0)
{

    GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;                //
    GPIO_Initure.Mode=GPIO_MODE_INPUT;      
    GPIO_Initure.Pull=GPIO_NOPULL ;
	GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH ;
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
	}
else
{
    GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;                //
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;      
    GPIO_Initure.Pull=GPIO_PULLUP ;
	GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH ;
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);

	}
}

 uchar SPI_ShiftByte(uchar _ucByte)
{
return SPI2_ReadWriteByte(_ucByte);
	
} 
void LCD_CmdWrite(uchar _ucRegAddr)
{
	
	#ifdef SPI
	SPI_CS=0;
SPI_ShiftByte(SPI_WRITE_CMD);
	SPI_ShiftByte(_ucRegAddr);
	SPI_CS=1;
//printf("0x%02x,0x%02x, ",SPI_WRITE_CMD, (uchar)_ucRegAddr);
#endif	
#ifdef P_8BIT
DATA_BUS&=0XFF00;
	  rd1;
  cs0;
  rs0;
  DATA_BUS |= _ucRegAddr;
  wr0;
  wr1;
  cs1;
  //DATA_BUS = 0xff;
  rs1;
#endif	
#ifdef FSMC_16BIT
	 LCD_CmdWrite_FSMC(_ucRegAddr);
#endif	
}

 void LCD_DataWrite(uint _ucRegValue)
{
		#ifdef SPI
	SPI_CS=0;
	SPI_ShiftByte(SPI_WRITE_DATA);
	SPI_ShiftByte(_ucRegValue);
	SPI_CS=1;
//printf("0x%02x,0x%02x, ",SPI_WRITE_DATA, (uchar)_ucRegValue);
#endif	
#ifdef P_8BIT
DATA_BUS&=0XFF00;
     rd1;       
  cs0;
  rs1;
  DATA_BUS |= _ucRegValue;
  wr0;
  wr1;
  cs1;
  //DATA_BUS = 0xff;
  rs0;
#endif	
	#ifdef FSMC_16BIT
	LCD_DataWrite_FSMC(_ucRegValue);
#endif	
}
 void LCD_DataWrite_Fast(uint _ucRegValue)
{
		#ifdef SPI
	SPI_CS=0;
	SPI_ShiftByte(SPI_WRITE_DATA);
	SPI_ShiftByte(_ucRegValue);
	SPI_CS=1;

#endif	
#ifdef P_8BIT
DATA_BUS&=0XFF00;
     rd1;       
  cs0;
  rs1;
  DATA_BUS |= _ucRegValue;
  wr0;
  wr1;
  cs1;
  //DATA_BUS = 0xff;
  rs0;
#endif	
	#ifdef FSMC_16BIT
	LCD_DataWrite_FSMC(_ucRegValue);
#endif	
}


 uchar LCD_DataRead(void)
{

    uint value=0;
			#ifdef SPI
	SPI_CS=0;
	SPI_ShiftByte(SPI_READ_DATA);
	value = SPI_ShiftByte(0x00);
	SPI_CS=1;
#endif	
#ifdef P_8BIT
IOSET(0);
  wr1;
  cs0;
  rs1;
  rd0;
  
  value= RDATA&0X00FF;
 // value= RDATA&0X00FF;
  rd1;
  cs1;
  rs0; 
IOSET(1);// DATA_BUS=0xffff;
#endif	
	#ifdef FSMC_16BIT
value=LCD_DataRead_FSMC();
#endif	

 	return value;
}
 uchar LCD_StatusRead(void)
{	uchar value=0;
		#ifdef SPI
		SPI_CS=0;
	SPI_ShiftByte(SPI_READ_STATUS);
	value = SPI_ShiftByte(0x00);
	SPI_CS=1;
#endif	
#ifdef P_8BIT
  IOSET(0);
  cs0;
  rs0;
  wr1;
  rd1;
  rd0;

 value= RDATA;
  value= RDATA;
   value= RDATA;
  rd1;
  cs1;
  IOSET(1);
  rs1; //DATA_BUS = 0xff;
#endif	
	#ifdef FSMC_16BIT
value=LCD_StatusRead_FSMC();
#endif	
return value;

}
void LCD_Reg_Write(uchar reg,uchar value)
{
LCD_CmdWrite(reg);
LCD_DataWrite(value);
}
uchar LCD_Reg_Read(uchar reg)
{
LCD_CmdWrite(reg);
return  LCD_DataRead();
}

///////////////////////以上为基本读写函数，请客户根据不同单片机，更改以上函数///////////////////





 void  Write_Dir(uchar _ucRegAddr, uchar _ucRegValue)
{
	
  LCD_CmdWrite(_ucRegAddr);
  LCD_DataWrite(_ucRegValue);

  }

void Chk_DMA_Busy(void)
{
	uchar temp; 	
	do
	{
	LCD_CmdWrite(0xBF);
	temp =LCD_DataRead();
	} while((temp&0x01)==0x01);  

}
void Select_LCD_Sync_Mode(void)
{
/*
To Control panel's synchronous signals
0: Sync Mode: Enable XVSYNC, XHSYNC, XDE
1: DE Mode: Only XDE enable, XVSYNC & XHSYNC in idle state
*/
	unsigned char temp;
	LCD_CmdWrite(0x10);
	temp = LCD_DataRead();
    temp &= cClrD0;
	LCD_DataWrite(temp);
}
void Select_LCD_DE_Mode(void)
{
/*
To Control panel's synchronous signals
0: Sync Mode: Enable XVSYNC, XHSYNC, XDE
1: DE Mode: Only XDE enable, XVSYNC & XHSYNC in idle state
*/
	unsigned char temp;
	LCD_CmdWrite(0x10);
	temp = LCD_DataRead();
    temp |= cSetD0;
	LCD_DataWrite(temp);
}
void LCD_Reset(void)
{
   LcdReset=0;
	delay_ms(200);
   LcdReset=1;
	delay_ms(200);
}
void LCD_640x480_Init(void)
{

LCD_Reset();
Lcd_Fre=30;
LCD_PLL_Initial();
  LCD_SDRAM_initail(MCLK);
	TFT_24bit();
	Host_Bus_8bit();
	RGB_16b_16bpp();
	MemWrite_Left_Right_Top_Down();
	Graphic_Mode();
	Memory_Select_SDRAM();
 	// Select_LCD_Sync_Mode();	
 Select_LCD_DE_Mode();
   //PCLK_Falling();
	 PCLK_Rising();
	VSCAN_T_to_B();
	PDATA_Set_RGB();
	HSYNC_Low_Active();
	//HSYNC_High_Active();
	VSYNC_Low_Active();
	//VSYNC_High_Active();
	
	
DE_High_Active();
//		DE_Low_Active();
/*
LCD_HorizontalWidth_VerticalHeight(640,480);
		LCD_Horizontal_Non_Display(134);//INNOLUX800x48046
		LCD_HSYNC_Start_Position(16);//INNOLUX800x48016~354
		LCD_HSYNC_Pulse_Width(10);//INNOLUX800x4801~40
		LCD_Vertical_Non_Display(11);//INNOLUX800x48023
		LCD_VSYNC_Start_Position(32);//INNOLUX800x4801~147
		LCD_VSYNC_Pulse_Width(2);//INNOLUX800x4801~20
*/
}
void LCD_1024x600_Init(void)
{

LCD_Reset();
LCD_PLL_Initial();
  LCD_SDRAM_initail(MCLK);
	TFT_24bit();
#ifdef FSMC_16BIT
	Host_Bus_16bit();
#else
	Host_Bus_8bit();
#endif
	RGB_16b_16bpp();
	MemWrite_Left_Right_Top_Down();
	Graphic_Mode();
	Memory_Select_SDRAM();
	Select_LCD_DE_Mode();
   //Select_LCD_Sync_Mode();	
	PCLK_Falling();
	//PCLK_Rising();
	HSCAN_L_to_R();
	VSCAN_T_to_B();
	PDATA_Set_RGB();

	HSYNC_Low_Active();
	VSYNC_Low_Active();
	DE_High_Active();

}
void LCD_800x480_Init(void)
{

LCD_Reset();

LCD_PLL_Initial();
  LCD_SDRAM_initail(MCLK);

	Host_Bus_8bit();

	MemWrite_Left_Right_Top_Down();

	Memory_Select_SDRAM();
TFT_24bit();
	Memory_XY_Mode();
	Memory_16bpp_Mode();
	Select_Main_Window_16bpp();
RGB_16b_16bpp();

	 //Select_LCD_DE_Mode();
  Select_LCD_Sync_Mode();	
    PCLK_Falling();
	//PCLK_Rising();
	VSCAN_T_to_B();
	PDATA_Set_RGB();
	HSYNC_Low_Active();
	VSYNC_Low_Active();
	
	//HSYNC_High_Active();
	//VSYNC_High_Active();
	DE_High_Active();
//	DE_Low_Active();

}
void LCD_480x272_Init(void)
{

LCD_Reset();
Lcd_Fre=10;
LCD_PLL_Initial();
  LCD_SDRAM_initail(MCLK);
	
	TFT_24bit();
	Host_Bus_8bit();

	MemWrite_Left_Right_Top_Down();
	Graphic_Mode();
	Memory_Select_SDRAM();
	//Select_LCD_DE_Mode();
   Select_LCD_Sync_Mode();	
    PCLK_Falling();
	//PCLK_Rising();
	VSCAN_T_to_B();//横屏设置
	//VSCAN_B_to_T();//竖屏设置
	//Font_90_degree();//竖屏设置
	PDATA_Set_RGB();
	HSYNC_Low_Active();
	VSYNC_Low_Active();
	DE_High_Active();
	//DE_Low_Active();




}
void LCD_320x240_Init(void)
{

LCD_Reset();
Lcd_Fre=15;
LCD_PLL_Initial();
  LCD_SDRAM_initail(MCLK);

//Color_Bar_ON();
// Set_Backlight(100);


	
	Display_ON(); 
	TFT_24bit();
	Host_Bus_8bit();
	RGB_16b_16bpp();
	MemWrite_Left_Right_Top_Down();
	Graphic_Mode();
	Memory_Select_SDRAM();
   Select_LCD_Sync_Mode();	
   	PCLK_Rising();	
   // PCLK_Falling();
	VSCAN_T_to_B();
	PDATA_Set_RGB();
	
//**[13h]**//
	HSYNC_Low_Active();
	VSYNC_Low_Active();
//	DE_High_Active();
	DE_Low_Active();
/*
LCD_HorizontalWidth_VerticalHeight(320,240);
LCD_Horizontal_Non_Display(68);
LCD_HSYNC_Start_Position(18);
LCD_HSYNC_Pulse_Width(2);
LCD_Vertical_Non_Display(18);
LCD_VSYNC_Start_Position(2);
LCD_VSYNC_Pulse_Width(2);
*/
}
void screeninit(void)
{	 Current_Layer=Layer0;Draw_Layer=Layer0;


	LCD_HorizontalWidth_VerticalHeight(lcdx, lcdy);
	LCD_Horizontal_Non_Display(LCD_HBPD);
	LCD_HSYNC_Start_Position(LCD_HFPD);
	LCD_HSYNC_Pulse_Width(LCD_HSPW);
	LCD_Vertical_Non_Display(LCD_VBPD);
	LCD_VSYNC_Start_Position(LCD_VFPD);
	LCD_VSYNC_Pulse_Width(LCD_VSPW);

/*
	LCD_HorizontalWidth_VerticalHeight(800,480);
LCD_Horizontal_Non_Display(46);//INNOLUX800x48046
		LCD_HSYNC_Start_Position(210);//INNOLUX800x48016~354
		LCD_HSYNC_Pulse_Width(10);//INNOLUX800x4801~40
		LCD_Vertical_Non_Display(23);//INNOLUX800x48023
		LCD_VSYNC_Start_Position(22);//INNOLUX800x4801~147
		LCD_VSYNC_Pulse_Width(10);//INNOLUX800x4801~20
*/
		#ifdef FSMC_16BIT
		Host_Bus_16bit();//用FSMC16位模式，需要将总线设置成16位，寄存器操作都是用低8位的，只是颜色寄存器是用16位
	#endif			

	Canvas_Image_Start_address(Current_Layer);
	Canvas_image_width(lcdx);
	
	Active_Window_XY(0,0);//write/draw
	Active_Window_WH(lcdx,lcdy);
	
	
	Main_Image_Start_Address(Current_Layer);				
	Main_Image_Width(lcdx);	


	Main_Window_Start_XY(0,0);	


TFT_24bit();
	Memory_XY_Mode();
	Memory_16bpp_Mode();
	Select_Main_Window_16bpp();
RGB_16b_16bpp();
/*

			Select_PWM1_Clock_Divided_By_1();
Select_PWM1();
	Set_Timer1_Count_Buffer(52000); 
    Set_Timer1_Compare_Buffer(20000);

		  Font_Width_X1();
      Font_Height_X1();
					Foreground_color_16M(Fcolor);
Background_color_16M(Bcolor);
*/
	}
void LCD_Init(void)
{
lcd_init_stage=1;

#ifdef LCD_320240
LCD_320x240_Init();//分辨率320*240的初始化函数
#endif	
#ifdef LCD43inch_480272
LCD_480x272_Init();//分辨率480*272的初始化函数
#endif	
#ifdef LCD50inch_480272
LCD_480x272_Init();//分辨率480*272的初始化函数
#endif	
#ifdef  LCD_640480
LCD_640x480_Init();//分辨率640*480的初始化函数
#endif	
#ifdef  LCD50inch_800480
LCD_800x480_Init();//分辨率800*480的初始化函数
#endif	
#ifdef LCD70inch_800480
LCD_800x480_Init();//分辨率800*480的初始化函数
#endif	
#ifdef  LCD50inch_1024600
lcd_init_stage=10;
LCD_1024x600_Init();//分辨率1024*600的初始化函数
lcd_init_stage=11;
#endif	
#ifdef  LCD70inch_1024600
LCD_1024x600_Init();//分辨率1024*600的初始化函数
lcd_init_stage=11;
#endif	
#ifdef LCD102inch_1024600
LCD_1024x600_Init();//分辨率1024*600的初始化函数
lcd_init_stage=11;
#endif	
		#ifdef NAND_FLASH
LCD_Reg_Write(0xb7,LCD_Reg_Read(0xb7) | 0x40);
	#endif	

	screeninit();
lcd_init_stage=20;
lcd_init_stage=21;
/* LVGL uses built-in fonts; external font flash init can block SFI DMA here. */
/* Init_Font(); */
lcd_init_stage=99;

}

void Enable_PLL(void)
{
	unsigned char temp;
	LCD_CmdWrite(0x01);
	temp = LCD_DataRead();
	temp |= cSetD7;
	LCD_DataWrite(temp);

  delay_us(100);


}
void Disable_PLL(void)
{
/*  0: PLL disable; allow change PLL parameter.
    1: PLL enable; cannot change PLL parameter.*/
	unsigned char temp;
	LCD_CmdWrite(0x01);
	temp = LCD_DataRead();
	temp &= cClrD7;
	LCD_DataWrite(temp);

    delay_us(100);
	//while( LCD_RegisterRead(0x01)&0x80 );//check PLL was disabled.  
}
 void LCD_PLL_Initial(void) 
{
	unsigned int PCLK_M = 11;
	unsigned char PCLK_N = 1;
	unsigned char PCLK_OD = 2;
	unsigned char PCLK_Ext_ODR = 0;
	
	// 23-138MHZ,27-162MHZ,30-180MHZ,32-192MHZ;
	 unsigned int MCLK_M = 30; // only for nor flash
	//unsigned int MCLK_M = 27; 	 // for nand flash
	unsigned char MCLK_N = 1;
	unsigned char MCLK_OD = 1;
	
	// 23-138MHZ,27-162MHZ,30-180MHZ,32-192MHZ;
	// unsigned int CCLK_M = 28; // only for nor flash
	unsigned int CCLK_M = 25; 	 // for nand flash
	unsigned char CCLK_N = 1;
	unsigned char CCLK_OD = 1;

	unsigned char tempReg;
	unsigned long temp = 0;
	unsigned int temp1 = 0;

	MCLK = MCLK_M * 6;
	
	temp = (LCD_HBPD + LCD_HFPD + LCD_HSPW + lcdx) * (LCD_VBPD + LCD_VFPD + LCD_VSPW + lcdy) * 60;
	temp1 = temp/1000000;
	PCLK_M = temp1/3;
	if((temp1%3) == 2)
		PCLK_M++;
	

	{
		tempReg = ((PCLK_OD & 0x03) << 6) | ((PCLK_Ext_ODR & 0x01) << 5) | ((PCLK_N & 0x0F) << 1);
		LCD_Reg_Write(0x05, tempReg);
		LCD_Reg_Write(0x06, PCLK_M & 0xFF);
	}


	{
		tempReg = ((MCLK_OD & 0x03) << 6) | ((MCLK_N & 0x0F) << 1);
		LCD_Reg_Write(0x07, tempReg);
		LCD_Reg_Write(0x08, MCLK_M & 0xFF);
	}


	{
		tempReg = ((CCLK_OD & 0x03) << 6) | ((CCLK_N & 0x0F) << 1);
		LCD_Reg_Write(0x09, tempReg);
		LCD_Reg_Write(0x0A, CCLK_M & 0xFF);
	}

	tempReg = LCD_Reg_Read(0x00);
	LCD_Reg_Write(0x00, tempReg | 0x80);

	while ((LCD_Reg_Read(0x00) & 0x80) == 0)
		delay_us(100);

/*

	LCD_CmdWrite(0x05);
	LCD_DataWrite(0xc2);
	LCD_CmdWrite(0x07);
	LCD_DataWrite(0x42);
	LCD_CmdWrite(0x09);
	LCD_DataWrite(0x42);

	LCD_CmdWrite(0x06);
	LCD_DataWrite(0x04);
	LCD_CmdWrite(0x08);
	LCD_DataWrite(0x1b);
	LCD_CmdWrite(0x0a);
	LCD_DataWrite(0x19);
      
	LCD_CmdWrite(0x00);
	delay_us(100);
	LCD_DataWrite(0x80);
*/
delay_ms(10);

}

void LCD_SDRAM_initail(unsigned char mclk)
{

uint8_t tempReg = 0;
	unsigned short sdram_itv;

	LCD_Reg_Write(0xe0, 0x29);
	LCD_Reg_Write(0xe1, 0x03); // CAS:2=0x02--768CAS:3=0x03--758
	sdram_itv = (64000 * mclk) / 4096;

	LCD_Reg_Write(0xe2, sdram_itv);
	LCD_Reg_Write(0xe3, sdram_itv >> 8);

	tempReg = LCD_Reg_Read(0xE4);

	tempReg |= (0x00 | 0x04);

	LCD_Reg_Write(0xE4, tempReg); 
	LCD_Reg_Write(0xE0, 0x0);
	LCD_Reg_Write(0xE1, (8 << 4) | 7);
	LCD_Reg_Write(0xE2, (4 << 4) | 0);
	LCD_Reg_Write(0xE3, (2 << 4) | 6);
	LCD_Reg_Write(0xE4, tempReg & 0xFB); 
	

	tempReg = LCD_Reg_Read(0xE4);
	LCD_Reg_Write(0xE4, tempReg | 0x01);
	Check_SDRAM_Ready();
delay_ms(10);
/*
  Write_Dir(0xe0,0x29);	
	Write_Dir(0xe1,0x03);
	Write_Dir(0xe2,0xe3);
	Write_Dir(0xe3,0x09);
	Write_Dir(0xe4,0x11);

	Check_SDRAM_Ready();	  
 */
delay_ms(100);

}
void Check_Mem_WR_FIFO_not_Full(void)
{
/*	0: Memory Write FIFO is not full.
	1: Memory Write FIFO is full.		*/
	do
	{
		
	}while( LCD_StatusRead()&0x80 );
}
void Check_Mem_WR_FIFO_Empty(void)
{
/*	0: Memory Write FIFO is not empty.
	1: Memory Write FIFO is empty.		*/	
	do
	{

	}while( (LCD_StatusRead()&0x40) == 0x00 );
}
void Check_Mem_RD_FIFO_not_Full(void)
{
/*	0: Memory Read FIFO is not full.
	1: Memory Read FIFO is full.		*/
	do
	{

	}while( LCD_StatusRead()&0x20 );
}
void Check_Mem_RD_FIFO_not_Empty(void)
{
/*	0: Memory Read FIFO is not empty.
	1: Memory Read FIFO is empty.
		*/
	do
	{

	}while( LCD_StatusRead()&0x10 );
}
void Check_2D_Busy(void)
{
  do
 {	   
 }while( LCD_StatusRead()&0x08 );
     
}
void Check_SDRAM_Ready(void)
{
/*	0: SDRAM is not ready for access
	1: SDRAM is ready for access		*/	
	unsigned char temp; 	
	do
	{	
		temp=LCD_StatusRead();   	
	}while( (temp&0x04) == 0x00 );
}


unsigned char Power_Saving_Status(void)
{
  	  unsigned char temp;

	  if((LCD_StatusRead()&0x02)==0x02)
	    temp = 1;
	  else
	    temp = 0;

	  return temp;
}

 
void Check_Power_is_Normal(void)//琌 Normal mode 碞瞒秨
{
/*	0: Normal operation state
	1: Power saving state		*/	
	do
	{

	}while( LCD_StatusRead()&0x02 );
}
void Check_Power_is_Saving(void)//琌 Saving mode 碞瞒秨
{
/*	0: Normal operation state
	1: Power saving state		*/	
	do
	{

	}while( (LCD_StatusRead()&0x02) == 0x00 );
}
void Check_NO_Interrupt(void)//⊿Τ Interrupt 碞瞒秨
{
/*	0: without interrupt event
	1: interrupt event occur
		*/	
	do
	{

	}while( LCD_StatusRead()&0x01 );
}
void Check_Interrupt_Occur(void)//Τ Interrupt 碞瞒秨
{
/*	0: without interrupt event
	1: interrupt event occur
		*/	
	do
	{

	}while( (LCD_StatusRead()&0x01) == 0x00 );
}

void Check_Busy_Draw(void)
{
unsigned char temp;

//(0)
//	delay_ms(10);
	
	
//(A)
	do
	{
		temp=LCD_StatusRead();
	}	while(temp&0x08);
/*
//(B)
	do
	{
		LCD_CmdWrite(0x67);
		temp=LCD_DataRead();
	}	while(temp&0x80);

//(C)
	do
	{
		//(1)
		temp=LCD_RegisterRead(0x76);
		//(2)
		//LCD_CmdWrite(0x76);
		//temp=LCD_DataRead();
	}	while(temp&0x80);
*/

//delay_ms(10);
}

//[00h]=========================================================================

void LCD_SW_Reset(void)//IC Resetぃ穦睲Register/Memory
{
	unsigned char temp;

	LCD_CmdWrite(0x00);
	temp = LCD_DataRead();
	temp |= 0x01;
	LCD_DataWrite(temp);

    do
    {
	    temp = LCD_DataRead();
    }
    while( temp&0x01 );
}

//[01h][01h][01h][01h][01h][01h][01h][01h][01h][01h][01h][01h][01h][01h][01h][01h]

void LCD_Sleep(void)
{
/*  0: Normal mode.
    1: Sleep mode.*/
	unsigned char temp;
	LCD_CmdWrite(0x01);
	temp = LCD_DataRead();
	temp |= cSetD6;
	LCD_DataWrite(temp);
}
void LCD_WakeUp(void)
{
/*  0: Normal mode.
    1: Sleep mode.*/
	unsigned char temp;
	LCD_CmdWrite(0x01);
	temp = LCD_DataRead();
	temp &= cClrD6;
	LCD_DataWrite(temp);
}
void Key_Scan_Enable(void)
{
/*  0: Disable.
    1: Enable.*/
	unsigned char temp;
	LCD_CmdWrite(0x01);
	temp = LCD_DataRead();
	temp |= cSetD5;
	LCD_DataWrite(temp);    
}
void Key_Scan_Disable(void)
{
/*  0: Disable.
    1: Enable.*/
	unsigned char temp;
	LCD_CmdWrite(0x01);
	temp = LCD_DataRead();
	temp &= cClrD5;
	LCD_DataWrite(temp);    
}
void TFT_24bit(void)
{
/*  00b: 24-bits output.
    01b: 18-bits output, unused pins are set as GPIO.
    10b: 16-bits output, unused pins are set as GPIO.
    11b: LVDS, all 24-bits unused output pins are set as GPIO.*/
	unsigned char temp;
	LCD_CmdWrite(0x01);
	temp = LCD_DataRead();
	temp &= cClrD4;
    temp &= cClrD3;
	LCD_DataWrite(temp);    
}
void TFT_18bit(void)
{
/*  00b: 24-bits output.
    01b: 18-bits output, unused pins are set as GPIO.
    10b: 16-bits output, unused pins are set as GPIO.
    11b: LVDS, all 24-bits unused output pins are set as GPIO.*/
	unsigned char temp;
	LCD_CmdWrite(0x01);
	temp = LCD_DataRead();
	temp &= cClrD4;
    temp |= cSetD3;
	LCD_DataWrite(temp);  
}
void TFT_16bit(void)
{
/*  00b: 24-bits output.
    01b: 18-bits output, unused pins are set as GPIO.
    10b: 16-bits output, unused pins are set as GPIO.
    11b: LVDS, all 24-bits unused output pins are set as GPIO.*/
	unsigned char temp;
	LCD_CmdWrite(0x01);
	temp = LCD_DataRead();
	temp |= cSetD4;
    temp &= cClrD3;
	LCD_DataWrite(temp);  
}
void TFT_LVDS(void)
{
/*  00b: 24-bits output.
    01b: 18-bits output, unused pins are set as GPIO.
    10b: 16-bits output, unused pins are set as GPIO.
    11b: LVDS, all 24-bits unused output pins are set as GPIO.*/
	unsigned char temp;
	LCD_CmdWrite(0x01);
	temp = LCD_DataRead();
	temp |= cSetD4;
    temp |= cSetD3;
	LCD_DataWrite(temp);  
}

void LCD_I2CM_Enable(void)
{
/*  I2C master Interface Enable/Disable
    0: Disable
    1: Enable*/
	unsigned char temp;
	LCD_CmdWrite(0x01);
	temp = LCD_DataRead();
	temp |= cSetD2;
	LCD_DataWrite(temp);    
}

void LCD_I2CM_Disable(void)
{
/*  I2C master Interface Enable/Disable
    0: Disable
    1: Enable*/
	unsigned char temp;
	LCD_CmdWrite(0x01);
	temp = LCD_DataRead();
	temp &= cClrD2;
	LCD_DataWrite(temp);     
}

void Enable_SFlash_SPI(void)
{
/*  Serial Flash SPI Interface Enable/Disable
    0: Disable
    1: Enable*/
	unsigned char temp;
	LCD_CmdWrite(0x01);
	temp = LCD_DataRead();
	temp |= cSetD1;
	LCD_DataWrite(temp);     
}

void Disable_SFlash_SPI(void)
{
/*  Serial Flash SPI Interface Enable/Disable
    0: Disable
    1: Enable*/
	unsigned char temp;
	LCD_CmdWrite(0x01);
	temp = LCD_DataRead();
	temp &= cClrD1;
	LCD_DataWrite(temp);     
}
void Host_Bus_8bit(void)
{
/*  Parallel Host Data Bus Width Selection
    0: 8-bit Parallel Host Data Bus.
    1: 16-bit Parallel Host Data Bus.*/
	unsigned char temp;
	LCD_CmdWrite(0x01);
	temp = LCD_DataRead(); 
	temp &= cClrD0;		  
	LCD_DataWrite(temp);

}
void Host_Bus_16bit(void)
{
/*  Parallel Host Data Bus Width Selection
    0: 8-bit Parallel Host Data Bus.
    1: 16-bit Parallel Host Data Bus.*/
	unsigned char temp;
	LCD_CmdWrite(0x01);
	temp = LCD_DataRead();
	temp |= cSetD0;		 
   LCD_DataWrite(temp);
}

//[02h][02h][02h][02h][02h][02h][02h][02h][02h][02h][02h][02h][02h][02h][02h][02h]

void RGB_8b_8bpp(void)
{
	unsigned char temp;
	LCD_CmdWrite(0x02);
	temp = LCD_DataRead();
	temp &= cClrD7;
	temp &= cClrD6;
	LCD_DataWrite(temp);
}
void RGB_8b_16bpp(void)
{
	unsigned char temp;
	LCD_CmdWrite(0x02);
	temp = LCD_DataRead();
	temp &= cClrD7;
	temp |= cSetD6;
	LCD_DataWrite(temp);
}
void RGB_8b_24bpp(void)
{
	unsigned char temp;
	LCD_CmdWrite(0x02);
	temp = LCD_DataRead();
	temp &= cClrD7;
	temp &= cClrD6;
	LCD_DataWrite(temp);
}

void RGB_16b_8bpp(void)
{
	unsigned char temp;
	LCD_CmdWrite(0x02);
	temp = LCD_DataRead();
	temp |= cSetD7;
    temp &= cClrD6;
	LCD_DataWrite(temp);
}
void RGB_16b_16bpp(void)
{
	unsigned char temp;
	LCD_CmdWrite(0x02);
	temp = LCD_DataRead();
	temp &= cClrD7;
	temp |= cSetD6;	
	LCD_DataWrite(temp);

}
void RGB_16b_24bpp_mode1(void)
{
	unsigned char temp;
	LCD_CmdWrite(0x02);
	temp = LCD_DataRead();
	temp &= cClrD7;
	temp &= cClrD6;
	LCD_DataWrite(temp);
}
void RGB_16b_24bpp_mode2(void)
{
	unsigned char temp;
	LCD_CmdWrite(0x02);
	temp = LCD_DataRead();
	temp |= cSetD7;
	temp |= cSetD6;
	LCD_DataWrite(temp);
}

void MemRead_Left_Right_Top_Down(void)
{
	unsigned char temp;
	LCD_CmdWrite(0x02);
	temp = LCD_DataRead();
	temp &= cClrD5;
	temp &= cClrD4;
	LCD_DataWrite(temp);
}
void MemRead_Right_Left_Top_Down(void)
{
	unsigned char temp;
	LCD_CmdWrite(0x02);
	temp = LCD_DataRead();
	temp &= cClrD5;
	temp |= cSetD4;
	LCD_DataWrite(temp);
}
void MemRead_Top_Down_Left_Right(void)
{
	unsigned char temp;
	LCD_CmdWrite(0x02);
	temp = LCD_DataRead();
	temp |= cSetD5;
    temp &= cClrD4;
	LCD_DataWrite(temp);
}
void MemRead_Down_Top_Left_Right(void)
{
	unsigned char temp;
	LCD_CmdWrite(0x02);
	temp = LCD_DataRead();
	temp |= cSetD5;
	temp |= cSetD4;
	LCD_DataWrite(temp);
}
void MemWrite_Left_Right_Top_Down(void)
{
	unsigned char temp;
	LCD_CmdWrite(0x02);
	temp = LCD_DataRead();
	temp &= cClrD2;
	temp &= cClrD1;
	LCD_DataWrite(temp);
}
void MemWrite_Right_Left_Top_Down(void)
{
	unsigned char temp;
	LCD_CmdWrite(0x02);
	temp = LCD_DataRead();
	temp &= cClrD2;
	temp |= cSetD1;
	LCD_DataWrite(temp);
}
void MemWrite_Top_Down_Left_Right(void)
{
	unsigned char temp;
	LCD_CmdWrite(0x02);
	temp = LCD_DataRead();
	temp |= cSetD2;
    temp &= cClrD1;
	LCD_DataWrite(temp);
}
void MemWrite_Down_Top_Left_Right(void)
{
	unsigned char temp;
	LCD_CmdWrite(0x02);
	temp = LCD_DataRead();
	temp |= cSetD2;
	temp |= cSetD1;
	LCD_DataWrite(temp);
}
//[03h][03h][03h][03h][03h][03h][03h][03h][03h][03h][03h][03h][03h][03h][03h][03h]
void Interrupt_Active_Low(void)
{
/*  MPU Interrupt active level
    0 : active low.
    1 : active high.*/
	unsigned char temp;
	LCD_CmdWrite(0x03);
	temp = LCD_DataRead();
	temp &= cClrD7;
	LCD_DataWrite(temp);
}
void Interrupt_Active_High(void)
{
/*  MPU Interrupt active level
    0 : active low.
    1 : active high.*/
	unsigned char temp;
	LCD_CmdWrite(0x03);
	temp = LCD_DataRead();
	temp |= cSetD7;
	LCD_DataWrite(temp);
}
void ExtInterrupt_Debounce(void)
{
/*  External interrupt de-bounce
    0 : without de-bounce
    1 : enable de-bounce (1024 OSC clock)*/
	unsigned char temp;
	LCD_CmdWrite(0x03);
	temp = LCD_DataRead();
	temp |= cSetD6;
	LCD_DataWrite(temp);
}
void ExtInterrupt_Nodebounce(void)
{
/*  External interrupt de-bounce
    0 : without de-bounce
    1 : enable de-bounce (1024 OSC clock)*/
	unsigned char temp;
	LCD_CmdWrite(0x03);
	temp = LCD_DataRead();
	temp &= cClrD6;
	LCD_DataWrite(temp);
}
void ExtInterrupt_Input_Low_Level_Trigger(void)
{
	unsigned char temp;
	LCD_CmdWrite(0x03);
	temp = LCD_DataRead();
	temp &= cClrD5;
    temp &= cClrD4;
	LCD_DataWrite(temp);
}
void ExtInterrupt_Input_High_Level_Trigger(void)
{
	unsigned char temp;
	LCD_CmdWrite(0x03);
	temp = LCD_DataRead();
	temp |= cSetD5;
    temp &= cClrD4;
	LCD_DataWrite(temp);
}
void ExtInterrupt_Input_Falling_Edge_Trigger(void)
{
	unsigned char temp;
	LCD_CmdWrite(0x03);
	temp = LCD_DataRead();
    temp &= cClrD5;
    temp |= cSetD4;
	LCD_DataWrite(temp);
}
void ExtInterrupt_Input_Rising_Edge_Trigger(void)
{
	unsigned char temp;
	LCD_CmdWrite(0x03);
	temp = LCD_DataRead();
	temp |= cSetD5;
    temp |= cSetD4;
	LCD_DataWrite(temp);
}
void LVDS_Format1(void)
{
	unsigned char temp;
	LCD_CmdWrite(0x03);
	temp = LCD_DataRead();
    temp &= cClrD3;
	LCD_DataWrite(temp);
}
void LVDS_Format2(void)
{
	unsigned char temp;
	LCD_CmdWrite(0x03);
	temp = LCD_DataRead();
    temp |= cSetD3;
	LCD_DataWrite(temp);
}
void Graphic_Mode_8876(void)
{
	unsigned char temp;
	LCD_CmdWrite(0x03);
	temp = LCD_DataRead();
    temp &= cClrD2;
	LCD_DataWrite(temp);
}

void Memory_Select_SDRAM(void)
{
	unsigned char temp;
	LCD_CmdWrite(0x03);
	temp = LCD_DataRead();
    temp &= cClrD1;
    temp &= cClrD0;	// B
	LCD_DataWrite(temp);
}
void Memory_Select_Graphic_Cursor_RAM(void)
{
	unsigned char temp;
	LCD_CmdWrite(0x03);
	temp = LCD_DataRead();
    temp |= cSetD1;
    temp &= cClrD0;
	LCD_DataWrite(temp);
}
void Memory_Select_Color_Palette_RAM(void)
{
	unsigned char temp;
	LCD_CmdWrite(0x03);
	temp = LCD_DataRead();
    temp |= cSetD1;
    temp |= cSetD0;
	LCD_DataWrite(temp);
}

//[05h]=========================================================================
//[06h]=========================================================================
//[07h]=========================================================================
//[08h]=========================================================================
//[09h]=========================================================================
//[0Ah]=========================================================================
//[0Bh]=========================================================================
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{
    LCD_CmdWrite(0x5F);
	LCD_DataWrite(Xpos);  
    LCD_CmdWrite(0x60);	   
	LCD_DataWrite(Xpos>>8);
    LCD_CmdWrite(0x61);
	LCD_DataWrite(Ypos);
    LCD_CmdWrite(0x62);	   
	LCD_DataWrite(Ypos>>8);
}	
void LCD_WriteRAM_Prepare(void)
{
  LCD_CmdWrite(0x04); //
}
void Enable_Resume_Interrupt(void)
{
/*
Resume Interrupt Enable
0: Disable.
1: Enable.
*/
	unsigned char temp;
	LCD_CmdWrite(0x0B);
	temp = LCD_DataRead();
    temp |= cSetD7;
	LCD_DataWrite(temp);
}
void Disable_Resume_Interrupt(void)
{
/*
Resume Interrupt Enable
0: Disable.
1: Enable.
*/
	unsigned char temp;
	LCD_CmdWrite(0x0B);
	temp = LCD_DataRead();
    temp &= cClrD7;
	LCD_DataWrite(temp);
}
void Enable_ExtInterrupt_Input(void)
{
/*
External Interrupt (PS[0] pin) Enable
0: Disable.
1: Enable.
*/
	unsigned char temp;
	LCD_CmdWrite(0x0B);
	temp = LCD_DataRead();
    temp |= cSetD6;
	LCD_DataWrite(temp);
}
void Disable_ExtInterrupt_Input(void)
{
/*
External Interrupt (PS[0] pin) Enable
0: Disable.
1: Enable.
*/
	unsigned char temp;
	LCD_CmdWrite(0x0B);
	temp = LCD_DataRead();
    temp &= cClrD6;
	LCD_DataWrite(temp);
}
void Enable_I2CM_Interrupt(void)
{
/*
I2C Master Interrupt Enable
0: Disable.
1: Enable.
*/
	unsigned char temp;
	LCD_CmdWrite(0x0B);
	temp = LCD_DataRead();
    temp |= cSetD5;
	LCD_DataWrite(temp);
}
void Disable_I2CM_Interrupt(void)
{
/*
I2C Master Interrupt Enable
0: Disable.
1: Enable.
*/
	unsigned char temp;
	LCD_CmdWrite(0x0B);
	temp = LCD_DataRead();
    temp &= cClrD5;
	LCD_DataWrite(temp);
}
void Enable_Vsync_Interrupt(void)
{
/*
Vsync time base interrupt Enable Bit
0: Disable Interrupt.
1: Enable Interrupt.
*/
	unsigned char temp;
	LCD_CmdWrite(0x0B);
	temp = LCD_DataRead();
    temp |= cSetD4;
	LCD_DataWrite(temp);
}
void Disable_Vsync_Interrupt(void)
{
/*
Vsync time base interrupt Enable Bit
0: Disable Interrupt.
1: Enable Interrupt.
*/
	unsigned char temp;
	LCD_CmdWrite(0x0B);
	temp = LCD_DataRead();
    temp &= cClrD4;
	LCD_DataWrite(temp);
}
void Enable_KeyScan_Interrupt(void)
{
/*
Key Scan Interrupt Enable Bit
0: Disable Key scan interrupt.
1: Enable Key scan interrupt.
*/
	unsigned char temp;
	LCD_CmdWrite(0x0B);
	temp = LCD_DataRead();
    temp |= cSetD3;
	LCD_DataWrite(temp);
}
void Disable_KeyScan_Interrupt(void)
{
/*
Key Scan Interrupt Enable Bit
0: Disable Key scan interrupt.
1: Enable Key scan interrupt.
*/
	unsigned char temp;
	LCD_CmdWrite(0x0B);
	temp = LCD_DataRead();
    temp &= cClrD3;
	LCD_DataWrite(temp);
}
void Enable_DMA_Draw_BTE_Interrupt(void)
{
/*
Serial flash DMA Complete | Draw task finished | BTE Process
Complete etc. Interrupt Enable
0: Disable Interrupt.
1: Enable Interrupt.
*/
	unsigned char temp;
	LCD_CmdWrite(0x0B);
	temp = LCD_DataRead();
    temp |= cSetD2;
	LCD_DataWrite(temp);
}
void Disable_DMA_Draw_BTE_Interrupt(void)
{
/*
Serial flash DMA Complete | Draw task finished | BTE Process
Complete etc. Interrupt Enable
0: Disable Interrupt.
1: Enable Interrupt.
*/
	unsigned char temp;
	LCD_CmdWrite(0x0B);
	temp = LCD_DataRead();
    temp &= cClrD2;
	LCD_DataWrite(temp);
}
void Enable_PWM1_Interrupt(void)
{
/*
PWM timer 1 Interrupt Enable Bit
0: Disable Interrupt.
1: Enable Interrupt.
*/
	unsigned char temp;
	LCD_CmdWrite(0x0B);
	temp = LCD_DataRead();
    temp |= cSetD1;
	LCD_DataWrite(temp);
}
void Disable_PWM1_Interrupt(void)
{
/*
PWM timer 1 Interrupt Enable Bit
0: Disable Interrupt.
1: Enable Interrupt.
*/
	unsigned char temp;
	LCD_CmdWrite(0x0B);
	temp = LCD_DataRead();
    temp &= cClrD1;
	LCD_DataWrite(temp);
}
void Enable_PWM0_Interrupt(void)
{
/*
PWM timer 0 Interrupt Enable Bit
0: Disable Interrupt.
1: Enable Interrupt.
*/
	unsigned char temp;
	LCD_CmdWrite(0x0B);
	temp = LCD_DataRead();
    temp |= cSetD0;
	LCD_DataWrite(temp);
}
void Disable_PWM0_Interrupt(void)
{
/*
PWM timer 0 Interrupt Enable Bit
0: Disable Interrupt.
1: Enable Interrupt.
*/
	unsigned char temp;
	LCD_CmdWrite(0x0B);
	temp = LCD_DataRead();
    temp &= cClrD0;
	LCD_DataWrite(temp);
}

//[0Ch]=========================================================================
unsigned char Read_Interrupt_status(void)
{
/*
[Bit7]Read Function ..Resume Interrupt Status
0: No Resume interrupt happens.
1: Resume interrupt happens.
[Bit6]Read Function .. PS[0] pin Interrupt Status
0: No PS[0] pin interrupt happens.
1: PS[0] pin interrupt happens.
[Bit5]Read Function .. I2C master Interrupt Status
0: No I2C master interrupt happens.
1: I2C master interrupt happens.
[Bit4]Read Function .. Vsync Interrupt Status
0: No interrupt happens.
1: interrupt happens.
[Bit3]Read Function ..Key Scan Interrupt Status
0: No Key Scan interrupt happens.
1: Key Scan interrupt happens.
[Bit2]Read Function..Interrupt Status
0: No interrupt happens.
1: interrupt happens.
[Bit1]Read Function..Interrupt Status
0: No interrupt happens.
1: interrupt happens.
[Bit0]Read Function..Interrupt Status
0: No interrupt happens.
1: interrupt happens.
*/
	unsigned char temp;
	LCD_CmdWrite(0x0C);
	temp = LCD_DataRead();
    return temp;
}
void Clear_Resume_Interrupt_Flag(void)
{
/*
Resume Interrupt flag
Write Function .. Resume Interrupt Clear Bit
0: No operation.
1: Clear Resume interrupt.
*/
	unsigned char temp;
	LCD_CmdWrite(0x0C);
	temp = LCD_DataRead();
    temp |= cSetD7;
	LCD_DataWrite(temp);
}
void Clear_ExtInterrupt_Input_Flag(void)
{
/*
External Interrupt (PS[0] pin) flag
Write Function .. PS[0] pin edge Interrupt Clear Bit
0: No operation.
1: Clear the PS[0] pin edge interrupt.
*/
	unsigned char temp;
	LCD_CmdWrite(0x0C);
	temp = LCD_DataRead();
    temp |= cSetD6;
	LCD_DataWrite(temp);
}
void Clear_I2CM_Interrupt_Flag(void)
{
/*
I2C master Interrupt flag
Write Function.. I2C master Interrupt Clear Bit
0: No operation.
1: Clear the I2C master interrupt.
*/
	unsigned char temp;
	LCD_CmdWrite(0x0C);
	temp = LCD_DataRead();
    temp |= cSetD5;
	LCD_DataWrite(temp);
}
void Clear_Vsync_Interrupt_Flag(void)
{
/*
Vsync Time base interrupt flag
Write Function ..Vsync Interrupt Clear Bit
0: No operation.
1: Clear the interrupt.
*/
	unsigned char temp;
	LCD_CmdWrite(0x0C);
	temp = LCD_DataRead();
    temp |= cSetD4;
	LCD_DataWrite(temp);
}
void Clear_KeyScan_Interrupt_Flag(void)
{
/*
Key Scan Interrupt flag
Write Function..Key Scan Interrupt Clear Bit
0: No operation.
1: Clear the Key Scan interrupt.
*/
	unsigned char temp;
	LCD_CmdWrite(0x0C);
	temp = LCD_DataRead();
    temp |= cSetD3;
	LCD_DataWrite(temp);
}
void Clear_DMA_Draw_BTE_Interrupt_Flag(void)
{
/*
Serial flash DMA Complete | Draw task finished | BTE
Process Complete etc. Interrupt flag
Write Function.. Interrupt Clear Bit
0: No operation.
1: Clear interrupt.
*/
	unsigned char temp;
	LCD_CmdWrite(0x0C);
	temp = LCD_DataRead();
    temp |= cSetD2;
	LCD_DataWrite(temp);
}
void Clear_PWM1_Interrupt_Flag(void)
{
/*
PWM 1 timer Interrupt flag
Write Function..Interrupt Clear Bit
0: No operation.
1: Clear interrupt.
*/
	unsigned char temp;
	LCD_CmdWrite(0x0C);
	temp = LCD_DataRead();
    temp |= cSetD1;
	LCD_DataWrite(temp);
}
void Clear_PWM0_Interrupt_Flag(void)
{
/*
PWM 0 timer Interrupt flag
Write Function..Interrupt Clear Bit
0: No operation.
1: Clear interrupt.
*/
	unsigned char temp;
	LCD_CmdWrite(0x0C);
	temp = LCD_DataRead();
    temp |= cSetD0;
	LCD_DataWrite(temp);
}
//[0Dh]=========================================================================
void Mask_Resume_Interrupt_Flag(void)
{
/*
Mask Resume Interrupt Flag
0: Enable.
1: Mask.
*/
	unsigned char temp;
	LCD_CmdWrite(0x0D);
	temp = LCD_DataRead();
    temp |= cSetD7;
	LCD_DataWrite(temp);
}
void Mask_ExtInterrupt_Input_Flag(void)
{
/*
Mask External Interrupt (PS[0] pin) Flag
0: Enable.
1: Mask.
*/
	unsigned char temp;
	LCD_CmdWrite(0x0D);
	temp = LCD_DataRead();
    temp |= cSetD6;
	LCD_DataWrite(temp);
}
void Mask_I2CM_Interrupt_Flag(void)
{
/*
Mask I2C Master Interrupt Flag
0: Enable.
1: Mask.
*/
	unsigned char temp;
	LCD_CmdWrite(0x0D);
	temp = LCD_DataRead();
    temp |= cSetD5;
	LCD_DataWrite(temp);
}
void Mask_Vsync_Interrupt_Flag(void)
{
/*
Mask Vsync time base interrupt Flag
0: Enable.
1: Mask.
*/
	unsigned char temp;
	LCD_CmdWrite(0x0D);
	temp = LCD_DataRead();
    temp |= cSetD4;
	LCD_DataWrite(temp);
}
void Mask_KeyScan_Interrupt_Flag(void)
{
/*
Mask Key Scan Interrupt Flag
0: Enable.
1: Mask.
*/
	unsigned char temp;
	LCD_CmdWrite(0x0D);
	temp = LCD_DataRead();
    temp |= cSetD3;
	LCD_DataWrite(temp);
}
void Mask_DMA_Draw_BTE_Interrupt_Flag(void)
{
/*
Mask Serial flash DMA Complete | Draw task finished | BTE
Process Complete etc. Interrupt Flag
0: Enable.
1: Mask.
*/
	unsigned char temp;
	LCD_CmdWrite(0x0D);
	temp = LCD_DataRead();
    temp |= cSetD2;
	LCD_DataWrite(temp);
}
void Mask_PWM1_Interrupt_Flag(void)
{
/*
Mask PWM timer 1 Interrupt Flag
0: Enable.
1: Mask.
*/
	unsigned char temp;
	LCD_CmdWrite(0x0D);
	temp = LCD_DataRead();
    temp |= cSetD1;
	LCD_DataWrite(temp);
}
void Mask_PWM0_Interrupt_Flag(void)
{
/*
Mask PWM timer 0 Interrupt Flag
0: Enable.
1: Mask.
*/
	unsigned char temp;
	LCD_CmdWrite(0x0D);
	temp = LCD_DataRead();
    temp |= cSetD0;
	LCD_DataWrite(temp);
}

void Enable_Resume_Interrupt_Flag(void)
{
/*
Mask Resume Interrupt Flag
0: Enable.
1: Mask.
*/
	unsigned char temp;
	LCD_CmdWrite(0x0D);
	temp = LCD_DataRead();
    temp &= cClrD7;
	LCD_DataWrite(temp);
}
void Enable_ExtInterrupt_Inpur_Flag(void)
{
/*
Mask External Interrupt (PS[0] pin) Flag
0: Enable.
1: Mask.
*/
	unsigned char temp;
	LCD_CmdWrite(0x0D);
	temp = LCD_DataRead();
    temp &= cClrD6;
	LCD_DataWrite(temp);
}
void Enable_I2CM_Interrupt_Flag(void)
{
/*
Mask I2C Master Interrupt Flag
0: Enable.
1: Mask.
*/
	unsigned char temp;
	LCD_CmdWrite(0x0D);
	temp = LCD_DataRead();
    temp &= cClrD5;
	LCD_DataWrite(temp);
}
void Enable_Vsync_Interrupt_Flag(void)
{
/*
Mask Vsync time base interrupt Flag
0: Enable.
1: Mask.
*/
	unsigned char temp;
	LCD_CmdWrite(0x0D);
	temp = LCD_DataRead();
    temp &= cClrD4;
	LCD_DataWrite(temp);
}
void Enable_KeyScan_Interrupt_Flag(void)
{
/*
Mask Key Scan Interrupt Flag
0: Enable.
1: Mask.
*/
	unsigned char temp;
	LCD_CmdWrite(0x0D);
	temp = LCD_DataRead();
    temp &= cClrD3;
	LCD_DataWrite(temp);
}
void Enable_DMA_Draw_BTE_Interrupt_Flag(void)
{
/*
Mask Serial flash DMA Complete | Draw task finished | BTE
Process Complete etc. Interrupt Flag
0: Enable.
1: Mask.
*/
	unsigned char temp;
	LCD_CmdWrite(0x0D);
	temp = LCD_DataRead();
    temp &= cClrD2;
	LCD_DataWrite(temp);
}
void Enable_PWM1_Interrupt_Flag(void)
{
/*
Mask PWM timer 1 Interrupt Flag
0: Enable.
1: Mask.
*/
	unsigned char temp;
	LCD_CmdWrite(0x0D);
	temp = LCD_DataRead();
    temp &= cClrD1;
	LCD_DataWrite(temp);
}
void Enable_PWM0_Interrupt_Flag(void)
{
/*
Mask PWM timer 0 Interrupt Flag
0: Enable.
1: Mask.
*/
	unsigned char temp;
	LCD_CmdWrite(0x0D);
	temp = LCD_DataRead();
    temp &= cClrD0;
	LCD_DataWrite(temp);
}

//[0Eh]=========================================================================
void Enable_GPIOF_PullUp(void)
{
/*
GPIO_F[7:0] Pull-Up Enable (XPDAT[23:19, 15:13])
0: Pull-Up Disable
1: Pull-Up Enable
*/
	unsigned char temp;
	LCD_CmdWrite(0x0E);
	temp = LCD_DataRead();
    temp |= cSetD5;
	LCD_DataWrite(temp);
}
void Enable_GPIOE_PullUp(void)
{
/*
GPIO_E[7:0] Pull-Up Enable (XPDAT[12:10, 7:3])
0: Pull-Up Disable
1: Pull-Up Enable
*/
	unsigned char temp;
	LCD_CmdWrite(0x0E);
	temp = LCD_DataRead();
    temp |= cSetD4;
	LCD_DataWrite(temp);
}
void Enable_GPIOD_PullUp(void)
{
/*
GPIO_D[7:0] Pull-Up Enable (XPDAT[18, 2, 17, 16, 9, 8, 1,0])
0: Pull-Up Disable
1: Pull-Up Enable
*/
	unsigned char temp;
	LCD_CmdWrite(0x0E);
	temp = LCD_DataRead();
    temp |= cSetD3;
	LCD_DataWrite(temp);
}
void Enable_GPIOC_PullUp(void)
{
/*
GPIO_C[6:0] Pull-Up Enable (XSDA, XSCL, XnSFCS1,
XnSFCS0, XMISO, XMOSI , XSCK)
0: Pull-Up Disable
1: Pull-Up Enable
*/
	unsigned char temp;
	LCD_CmdWrite(0x0E);
	temp = LCD_DataRead();
    temp |= cSetD2;
	LCD_DataWrite(temp);
}
void Enable_XDB15_8_PullUp(void)
{
/*
XDB[15:8] Pull-Up Enable
0: Pull-Up Disable
1: Pull-Up Enable
*/
	unsigned char temp;
	LCD_CmdWrite(0x0E);
	temp = LCD_DataRead();
    temp |= cSetD1;
	LCD_DataWrite(temp);
}
void Enable_XDB7_0_PullUp(void)
{
/*
XDB[7:0] Pull-Up Enable
0: Pull-Up Disable
1: Pull-Up Enable
*/
	unsigned char temp;
	LCD_CmdWrite(0x0E);
	temp = LCD_DataRead();
    temp |= cSetD0;
	LCD_DataWrite(temp);
}
void Disable_GPIOF_PullUp(void)
{
/*
GPIO_F[7:0] Pull-Up Enable (XPDAT[23:19, 15:13])
0: Pull-Up Disable
1: Pull-Up Enable
*/
	unsigned char temp;
	LCD_CmdWrite(0x0E);
	temp = LCD_DataRead();
    temp &= cClrD5;
	LCD_DataWrite(temp);
}
void Disable_GPIOE_PullUp(void)
{
/*
GPIO_E[7:0] Pull-Up Enable (XPDAT[12:10, 7:3])
0: Pull-Up Disable
1: Pull-Up Enable
*/
	unsigned char temp;
	LCD_CmdWrite(0x0E);
	temp = LCD_DataRead();
    temp &= cClrD4;
	LCD_DataWrite(temp);
}
void Disable_GPIOD_PullUp(void)
{
/*
GPIO_D[7:0] Pull-Up Enable (XPDAT[18, 2, 17, 16, 9, 8, 1,0])
0: Pull-Up Disable
1: Pull-Up Enable
*/
	unsigned char temp;
	LCD_CmdWrite(0x0E);
	temp = LCD_DataRead();
    temp &= cClrD3;
	LCD_DataWrite(temp);
}
void Disable_GPIOC_PullUp(void)
{
/*
GPIO_C[6:0] Pull-Up Enable (XSDA, XSCL, XnSFCS1,
XnSFCS0, XMISO, XMOSI , XSCK)
0: Pull-Up Disable
1: Pull-Up Enable
*/
	unsigned char temp;
	LCD_CmdWrite(0x0E);
	temp = LCD_DataRead();
    temp &= cClrD2;
	LCD_DataWrite(temp);
}
void Disable_XDB15_8_PullUp(void)
{
/*
XDB[15:8] Pull-Up Enable
0: Pull-Up Disable
1: Pull-Up Enable
*/
	unsigned char temp;
	LCD_CmdWrite(0x0E);
	temp = LCD_DataRead();
    temp &= cClrD1;
	LCD_DataWrite(temp);
}
void Disable_XDB7_0_PullUp(void)
{
/*
XDB[7:0] Pull-Up Enable
0: Pull-Up Disable
1: Pull-Up Enable
*/
	unsigned char temp;
	LCD_CmdWrite(0x0E);
	temp = LCD_DataRead();
    temp &= cClrD0;
	LCD_DataWrite(temp);
}
//[0Fh]=========================================================================
void XPDAT18_Set_GPIO_D7(void)
{
/*
XPDAT[18]  not scan function select
0: GPIO-D7
1: KOUT[4]
*/
	unsigned char temp;
	LCD_CmdWrite(0x0F);
	temp = LCD_DataRead();
    temp &= cClrD7;
	LCD_DataWrite(temp);
}
void XPDAT18_Set_KOUT4(void)
{
/*
XPDAT[18]  not scan function select
0: GPIO-D7
1: KOUT[4]
*/
	unsigned char temp;
	LCD_CmdWrite(0x0F);
	temp = LCD_DataRead();
    temp |= cSetD7;
	LCD_DataWrite(temp);
}
void XPDAT17_Set_GPIO_D5(void)
{
/*
XPDAT[17]  not scan function select
0: GPIO-D5
1: KOUT[2]
*/
	unsigned char temp;
	LCD_CmdWrite(0x0F);
	temp = LCD_DataRead();
    temp &= cClrD6;
	LCD_DataWrite(temp);
}
void XPDAT17_Set_KOUT2(void)
{
/*
XPDAT[17]  not scan function select
0: GPIO-D5
1: KOUT[2]
*/
	unsigned char temp;
	LCD_CmdWrite(0x0F);
	temp = LCD_DataRead();
    temp |= cSetD6;
	LCD_DataWrite(temp);
}
void XPDAT16_Set_GPIO_D4(void)
{
/*
XPDAT[16]  not scan function select
0: GPIO-D4
1: KOUT[1]
*/
	unsigned char temp;
	LCD_CmdWrite(0x0F);
	temp = LCD_DataRead();
    temp &= cClrD5;
	LCD_DataWrite(temp);
}
void XPDAT16_Set_KOUT1(void)
{
/*
XPDAT[16]  not scan function select
0: GPIO-D4
1: KOUT[1]
*/
	unsigned char temp;
	LCD_CmdWrite(0x0F);
	temp = LCD_DataRead();
    temp |= cSetD5;
	LCD_DataWrite(temp);
}
void XPDAT9_Set_GPIO_D3(void)
{
/*
XPDAT[9]  not scan function select
0: GPIO-D3
1: KOUT[3]
*/
	unsigned char temp;
	LCD_CmdWrite(0x0F);
	temp = LCD_DataRead();
    temp &= cClrD4;
	LCD_DataWrite(temp);
}
void XPDAT9_Set_KOUT3(void)
{
/*
XPDAT[9]  not scan function select
0: GPIO-D3
1: KOUT[3]
*/
	unsigned char temp;
	LCD_CmdWrite(0x0F);
	temp = LCD_DataRead();
    temp |= cSetD4;
	LCD_DataWrite(temp);
}
void XPDAT8_Set_GPIO_D2(void)
{
/*
XPDAT[8]  not scan function select
0: GPIO-D2
1: KIN[3]
*/
	unsigned char temp;
	LCD_CmdWrite(0x0F);
	temp = LCD_DataRead();
    temp &= cClrD3;
	LCD_DataWrite(temp);
}
void XPDAT8_Set_KIN3(void)
{
/*
XPDAT[8]  not scan function select
0: GPIO-D2
1: KIN[3]
*/
	unsigned char temp;
	LCD_CmdWrite(0x0F);
	temp = LCD_DataRead();
    temp |= cSetD3;
	LCD_DataWrite(temp);
}
void XPDAT2_Set_GPIO_D6(void)
{
/*
XPDAT[2]  not scan function select
0: GPIO-D6
1: KIN[4]
*/
	unsigned char temp;
	LCD_CmdWrite(0x0F);
	temp = LCD_DataRead();
    temp &= cClrD2;
	LCD_DataWrite(temp);
}
void XPDAT2_Set_KIN4(void)
{
/*
XPDAT[2]  not scan function select
0: GPIO-D6
1: KIN[4]
*/
	unsigned char temp;
	LCD_CmdWrite(0x0F);
	temp = LCD_DataRead();
    temp |= cSetD2;
	LCD_DataWrite(temp);
}
void XPDAT1_Set_GPIO_D1(void)
{
/*
XPDAT[1]  not scan function select
0: GPIO-D1
1: KIN[2]
*/
	unsigned char temp;
	LCD_CmdWrite(0x0F);
	temp = LCD_DataRead();
    temp &= cClrD1;
	LCD_DataWrite(temp);
}
void XPDAT1_Set_KIN2(void)
{
/*
XPDAT[1]  not scan function select
0: GPIO-D1
1: KIN[2]
*/
	unsigned char temp;
	LCD_CmdWrite(0x0F);
	temp = LCD_DataRead();
    temp |= cSetD1;
	LCD_DataWrite(temp);
}
void XPDAT0_Set_GPIO_D0(void)
{
/*
XPDAT[0]  not scan function select
0: GPIO-D0
1: KIN[1]
*/
	unsigned char temp;
	LCD_CmdWrite(0x0F);
	temp = LCD_DataRead();
    temp &= cClrD0;
	LCD_DataWrite(temp);
}
void XPDAT0_Set_KIN1(void)
{
/*
XPDAT[0]  not scan function select
0: GPIO-D0
1: KIN[1]
*/
	unsigned char temp;
	LCD_CmdWrite(0x0F);
	temp = LCD_DataRead();
    temp |= cSetD0;
	LCD_DataWrite(temp);
}

//[10h]=========================================================================
void Enable_PIP1(void)
{
/*
PIP 1 window Enable/Disable
0 : PIP 1 window disable.
1 : PIP 1 window enable
PIP 1 window always on top of PIP 2 window.
*/
	unsigned char temp;
	LCD_CmdWrite(0x10);
	temp = LCD_DataRead();
    temp |= cSetD7;
	LCD_DataWrite(temp);
}
void Disable_PIP1(void)
{
/*
PIP 1 window Enable/Disable
0 : PIP 1 window disable.
1 : PIP 1 window enable
PIP 1 window always on top of PIP 2 window.
*/
	unsigned char temp;
	LCD_CmdWrite(0x10);
	temp = LCD_DataRead();
    temp &= cClrD7;
	LCD_DataWrite(temp);
}
void Enable_PIP2(void)
{
/*
PIP 2 window Enable/Disable
0 : PIP 2 window disable.
1 : PIP 2 window enable
PIP 1 window always on top of PIP 2 window.
*/
	unsigned char temp;
	LCD_CmdWrite(0x10);
	temp = LCD_DataRead();
    temp |= cSetD6;
	LCD_DataWrite(temp);
}
void Disable_PIP2(void)
{
/*
PIP 2 window Enable/Disable
0 : PIP 2 window disable.
1 : PIP 2 window enable
PIP 1 window always on top of PIP 2 window.
*/
	unsigned char temp;
	LCD_CmdWrite(0x10);
	temp = LCD_DataRead();
    temp &= cClrD6;
	LCD_DataWrite(temp);
}
void Select_PIP1_Parameter(void)
{
/*
0: To configure PIP 1ˇs parameters.
1: To configure PIP 2ˇs parameters..
*/
	unsigned char temp;
	LCD_CmdWrite(0x10);
	temp = LCD_DataRead();
    temp &= cClrD4;
	LCD_DataWrite(temp);
}
void Select_PIP2_Parameter(void)
{
/*
0: To configure PIP 1ˇs parameters.
1: To configure PIP 2ˇs parameters..
*/
	unsigned char temp;
	LCD_CmdWrite(0x10);
	temp = LCD_DataRead();
    temp |= cSetD4;
	LCD_DataWrite(temp);
}
void Select_Main_Window_8bpp(void)
{
/*
Main Window Color Depth Setting
00b: 8-bpp generic TFT, i.e. 256 colors.
01b: 16-bpp generic TFT, i.e. 65K colors.
1xb: 24-bpp generic TFT, i.e. 1.67M colors.
*/
	unsigned char temp;
	LCD_CmdWrite(0x10);
	temp = LCD_DataRead();
    temp &= cClrD3;
    temp &= cClrD2;
	LCD_DataWrite(temp);
}
void Select_Main_Window_16bpp(void)
{
/*
Main Window Color Depth Setting
00b: 8-bpp generic TFT, i.e. 256 colors.
01b: 16-bpp generic TFT, i.e. 65K colors.
1xb: 24-bpp generic TFT, i.e. 1.67M colors.
*/
	unsigned char temp;
	LCD_CmdWrite(0x10);
	temp = LCD_DataRead();
    temp &= cClrD3;
    temp |= cSetD2;
	LCD_DataWrite(temp);
}
void Select_Main_Window_24bpp(void)
{
/*
Main Window Color Depth Setting
00b: 8-bpp generic TFT, i.e. 256 colors.
01b: 16-bpp generic TFT, i.e. 65K colors.
1xb: 24-bpp generic TFT, i.e. 1.67M colors.
*/
	unsigned char temp;
	LCD_CmdWrite(0x10);
	temp = LCD_DataRead();
 temp |= cSetD3;
    temp  &= cClrD2;
	LCD_DataWrite(temp);
}
//[11h]=========================================================================
void Select_PIP2_Window_8bpp(void)
{
/*
PIP 1 Window Color Depth Setting
00b: 8-bpp generic TFT, i.e. 256 colors.
01b: 16-bpp generic TFT, i.e. 65K colors.
1xb: 24-bpp generic TFT, i.e. 1.67M colors.
*/
	unsigned char temp;
	LCD_CmdWrite(0x11);
	temp = LCD_DataRead();
    temp &= cClrD1;
    temp &= cClrD0;
	LCD_DataWrite(temp);
}
void Select_PIP2_Window_16bpp(void)
{
/*
PIP 1 Window Color Depth Setting
00b: 8-bpp generic TFT, i.e. 256 colors.
01b: 16-bpp generic TFT, i.e. 65K colors.
1xb: 24-bpp generic TFT, i.e. 1.67M colors.
*/
	unsigned char temp;
	LCD_CmdWrite(0x11);
	temp = LCD_DataRead();
    temp &= cClrD1;
    temp |= cSetD0;
	LCD_DataWrite(temp);
}
void Select_PIP2_Window_24bpp(void)
{
/*
PIP 1 Window Color Depth Setting
00b: 8-bpp generic TFT, i.e. 256 colors.
01b: 16-bpp generic TFT, i.e. 65K colors.
1xb: 24-bpp generic TFT, i.e. 1.67M colors.
*/
	unsigned char temp;
	LCD_CmdWrite(0x11);
	temp = LCD_DataRead();
    temp |= cSetD1;
    temp &= cClrD0;
	LCD_DataWrite(temp);
}
void Select_PIP1_Window_8bpp(void)
{
/*
PIP 2 Window Color Depth Setting
00b: 8-bpp generic TFT, i.e. 256 colors.
01b: 16-bpp generic TFT, i.e. 65K colors.
1xb: 24-bpp generic TFT, i.e. 1.67M colors.
*/
	unsigned char temp;
	LCD_CmdWrite(0x11);
	temp = LCD_DataRead();
    temp &= cClrD3;
    temp &= cClrD2;
	LCD_DataWrite(temp);
}
void Select_PIP1_Window_16bpp(void)
{
/*
PIP 2 Window Color Depth Setting
00b: 8-bpp generic TFT, i.e. 256 colors.
01b: 16-bpp generic TFT, i.e. 65K colors.
1xb: 24-bpp generic TFT, i.e. 1.67M colors.
*/
	unsigned char temp;
	LCD_CmdWrite(0x11);
	temp = LCD_DataRead();
    temp &= cClrD3;
    temp |= cSetD2;
	LCD_DataWrite(temp);
}
void Select_PIP1_Window_24bpp(void)
{
/*
PIP 2 Window Color Depth Setting
00b: 8-bpp generic TFT, i.e. 256 colors.
01b: 16-bpp generic TFT, i.e. 65K colors.
1xb: 24-bpp generic TFT, i.e. 1.67M colors.
*/
	unsigned char temp;
	LCD_CmdWrite(0x11);
	temp = LCD_DataRead();
    temp |= cSetD3;
    temp &= cClrD2;
	LCD_DataWrite(temp);
}

//[12h]=========================================================================
void PCLK_Rising(void)	 
{
/*
PCLK Inversion
0: PDAT, DE, HSYNC etc. Drive(/ change) at PCLK falling edge.
1: PDAT, DE, HSYNC etc. Drive(/ change) at PCLK rising edge.
*/
	unsigned char temp;
	LCD_CmdWrite(0x12);
	temp = LCD_DataRead();
    temp &= cClrD7;
	LCD_DataWrite(temp);
}
void PCLK_Falling(void)
{
/*
PCLK Inversion
0: PDAT, DE, HSYNC etc. Drive(/ change) at PCLK falling edge.
1: PDAT, DE, HSYNC etc. Drive(/ change) at PCLK rising edge.
*/
	unsigned char temp;
	LCD_CmdWrite(0x12);
	temp = LCD_DataRead();
    temp |= cSetD7;
	LCD_DataWrite(temp);
	
}
void Display_ON(void)
{
/*	
Display ON/OFF
0b: Display Off.
1b: Display On.
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x12);
	temp = LCD_DataRead();
	temp |= cSetD6;
	LCD_DataWrite(temp);
	
	

	
	
}

void Display_OFF(void)
{
/*	
Display ON/OFF
0b: Display Off.
1b: Display On.
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x12);
	temp = LCD_DataRead();
	temp &= cClrD6;
	LCD_DataWrite(temp);

}
void Color_Bar_ON(void)
{
/*	
Display Test Color Bar
0b: Disable.
1b: Enable.
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x12);
	temp = LCD_DataRead();
	temp |= cSetD5;
	
	LCD_DataWrite(temp);
}
void Color_Bar_OFF(void)
{
/*	
Display Test Color Bar
0b: Disable.
1b: Enable.
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x12);
	temp = LCD_DataRead();
	temp &= cClrD5;
	LCD_DataWrite(temp);
}
void HSCAN_L_to_R(void)
{
/*	
Horizontal Scan Direction
0 : From Left to Right
1 : From Right to Left
PIP window will be disabled when HDIR set as 1.
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x12);
	temp = LCD_DataRead();
	temp &= cClrD4;
	LCD_DataWrite(temp);
}
void HSCAN_R_to_L(void)
{
/*	
Horizontal Scan Direction
0 : From Left to Right
1 : From Right to Left
PIP window will be disabled when HDIR set as 1.
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x12);
	temp = LCD_DataRead();
	temp |= cSetD4;
	LCD_DataWrite(temp);
}
void VSCAN_T_to_B(void)
{
/*	
Vertical Scan direction
0 : From Top to Bottom
1 : From bottom to Top
PIP window will be disabled when VDIR set as 1.
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x12);
	temp = LCD_DataRead();
	temp &= cClrD3;
	LCD_DataWrite(temp);
}
void VSCAN_B_to_T(void)
{
/*	
Vertical Scan direction
0 : From Top to Bottom
1 : From bottom to Top
PIP window will be disabled when VDIR set as 1.
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x12);
	temp = LCD_DataRead();
	temp |= cSetD3;
	LCD_DataWrite(temp);
}
void PDATA_Set_RGB(void)
{
/*	
parallel PDATA[23:0] Output Sequence
000b : RGB.
001b : RBG.
010b : GRB.
011b : GBR.
100b : BRG.
101b : BGR.
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x12);
	temp = LCD_DataRead();
    temp &=0xf8;
	LCD_DataWrite(temp);
}
void PDATA_Set_RBG(void)
{
/*	
parallel PDATA[23:0] Output Sequence
000b : RGB.
001b : RBG.
010b : GRB.
011b : GBR.
100b : BRG.
101b : BGR.
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x12);
	temp = LCD_DataRead();
	temp &=0xf8;
    temp |= cSetD0;
	LCD_DataWrite(temp);
}
void PDATA_Set_GRB(void)
{
/*	
parallel PDATA[23:0] Output Sequence
000b : RGB.
001b : RBG.
010b : GRB.
011b : GBR.
100b : BRG.
101b : BGR.
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x12);
	temp = LCD_DataRead();
	temp &=0xf8;
    temp |= cSetD1;
	LCD_DataWrite(temp);
}
void PDATA_Set_GBR(void)
{
/*	
parallel PDATA[23:0] Output Sequence
000b : RGB.
001b : RBG.
010b : GRB.
011b : GBR.
100b : BRG.
101b : BGR.
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x12);
	temp = LCD_DataRead();
	temp &=0xf8;
    temp |= cSetD1;
    temp |= cSetD0;
	LCD_DataWrite(temp);
}
void PDATA_Set_BRG(void)
{
/*	
parallel PDATA[23:0] Output Sequence
000b : RGB.
001b : RBG.
010b : GRB.
011b : GBR.
100b : BRG.
101b : BGR.
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x12);
	temp = LCD_DataRead();
	temp &=0xf8;
	temp |= cSetD2;
	LCD_DataWrite(temp);
}
void PDATA_Set_BGR(void)
{
/*	
parallel PDATA[23:0] Output Sequence
000b : RGB.
001b : RBG.
010b : GRB.
011b : GBR.
100b : BRG.
101b : BGR.
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x12);
	temp = LCD_DataRead();
	temp &=0xf8;
	temp |= cSetD2;
    temp |= cSetD0;
	LCD_DataWrite(temp);
}

void PDATA_IDLE_STATE(void)
{
	unsigned char temp;
	
	LCD_CmdWrite(0x12);
	temp = LCD_DataRead();
	temp |=0x07;
	LCD_DataWrite(temp);

}


//[13h]=========================================================================
void HSYNC_Low_Active(void)
{
/*	
HSYNC Polarity
0 : Low active.
1 : High active.
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x13);
	temp = LCD_DataRead();
	temp &= cClrD7;
	LCD_DataWrite(temp);
}
void HSYNC_High_Active(void)
{
/*	
HSYNC Polarity
0 : Low active.
1 : High active.
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x13);
	temp = LCD_DataRead();	 
	temp |= cSetD7;
	LCD_DataWrite(temp);
}
void VSYNC_Low_Active(void)
{
/*	
VSYNC Polarity
0 : Low active.
1 : High active.
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x13);
	temp = LCD_DataRead();
	temp &= cClrD6;	
	LCD_DataWrite(temp);
}
void VSYNC_High_Active(void)
{
/*	
VSYNC Polarity
0 : Low active.
1 : High active.
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x13);
	temp = LCD_DataRead();
	temp |= cSetD6;
	LCD_DataWrite(temp);
}
void DE_Low_Active(void)
{
/*	
DE Polarity
0 : High active.
1 : Low active.
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x13);
	temp = LCD_DataRead();
    temp |= cSetD5;
	LCD_DataWrite(temp);
}
void DE_High_Active(void)
{
/*	
DE Polarity
0 : High active.
1 : Low active.
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x13);
	temp = LCD_DataRead();
	temp &= cClrD5;
	LCD_DataWrite(temp);
}
void Idle_DE_Low(void)
{
/*	
DE IDLE STATE(When STANDBY or DISPLAY OFF )
0 : Pin ¨DE〃 output is low.
1 : Pin ¨DE〃 output is high.
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x13);
	temp = LCD_DataRead();
    temp &= cClrD4;
	LCD_DataWrite(temp);
}
void Idle_DE_High(void)
{
/*	
DE IDLE STATE(When STANDBY or DISPLAY OFF )
0 : Pin ¨DE〃 output is low.
1 : Pin ¨DE〃 output is high.
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x13);
	temp = LCD_DataRead();
	temp |= cSetD4;
	LCD_DataWrite(temp);
}
void Idle_PCLK_Low(void)
{
/*	
PCLK IDLE STATE(When STANDBY or DISPLAY OFF )
0 : Pin ¨PCLK〃 output is low.
1 : Pin ¨PCLK〃 output is high.
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x13);
	temp = LCD_DataRead();
    temp &= cClrD3;
	LCD_DataWrite(temp);
}
void Idle_PCLK_High(void)
{
/*	
PCLK IDLE STATE(When STANDBY or DISPLAY OFF )
0 : Pin ¨PCLK〃 output is low.
1 : Pin ¨PCLK〃 output is high.
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x13);
	temp = LCD_DataRead();
	temp |= cSetD3;
	LCD_DataWrite(temp);
}
void Idle_PDAT_Low(void)
{
/*	
PDAT IDLE STATE(When STANDBY or DISPLAY OFF )
0 : Pins ¨PDAT[23:0]〃 output is low.
1 : Pins ¨PCLK[23:0]〃 output is high.
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x13);
	temp = LCD_DataRead();
    temp &= cClrD2;
	LCD_DataWrite(temp);
}
void Idle_PDAT_High(void)
{
/*	
PDAT IDLE STATE(When STANDBY or DISPLAY OFF )
0 : Pins ¨PDAT[23:0]〃 output is low.
1 : Pins ¨PCLK[23:0]〃 output is high.
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x13);
	temp = LCD_DataRead();
	temp |= cSetD2;
	LCD_DataWrite(temp);
}
void Idle_HSYNC_Low(void)
{
/*	
HSYNC IDLE STATE(When STANDBY or DISPLAY OFF )
0 : Pin ¨HSYNC〃 output is low.
1 : Pin ¨HSYNC〃 output is high.
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x13);
	temp = LCD_DataRead();
    temp &= cClrD1;
	LCD_DataWrite(temp);
}
void Idle_HSYNC_High(void)
{
/*	
HSYNC IDLE STATE(When STANDBY or DISPLAY OFF )
0 : Pin ¨HSYNC〃 output is low.
1 : Pin ¨HSYNC〃 output is high.
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x13);
	temp = LCD_DataRead();
	temp |= cSetD1;
	LCD_DataWrite(temp);
}
void Idle_VSYNC_Low(void)
{
/*	
VSYNC IDLE STATE(When STANDBY or DISPLAY OFF )
0 : Pin ¨VSYNC〃 output is low.
1 : Pin ¨VSYNC〃 output is high.
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x13);
	temp = LCD_DataRead();
    temp &= cClrD0;
	LCD_DataWrite(temp);
}
void Idle_VSYNC_High(void)
{
/*	
VSYNC IDLE STATE(When STANDBY or DISPLAY OFF )
0 : Pin ¨VSYNC〃 output is low.
1 : Pin ¨VSYNC〃 output is high.
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x13);
	temp = LCD_DataRead();
	temp |= cSetD0;
	LCD_DataWrite(temp);
}

//[14h][15h][1Ah][1Bh]=========================================================================
void LCD_HorizontalWidth_VerticalHeight(unsigned short WX,unsigned short HY)
{
/*
[14h] Horizontal Display Width Setting Bit[7:0]
[15h] Horizontal Display Width Fine Tuning (HDWFT) [3:0]
The register specifies the LCD panel horizontal display width in
the unit of 8 pixels resolution.
Horizontal display width(pixels) = (HDWR + 1) * 8 + HDWFTR

[1Ah] Vertical Display Height Bit[7:0]
Vertical Display Height(Line) = VDHR + 1
[1Bh] Vertical Display Height Bit[10:8]
Vertical Display Height(Line) = VDHR + 1
*/
	unsigned char temp;
	if(WX<8)
    {
	LCD_CmdWrite(0x14);
	LCD_DataWrite(0x00);
    
	LCD_CmdWrite(0x15);
	LCD_DataWrite(WX);
    
    temp=HY-1;
	LCD_CmdWrite(0x1A);
	LCD_DataWrite(temp);
	    
	temp=(HY-1)>>8;
	LCD_CmdWrite(0x1B);
	LCD_DataWrite(temp);
	}
	else
	{
    temp=(WX/8)-1;
	LCD_CmdWrite(0x14);
	LCD_DataWrite(temp);
    
    temp=WX%8;
	LCD_CmdWrite(0x15);
	LCD_DataWrite(temp);
    
    temp=HY-1;
	LCD_CmdWrite(0x1A);
	LCD_DataWrite(temp);
	    
	temp=(HY-1)>>8;
	LCD_CmdWrite(0x1B);
	LCD_DataWrite(temp);
	}
}
//[16h][17h]=========================================================================
void LCD_Horizontal_Non_Display(unsigned short WX)
{
/*
[16h] Horizontal Non-Display Period(HNDR) Bit[4:0]
This register specifies the horizontal non-display period. Also
called back porch.
Horizontal non-display period(pixels) = (HNDR + 1) * 8 + HNDFTR

[17h] Horizontal Non-Display Period Fine Tuning(HNDFT) [3:0]
This register specifies the fine tuning for horizontal non-display
period; it is used to support the SYNC mode panel. Each level of
this modulation is 1-pixel.
Horizontal non-display period(pixels) = (HNDR + 1) * 8 + HNDFTR
*/
	unsigned char temp;
	if(WX<8)
	{
	LCD_CmdWrite(0x16);
	LCD_DataWrite(0x00);
    
	LCD_CmdWrite(0x17);
	LCD_DataWrite(WX);
	}
	else
	{
    temp=(WX/8)-1;
	LCD_CmdWrite(0x16);
	LCD_DataWrite(temp);
    
    temp=WX%8;
	LCD_CmdWrite(0x17);
	LCD_DataWrite(temp);
	}	
}

//[18h]=========================================================================
void LCD_HSYNC_Start_Position(unsigned short WX)
{
/*
[18h] HSYNC Start Position[4:0]
The starting position from the end of display area to the
beginning of HSYNC. Each level of this modulation is 8-pixel.
Also called front porch.
HSYNC Start Position(pixels) = (HSTR + 1)x8
*/
	unsigned char temp;
	if(WX<8)
	{
	LCD_CmdWrite(0x18);
	LCD_DataWrite(0x00);
	}
	else
	{
    temp=(WX/8)-1;
	LCD_CmdWrite(0x18);
	LCD_DataWrite(temp);	
	}
}
//[19h]=========================================================================
void LCD_HSYNC_Pulse_Width(unsigned short WX)
{
/*
[19h] HSYNC Pulse Width(HPW) [4:0]
The period width of HSYNC.
HSYNC Pulse Width(pixels) = (HPW + 1)x8
*/
	unsigned char temp;
	if(WX<8)
	{
	LCD_CmdWrite(0x19);
	LCD_DataWrite(0x00);
	}
	else
	{
    temp=(WX/8)-1;
	LCD_CmdWrite(0x19);
	LCD_DataWrite(temp);	
	}
}
//[1Ch][1Dh]=========================================================================
void LCD_Vertical_Non_Display(unsigned short HY)
{
/*
[1Ch] Vertical Non-Display Period Bit[7:0]
Vertical Non-Display Period(Line) = (VNDR + 1)

[1Dh] Vertical Non-Display Period Bit[9:8]
Vertical Non-Display Period(Line) = (VNDR + 1)
*/
	unsigned char temp;
    temp=HY-1;
	LCD_CmdWrite(0x1C);
	LCD_DataWrite(temp);

	LCD_CmdWrite(0x1D);
	LCD_DataWrite(temp>>8);
}
//[1Eh]=========================================================================
void LCD_VSYNC_Start_Position(unsigned short HY)
{
/*
[1Eh] VSYNC Start Position[7:0]
The starting position from the end of display area to the beginning of VSYNC.
VSYNC Start Position(Line) = (VSTR + 1)
*/
	unsigned char temp;
    temp=HY-1;
	LCD_CmdWrite(0x1E);
	LCD_DataWrite(temp);
}
//[1Fh]=========================================================================
void LCD_VSYNC_Pulse_Width(unsigned short HY)
{
/*
[1Fh] VSYNC Pulse Width[5:0]
The pulse width of VSYNC in lines.
VSYNC Pulse Width(Line) = (VPWR + 1)
*/
	unsigned char temp;
    temp=HY-1;
	LCD_CmdWrite(0x1F);
	LCD_DataWrite(temp);
}
//[20h][21h][22h][23h]=========================================================================
void Main_Image_Start_Address(unsigned long Addr)	
{
/*
[20h] Main Image Start Address[7:2]
[21h] Main Image Start Address[15:8]
[22h] Main Image Start Address [23:16]
[23h] Main Image Start Address [31:24]
*/
	Write_Dir(0x20,Addr);
	Write_Dir(0x21,Addr>>8);
	Write_Dir(0x22,Addr>>16);
	Write_Dir(0x23,Addr>>24);
}
//[24h][25h]=========================================================================
void Main_Image_Width(unsigned short WX)	
{
/*
[24h] Main Image Width [7:0]
[25h] Main Image Width [12:8]
Unit: Pixel.
It must be divisible by 4. MIW Bit [1:0] tie to ¨0〃 internally.
The value is physical pixel number. Maximum value is 8188 pixels
*/
	Write_Dir(0x24,WX);
	Write_Dir(0x25,WX>>8);
}
//[26h][27h][28h][29h]=========================================================================
void Main_Window_Start_XY(unsigned short WX,unsigned short HY)	
{
/*
[26h] Main Window Upper-Left corner X-coordination [7:0]
[27h] Main Window Upper-Left corner X-coordination [12:8]
Reference Main Image coordination.
Unit: Pixel
It must be divisible by 4. MWULX Bit [1:0] tie to ¨0〃 internally.
X-axis coordination plus Horizontal display width cannot large than 8188.

[28h] Main Window Upper-Left corner Y-coordination [7:0]
[29h] Main Window Upper-Left corner Y-coordination [12:8]
Reference Main Image coordination.
Unit: Pixel
Range is between 0 and 8191.
*/
	Write_Dir(0x26,WX);
	Write_Dir(0x27,WX>>8);

	Write_Dir(0x28,HY);
	Write_Dir(0x29,HY>>8);
}
//[2Ah][2Bh][2Ch][2Dh]=========================================================================
void PIP_Display_Start_XY(unsigned short WX,unsigned short HY)	
{
/*
[2Ah] PIP Window Display Upper-Left corner X-coordination [7:0]
[2Bh] PIP Window Display Upper-Left corner X-coordination [12:8]
Reference Main Window coordination.
Unit: Pixel
It must be divisible by 4. PWDULX Bit [1:0] tie to ¨0〃 internally.
X-axis coordination should less than horizontal display width.
According to bit of Select Configure PIP 1 or 2 Windowˇs parameters. 
Function bit will be configured for relative PIP window.

[2Ch] PIP Window Display Upper-Left corner Y-coordination [7:0]
[2Dh] PIP Window Display Upper-Left corner Y-coordination [12:8]
Reference Main Window coordination.
Unit: Pixel
Y-axis coordination should less than vertical display height.
According to bit of Select Configure PIP 1 or 2 Windowˇs parameters.
Function bit will be configured for relative PIP window.
*/
	Write_Dir(0x2A,WX);
	Write_Dir(0x2B,WX>>8);
	
	Write_Dir(0x2C,HY);
	Write_Dir(0x2D,HY>>8);
}
//[2Eh][2Fh][30h][31h]=========================================================================
void PIP_Image_Start_Address(unsigned long Addr)	
{
/*
[2Eh] PIP Image Start Address[7:2]
[2Fh] PIP Image Start Address[15:8]
[30h] PIP Image Start Address [23:16]
[31h] PIP Image Start Address [31:24]
*/
	Write_Dir(0x2E,Addr);
	Write_Dir(0x2F,Addr>>8);
	Write_Dir(0x30,Addr>>16);
	Write_Dir(0x31,Addr>>24);
}
//[32h][33h]=========================================================================
void PIP_Image_Width(unsigned short WX)	
{
/*
[32h] PIP Image Width [7:0]
[33h] PIP Image Width [12:8]
Unit: Pixel.
It must be divisible by 4. PIW Bit [1:0] tie to ¨0〃 internally.
The value is physical pixel number.
This width should less than horizontal display width.
According to bit of Select Configure PIP 1 or 2 Windowˇs parameters.
Function bit will be configured for relative PIP window.
*/
	Write_Dir(0x32,WX);
	Write_Dir(0x33,WX>>8);
}
//[34h][35h][36h][37h]=========================================================================
void PIP_Window_Image_Start_XY(unsigned short WX,unsigned short HY)	
{
/*
[34h] PIP 1 or 2 Window Image Upper-Left corner X-coordination [7:0]
[35h] PIP Window Image Upper-Left corner X-coordination [12:8]
Reference PIP Image coordination.
Unit: Pixel
It must be divisible by 4. PWIULX Bit [1:0] tie to ¨0〃 internally.
X-axis coordination plus PIP image width cannot large than 8188.
According to bit of Select Configure PIP 1 or 2 Windowˇs parameters. 
Function bit will be configured for relative PIP window.

[36h] PIP Windows Display Upper-Left corner Y-coordination [7:0]
[37h] PIP Windows Image Upper-Left corner Y-coordination [12:8]
Reference PIP Image coordination.
Unit: Pixel
Y-axis coordination plus PIP window height should less than 8191.
According to bit of Select Configure PIP 1 or 2 Windowˇs parameters. 
Function bit will be configured for relative PIP window.
*/
	Write_Dir(0x34,WX);
	Write_Dir(0x35,WX>>8);
	
	Write_Dir(0x36,HY);
	Write_Dir(0x37,HY>>8);
}
//[38h][39h][3Ah][3Bh]=========================================================================
void PIP_Window_Width_Height(unsigned short WX,unsigned short HY)	
{
/*
[38h] PIP Window Width [7:0]
[39h] PIP Window Width [10:8]
Unit: Pixel.
It must be divisible by 4. The value is physical pixel number.
Maximum value is 8188 pixels.
According to bit of Select Configure PIP 1 or 2 Windowˇs parameters. 
Function bit will be configured for relative PIP window.

[3Ah] PIP Window Height [7:0]
[3Bh] PIP Window Height [10:8]
Unit: Pixel
The value is physical pixel number. Maximum value is 8191 pixels.
According to bit of Select Configure PIP 1 or 2 Windowˇs parameters. 
Function bit will be configured for relative PIP window.
*/
	Write_Dir(0x38,WX);
	Write_Dir(0x39,WX>>8);
	
	Write_Dir(0x3A,HY);
	Write_Dir(0x3B,HY>>8);
}

//[3Ch]=========================================================================
void Enable_Graphic_Cursor(void)	
{
/*
Graphic Cursor Enable
0 : Graphic Cursor disable.
1 : Graphic Cursor enable.
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x3C);
	temp = LCD_DataRead();
	temp |= cSetD4;
	LCD_DataWrite(temp);
}
void Disable_Graphic_Cursor(void)	
{
/*
Graphic Cursor Enable
0 : Graphic Cursor disable.
1 : Graphic Cursor enable.
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x3C);
	temp = LCD_DataRead();
	temp &= cClrD4;
	LCD_DataWrite(temp);
}
//
void Select_Graphic_Cursor_1(void)	
{
/*
Graphic Cursor Selection Bit
Select one from four graphic cursor types. (00b to 11b)
00b : Graphic Cursor Set 1.
01b : Graphic Cursor Set 2.
10b : Graphic Cursor Set 3.
11b : Graphic Cursor Set 4.
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x3C);
	temp = LCD_DataRead();
	temp &= cClrD3;
	temp &= cClrD2;
	LCD_DataWrite(temp);
}
void Select_Graphic_Cursor_2(void)	
{
/*
Graphic Cursor Selection Bit
Select one from four graphic cursor types. (00b to 11b)
00b : Graphic Cursor Set 1.
01b : Graphic Cursor Set 2.
10b : Graphic Cursor Set 3.
11b : Graphic Cursor Set 4.
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x3C);
	temp = LCD_DataRead();
	temp &= cClrD3;
	temp |= cSetD2;
	LCD_DataWrite(temp);
}
void Select_Graphic_Cursor_3(void)	
{
/*
Graphic Cursor Selection Bit
Select one from four graphic cursor types. (00b to 11b)
00b : Graphic Cursor Set 1.
01b : Graphic Cursor Set 2.
10b : Graphic Cursor Set 3.
11b : Graphic Cursor Set 4.
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x3C);
	temp = LCD_DataRead();
	temp |= cSetD3;
	temp &= cClrD2;
	LCD_DataWrite(temp);
}
void Select_Graphic_Cursor_4(void)	
{
/*
Graphic Cursor Selection Bit
Select one from four graphic cursor types. (00b to 11b)
00b : Graphic Cursor Set 1.
01b : Graphic Cursor Set 2.
10b : Graphic Cursor Set 3.
11b : Graphic Cursor Set 4.
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x3C);
	temp = LCD_DataRead();
	temp |= cSetD3;
	temp |= cSetD2;
	LCD_DataWrite(temp);
}
//
void Enable_Text_Cursor(void)	
{
/*
Text Cursor Enable
0 : Disable.
1 : Enable.
Text cursor & Graphic cursor cannot enable simultaneously.
Graphic cursor has higher priority then Text cursor if enabled simultaneously.
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x3C);
	temp = LCD_DataRead();
	temp |= cSetD1;
	LCD_DataWrite(temp);
}
void Disable_Text_Cursor(void)	
{
/*
Text Cursor Enable
0 : Disable.
1 : Enable.
Text cursor & Graphic cursor cannot enable simultaneously.
Graphic cursor has higher priority then Text cursor if enabled simultaneously.
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x3C);
	temp = LCD_DataRead();
	temp &= cClrD1;
	LCD_DataWrite(temp);
}
//
void Enable_Text_Cursor_Blinking(void)	
{
/*
Text Cursor Blinking Enable
0 : Disable.
1 : Enable.
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x3C);
	temp = LCD_DataRead();
	temp |= cSetD0;
	LCD_DataWrite(temp);
}
void Disable_Text_Cursor_Blinking(void)	
{
/*
Text Cursor Blinking Enable
0 : Disable.
1 : Enable.
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x3C);
	temp = LCD_DataRead();
	temp &= cClrD0;
	LCD_DataWrite(temp);
}
//[3Dh]=========================================================================
void Blinking_Time_Frames(unsigned char temp)	
{
/*
Text Cursor Blink Time Setting (Unit: Frame)
00h : 1 frame time.
01h : 2 frames time.
02h : 3 frames time.
:
FFh : 256 frames time.
*/
	LCD_CmdWrite(0x3D);
	LCD_DataWrite(temp);
}
//[3Eh]=========================================================================
void Text_Cursor_H_V(unsigned short WX,unsigned short HY)
{
/*
[3Eh]
Text Cursor Horizontal Size Setting[4:0]
Unit : Pixel
Zero-based number. Value ¨0〃 means 1 pixel.
Note : When font is enlarged, the cursor setting will multiply the
same times as the font enlargement.
[3Fh]
Text Cursor Vertical Size Setting[4:0]
Unit : Pixel
Zero-based number. Value ¨0〃 means 1 pixel.
Note : When font is enlarged, the cursor setting will multiply the
same times as the font enlargement.
*/
	LCD_CmdWrite(0x3E);
	LCD_DataWrite(WX);
	LCD_CmdWrite(0x3F);
	LCD_DataWrite(HY);
}
//[40h][41h][42h][43h]=========================================================================
void Graphic_Cursor_XY(unsigned short WX,unsigned short HY)
{
/*
[40h] Graphic Cursor Horizontal Location[7:0]
[41h] Graphic Cursor Horizontal Location[12:8]
[42h] Graphic Cursor Vertical Location[7:0]
[43h] Graphic Cursor Vertical Location[12:8]
Reference main Window coordination.
*/	
	Write_Dir(0x40,WX);
	Write_Dir(0x41,WX>>8);

	Write_Dir(0x42,HY);
	Write_Dir(0x43,HY>>8);
}
//[44h]=========================================================================
void Set_Graphic_Cursor_Color_1(unsigned char temp)
{
/*
[44h] Graphic Cursor Color 0 with 256 Colors
RGB Format [7:0] = RRRGGGBB.
*/	
	Write_Dir(0x44,temp);
}
//[45h]=========================================================================
void Set_Graphic_Cursor_Color_2(unsigned char temp)
{
/*
[45h] Graphic Cursor Color 1 with 256 Colors
RGB Format [7:0] = RRRGGGBB.
*/	
	Write_Dir(0x45,temp);
}
//[50h][51h][52h][53h]=========================================================================
void Canvas_Image_Start_address(unsigned long Addr)	
{
/*
[50h] Start address of Canvas [7:0]
[51h] Start address of Canvas [15:8]
[52h] Start address of Canvas [23:16]
[53h] Start address of Canvas [31:24]
*/
	Write_Dir(0x50,Addr);
	Write_Dir(0x51,Addr>>8);
	Write_Dir(0x52,Addr>>16);
	Write_Dir(0x53,Addr>>24);
}
//[54h][55h]=========================================================================
void Canvas_image_width(unsigned short WX)	
{
/*
[54h] Canvas image width [7:2]
[55h] Canvas image width [12:8]
*/
	Write_Dir(0x54,WX);
	Write_Dir(0x55,WX>>8);
}
//[56h][57h][58h][59h]=========================================================================
void Active_Window_XY(unsigned short WX,unsigned short HY)	
{
/*
[56h] Active Window Upper-Left corner X-coordination [7:0]
[57h] Active Window Upper-Left corner X-coordination [12:8]
[58h] Active Window Upper-Left corner Y-coordination [7:0]
[59h] Active Window Upper-Left corner Y-coordination [12:8]
*/
	Write_Dir(0x56,WX);
	Write_Dir(0x57,WX>>8);
	
	Write_Dir(0x58,HY);
	Write_Dir(0x59,HY>>8);
}
//[5Ah][5Bh][5Ch][5Dh]=========================================================================
void Active_Window_WH(unsigned short WX,unsigned short HY)	
{
/*
[5Ah] Width of Active Window [7:0]
[5Bh] Width of Active Window [12:8]
[5Ch] Height of Active Window [7:0]
[5Dh] Height of Active Window [12:8]
*/
	Write_Dir(0x5A,WX);
	Write_Dir(0x5B,WX>>8);
 
 	Write_Dir(0x5C,HY);
	Write_Dir(0x5D,HY>>8);
}
//[5Eh]=========================================================================
void Select_Write_Data_Position(void)	
{
/*
Select to read back Graphic Read/Write position
0: read back Graphic Write position
1: read back Graphic Read position
*/
	unsigned char temp;

	LCD_CmdWrite(0x5E);
	temp = LCD_DataRead();
	temp &= cClrD3;
	LCD_DataWrite(temp);
}
void Select_Read_Data_Position(void)	
{
/*
Select to read back Graphic Read/Write position
0: read back Graphic Write position
1: read back Graphic Read position
*/
	unsigned char temp;

	LCD_CmdWrite(0x5E);
	temp = LCD_DataRead();
	temp |= cSetD3;
	LCD_DataWrite(temp);
}
void Memory_XY_Mode(void)	
{
/*
Canvas addressing mode
0: Block mode (X-Y coordination addressing)
1: linear mode
*/
	unsigned char temp;

	LCD_CmdWrite(0x5E);
	temp = LCD_DataRead();
	temp &= cClrD2;
	LCD_DataWrite(temp);
}
void Memory_Linear_Mode(void)	
{
/*
Canvas addressing mode
0: Block mode (X-Y coordination addressing)
1: linear mode
*/
	unsigned char temp;

	LCD_CmdWrite(0x5E);
	temp = LCD_DataRead();
	temp |= cSetD2;
	LCD_DataWrite(temp);
}
void Memory_8bpp_Mode(void)	
{
/*
Canvas imageˇs color depth & memory R/W data width
In Block Mode:
00: 8bpp
01: 16bpp
1x: 24bpp
In Linear Mode:
X0: 8-bits memory data read/write.
X1: 16-bits memory data read/write
*/
	unsigned char temp;

	LCD_CmdWrite(0x5E);
	temp = LCD_DataRead();
	temp &= cClrD1;
	temp &= cClrD0;
	LCD_DataWrite(temp);
}
void Memory_16bpp_Mode(void)	
{
/*
Canvas imageˇs color depth & memory R/W data width
In Block Mode:
00: 8bpp
01: 16bpp
1x: 24bpp
In Linear Mode:
X0: 8-bits memory data read/write.
X1: 16-bits memory data read/write
*/
	unsigned char temp;

	LCD_CmdWrite(0x5E);
	temp = LCD_DataRead();
	temp &= cClrD1;
	temp |= cSetD0;
	LCD_DataWrite(temp);
}
void Memory_24bpp_Mode(void)	
{
/*
Canvas imageˇs color depth & memory R/W data width
In Block Mode:
00: 8bpp
01: 16bpp
1x: 24bpp
In Linear Mode:
X0: 8-bits memory data read/write.
X1: 16-bits memory data read/write
*/
	unsigned char temp;

	LCD_CmdWrite(0x5E);
	temp = LCD_DataRead();
	temp |= cSetD1;
	temp &= cClrD0;
	LCD_DataWrite(temp);
}

//[5Fh][60h][61h][62h]=========================================================================
void Goto_Pixel_XY(unsigned short WX,unsigned short HY)	
{
/*
[Write]: Set Graphic Read/Write position
[Read]: Current Graphic Read/Write position
Read back is Read position or Write position depends on
REG[5Eh] bit3, Select to read back Graphic Read/Write position.
When DPRAM Linear mode:Graphic Read/Write Position [31:24][23:16][15:8][7:0]
When DPRAM Active window mode:Graphic Read/Write 
Horizontal Position [12:8][7:0], 
Vertical Position [12:8][7:0].
Reference Canvas image coordination. Unit: Pixel
*/
	Write_Dir(0x5F,WX);
	Write_Dir(0x60,WX>>8);
	
	Write_Dir(0x61,HY);
	Write_Dir(0x62,HY>>8);
}
void Goto_Linear_Addr(unsigned long Addr)
{
 Write_Dir(0x5F,Addr);
 Write_Dir(0x60,Addr>>8); 
 Write_Dir(0x61,Addr>>16);
 Write_Dir(0x62,Addr>>24);
}


//[63h][64h][65h][66h]=========================================================================
void Goto_Text_XY(unsigned short WX,unsigned short HY)	
{
/*
Write: Set Text Write position
Read: Current Text Write position
Text Write X-coordination [12:8][7:0]
Text Write Y-coordination [12:8][7:0]
Reference Canvas image coordination.
Unit: Pixel
*/
	Write_Dir(0x63,WX);
	Write_Dir(0x64,WX>>8);
	
	Write_Dir(0x65,HY);
	Write_Dir(0x66,HY>>8);
}
//[67h]=========================================================================
/*
[bit7]Draw Line / Triangle Start Signal
Write Function
0 : Stop the drawing function.
1 : Start the drawing function.
Read Function
0 : Drawing function complete.
1 : Drawing function is processing.
[bit5]Fill function for Triangle Signal
0 : Non fill.
1 : Fill.
[bit1]Draw Triangle or Line Select Signal
0 : Draw Line
1 : Draw Triangle
*/
void Start_Line(void)
{
	LCD_CmdWrite(0x67);
	LCD_DataWrite(0x80);
	Check_Busy_Draw();
}
void Start_Triangle(void)
{
	LCD_CmdWrite(0x67);
	LCD_DataWrite(0x82);//B1000_0010
	Check_Busy_Draw();
}
void Start_Triangle_Fill(void)
{

	LCD_CmdWrite(0x67);
	LCD_DataWrite(0xA2);//B1010_0010
	Check_Busy_Draw();
}
//[68h][69h][6Ah][6Bh]=========================================================================
//絬癬翴
void Line_Start_XY(unsigned short WX,unsigned short HY)
{
/*
[68h] Draw Line/Square/Triangle Start X-coordination [7:0]
[69h] Draw Line/Square/Triangle Start X-coordination [12:8]
[6Ah] Draw Line/Square/Triangle Start Y-coordination [7:0]
[6Bh] Draw Line/Square/Triangle Start Y-coordination [12:8]
*/
	LCD_CmdWrite(0x68);
	LCD_DataWrite(WX);

	LCD_CmdWrite(0x69);
	LCD_DataWrite(WX>>8);

	LCD_CmdWrite(0x6A);
	LCD_DataWrite(HY);

	LCD_CmdWrite(0x6B);
	LCD_DataWrite(HY>>8);
}
//[6Ch][6Dh][6Eh][6Fh]=========================================================================
//絬沧翴
void Line_End_XY(unsigned short WX,unsigned short HY)
{
/*
[6Ch] Draw Line/Square/Triangle End X-coordination [7:0]
[6Dh] Draw Line/Square/Triangle End X-coordination [12:8]
[6Eh] Draw Line/Square/Triangle End Y-coordination [7:0]
[6Fh] Draw Line/Square/Triangle End Y-coordination [12:8]
*/
	LCD_CmdWrite(0x6C);
	LCD_DataWrite(WX);

	LCD_CmdWrite(0x6D);
	LCD_DataWrite(WX>>8);

	LCD_CmdWrite(0x6E);
	LCD_DataWrite(HY);

	LCD_CmdWrite(0x6F);
	LCD_DataWrite(HY>>8);
}
//[68h]~[73h]=========================================================================
//à-翴1
void Triangle_Point1_XY(unsigned short WX,unsigned short HY)
{
/*
[68h] Draw Line/Square/Triangle Start X-coordination [7:0]
[69h] Draw Line/Square/Triangle Start X-coordination [12:8]
[6Ah] Draw Line/Square/Triangle Start Y-coordination [7:0]
[6Bh] Draw Line/Square/Triangle Start Y-coordination [12:8]
*/
	LCD_CmdWrite(0x68);
	LCD_DataWrite(WX);

	LCD_CmdWrite(0x69);
	LCD_DataWrite(WX>>8);

	LCD_CmdWrite(0x6A);
	LCD_DataWrite(HY);

	LCD_CmdWrite(0x6B);
	LCD_DataWrite(HY>>8);
}
//à-翴2
void Triangle_Point2_XY(unsigned short WX,unsigned short HY)
{
/*
[6Ch] Draw Line/Square/Triangle End X-coordination [7:0]
[6Dh] Draw Line/Square/Triangle End X-coordination [12:8]
[6Eh] Draw Line/Square/Triangle End Y-coordination [7:0]
[6Fh] Draw Line/Square/Triangle End Y-coordination [12:8]
*/
	LCD_CmdWrite(0x6C);
	LCD_DataWrite(WX);

	LCD_CmdWrite(0x6D);
	LCD_DataWrite(WX>>8);

	LCD_CmdWrite(0x6E);
	LCD_DataWrite(HY);

	LCD_CmdWrite(0x6F);
	LCD_DataWrite(HY>>8);
}
//à-翴3
void Triangle_Point3_XY (unsigned short WX,unsigned short HY)
{
/*
[70h] Draw Triangle Point 3 X-coordination [7:0]
[71h] Draw Triangle Point 3 X-coordination [12:8]
[72h] Draw Triangle Point 3 Y-coordination [7:0]
[73h] Draw Triangle Point 3 Y-coordination [12:8]
*/
	LCD_CmdWrite(0x70);
	LCD_DataWrite(WX);

	LCD_CmdWrite(0x71);
	LCD_DataWrite(WX>>8);

	LCD_CmdWrite(0x72);
	LCD_DataWrite(HY);

	LCD_CmdWrite(0x73);
	LCD_DataWrite(HY>>8);
}
//よ癬翴
void Square_Start_XY(unsigned short WX,unsigned short HY)
{
/*
[68h] Draw Line/Square/Triangle Start X-coordination [7:0]
[69h] Draw Line/Square/Triangle Start X-coordination [12:8]
[6Ah] Draw Line/Square/Triangle Start Y-coordination [7:0]
[6Bh] Draw Line/Square/Triangle Start Y-coordination [12:8]
*/
	LCD_CmdWrite(0x68);
	LCD_DataWrite(WX);

	LCD_CmdWrite(0x69);
	LCD_DataWrite(WX>>8);

	LCD_CmdWrite(0x6A);
	LCD_DataWrite(HY);

	LCD_CmdWrite(0x6B);
	LCD_DataWrite(HY>>8);
}
//よ沧翴
void Square_End_XY(unsigned short WX,unsigned short HY)
{
/*
[6Ch] Draw Line/Square/Triangle End X-coordination [7:0]
[6Dh] Draw Line/Square/Triangle End X-coordination [12:8]
[6Eh] Draw Line/Square/Triangle End Y-coordination [7:0]
[6Fh] Draw Line/Square/Triangle End Y-coordination [12:8]
*/
	LCD_CmdWrite(0x6C);
	LCD_DataWrite(WX);

	LCD_CmdWrite(0x6D);
	LCD_DataWrite(WX>>8);

	LCD_CmdWrite(0x6E);
	LCD_DataWrite(HY);

	LCD_CmdWrite(0x6F);
	LCD_DataWrite(HY>>8);
}
//[76h]=========================================================================
/*
[bit7]
Draw Circle / Ellipse / Square /Circle Square Start Signal 
Write Function
0 : Stop the drawing function.
1 : Start the drawing function.
Read Function
0 : Drawing function complete.
1 : Drawing function is processing.
[bit6]
Fill the Circle / Ellipse / Square / Circle Square Signal
0 : Non fill.
1 : fill.
[bit5 bit4]
Draw Circle / Ellipse / Square / Ellipse Curve / Circle Square Select
00 : Draw Circle / Ellipse
01 : Draw Circle / Ellipse Curve
10 : Draw Square.
11 : Draw Circle Square.
[bit1 bit0]
Draw Circle / Ellipse Curve Part Select
00 : 
01 : 
10 : 
11 : 
*/
void Start_Circle_or_Ellipse(void)
{
	LCD_CmdWrite(0x76);
	LCD_DataWrite(0x80);//B1000_XXXX
	Check_Busy_Draw();	
}
void Start_Circle_or_Ellipse_Fill(void)
{
	LCD_CmdWrite(0x76);
	LCD_DataWrite(0xC0);//B1100_XXXX
	Check_Busy_Draw();	
}
//
void Start_Left_Down_Curve(void)
{
	LCD_CmdWrite(0x76);
	LCD_DataWrite(0x90);//B1001_XX00
	Check_Busy_Draw();	
}
void Start_Left_Up_Curve(void)
{
	LCD_CmdWrite(0x76);
	LCD_DataWrite(0x91);//B1001_XX01
	Check_Busy_Draw();	
}
void Start_Right_Up_Curve(void)
{
	LCD_CmdWrite(0x76);
	LCD_DataWrite(0x92);//B1001_XX10
	Check_Busy_Draw();	
}
void Start_Right_Down_Curve(void)
{
	LCD_CmdWrite(0x76);
	LCD_DataWrite(0x93);//B1001_XX11
	Check_Busy_Draw();	
}
//
void Start_Left_Down_Curve_Fill(void)
{
	LCD_CmdWrite(0x76);
	LCD_DataWrite(0xD0);//B1101_XX00
	Check_Busy_Draw();
}
void Start_Left_Up_Curve_Fill(void)
{
	LCD_CmdWrite(0x76);
	LCD_DataWrite(0xD1);//B1101_XX01
	Check_Busy_Draw();
}
void Start_Right_Up_Curve_Fill(void)
{
	LCD_CmdWrite(0x76);
	LCD_DataWrite(0xD2);//B1101_XX10
	Check_Busy_Draw();
}
void Start_Right_Down_Curve_Fill(void)
{
	LCD_CmdWrite(0x76);
	LCD_DataWrite(0xD3);//B1101_XX11
	Check_Busy_Draw();
}
//
void Start_Square(void)
{
	LCD_CmdWrite(0x76);
	LCD_DataWrite(0xA0);//B1010_XXXX
	Check_Busy_Draw();
}
void Start_Square_Fill(void)
{
	LCD_CmdWrite(0x76);
	LCD_DataWrite(0xE0);//B1110_XXXX
	Check_Busy_Draw();
}
void Start_Circle_Square(void)
{
	LCD_CmdWrite(0x76);
	LCD_DataWrite(0xB0);//B1011_XXXX
	Check_Busy_Draw();	
}
void Start_Circle_Square_Fill(void)
{
	LCD_CmdWrite(0x76);
	LCD_DataWrite(0xF0);//B1111_XXXX
	Check_Busy_Draw();	
}
//[77h]~[7Eh]=========================================================================
//蛾いみ
void Circle_Center_XY(unsigned short WX,unsigned short HY)
{
/*
[7Bh] Draw Circle/Ellipse/Circle Square Center X-coordination [7:0]
[7Ch] Draw Circle/Ellipse/Circle Square Center X-coordination [12:8]
[7Dh] Draw Circle/Ellipse/Circle Square Center Y-coordination [7:0]
[7Eh] Draw Circle/Ellipse/Circle Square Center Y-coordination [12:8]
*/
	LCD_CmdWrite(0x7B);
	LCD_DataWrite(WX);

	LCD_CmdWrite(0x7C);
	LCD_DataWrite(WX>>8);

	LCD_CmdWrite(0x7D);
	LCD_DataWrite(HY);

	LCD_CmdWrite(0x7E);
	LCD_DataWrite(HY>>8);
}
//掘蛾いみ
void Ellipse_Center_XY(unsigned short WX,unsigned short HY)
{
/*
[7Bh] Draw Circle/Ellipse/Circle Square Center X-coordination [7:0]
[7Ch] Draw Circle/Ellipse/Circle Square Center X-coordination [12:8]
[7Dh] Draw Circle/Ellipse/Circle Square Center Y-coordination [7:0]
[7Eh] Draw Circle/Ellipse/Circle Square Center Y-coordination [12:8]
*/
	LCD_CmdWrite(0x7B);
	LCD_DataWrite(WX);

	LCD_CmdWrite(0x7C);
	LCD_DataWrite(WX>>8);

	LCD_CmdWrite(0x7D);
	LCD_DataWrite(HY);

	LCD_CmdWrite(0x7E);
	LCD_DataWrite(HY>>8);
}
//蛾畖
void Circle_Radius_R(unsigned short WX)
{
/*
[77h] Draw Circle/Ellipse/Circle Square Major radius [7:0]
[78h] Draw Circle/Ellipse/Circle Square Major radius [12:8]
[79h] Draw Circle/Ellipse/Circle Square Minor radius [7:0]
[7Ah] Draw Circle/Ellipse/Circle Square Minor radius [12:8]
*/
	LCD_CmdWrite(0x77);
	LCD_DataWrite(WX);

	LCD_CmdWrite(0x78);
	LCD_DataWrite(WX>>8);

	LCD_CmdWrite(0x79);
	LCD_DataWrite(WX);

	LCD_CmdWrite(0x7A);
	LCD_DataWrite(WX>>8);
}

//掘蛾畖
void Ellipse_Radius_RxRy(unsigned short WX,unsigned short HY)
{
/*
[77h] Draw Circle/Ellipse/Circle Square Major radius [7:0]
[78h] Draw Circle/Ellipse/Circle Square Major radius [12:8]
[79h] Draw Circle/Ellipse/Circle Square Minor radius [7:0]
[7Ah] Draw Circle/Ellipse/Circle Square Minor radius [12:8]
*/
	LCD_CmdWrite(0x77);
	LCD_DataWrite(WX);

	LCD_CmdWrite(0x78);
	LCD_DataWrite(WX>>8);

	LCD_CmdWrite(0x79);
	LCD_DataWrite(HY);

	LCD_CmdWrite(0x7A);
	LCD_DataWrite(HY>>8);
}

//よ锣à畖
void Circle_Square_Radius_RxRy(unsigned short WX,unsigned short HY)
{
/*
[77h] Draw Circle/Ellipse/Circle Square Major radius [7:0]
[78h] Draw Circle/Ellipse/Circle Square Major radius [12:8]
[79h] Draw Circle/Ellipse/Circle Square Minor radius [7:0]
[7Ah] Draw Circle/Ellipse/Circle Square Minor radius [12:8]
*/
	LCD_CmdWrite(0x77);
	LCD_DataWrite(WX);

	LCD_CmdWrite(0x78);
	LCD_DataWrite(WX>>8);

	LCD_CmdWrite(0x79);
	LCD_DataWrite(HY);

	LCD_CmdWrite(0x7A);
	LCD_DataWrite(HY>>8);
}

//[84h]=========================================================================
void Set_PWM_Prescaler_1_to_256(unsigned short WX)
{
/*
PWM Prescaler Register
These 8 bits determine prescaler value for Timer 0 and 1.
Time base is ¨Core_Freq / (Prescaler + 1)〃
*/
	WX=WX-1;
	LCD_CmdWrite(0x84);
	LCD_DataWrite(WX);
}
//[85h]=========================================================================
void Select_PWM1_Clock_Divided_By_1(void)
{
/*
Select MUX input for PWM Timer 1.
00 = 1; 01 = 1/2; 10 = 1/4 ; 11 = 1/8;
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x85);
	temp = LCD_DataRead();
	temp &= cClrD7;
	temp &= cClrD6;
	LCD_DataWrite(temp);
}
void Select_PWM1_Clock_Divided_By_2(void)
{
/*
Select MUX input for PWM Timer 1.
00 = 1; 01 = 1/2; 10 = 1/4 ; 11 = 1/8;
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x85);
	temp = LCD_DataRead();
	temp &= cClrD7;
	temp |= cSetD6;
	LCD_DataWrite(temp);
}
void Select_PWM1_Clock_Divided_By_4(void)
{
/*
Select MUX input for PWM Timer 1.
00 = 1; 01 = 1/2; 10 = 1/4 ; 11 = 1/8;
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x85);
	temp = LCD_DataRead();
	temp |= cSetD7;
	temp &= cClrD6;
	LCD_DataWrite(temp);
}
void Select_PWM1_Clock_Divided_By_8(void)
{
/*
Select MUX input for PWM Timer 1.
00 = 1; 01 = 1/2; 10 = 1/4 ; 11 = 1/8;
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x85);
	temp = LCD_DataRead();
	temp |= cSetD7;
	temp |= cSetD6;
	LCD_DataWrite(temp);
}
void Select_PWM0_Clock_Divided_By_1(void)
{
/*
Select MUX input for PWM Timer 0.
00 = 1; 01 = 1/2; 10 = 1/4 ; 11 = 1/8;
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x85);
	temp = LCD_DataRead();
	temp &= cClrD5;
	temp &= cClrD4;
	LCD_DataWrite(temp);
}
void Select_PWM0_Clock_Divided_By_2(void)
{
/*
Select MUX input for PWM Timer 0.
00 = 1; 01 = 1/2; 10 = 1/4 ; 11 = 1/8;
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x85);
	temp = LCD_DataRead();
	temp &= cClrD5;
	temp |= cSetD4;
	LCD_DataWrite(temp);
}
void Select_PWM0_Clock_Divided_By_4(void)
{
/*
Select MUX input for PWM Timer 0.
00 = 1; 01 = 1/2; 10 = 1/4 ; 11 = 1/8;
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x85);
	temp = LCD_DataRead();
	temp |= cSetD5;
	temp &= cClrD4;
	LCD_DataWrite(temp);
}
void Select_PWM0_Clock_Divided_By_8(void)
{
/*
Select MUX input for PWM Timer 0.
00 = 1; 01 = 1/2; 10 = 1/4 ; 11 = 1/8;
*/
	unsigned char temp;
	
	LCD_CmdWrite(0x85);
	temp = LCD_DataRead();
	temp |= cSetD5;
	temp |= cSetD4;
	LCD_DataWrite(temp);
}
//[85h].[bit3][bit2]=========================================================================
/*
XPWM[1] pin function control
0X:	XPWM[1] output system error flag (REG[00h] bit[1:0], Scan bandwidth insufficient + Memory access out of range)
10:	XPWM[1] enabled and controlled by PWM timer 1
11:	XPWM[1] output oscillator clock
//If XTEST[0] set high, then XPWM[1] will become panel scan clock input.
*/
void Select_PWM1_is_ErrorFlag(void)
{
	unsigned char temp;
	
	LCD_CmdWrite(0x85);
	temp = LCD_DataRead();
	temp &= cClrD3;
	LCD_DataWrite(temp);
}
void Select_PWM1(void)
{
	unsigned char temp;
	
	LCD_CmdWrite(0x85);
	temp = LCD_DataRead();
	temp |= cSetD3;
	temp &= cClrD2;
	LCD_DataWrite(temp);
}
void Select_PWM1_is_Osc_Clock(void)
{
	unsigned char temp;
	
	LCD_CmdWrite(0x85);
	temp = LCD_DataRead();
	temp |= cSetD3;
	temp |= cSetD2;
	LCD_DataWrite(temp);
}
//[85h].[bit1][bit0]=========================================================================
/*
XPWM[0] pin function control
0X: XPWM[0] becomes GPIO-C[7]
10: XPWM[0] enabled and controlled by PWM timer 0
11: XPWM[0] output core clock
*/
void Select_PWM0_is_GPIO_C7(void)
{
	unsigned char temp;
	
	LCD_CmdWrite(0x85);
	temp = LCD_DataRead();
	temp &= cClrD1;
	LCD_DataWrite(temp);
}
void Select_PWM0(void)
{
	unsigned char temp;
	
	LCD_CmdWrite(0x85);
	temp = LCD_DataRead();
	temp |= cSetD1;
	temp &= cClrD0;
	LCD_DataWrite(temp);
}
void Select_PWM0_is_Core_Clock(void)
{
	unsigned char temp;
	
	LCD_CmdWrite(0x85);
	temp = LCD_DataRead();
	temp |= cSetD1;
	temp |= cSetD0;
	LCD_DataWrite(temp);
}
//[86h]=========================================================================
//[86h]PWM1
void Enable_PWM1_Inverter(void)
{
/*
PWM Timer 1 output inverter on/off.
Determine the output inverter on/off for Timer 1. 
0 = Inverter off 
1 = Inverter on for PWM1
*/
	unsigned char temp;
	LCD_CmdWrite(0x86);
	temp = LCD_DataRead();
	temp |= cSetD6;
	LCD_DataWrite(temp);
}
void Disable_PWM1_Inverter(void)
{
/*
PWM Timer 1 output inverter on/off.
Determine the output inverter on/off for Timer 1. 
0 = Inverter off 
1 = Inverter on for PWM1
*/
	unsigned char temp;
	LCD_CmdWrite(0x86);
	temp = LCD_DataRead();
	temp &= cClrD6;
	LCD_DataWrite(temp);
}
void Auto_Reload_PWM1(void)
{
/*
PWM Timer 1 auto reload on/off
Determine auto reload on/off for Timer 1. 
0 = One-shot 
1 = Interval mode(auto reload)
*/
	unsigned char temp;
	LCD_CmdWrite(0x86);
	temp = LCD_DataRead();
	temp |= cSetD5;
	LCD_DataWrite(temp);
}
void One_Shot_PWM1(void)
{
/*
PWM Timer 1 auto reload on/off
Determine auto reload on/off for Timer 1. 
0 = One-shot 
1 = Interval mode(auto reload)
*/
	unsigned char temp;
	LCD_CmdWrite(0x86);
	temp = LCD_DataRead();
	temp &= cClrD5;
	LCD_DataWrite(temp);
}
void Start_PWM1(void)
{
/*
PWM Timer 1 start/stop
Determine start/stop for Timer 1. 
0 = Stop 
1 = Start for Timer 1
*/
	unsigned char temp;
	LCD_CmdWrite(0x86);
	temp = LCD_DataRead();
	temp |= cSetD4;
	LCD_DataWrite(temp);
}
void Stop_PWM1(void)
{
/*
PWM Timer 1 start/stop
Determine start/stop for Timer 1. 
0 = Stop 
1 = Start for Timer 1
*/
	unsigned char temp;
	LCD_CmdWrite(0x86);
	temp = LCD_DataRead();
	temp &= cClrD4;
	LCD_DataWrite(temp);
}
//[86h]PWM0
void Enable_PWM0_Dead_Zone(void)
{
/*
PWM Timer 0 Dead zone enable
Determine the dead zone operation. 0 = Disable. 1 = Enable.
*/
	unsigned char temp;
	LCD_CmdWrite(0x86);
	temp = LCD_DataRead();
	temp |= cSetD3;
	LCD_DataWrite(temp);
}
void Disable_PWM0_Dead_Zone(void)
{
/*
PWM Timer 0 Dead zone enable
Determine the dead zone operation. 0 = Disable. 1 = Enable.
*/
	unsigned char temp;
	LCD_CmdWrite(0x86);
	temp = LCD_DataRead();
	temp &= cClrD3;
	LCD_DataWrite(temp);
}
void Enable_PWM0_Inverter(void)
{
/*
PWM Timer 0 output inverter on/off
Determine the output inverter on/off for Timer 0. 
0 = Inverter off 
1 = Inverter on for PWM0
*/
	unsigned char temp;
	LCD_CmdWrite(0x86);
	temp = LCD_DataRead();
	temp |= cSetD2;
	LCD_DataWrite(temp);
}
void Disable_PWM0_Inverter(void)
{
/*
PWM Timer 0 output inverter on/off
Determine the output inverter on/off for Timer 0. 
0 = Inverter off 
1 = Inverter on for PWM0
*/
	unsigned char temp;
	LCD_CmdWrite(0x86);
	temp = LCD_DataRead();
	temp &= cClrD2;
	LCD_DataWrite(temp);
}
void Auto_Reload_PWM0(void)
{
/*
PWM Timer 0 auto reload on/off
Determine auto reload on/off for Timer 0. 
0 = One-shot 
1 = Interval mode(auto reload)
*/
	unsigned char temp;
	LCD_CmdWrite(0x86);
	temp = LCD_DataRead();
	temp |= cSetD1;
	LCD_DataWrite(temp);
}
void One_Shot_PWM0(void)
{
/*
PWM Timer 1 auto reload on/off
Determine auto reload on/off for Timer 1. 
0 = One-shot 
1 = Interval mode(auto reload)
*/
	unsigned char temp;
	LCD_CmdWrite(0x86);
	temp = LCD_DataRead();
	temp &= cClrD1;
	LCD_DataWrite(temp);
}
void Start_PWM0(void)
{
/*
PWM Timer 0 start/stop
Determine start/stop for Timer 0. 
0 = Stop 
1 = Start for Timer 0
*/
	unsigned char temp;
	LCD_CmdWrite(0x86);
	temp = LCD_DataRead();
	temp |= cSetD0;
	LCD_DataWrite(temp);
}
void Stop_PWM0(void)
{
/*
PWM Timer 0 start/stop
Determine start/stop for Timer 0. 
0 = Stop 
1 = Start for Timer 0
*/
	unsigned char temp;
	LCD_CmdWrite(0x86);
	temp = LCD_DataRead();
	temp &= cClrD0;
	LCD_DataWrite(temp);
}
//[87h]=========================================================================
void Set_Timer0_Dead_Zone_Length(unsigned char temp)
{
/*
Timer 0 Dead zone length register
These 8 bits determine the dead zone length. The 1 unit time of
the dead zone length is equal to that of timer 0.
*/
	LCD_CmdWrite(0x87);
	LCD_DataWrite(temp);
}
//[88h][89h]=========================================================================
void Set_Timer0_Compare_Buffer(unsigned short WX)
{
/*
Timer 0 compare buffer register
Compare buffer register total has 16 bits.
When timer counter equal or less than compare buffer register will cause PWM out
high level if inv_on bit is off.
*/
	LCD_CmdWrite(0x88);
	LCD_DataWrite(WX);
	LCD_CmdWrite(0x89);
	LCD_DataWrite(WX>>8);
}
//[8Ah][8Bh]=========================================================================
void Set_Timer0_Count_Buffer(unsigned short WX)
{
/*
Timer 0 count buffer register
Count buffer register total has 16 bits.
When timer counter equal to 0 will cause PWM timer reload Count buffer register if reload_en bit set as enable.
It may read back timer counterˇs real time value when PWM timer start.
*/
	LCD_CmdWrite(0x8A);
	LCD_DataWrite(WX);
	LCD_CmdWrite(0x8B);
	LCD_DataWrite(WX>>8);
}
//[8Ch][8Dh]=========================================================================
void Set_Timer1_Compare_Buffer(unsigned short WX)
{
/*
Timer 0 compare buffer register
Compare buffer register total has 16 bits.
When timer counter equal or less than compare buffer register will cause PWM out
high level if inv_on bit is off.
*/
	LCD_CmdWrite(0x8C);
	LCD_DataWrite(WX);
	LCD_CmdWrite(0x8D);
	LCD_DataWrite(WX>>8);
}
//[8Eh][8Fh]=========================================================================
void Set_Timer1_Count_Buffer(unsigned short WX)
{
/*
Timer 0 count buffer register
Count buffer register total has 16 bits.
When timer counter equal to 0 will cause PWM timer reload Count buffer register if reload_en bit set as enable.
It may read back timer counterˇs real time value when PWM timer start.
*/
	LCD_CmdWrite(0x8E);
	LCD_DataWrite(WX);
	LCD_CmdWrite(0x8F);
	LCD_DataWrite(WX>>8);
}


//[90h]~[B5h]=========================================================================

//[90h]=========================================================================
void BTE_Enable(void)
{	
/*
BTE Function Enable
0 : BTE Function disable.
1 : BTE Function enable.
*/
    unsigned char temp;
    LCD_CmdWrite(0x90);
    temp = LCD_DataRead();
    temp |= cSetD4 ;
	LCD_DataWrite(temp);  
}

//[90h]=========================================================================
void BTE_Disable(void)
{	
/*
BTE Function Enable
0 : BTE Function disable.
1 : BTE Function enable.
*/
    unsigned char temp;
    LCD_CmdWrite(0x90);
    temp = LCD_DataRead();
    temp &= cClrD4 ;
	LCD_DataWrite(temp);  
}

//[90h]=========================================================================
void Check_BTE_Busy(void)
{	
/*
BTE Function Status
0 : BTE Function is idle.
1 : BTE Function is busy.
*/
	unsigned char temp; 	
	do
	{
		temp=LCD_StatusRead();
	}while(temp&0x08);

}
//[90h]=========================================================================
void Pattern_Format_8X8(void)
{	
/*
Pattern Format
0 : 8X8
1 : 16X16
*/
    unsigned char temp;
    LCD_CmdWrite(0x90);
    temp = LCD_DataRead();
    temp &= cClrD0 ;
	LCD_DataWrite(temp);
}	
//[90h]=========================================================================
void Pattern_Format_16X16(void)
{	
/*
Pattern Format
0 : 8X8
1 : 16X16
*/
    unsigned char temp;
    LCD_CmdWrite(0x90);
    temp = LCD_DataRead();
    temp |= cSetD0 ;
	  LCD_DataWrite(temp);
}	

//[91h]=========================================================================
void BTE_ROP_Code(unsigned char setx)
{	
/*
BTE ROP Code[Bit7:4]
	
0000 : 0(Blackness)
0001 : ~S0.~S1 or ~ ( S0+S1 )
0010 : ~S0.S1
0011 : ~S0
0100 : S0.~S1
0101 : ~S1
0110 : S0^S1
0111 : ~S0+~S1 or ~ ( S0.S1 )
1000 : S0.S1
1001 : ~ ( S0^S1 )
1010 : S1
1011 : ~S0+S1
1100 : S0
1101 : S0+~S1
1110 : S0+S1
1111 : 1 ( Whiteness )
*/
    unsigned char temp;
    LCD_CmdWrite(0x91);
    temp = LCD_DataRead();
    temp &= 0x0f ;
		temp |= (setx<<4);
	  LCD_DataWrite(temp);
}
	
//[91h]=========================================================================
void BTE_Operation_Code(unsigned char setx)
{	
/*
BTE Operation Code[Bit3:0]
	
0000 : MPU Write BTE with ROP.
0001 : MPU Read BTE w/o ROP.
0010 : Memory copy (move) BTE in positive direction with ROP.
0011 : Memory copy (move) BTE in negative direction with ROP.
0100 : MPU Transparent Write BTE. (w/o ROP.)
0101 : Transparent Memory copy (move) BTE in positive direction (w/o ROP.)
0110 : Pattern Fill with ROP.
0111 : Pattern Fill with key-chroma
1000 : Color Expansion
1001 : Color Expansion with transparency
1010 : Move BTE in positive direction with Alpha blending
1011 : MPU Write BTE with Alpha blending
1100 : Solid Fill
1101 : Reserved
1110 : Reserved
1111 : Reserved
*/
    unsigned char temp;
    LCD_CmdWrite(0x91);
    temp = LCD_DataRead();
    temp &= 0xf0 ;
		temp |= setx ;
	  LCD_DataWrite(temp);

}
//[92h]=========================================================================
void BTE_S0_Color_8bpp(void)
{	
/*
S0 Color Depth
00 : 256 Color
01 : 64k Color
1x : 16M Color
*/	
    unsigned char temp;
    LCD_CmdWrite(0x92);
    temp = LCD_DataRead();
    temp &= cClrD6 ;
		temp &= cClrD5 ;
	  LCD_DataWrite(temp);
}	
//[92h]=========================================================================
void BTE_S0_Color_16bpp(void)
{	
/*
S0 Color Depth
00 : 256 Color
01 : 64k Color
1x : 16M Color
*/	
    unsigned char temp;
    LCD_CmdWrite(0x92);
    temp = LCD_DataRead();
    temp &= cClrD6 ;
		temp |= cSetD5 ;
	  LCD_DataWrite(temp);

}	
//[92h]=========================================================================
void BTE_S0_Color_24bpp(void)
{	
/*
S0 Color Depth
00 : 256 Color
01 : 64k Color
1x : 16M Color
*/	
    unsigned char temp;
    LCD_CmdWrite(0x92);
    temp = LCD_DataRead();
    temp |= cSetD6 ;
		//temp |= cSetD5 ;
	  LCD_DataWrite(temp);
}
//[92h]=========================================================================
void BTE_S1_Color_8bpp(void)
{	
/*
S1 Color Depth
000 : 256 Color
001 : 64k Color
010 : 16M Color
011 : Constant Color
100 : 8 bit pixel alpha blending
101 : 16 bit pixel alpha blending
*/	
    unsigned char temp;
    LCD_CmdWrite(0x92);
    temp = LCD_DataRead();
    temp &= cClrD4 ;
		temp &= cClrD3 ;
	  temp &= cClrD2 ;
	  LCD_DataWrite(temp);
}	
//[92h]=========================================================================
void BTE_S1_Color_16bpp(void)
{	
/*
S1 Color Depth
000 : 256 Color
001 : 64k Color
010 : 16M Color
011 : Constant Color
100 : 8 bit pixel alpha blending
101 : 16 bit pixel alpha blending
*/	
    unsigned char temp;
    LCD_CmdWrite(0x92);
    temp = LCD_DataRead();
    temp &= cClrD4 ;
		temp &= cClrD3 ;
	  temp |= cSetD2 ;
	  LCD_DataWrite(temp);

}
//[92h]=========================================================================
void BTE_S1_Color_24bpp(void)
{	
/*
S1 Color Depth
000 : 256 Color
001 : 64k Color
010 : 16M Color
011 : Constant Color
100 : 8 bit pixel alpha blending
101 : 16 bit pixel alpha blending
*/	
    unsigned char temp;
    LCD_CmdWrite(0x92);
    temp = LCD_DataRead();
    temp &= cClrD4 ;
		temp |= cSetD3 ;
	  temp &= cClrD2 ;
	  LCD_DataWrite(temp);
}

//[92h]=========================================================================
void BTE_S1_Color_Constant(void)
{	
/*
S1 Color Depth
000 : 256 Color
001 : 64k Color
010 : 16M Color
011 : Constant Color
100 : 8 bit pixel alpha blending
101 : 16 bit pixel alpha blending
*/	
    unsigned char temp;
    LCD_CmdWrite(0x92);
    temp = LCD_DataRead();
    temp &= cClrD4 ;
		temp |= cSetD3 ;
	  temp |= cSetD2 ;
	  LCD_DataWrite(temp);
}



//[92h]=========================================================================
void BTE_S1_Color_8bit_Alpha(void)
{	
/*
S1 Color Depth
000 : 256 Color
001 : 64k Color
010 : 16M Color
011 : Constant Color
100 : 8 bit pixel alpha blending
101 : 16 bit pixel alpha blending
*/	
    unsigned char temp;
    LCD_CmdWrite(0x92);
    temp = LCD_DataRead();
    temp |= cSetD4 ;
		temp &= cClrD3 ;
	  temp &= cClrD2 ;
	  LCD_DataWrite(temp);
}

//[92h]=========================================================================
void BTE_S1_Color_16bit_Alpha(void)
{	
/*
S1 Color Depth
000 : 256 Color
001 : 64k Color
010 : 16M Color
011 : Constant Color
100 : 8 bit pixel alpha blending
101 : 16 bit pixel alpha blending
*/	
    unsigned char temp;
    LCD_CmdWrite(0x92);
    temp = LCD_DataRead();
    temp |= cSetD4 ;
		temp &= cClrD3 ;
	  temp |= cSetD2 ;
	  LCD_DataWrite(temp);
}

//[92h]=========================================================================
void BTE_Destination_Color_8bpp(void)
{	
/*
Destination Color Depth
00 : 256 Color
01 : 64k Color
1x : 16M Color
*/	
    unsigned char temp;
    LCD_CmdWrite(0x92);
    temp = LCD_DataRead();
    temp &= cClrD1 ;
		temp &= cClrD0 ;
	  LCD_DataWrite(temp);
}	
//[92h]=========================================================================
void BTE_Destination_Color_16bpp(void)
{	
/*
Destination Color Depth
00 : 256 Color
01 : 64k Color
1x : 16M Color
*/	
    unsigned char temp;
    LCD_CmdWrite(0x92);
    temp = LCD_DataRead();
    temp &= cClrD1 ;
		temp |= cSetD0 ;
	  LCD_DataWrite(temp);

}	
//[92h]=========================================================================
void BTE_Destination_Color_24bpp(void)
{	
/*
Destination Color Depth
00 : 256 Color
10 : 64k Color
1x : 16M Color
*/	
    unsigned char temp;
    LCD_CmdWrite(0x92);
    temp = LCD_DataRead();
    temp |= cSetD1 ;
		//temp |= cSetD0 ;
	  LCD_DataWrite(temp);
}


//[93h][94h][95h][96h]=========================================================================
void BTE_S0_Memory_Start_Address(unsigned long Addr)	
{
/*
[93h] BTE S0 Memory Start Address [7:0]
[94h] BTE S0 Memory Start Address [15:8]
[95h] BTE S0 Memory Start Address [23:16]
[96h] BTE S0 Memory Start Address [31:24]
Bit [1:0] tie to ¨0〃 internally.
*/
	Write_Dir(0x93,Addr);
	Write_Dir(0x94,Addr>>8);
	Write_Dir(0x95,Addr>>16);
	Write_Dir(0x96,Addr>>24);
}


//[97h][98h]=========================================================================
void BTE_S0_Image_Width(unsigned short WX)	
{
/*
[97h] BTE S0 Image Width [7:0]
[98h] BTE S0 Image Width [12:8]
Unit: Pixel.
Bit [1:0] tie to ¨0〃 internally.
*/
	Write_Dir(0x97,WX);
	Write_Dir(0x98,WX>>8);
}


//[99h][9Ah][9Bh][9Ch]=========================================================================
void BTE_S0_Window_Start_XY(unsigned short WX,unsigned short HY)	
{
/*
[99h] BTE S0 Window Upper-Left corner X-coordination [7:0]
[9Ah] BTE S0 Window Upper-Left corner X-coordination [12:8]
[9Bh] BTE S0 Window Upper-Left corner Y-coordination [7:0]
[9Ch] BTE S0 Window Upper-Left corner Y-coordination [12:8]
*/
	Write_Dir(0x99,WX);
	Write_Dir(0x9A,WX>>8);

	Write_Dir(0x9B,HY);
	Write_Dir(0x9C,HY>>8);
}




//[9Dh][9Eh][9Fh][A0h]=========================================================================
void BTE_S1_Memory_Start_Address(unsigned long Addr)	
{
/*
[9Dh] BTE S1 Memory Start Address [7:0]
[9Eh] BTE S1 Memory Start Address [15:8]
[9Fh] BTE S1 Memory Start Address [23:16]
[A0h] BTE S1 Memory Start Address [31:24]
Bit [1:0] tie to ¨0〃 internally.
*/
	Write_Dir(0x9D,Addr);
	Write_Dir(0x9E,Addr>>8);
	Write_Dir(0x9F,Addr>>16);
	Write_Dir(0xA0,Addr>>24);
}


//Input data format:R3G3B2
void S1_Constant_color_256(unsigned char temp)
{
    LCD_CmdWrite(0x9D);
    LCD_DataWrite(temp);

    LCD_CmdWrite(0x9E);
    LCD_DataWrite(temp<<3);

    LCD_CmdWrite(0x9F);
    LCD_DataWrite(temp<<6);
}

//Input data format:R5G6B6
void S1_Constant_color_65k(unsigned short temp)
{
    LCD_CmdWrite(0x9D);
    LCD_DataWrite(temp>>8);

    LCD_CmdWrite(0x9E);
    LCD_DataWrite(temp>>3);

    LCD_CmdWrite(0x9F);
    LCD_DataWrite(temp<<3);
}

//Input data format:R8G8B8
void S1_Constant_color_16M(unsigned long temp)
{
    LCD_CmdWrite(0x9D);
    LCD_DataWrite(temp>>16);

    LCD_CmdWrite(0x9E);
    LCD_DataWrite(temp>>8);

    LCD_CmdWrite(0x9F);
    LCD_DataWrite(temp);
}




//[A1h][A2h]=========================================================================
void BTE_S1_Image_Width(unsigned short WX)	
{
/*
[A1h] BTE S1 Image Width [7:0]
[A2h] BTE S1 Image Width [12:8]
Unit: Pixel.
Bit [1:0] tie to ¨0〃 internally.
*/
	Write_Dir(0xA1,WX);
	Write_Dir(0xA2,WX>>8);
}


//[A3h][A4h][A5h][A6h]=========================================================================
void BTE_S1_Window_Start_XY(unsigned short WX,unsigned short HY)	
{
/*
[A3h] BTE S1 Window Upper-Left corner X-coordination [7:0]
[A4h] BTE S1 Window Upper-Left corner X-coordination [12:8]
[A5h] BTE S1 Window Upper-Left corner Y-coordination [7:0]
[A6h] BTE S1 Window Upper-Left corner Y-coordination [12:8]
*/
	Write_Dir(0xA3,WX);
	Write_Dir(0xA4,WX>>8);

	Write_Dir(0xA5,HY);
	Write_Dir(0xA6,HY>>8);
}




//[A7h][A8h][A9h][AAh]=========================================================================
void BTE_Destination_Memory_Start_Address(unsigned long Addr)	
{
/*
[A7h] BTE Destination Memory Start Address [7:0]
[A8h] BTE Destination Memory Start Address [15:8]
[A9h] BTE Destination Memory Start Address [23:16]
[AAh] BTE Destination Memory Start Address [31:24]
Bit [1:0] tie to ¨0〃 internally.
*/
	Write_Dir(0xA7,Addr);
	Write_Dir(0xA8,Addr>>8);
	Write_Dir(0xA9,Addr>>16);
	Write_Dir(0xAA,Addr>>24);
}


//[ABh][ACh]=========================================================================
void BTE_Destination_Image_Width(unsigned short WX)	
{
/*
[ABh] BTE Destination Image Width [7:0]
[ACh] BTE Destination Image Width [12:8]
Unit: Pixel.
Bit [1:0] tie to ¨0〃 internally.
*/
	Write_Dir(0xAB,WX);
	Write_Dir(0xAC,WX>>8);
}


//[ADh][AEh][AFh][B0h]=========================================================================
void BTE_Destination_Window_Start_XY(unsigned short WX,unsigned short HY)	
{
/*
[ADh] BTE Destination Window Upper-Left corner X-coordination [7:0]
[AEh] BTE Destination Window Upper-Left corner X-coordination [12:8]
[AFh] BTE Destination Window Upper-Left corner Y-coordination [7:0]
[B0h] BTE Destination Window Upper-Left corner Y-coordination [12:8]
*/
	Write_Dir(0xAD,WX);
	Write_Dir(0xAE,WX>>8);

	Write_Dir(0xAF,HY);
	Write_Dir(0xB0,HY>>8);
}


//[B1h][B2h][B3h][B4h]===============================================================

void BTE_Window_Size(unsigned short WX, unsigned short WY)

{
/*
[B1h] BTE Window Width [7:0]
[B2h] BTE Window Width [12:8]

[B3h] BTE Window Height [7:0]
[B4h] BTE Window Height [12:8]
*/
        Write_Dir(0xB1,WX);
        Write_Dir(0xB2,WX>>8);
	
	    Write_Dir(0xB3,WY);
        Write_Dir(0xB4,WY>>8);
}

//[B5h]=========================================================================
void BTE_Alpha_Blending_Effect(unsigned char temp)
{	
/*
Window Alpha Blending effect for S0 & S1
The value of alpha in the color code ranges from 0.0 to 1.0,
where 0.0 represents a fully transparent color, and 1.0
represents a fully opaque color.
00h: 0
01h: 1/32
02h: 2/32
:
1Eh: 30/32
1Fh: 31/32
2Xh: 1
Output Effect = (S0 image x (1 - alpha setting value)) + (S1 image x alpha setting value)
*/
    LCD_CmdWrite(0xB5);
	LCD_DataWrite(temp);  
}


//[B6h]=========================================================================
void Start_SFI_DMA(void)
{
	unsigned char temp;
	LCD_CmdWrite(0xB6);
	temp = LCD_DataRead();
    temp |= cSetD0;
	LCD_DataWrite(temp);
}

void Check_Busy_SFI_DMA(void)
{
	LCD_CmdWrite(0xB6);
	do
	{		
	}while((LCD_DataRead()&0x01)==0x01);
}


//[B7h]=========================================================================
void Select_SFI_0(void)
{
/*[bit7]
Serial Flash/ROM I/F # Select
0: Serial Flash/ROM 0 I/F is selected.
1: Serial Flash/ROM 1 I/F is selected.
*/
	unsigned char temp;
	LCD_CmdWrite(0xB7);
	temp = LCD_DataRead();
    temp &= cClrD7;
	LCD_DataWrite(temp);
}
void Select_SFI_1(void)
{
/*[bit7]
Serial Flash/ROM I/F # Select
0: Serial Flash/ROM 0 I/F is selected.
1: Serial Flash/ROM 1 I/F is selected.
*/
	unsigned char temp;
	LCD_CmdWrite(0xB7);
	temp = LCD_DataRead();
    temp |= cSetD7;
	LCD_DataWrite(temp);
}
void Select_SFI_Font_Mode(void)
{
/*[bit6]
Serial Flash /ROM Access Mode
0: Font mode  for external cgrom
1: DMA mode  for cgram , pattern , bootstart image or osd
*/
	unsigned char temp;
	LCD_CmdWrite(0xB7);
	temp = LCD_DataRead();
    temp &= cClrD6;
	LCD_DataWrite(temp);
}
void Select_SFI_DMA_Mode(void)
{
/*[bit6]
Serial Flash /ROM Access Mode
0: Font mode  for external cgrom
1: DMA mode  for cgram , pattern , bootstart image or osd
*/
	unsigned char temp;
	LCD_CmdWrite(0xB7);
	temp = LCD_DataRead();
    temp |= cSetD6;
	LCD_DataWrite(temp);
}
void Select_SFI_24bit_Address(void)
{
/*[bit5]
Serial Flash/ROM Address Mode
0: 24 bits address mode
1: 32 bits address mode
*/
	unsigned char temp;
	LCD_CmdWrite(0xB7);
	temp = LCD_DataRead();
    temp &= cClrD5;
	LCD_DataWrite(temp);
}
void Select_SFI_32bit_Address(void)
{
/*[bit5]
Serial Flash/ROM Address Mode
0: 24 bits address mode
1: 32 bits address mode
*/
	unsigned char temp;
	LCD_CmdWrite(0xB7);
	temp = LCD_DataRead();
    temp |= cSetD5;
	LCD_DataWrite(temp);
}
void Select_SFI_Waveform_Mode_0(void)
{
/*[bit4]
Serial Flash/ROM Waveform Mode
Mode 0.
Mode 3.
*/
	unsigned char temp;
	LCD_CmdWrite(0xB7);
	temp = LCD_DataRead();
    temp &= cClrD4;
	LCD_DataWrite(temp);
}
void Select_SFI_Waveform_Mode_3(void)
{
/*[bit4]
Serial Flash/ROM Waveform Mode
Mode 0.
Mode 3.
*/
	unsigned char temp;
	LCD_CmdWrite(0xB7);
	temp = LCD_DataRead();
    temp |= cSetD4;
	LCD_DataWrite(temp);
}
void Select_SFI_0_DummyRead(void)
{
/*[bit3][bit2]
Serial Flash /ROM Read Cycle 0 RW
00b: no dummy cycle mode
01b: 1 dummy cycle mode
10b: 2 dummy cycle mode
11b: 4 dummy cycle mode
*/
	unsigned char temp;
	LCD_CmdWrite(0xB7);
	temp = LCD_DataRead();
	temp &= 0xF3;
	LCD_DataWrite(temp);
}
void Select_SFI_8_DummyRead(void)
{
	unsigned char temp;
	LCD_CmdWrite(0xB7);
	temp = LCD_DataRead();
	temp &= 0xF3;
    temp |= cSetD2;
	LCD_DataWrite(temp);
}
void Select_SFI_16_DummyRead(void)
{

	unsigned char temp;
	LCD_CmdWrite(0xB7);
	temp = LCD_DataRead();
	temp &= 0xF3;
    temp |= cSetD3;
	LCD_DataWrite(temp);
}
void Select_SFI_24_DummyRead(void)
{
	unsigned char temp;
	LCD_CmdWrite(0xB7);
	temp = LCD_DataRead();
    temp |= 0x0c;
	LCD_DataWrite(temp);
}
void Select_SFI_Single_Mode(void)
{
/*[bit1][bit0]
Serial Flash /ROM I/F Data Latch Mode Select
0Xb: Single Mode
10b: Dual Mode 0.
11b: Dual Mode 1.
*/
	unsigned char temp;
	LCD_CmdWrite(0xB7);
	temp = LCD_DataRead();
	temp &= 0xFC;
	LCD_DataWrite(temp);
}
void Select_SFI_Dual_Mode0(void)
{
	unsigned char temp;
	LCD_CmdWrite(0xB7);
	temp = LCD_DataRead();
	temp &= 0xFC;
    temp |= cSetD1;
	LCD_DataWrite(temp);
}
void Select_SFI_Dual_Mode1(void)
{
	unsigned char temp;
	LCD_CmdWrite(0xB7);
	temp = LCD_DataRead();
    temp |= 0x03;
	LCD_DataWrite(temp);
}

//REG[B8h] SPI master Tx /Rx FIFO Data Register (SPIDR) 
unsigned char SPI_Master_FIFO_Data_Put(unsigned char Data)
{
    unsigned char temp;
	LCD_CmdWrite(0xB8);
	LCD_DataWrite(Data);
	while(Tx_FIFO_Empty_Flag()==0);	//磅︽掸
	temp = SPI_Master_FIFO_Data_Get();
	return temp;
}

unsigned char SPI_Master_FIFO_Data_Get(void)
{
   unsigned char temp;

	while(Rx_FIFO_Empty_Flag()==1);//ぃ琌┕磅︽
	LCD_CmdWrite(0xB8);
	temp=LCD_DataRead();
	//while(Rx_FIFO_full_flag()); //硈尿糶16掸戈惠璶
   return temp;
}

//REG[B9h] SPI master Control Register (SPIMCR2) 
void Mask_SPI_Master_Interrupt_Flag(void)
{
  unsigned char temp;
  LCD_CmdWrite(0xB9);
  temp = LCD_DataRead();
  temp |= cSetD6;
  LCD_DataWrite(temp);

} 

void Select_nSS_drive_on_xnsfcs0(void)
{
  unsigned char temp;
  LCD_CmdWrite(0xB9);
  temp = LCD_DataRead();
  temp &= cClrD5;
  LCD_DataWrite(temp);

}

void Select_nSS_drive_on_xnsfcs1(void)
{
  unsigned char temp;
  LCD_CmdWrite(0xB9);
  temp = LCD_DataRead();
  temp |= cSetD5;
  LCD_DataWrite(temp);

}

//0: inactive (nSS port will goes high) 
void nSS_Inactive(void)
{
  unsigned char temp;
  LCD_CmdWrite(0xB9);
  temp = LCD_DataRead();
  temp &= cClrD4;
  LCD_DataWrite(temp);
}
//1: active (nSS port will goes low) 
void nSS_Active(void)
{
  unsigned char temp;
  LCD_CmdWrite(0xB9);
  temp = LCD_DataRead();
  temp |= cSetD4;
  LCD_DataWrite(temp);
}

//Interrupt enable for FIFO overflow error [OVFIRQEN] 
void OVFIRQEN_Enable(void)
{
  unsigned char temp;
  LCD_CmdWrite(0xB9);
  temp = LCD_DataRead();
  temp |= cSetD3;
  LCD_DataWrite(temp);

}
//Interrupt enable for while Tx FIFO empty & SPI engine/FSM idle
void EMTIRQEN_Enable(void)
{
  unsigned char temp;
  LCD_CmdWrite(0xB9);
  temp = LCD_DataRead();
  temp |= cSetD2;
  LCD_DataWrite(temp);
}


//At CPOL=0 the base value of the clock is zero   
//o  For CPHA=0, data are read on the clock's rising edge (low->high transition) and 
//data are changed on a falling edge (high->low clock transition). 
//o  For CPHA=1, data are read on the clock's falling edge and data are changed on a 
//rising edge. 

//At CPOL=1 the base value of the clock is one (inversion of CPOL=0)   
//o  For CPHA=0, data are read on clock's falling edge and data are changed on a 
//rising edge. 
//o  For CPHA=1, data are read on clock's rising edge and data are changed on a 
//falling edge.

void Reset_CPOL(void)
{
  unsigned char temp;
  LCD_CmdWrite(0xB9);
  temp = LCD_DataRead();
  temp &= cClrD1;
  LCD_DataWrite(temp);
}

void Set_CPOL(void)
{
  unsigned char temp;
  LCD_CmdWrite(0xB9);
  temp = LCD_DataRead();
  temp |= cSetD1;
  LCD_DataWrite(temp);
}


void Reset_CPHA(void)
{
  unsigned char temp;
  LCD_CmdWrite(0xB9);
  temp = LCD_DataRead();
  temp &= cClrD0;
  LCD_DataWrite(temp);
}

void Set_CPHA(void)
{
  unsigned char temp;
  LCD_CmdWrite(0xB9);
  temp = LCD_DataRead();
  temp |= cSetD0;
  LCD_DataWrite(temp);
}


//REG[BAh] SPI master Status Register (SPIMSR) 
unsigned char Tx_FIFO_Empty_Flag(void)
{
  LCD_CmdWrite(0xBA);
  if((LCD_DataRead()&0x80)==0x80)
  return 1;
  else
  return 0;
}

unsigned char Tx_FIFO_Full_Flag(void)
{
  LCD_CmdWrite(0xBA);
  if((LCD_DataRead()&0x40)==0x40)
  return 1;
  else
  return 0;
} 

unsigned char Rx_FIFO_Empty_Flag(void)
{
  LCD_CmdWrite(0xBA);
  if((LCD_DataRead()&0x20)==0x20)
  return 1;
  else
  return 0;
} 

unsigned char Rx_FIFO_full_flag(void)
{
   LCD_CmdWrite(0xBA);
   if((LCD_DataRead()&0x10)==0x10)
   return 1;
   else
   return 0;
} 

unsigned char OVFI_Flag(void)
{
   LCD_CmdWrite(0xBA);
   if((LCD_DataRead()&0x08)==0x08)
   return 1;
   else
   return 0;
}

void Clear_OVFI_Flag(void)
{
   unsigned char temp;
   LCD_CmdWrite(0xBA);
   temp = LCD_DataRead();
   temp |= cSetD3;
   LCD_DataWrite(temp);
}

unsigned char EMTI_Flag(void)
{
   LCD_CmdWrite(0xBA);
   if((LCD_DataRead()&0x04)==0x04)
   return 1;
   else
   return 0;
}

void Clear_EMTI_Flag(void)
{
   unsigned char temp;
   LCD_CmdWrite(0xBA);
   temp = LCD_DataRead();
   temp |= cSetD2;
   LCD_DataWrite(temp);
}


//REG[BB] SPI Clock period (SPIDIV) 
void SPI_Clock_Period(unsigned char temp)
{
   LCD_CmdWrite(0xBB);
   LCD_DataWrite(temp);
} 

//[BCh][BDh][BEh][BFh]=========================================================================
void SFI_DMA_Source_Start_Address(unsigned long Addr)
{
/*
DMA Source START ADDRESS
This bits index serial flash address [7:0][15:8][23:16][31:24]
*/
	LCD_CmdWrite(0xBC);
	LCD_DataWrite(Addr);
	LCD_CmdWrite(0xBD);
	LCD_DataWrite(Addr>>8);
	LCD_CmdWrite(0xBE);
	LCD_DataWrite(Addr>>16);
	LCD_CmdWrite(0xBF);
	LCD_DataWrite(Addr>>24);
}
//[C0h][C1h][C2h][C3h]=========================================================================
void SFI_DMA_Destination_Start_Address(unsigned long Addr)
{
/*
DMA Destination START ADDRESS 
[1:0]Fix at 0
This bits index SDRAM address [7:0][15:8][23:16][31:24]
*/
	LCD_CmdWrite(0xC0);
	LCD_DataWrite(Addr);
	LCD_CmdWrite(0xC1);
	LCD_DataWrite(Addr>>8);
	LCD_CmdWrite(0xC2);
	LCD_DataWrite(Addr>>16);
	LCD_CmdWrite(0xC3);
	LCD_DataWrite(Addr>>24);
}
//[C0h][C1h][C2h][C3h]=========================================================================
void SFI_DMA_Destination_Upper_Left_Corner(unsigned short WX,unsigned short HY)
{
/*
C0h
This register defines DMA Destination Window Upper-Left corner 
X-coordination [7:0] on Canvas area. 
When REG DMACR bit 1 = 1 (Block Mode) 
This register defines Destination address [7:2] in SDRAM. 
C1h
When REG DMACR bit 1 = 0 (Linear Mode) 
This register defines DMA Destination Window Upper-Left corner 
X-coordination [12:8] on Canvas area. 
When REG DMACR bit 1 = 1 (Block Mode) 
This register defines Destination address [15:8] in SDRAM.
C2h
When REG DMACR bit 1 = 0 (Linear Mode) 
This register defines DMA Destination Window Upper-Left corner
Y-coordination [7:0] on Canvas area. 
When REG DMACR bit 1 = 1 (Block Mode) 
This register defines Destination address [23:16] in SDRAM. 
C3h
When REG DMACR bit 1 = 0 (Linear Mode) 
This register defines DMA Destination Window Upper-Left corner 
Y-coordination [12:8] on Canvas area. 
When REG DMACR bit 1 = 1 (Block Mode) 
This register defines Destination address [31:24] in SDRAM. 
*/
 
 LCD_CmdWrite(0xC0);
 LCD_DataWrite(WX);
 LCD_CmdWrite(0xC1);
 LCD_DataWrite(WX>>8);
 
 LCD_CmdWrite(0xC2);
 LCD_DataWrite(HY);
 LCD_CmdWrite(0xC3);
 LCD_DataWrite(HY>>8);
}



//[C6h][C7h][C8h][C9h]=========================================================================
void SFI_DMA_Transfer_Number(unsigned long Addr)
{
/*
Unit : Pixel
When REG DMACR bit 1 = 0 (Linear Mode)
DMA Transfer Number [7:0][15:8][23:16][31:24]

When REG DMACR bit 1 = 1 (Block Mode)
DMA Block Width [7:0][15:8]
DMA Block HIGH[7:0][15:8]
*/
	LCD_CmdWrite(0xC6);
	LCD_DataWrite(Addr);
	LCD_CmdWrite(0xC7);
	LCD_DataWrite(Addr>>8);
	LCD_CmdWrite(0xC8);
	LCD_DataWrite(Addr>>16);
	LCD_CmdWrite(0xC9);
	LCD_DataWrite(Addr>>24);
}
void SFI_DMA_Transfer_Width_Height(unsigned short WX,unsigned short HY)
{
/*
When REG DMACR bit 1 = 0 (Linear Mode)
DMA Transfer Number [7:0][15:8][23:16][31:24]

When REG DMACR bit 1 = 1 (Block Mode)
DMA Block Width [7:0][15:8]
DMA Block HIGH[7:0][15:8]
*/
	LCD_CmdWrite(0xC6);
	LCD_DataWrite(WX);
	LCD_CmdWrite(0xC7);
	LCD_DataWrite(WX>>8);

	LCD_CmdWrite(0xC8);
	LCD_DataWrite(HY);
	LCD_CmdWrite(0xC9);
	LCD_DataWrite(HY>>8);
}
//[CAh][CBh]=========================================================================
void SFI_DMA_Source_Width(unsigned short WX)
{
/*
DMA Source Picture Width [7:0][12:8]
Unit: pixel
*/
	LCD_CmdWrite(0xCA);
	LCD_DataWrite(WX);
	LCD_CmdWrite(0xCB);
	LCD_DataWrite(WX>>8);
}

//[CCh]=========================================================================

void Font_Select_UserDefine_Mode(void)
{
/*[bit7-6]
User-defined Font /CGROM Font Selection Bit in Text Mode
00 : Internal CGROM
01 : Genitop serial flash
10 : User-defined Font
*/
	unsigned char temp;
	LCD_CmdWrite(0xCC);
	temp = LCD_DataRead();
    temp |= cSetD7;
	temp &= cClrD6;
	LCD_DataWrite(temp);
}
void CGROM_Select_Internal_CGROM(void)
{
/*[bit7-6]
User-defined Font /CGROM Font Selection Bit in Text Mode
00 : Internal CGROM
01 : Genitop serial flash
10 : User-defined Font
*/
	unsigned char temp;
	LCD_CmdWrite(0xCC);
	temp = LCD_DataRead();
	temp &= cClrD7;
    temp &= cClrD6;
	LCD_DataWrite(temp);
}
void CGROM_Select_Genitop_FontROM(void)
{
/*[bit7-6]
User-defined Font /CGROM Font Selection Bit in Text Mode
00 : Internal CGROM
01 : Genitop serial flash
10 : User-defined Font
*/
	unsigned char temp;
	LCD_CmdWrite(0xCC);
	temp = LCD_DataRead();
	temp &= cClrD7;
    temp |= cSetD6;
	LCD_DataWrite(temp);
}
void Font_Select_8x16_16x16(void)
{
/*[bit5-4]
Font Height Setting
00b : 8x16 / 16x16.
01b : 12x24 / 24x24.
10b : 16x32 / 32x32.
*** User-defined Font width is decided by font code. Genitop
serial flashˇs font width is decided by font code or GT Font ROM
control register.
*/
	unsigned char temp;
	LCD_CmdWrite(0xCC);
	temp = LCD_DataRead();
    temp &= cClrD5;
    temp &= cClrD4;
	LCD_DataWrite(temp);
}
void Font_Select_12x24_24x24(void)
{
/*[bit5-4]
Font Height Setting
00b : 8x16 / 16x16.
01b : 12x24 / 24x24.
10b : 16x32 / 32x32.
*** User-defined Font width is decided by font code. Genitop
serial flashˇs font width is decided by font code or GT Font ROM
control register.
*/
	unsigned char temp;
	LCD_CmdWrite(0xCC);
	temp = LCD_DataRead();
    temp &= cClrD5;
    temp |= cSetD4;
	LCD_DataWrite(temp);
}
void Font_Select_16x32_32x32(void)
{
/*[bit5-4]
Font Height Setting
00b : 8x16 / 16x16.
01b : 12x24 / 24x24.
10b : 16x32 / 32x32.
*** User-defined Font width is decided by font code. Genitop
serial flashˇs font width is decided by font code or GT Font ROM
control register.
*/
	unsigned char temp;
	LCD_CmdWrite(0xCC);
	temp = LCD_DataRead();
    temp |= cSetD5;
    temp &= cClrD4;
	LCD_DataWrite(temp);
}
void Internal_CGROM_Select_ISOIEC8859_1(void)
{
/*
Font Selection for internal CGROM
When FNCR0 B7 = 0 and B5 = 0, Internal CGROM supports the
8x16 character sets with the standard coding of ISO/IEC 8859-1~4, 
which supports English and most of European country languages.
00b : ISO/IEC 8859-1.
01b : ISO/IEC 8859-2.
10b : ISO/IEC 8859-3.
11b : ISO/IEC 8859-4.
*/
	unsigned char temp;
	LCD_CmdWrite(0xCC);
	temp = LCD_DataRead();
    temp &= cClrD1;
    temp &= cClrD0;
	LCD_DataWrite(temp);
}
void Internal_CGROM_Select_ISOIEC8859_2(void)
{
/*
Font Selection for internal CGROM
When FNCR0 B7 = 0 and B5 = 0, Internal CGROM supports the
8x16 character sets with the standard coding of ISO/IEC 8859-1~4, 
which supports English and most of European country languages.
00b : ISO/IEC 8859-1.
01b : ISO/IEC 8859-2.
10b : ISO/IEC 8859-3.
11b : ISO/IEC 8859-4.
*/
	unsigned char temp;
	LCD_CmdWrite(0xCC);
	temp = LCD_DataRead();
    temp &= cClrD1;
    temp |= cSetD0;
	LCD_DataWrite(temp);
}
void Internal_CGROM_Select_ISOIEC8859_3(void)
{
/*
Font Selection for internal CGROM
When FNCR0 B7 = 0 and B5 = 0, Internal CGROM supports the
8x16 character sets with the standard coding of ISO/IEC 8859-1~4, 
which supports English and most of European country languages.
00b : ISO/IEC 8859-1.
01b : ISO/IEC 8859-2.
10b : ISO/IEC 8859-3.
11b : ISO/IEC 8859-4.
*/
	unsigned char temp;
	LCD_CmdWrite(0xCC);
	temp = LCD_DataRead();
    temp |= cSetD1;
    temp &= cClrD0;
	LCD_DataWrite(temp);
}
void Internal_CGROM_Select_ISOIEC8859_4(void)
{
/*
Font Selection for internal CGROM
When FNCR0 B7 = 0 and B5 = 0, Internal CGROM supports the
8x16 character sets with the standard coding of ISO/IEC 8859-1~4, 
which supports English and most of European country languages.
00b : ISO/IEC 8859-1.
01b : ISO/IEC 8859-2.
10b : ISO/IEC 8859-3.
11b : ISO/IEC 8859-4.
*/
	unsigned char temp;
	LCD_CmdWrite(0xCC);
	temp = LCD_DataRead();
    temp |= cSetD1;
    temp |= cSetD0;
	LCD_DataWrite(temp);
}
//[CDh]=========================================================================
void Enable_Font_Alignment(void)
{
/*
Full Alignment Selection Bit
0 : Full alignment disable.
1 : Full alignment enable.
*/
	unsigned char temp;
	LCD_CmdWrite(0xCD);
	temp = LCD_DataRead();
    temp |= cSetD7;
	LCD_DataWrite(temp);
}
void Disable_Font_Alignment(void)
{
/*
Full Alignment Selection Bit
0 : Full alignment disable.
1 : Full alignment enable.
*/
	unsigned char temp;
	LCD_CmdWrite(0xCD);
	temp = LCD_DataRead();
    temp &= cClrD7;
	LCD_DataWrite(temp);
}
void Font_Background_select_Transparency(void)
{
/*
Font Transparency
0 : Font with background color.
1 : Font with background transparency.
*/
	unsigned char temp;
	LCD_CmdWrite(0xCD);
	temp = LCD_DataRead();
    temp |= cSetD6;
	LCD_DataWrite(temp);
}
void Font_Background_select_Color(void)
{
/*
Font Transparency
0 : Font with background color.
1 : Font with background transparency.
*/
	unsigned char temp;
	LCD_CmdWrite(0xCD);
	temp = LCD_DataRead();
    temp &= cClrD6;
	LCD_DataWrite(temp);
}
void Font_0_degree(void)
{
/*
Font Rotation
0 : Normal
Text direction from left to right then from top to bottom
1 : Counterclockwise 90 degree & horizontal flip
Text direction from top to bottom then from left to right
(it should accommodate with set VDIR as 1)
This attribute can be changed only when previous font write
finished (core_busy = 0)
*/
	unsigned char temp;
	LCD_CmdWrite(0xCD);
	temp = LCD_DataRead();
    temp &= cClrD4;
	LCD_DataWrite(temp);
}
void Font_90_degree(void)
{
/*
Font Rotation
0 : Normal
Text direction from left to right then from top to bottom
1 : Counterclockwise 90 degree & horizontal flip
Text direction from top to bottom then from left to right
(it should accommodate with set VDIR as 1)
This attribute can be changed only when previous font write
finished (core_busy = 0)
*/
	unsigned char temp;
	LCD_CmdWrite(0xCD);
	temp = LCD_DataRead();
    temp |= cSetD4;
	LCD_DataWrite(temp);
}
void Font_Width_X1(void)
{
/*
Horizontal Font Enlargement
00b : X1.
01b : X2.
10b : X3.
11b : X4.
*/
	unsigned char temp;
	LCD_CmdWrite(0xCD);
	temp = LCD_DataRead();
    temp &= cClrD3;
    temp &= cClrD2;
	LCD_DataWrite(temp);
}
void Font_Width_X2(void)
{
/*
Horizontal Font Enlargement
00b : X1.
01b : X2.
10b : X3.
11b : X4.
*/
	unsigned char temp;
	LCD_CmdWrite(0xCD);
	temp = LCD_DataRead();
    temp &= cClrD3;
    temp |= cSetD2;
	LCD_DataWrite(temp);
}
void Font_Width_X3(void)
{
/*
Horizontal Font Enlargement
00b : X1.
01b : X2.
10b : X3.
11b : X4.
*/
	unsigned char temp;
	LCD_CmdWrite(0xCD);
	temp = LCD_DataRead();
    temp |= cSetD3;
    temp &= cClrD2;
	LCD_DataWrite(temp);
}
void Font_Width_X4(void)
{
/*
Horizontal Font Enlargement
00b : X1.
01b : X2.
10b : X3.
11b : X4.
*/
	unsigned char temp;
	LCD_CmdWrite(0xCD);
	temp = LCD_DataRead();
    temp |= cSetD3;
    temp |= cSetD2;
	LCD_DataWrite(temp);
}
void Font_Height_X1(void)
{
/*
Vertical Font Enlargement
00b : X1.
01b : X2.
10b : X3.
11b : X4.
*/
	unsigned char temp;
	LCD_CmdWrite(0xCD);
	temp = LCD_DataRead();
    temp &= cClrD1;
    temp &= cClrD0;
	LCD_DataWrite(temp);
}
void Font_Height_X2(void)
{
/*
Vertical Font Enlargement
00b : X1.
01b : X2.
10b : X3.
11b : X4.
*/
	unsigned char temp;
	LCD_CmdWrite(0xCD);
	temp = LCD_DataRead();
    temp &= cClrD1;
    temp |= cSetD0;
	LCD_DataWrite(temp);
}
void Font_Height_X3(void)
{
/*
Vertical Font Enlargement
00b : X1.
01b : X2.
10b : X3.
11b : X4.
*/
	unsigned char temp;
	LCD_CmdWrite(0xCD);
	temp = LCD_DataRead();
    temp |= cSetD1;
    temp &= cClrD0;
	LCD_DataWrite(temp);
}
void Font_Height_X4(void)
{
/*
Vertical Font Enlargement
00b : X1.
01b : X2.
10b : X3.
11b : X4.
*/
	unsigned char temp;
	LCD_CmdWrite(0xCD);
	temp = LCD_DataRead();
    temp |= cSetD1;
    temp |= cSetD0;
	LCD_DataWrite(temp);
}

//[CEh]=========================================================================
void GTFont_Select_GT21L16TW_GT21H16T1W(void)
{
/*
GT Serial Font ROM Select
000b: GT21L16TW / GT21H16T1W
001b: GT23L16U2W
010b: GT23L24T3Y / GT23H24T3Y
011b: GT23L24M1Z
100b: GT23L32S4W / GT23H32S4W
101b: GT20L24F6Y
110b: GT21L24S1W
111b: GT22L16A1Y
*/
	unsigned char temp;
	LCD_CmdWrite(0xCE);
	temp = LCD_DataRead();
    temp &= cClrD7;
    temp &= cClrD6;
    temp &= cClrD5;
	LCD_DataWrite(temp);
}
void GTFont_Select_GT23L16U2W(void)
{
/*
GT Serial Font ROM Select
000b: GT21L16TW / GT21H16T1W
001b: GT23L16U2W
010b: GT23L24T3Y / GT23H24T3Y
011b: GT23L24M1Z
100b: GT23L32S4W / GT23H32S4W
101b: GT20L24F6Y
110b: GT21L24S1W
111b: GT22L16A1Y
*/
	unsigned char temp;
	LCD_CmdWrite(0xCE);
	temp = LCD_DataRead();
    temp &= cClrD7;
    temp &= cClrD6;
    temp |= cSetD5;
	LCD_DataWrite(temp);
}
void GTFont_Select_GT23L24T3Y_GT23H24T3Y(void)
{
/*
GT Serial Font ROM Select
000b: GT21L16TW / GT21H16T1W
001b: GT23L16U2W
010b: GT23L24T3Y / GT23H24T3Y
011b: GT23L24M1Z
100b: GT23L32S4W / GT23H32S4W
101b: GT20L24F6Y
110b: GT21L24S1W
111b: GT22L16A1Y
*/
	unsigned char temp;
	LCD_CmdWrite(0xCE);
	temp = LCD_DataRead();
    temp &= cClrD7;
    temp |= cSetD6;
    temp &= cClrD5;
	LCD_DataWrite(temp);
}
void GTFont_Select_GT23L24M1Z(void)
{
/*
GT Serial Font ROM Select
000b: GT21L16TW / GT21H16T1W
001b: GT23L16U2W
010b: GT23L24T3Y / GT23H24T3Y
011b: GT23L24M1Z
100b: GT23L32S4W / GT23H32S4W
101b: GT20L24F6Y
110b: GT21L24S1W
111b: GT22L16A1Y
*/
	unsigned char temp;
	LCD_CmdWrite(0xCE);
	temp = LCD_DataRead();
    temp &= cClrD7;
    temp |= cSetD6;
    temp |= cSetD5;
	LCD_DataWrite(temp);
}
void GTFont_Select_GT23L32S4W_GT23H32S4W(void)
{
/*
GT Serial Font ROM Select
000b: GT21L16TW / GT21H16T1W
001b: GT23L16U2W
010b: GT23L24T3Y / GT23H24T3Y
011b: GT23L24M1Z
100b: GT23L32S4W / GT23H32S4W
101b: GT20L24F6Y
110b: GT21L24S1W
111b: GT22L16A1Y
*/
	unsigned char temp;
	LCD_CmdWrite(0xCE);
	temp = LCD_DataRead();
    temp |= cSetD7;
    temp &= cClrD6;
    temp &= cClrD5;
	LCD_DataWrite(temp);
}
void GTFont_Select_GT20L24F6Y(void)
{
/*
GT Serial Font ROM Select
000b: GT21L16TW / GT21H16T1W
001b: GT23L16U2W
010b: GT23L24T3Y / GT23H24T3Y
011b: GT23L24M1Z
100b: GT23L32S4W / GT23H32S4W
101b: GT20L24F6Y
110b: GT21L24S1W
111b: GT22L16A1Y
*/
	unsigned char temp;
	LCD_CmdWrite(0xCE);
	temp = LCD_DataRead();
    temp |= cSetD7;
    temp &= cClrD6;
    temp |= cSetD5;
	LCD_DataWrite(temp);
}
void GTFont_Select_GT21L24S1W(void)
{
/*
GT Serial Font ROM Select
000b: GT21L16TW / GT21H16T1W
001b: GT23L16U2W
010b: GT23L24T3Y / GT23H24T3Y
011b: GT23L24M1Z
100b: GT23L32S4W / GT23H32S4W
101b: GT20L24F6Y
110b: GT21L24S1W
111b: GT22L16A1Y
*/
	unsigned char temp;
	LCD_CmdWrite(0xCE);
	temp = LCD_DataRead();
    temp |= cSetD7;
    temp |= cSetD6;
    temp &= cClrD5;
	LCD_DataWrite(temp);
}
void GTFont_Select_GT22L16A1Y(void)
{
/*
GT Serial Font ROM Select
000b: GT21L16TW / GT21H16T1W
001b: GT23L16U2W
010b: GT23L24T3Y / GT23H24T3Y
011b: GT23L24M1Z
100b: GT23L32S4W / GT23H32S4W
101b: GT20L24F6Y
110b: GT21L24S1W
111b: GT22L16A1Y
*/
	unsigned char temp;
	LCD_CmdWrite(0xCE);
	temp = LCD_DataRead();
    temp |= cSetD7;
    temp |= cSetD6;
    temp |= cSetD5;
	LCD_DataWrite(temp);
}

//[CFh]=========================================================================
void Set_GTFont_Decoder(unsigned char temp)
{
/*
[bit7-3]
FONT ROM Coding Setting
For specific GT serial Font ROM, the coding method must be set for decoding.
00000b: GB2312
00001b: GB12345/GB18030
00010b: BIG5
00011b: UNICODE
00100b: ASCII
00101b: UNI-Japanese
00110b: JIS0208
00111b: Latin/Greek/ Cyrillic / Arabic/Thai/Hebrew
01000b: Korea
10001b: ISO-8859-1
10010b: ISO-8859-2
10011b: ISO-8859-3
10100b: ISO-8859-4
10101b: ISO-8859-5
10110b: ISO-8859-6
10111b: ISO-8859-7
11000b: ISO-8859-8
11001b: ISO-8859-9
11010b: ISO-8859-10
11011b: ISO-8859-11
11100b: ISO-8859-12
11101b: ISO-8859-13
11110b: ISO-8859-14
11111b: ISO-8859-15

[bit1-0]
ASCII / Latin/Greek/ Cyrillic / Arabic
		(ASCII)		(Latin/Greek/Cyrillic)	(Arabic)
00b		Normal			Normal 					NA
01b		Arial 		Variable Width 			Presentation Forms-A
10b		Roman 			NA 					Presentation Forms-B
11b		Bold			NA 						NA
*/
	LCD_CmdWrite(0xCF);
	LCD_DataWrite(temp);
}
//[D0h]=========================================================================
void Font_Line_Distance(unsigned char temp)
{
/*[bit4-0]
Font Line Distance Setting
Setting the font character line distance when setting memory font
write cursor auto move. (Unit: pixel)
*/
	LCD_CmdWrite(0xD0);
	LCD_DataWrite(temp);
}
//[D1h]=========================================================================
void Set_Font_to_Font_Width(unsigned char temp)
{
/*[bit5-0]
Font to Font Width Setting (Unit: pixel)
*/
	LCD_CmdWrite(0xD1);
	LCD_DataWrite(temp);
}
//[D2h]~[D4h]=========================================================================
void Foreground_RGB(unsigned char RED,unsigned char GREEN,unsigned char BLUE)
{
/*
[D2h] Foreground Color - Red, for draw, text or color expansion
[D3h] Foreground Color - Green, for draw, text or color expansion
[D4h] Foreground Color - Blue, for draw, text or color expansion
*/  
    LCD_CmdWrite(0xD2);
 LCD_DataWrite(RED);
 
    LCD_CmdWrite(0xD3);
 LCD_DataWrite(GREEN);
  
    LCD_CmdWrite(0xD4);
 LCD_DataWrite(BLUE);
}

//Input data format:R3G3B2  
void Foreground_color_256(unsigned char temp) 
{
    LCD_CmdWrite(0xD2);
 LCD_DataWrite(temp);
 
    LCD_CmdWrite(0xD3);
 LCD_DataWrite(temp<<3);
  
    LCD_CmdWrite(0xD4);
 LCD_DataWrite(temp<<6);
}
  //Read data format:R5G6B6
uint Read_Foreground_color_65k(void)
{
	 	uint temp;	uint color;
	LCD_CmdWrite(0xD2);
	temp = LCD_DataRead();
	color=temp>>3;
	LCD_CmdWrite(0xD3);
	temp = LCD_DataRead();
	color=(color<<11)+((temp>>2)<<5);
	LCD_CmdWrite(0xD4);
	temp = LCD_DataRead();
	color=color+(temp>>3);;

 return color;
}
//Input data format:R5G6B5 
void Foreground_color_65k(unsigned short temp)
{
    LCD_CmdWrite(0xD2);
 LCD_DataWrite(temp>>8);
 
    LCD_CmdWrite(0xD3);
 LCD_DataWrite(temp>>3);
  
    LCD_CmdWrite(0xD4);
 LCD_DataWrite(temp<<3);
}
 
//Input data format:R8G8B8 
void Foreground_color_16M(unsigned long temp)
{
    LCD_CmdWrite(0xD2);
 LCD_DataWrite(temp>>16);
 
    LCD_CmdWrite(0xD3);
 LCD_DataWrite(temp>>8);
  
    LCD_CmdWrite(0xD4);
 LCD_DataWrite(temp);
}
 
 
 
//[D5h]~[D7h]=========================================================================
/*
[D5h] Background Color - Red, for Text or color expansion
[D6h] Background Color - Green, for Text or color expansion
[D7h] Background Color - Blue, for Text or color expansion
*/ 
void Background_RGB(unsigned char RED,unsigned char GREEN,unsigned char BLUE)
{
   
    LCD_CmdWrite(0xD5);
 LCD_DataWrite(RED);
  
    LCD_CmdWrite(0xD6);
 LCD_DataWrite(GREEN);
   
    LCD_CmdWrite(0xD7);
 LCD_DataWrite(BLUE);
}
 
//Input data format:R3G3B2
void Background_color_256(unsigned char temp)
{
    LCD_CmdWrite(0xD5);
 LCD_DataWrite(temp);
  
    LCD_CmdWrite(0xD6);
 LCD_DataWrite(temp<<3);
   
    LCD_CmdWrite(0xD7);
 LCD_DataWrite(temp<<6);
}
 
//Input data format:R5G6B6
void Background_color_65k(unsigned short temp)
{
    LCD_CmdWrite(0xD5);
 LCD_DataWrite(temp>>8);
  
    LCD_CmdWrite(0xD6);
 LCD_DataWrite(temp>>3);
   
    LCD_CmdWrite(0xD7);
 LCD_DataWrite(temp<<3);
}
 //Read data format:R5G6B6
uint Read_Background_color_65k(void)
{
	 	uint temp;	uint color;
	LCD_CmdWrite(0xD5);
	temp = LCD_DataRead();
	color=temp>>3;
	LCD_CmdWrite(0xD6);
	temp = LCD_DataRead();
	color=(color<<11)+((temp>>2)<<5);
	LCD_CmdWrite(0xD7);
	temp = LCD_DataRead();
	color=color+(temp>>3);;


 return color;
}
//Input data format:R8G8B8
void Background_color_16M(unsigned long temp)
{
    LCD_CmdWrite(0xD5);
 LCD_DataWrite(temp>>16);
  
    LCD_CmdWrite(0xD6);
 LCD_DataWrite(temp>>8);
   
    LCD_CmdWrite(0xD7);
 LCD_DataWrite(temp);
}

//[DBh]~[DEh]=========================================================================
void CGRAM_Start_address(unsigned long Addr)
{
/*
CGRAM START ADDRESS [31:0]
*/	 
    LCD_CmdWrite(0xDB);
	LCD_DataWrite(Addr);
    LCD_CmdWrite(0xDC);
	LCD_DataWrite(Addr>>8);
    LCD_CmdWrite(0xDD);
	LCD_DataWrite(Addr>>16);
    LCD_CmdWrite(0xDE);
	LCD_DataWrite(Addr>>24);
}

//[DFh]=========================================================================
/*
[bit7] Enter Power saving state
0: Normal state.
1: Enter power saving state.
[bit1][bit0] Power saving Mode definition
00: NA
01: Standby Mode
10: Suspend Mode
11: Sleep Mode
*/
void Power_Normal_Mode(void)
{
	LCD_CmdWrite(0xDF);
	LCD_DataWrite(0x00);
}
void Power_Saving_Standby_Mode(void)
{
	LCD_CmdWrite(0xDF);
	LCD_DataWrite(0x01);
	LCD_CmdWrite(0xDF);
	LCD_DataWrite(0x81);
}
void Power_Saving_Suspend_Mode(void)
{
	LCD_CmdWrite(0xDF);
//	LCD_DataWrite(0x02);
//	LCD_CmdWrite(0xDF);
	LCD_DataWrite(0x82);
}
void Power_Saving_Sleep_Mode(void)
{
	LCD_CmdWrite(0xDF);
//	LCD_DataWrite(0x03);
//	LCD_CmdWrite(0xDF);
	LCD_DataWrite(0x83);
}


//[E5h]~[E6h]=========================================================================
void LCD_I2CM_Clock_Prescale(unsigned short WX)
{
/*
I2C Master Clock Pre-scale [7:0]
I2C Master Clock Pre-scale [15:8]
*/  
    LCD_CmdWrite(0xE5);
 LCD_DataWrite(WX);
    LCD_CmdWrite(0xE6);
 LCD_DataWrite(WX>>8);
}
//[E7h]=========================================================================
void LCD_I2CM_Transmit_Data(unsigned char temp)
{
/*
I2C Master Transmit[7:0]
*/  
    LCD_CmdWrite(0xE7);
 LCD_DataWrite(temp);
}
//[E8h]=========================================================================
unsigned char LCD_I2CM_Receiver_Data(void)
{
/*
I2C Master Receiver [7:0]
*/  
 unsigned char temp;
    LCD_CmdWrite(0xE8);
 temp=LCD_DataRead();
 return temp;
}
//[E9h]=========================================================================
/*
[bit7] START
Generate (repeated) start condition and be cleared by hardware automatically
Note : This bit is always read as 0.
[bit6] STOP
Generate stop condition and be cleared by hardware automatically
Note : This bit is always read as 0.
[bit5]
READ(READ and WRITE canˇt be used simultaneously)
Read form slave and be cleared by hardware automatically
Note : This bit is always read as 0.
[bit4]
WRITE(READ and WRITE canˇt be used simultaneously)
Write to slave and be cleared by hardware automatically
Note : This bit is always read as 0.
[bit3] ACKNOWLEDGE
When as a I2C master receiver
0 : Sent ACK.
1 : Sent NACK.
Note : This bit is always read as 0.
[bit0] Noise Filter
0 : Enable.
1 : Disable.
*/
 
void LCD_I2CM_Stop(void)
{
 LCD_CmdWrite(0xE9);
#ifdef Disable_I2CM_Noise_Filter 
 LCD_DataWrite(0x40);
#endif
 
#ifdef Enable_I2CM_Noise_Filter 
 LCD_DataWrite(0x41);
#endif
}
void LCD_I2CM_Read_With_Ack(void)
{
 
 LCD_CmdWrite(0xE9);
#ifdef Disable_I2CM_Noise_Filter 
 LCD_DataWrite(0x20);
#endif
#ifdef Enable_I2CM_Noise_Filter 
 LCD_DataWrite(0x21);
#endif
}
 
void LCD_I2CM_Read_With_Nack(void)
{
 
 LCD_CmdWrite(0xE9);
#ifdef Disable_I2CM_Noise_Filter 
 LCD_DataWrite(0x68);
#endif
#ifdef Enable_I2CM_Noise_Filter 
 LCD_DataWrite(0x69);
#endif
}
 
void LCD_I2CM_Write_With_Start(void)
{
 LCD_CmdWrite(0xE9);
#ifdef Disable_I2CM_Noise_Filter 
 LCD_DataWrite(0x90);
#endif
 
#ifdef Enable_I2CM_Noise_Filter 
 LCD_DataWrite(0x91);
#endif
 
}
 
void LCD_I2CM_Write(void)
{
 LCD_CmdWrite(0xE9);
#ifdef Disable_I2CM_Noise_Filter 
 LCD_DataWrite(0x10);
#endif
#ifdef Enable_I2CM_Noise_Filter 
 LCD_DataWrite(0x11);
#endif
}
 
 
 
//[EAh]=========================================================================
/*
 0=Ack
 1=Nack
*/
unsigned char LCD_I2CM_Check_Slave_ACK(void)
{ 
unsigned char temp;
/*[bit7]
Received acknowledge from slave
0 : Acknowledge received.
1 : No Acknowledge received. 
*/ 
 LCD_CmdWrite(0xEA);
 temp=LCD_DataRead();
 if((temp&0x80)==0x80)
  return 1;
 else
  return 0; 
}
 

/*
 0=Idle
 1=Busy
*/
unsigned char LCD_I2CM_Bus_Busy(void)
{
 unsigned char temp; 
 
/*[bit6]
I2C Bus is Busy
0 : Idle.
1 : Busy.  
*/ 
 LCD_CmdWrite(0xEA);
 temp=LCD_DataRead();
 if((temp&0x40)==0x40)
  return 1;
 else
  return 0;    
}
 
/*
 0=Complete
 1=Transferring
*/
unsigned char LCD_I2CM_transmit_Progress(void)
{
 unsigned char temp; 
/*[bit6]
 0=Complete
 1=Transferring
*/ 
 LCD_CmdWrite(0xEA);
 temp=LCD_DataRead();
 if((temp&0x02)==0x02)
 return 1;
 else
 return 0;
}
 
 /*
 0= Arbitration win
 1= Arbitration lost
*/
unsigned char LCD_I2CM_Arbitration(void)
{
 unsigned char temp; 
/*[bit6]
I2C Bus is Busy
0 : Idle.
1 : Busy.  
*/ 
 LCD_CmdWrite(0xEA);
 temp=LCD_DataRead();
 temp&=0x01;
 return temp;
}



void Graphic_Mode(void)
{
	unsigned char temp;
	LCD_CmdWrite(0x03);
	temp = LCD_DataRead();
    temp &= cClrD2;
	LCD_DataWrite(temp);
}
void Text_Mode(void)
{
	unsigned char temp;
	LCD_CmdWrite(0x03);
	temp = LCD_DataRead();
    temp |= cSetD2;
	LCD_DataWrite(temp);
}





void Show_String(uchar *str)
{   
  Text_Mode();
  LCD_CmdWrite(0x04);
  while(*str != '\0')
  {
   LCD_DataWrite(*str);
   Check_Mem_WR_FIFO_not_Full();
  ++str;   
  }
   Check_2D_Busy();

   Graphic_Mode(); //back to graphic mode
}
void Font_Init(void)
{
	  Select_SFI_1();
      Select_SFI_Font_Mode();
      Select_SFI_24bit_Address();
      Select_SFI_Waveform_Mode_0();
      Select_SFI_0_DummyRead();
      Select_SFI_Single_Mode();
      SPI_Clock_Period(2);	 // Freq must setting <=20MHZ
	 
	  Enable_SFlash_SPI();

	  Set_GTFont_Decoder(0x01);	
}




void FLASH_Read_128Mb(unsigned char* pBuffer, unsigned int ReadAddr, unsigned int NumByteToRead)//从液晶内部flash读取图片位置信息（针对128Mb容量）
{

	  Flash_Sel()
  nSS_Inactive();
  SPI_Clock_Period(1);
  //Read ID 
  nSS_Active();	 
  SPI_Master_FIFO_Data_Put(0x0B );
  SPI_Master_FIFO_Data_Put((ReadAddr & 0xFF0000) >> 16);
  SPI_Master_FIFO_Data_Put((ReadAddr& 0xFF00) >> 8);
  SPI_Master_FIFO_Data_Put(ReadAddr & 0xFF);
SPI_Master_FIFO_Data_Put(0x00);
	  while(NumByteToRead--) 
  {

    *pBuffer=SPI_Master_FIFO_Data_Put(0xff);

    pBuffer++;
  }
	
	
  nSS_Inactive();



}

 void FLASH_Read_1024GMb(unsigned char *UpdataValue,unsigned int add,unsigned short ReadAddr)//从液晶内部flash读取图片位置信息（针对1024Mb容量）
{

	uint i=0;

		 
		 
  Flash_Sel()
  nSS_Inactive();W25N01_Set_BUF();
  SPI_Clock_Period(1);

  nSS_Active();	
 
  	   SPI_Master_FIFO_Data_Put(0x13);
  SPI_Master_FIFO_Data_Put(0xff);
    SPI_Master_FIFO_Data_Put((add/2048)>>8);
  SPI_Master_FIFO_Data_Put(add/2048);
  // SPI_Master_FIFO_Data_Put(0);
 // SPI_Master_FIFO_Data_Put(0);
    nSS_Inactive(); 
  	delay_ms(10);	 

 nSS_Active(); 
  SPI_Master_FIFO_Data_Put(0x03 );

  SPI_Master_FIFO_Data_Put(0);
  SPI_Master_FIFO_Data_Put(0);
SPI_Master_FIFO_Data_Put(0xff);

	for(i=0;i<(ReadAddr+12);i++)
  {
	 if((i>=ReadAddr)&(i<(ReadAddr+12)))
   { *UpdataValue=SPI_Master_FIFO_Data_Put(0xff); UpdataValue++;}
	else
  {SPI_Master_FIFO_Data_Put(0xff);}
    
  }
	
	
	
  nSS_Inactive(); 
	
  W25N01_Clear_BUF();

}
 void FLASH_Read_1024GMb_1(unsigned char *pBuffer,unsigned int ReadAddr,unsigned short NumByteToRead)//从液晶内部flash读取图片位置信息（针对1024Mb容量）
{



	 
	unsigned short page, endpage, remainder;	 
  Flash_Sel()
  nSS_Inactive();W25N01_Set_BUF();
  SPI_Clock_Period(1);


	
	remainder = ReadAddr % 2048;				   // Starting address of the picture in a page
	page = (ReadAddr / 2048);					   // The page number of the starting address of the picture
	endpage = ((ReadAddr + NumByteToRead) / 2048); // The page number of the ending address of the picture
	  Flash_Sel()
  nSS_Inactive();
  SPI_Clock_Period(1);
	if (page == endpage)
	{
		W25N01GV_ReadPageAddr_Data(pBuffer, page, remainder, NumByteToRead);
	}
	else
	{
		W25N01GV_ReadPageAddr_Data(pBuffer, page, remainder, 2048 - remainder);
		W25N01GV_ReadPageAddr_Data(&pBuffer[2048 - remainder], endpage, 0, NumByteToRead - (2048 - remainder));
	}
}

void W25N01_Write_Enable(void)
{
  nSS_Active();
  SPI_Master_FIFO_Data_Put(0x06);
  nSS_Inactive();
}


//**************************************************************//
//write disable
//**************************************************************//
void W25N01_Write_Disable(void)
{
  nSS_Active();
  SPI_Master_FIFO_Data_Put(0x04);
  nSS_Inactive();	
}
void W25N01_Write_Status_Register1(unsigned char data)
{
  W25N01_Write_Enable();
  nSS_Active();
  SPI_Master_FIFO_Data_Put(0x1F);
  SPI_Master_FIFO_Data_Put(0xA0);
  SPI_Master_FIFO_Data_Put(data); 
  nSS_Inactive();
	
}
//**************************************************************//
//**************************************************************//
void W25N01_Write_Status_Register2(unsigned char data)
{

  W25N01_Write_Enable();
  nSS_Active();
  SPI_Master_FIFO_Data_Put(0x1F);
  SPI_Master_FIFO_Data_Put(0xB0);
  SPI_Master_FIFO_Data_Put(data); 
  nSS_Inactive();
	
}
unsigned char W25N01_Read_Status_Register2(void)
{
  unsigned char temp;

  nSS_Active();
  SPI_Master_FIFO_Data_Put(0x0F);
  SPI_Master_FIFO_Data_Put(0xB0);
  delay_us(1);
  temp=SPI_Master_FIFO_Data_Put(0xff);//dummy cycle and read back value 
  nSS_Inactive();
	
  return(temp);
}
//**************************************************************//
//**************************************************************//
unsigned char W25N01_Read_Status_Register3(void)
{
  unsigned char temp;

  nSS_Active();
  SPI_Master_FIFO_Data_Put(0x0F);
  SPI_Master_FIFO_Data_Put(0xC0);
  delay_us(1);
  temp=SPI_Master_FIFO_Data_Put(0xff);//dummy cycle and read back value 
  nSS_Inactive();
	
  return(temp);
}
 void W25N01_Set_ECC(void)
{ 
  unsigned char temp;

  temp=W25N01_Read_Status_Register2();
  temp|=0x10;
  W25N01_Write_Status_Register2(temp);
  delay_us(500);
}
void W25N01_Clear_BUF(void)
{ 
  unsigned char temp;

  temp=W25N01_Read_Status_Register2();
  temp&=0xf7;
  W25N01_Write_Status_Register2(temp);
  delay_us(500);
}
void W25N01_Set_BUF(void)
{ 
  unsigned char temp;

  temp=W25N01_Read_Status_Register2();
  temp&=0xff;
  W25N01_Write_Status_Register2(temp);
  delay_us(500);
}
 void W25N01_Reset(void)
{


      Select_nSS_drive_on_xnsfcs0();
  nSS_Inactive();

 
  nSS_Active();
  SPI_Master_FIFO_Data_Put(0xff);//spi nand reset
  nSS_Inactive(); delay_ms(10);  

}
u8 W25N01GV_ReadSR(u8 reg)
{
	u8 byte = 0;
	nSS_Active();
	SPI_Master_FIFO_Data_Put(0x0f);
	SPI_Master_FIFO_Data_Put(reg);
	byte = SPI_Master_FIFO_Data_Put(0Xff);
	nSS_Inactive();
	return byte;
}
void W25N01GV_Write_SR(u8 reg, u8 val)
{
	nSS_Active();
	SPI_Master_FIFO_Data_Put(0x1f);
	SPI_Master_FIFO_Data_Put(reg);
	SPI_Master_FIFO_Data_Put(val);
	nSS_Inactive();
}
void W25N01GV_Wait_Busy(void)
{
	while ((W25N01GV_ReadSR(0xC0) & 0x01) == 0x01)
		;
}
void W25N01GV_ReadPageAddr_Data(u8 *pBuffer, u32 PageNum, u32 PageAddr, u16 NumByteToRead)
{
	u16 i;

	//------ Retrieve data from flash, and store the data to BUFF-----
	nSS_Active(); // Enable flash
	SPI_Master_FIFO_Data_Put(0x13);
	SPI_Master_FIFO_Data_Put(0x00);
	SPI_Master_FIFO_Data_Put((u8)((PageNum) >> 8)); // Sending 16bit address
	SPI_Master_FIFO_Data_Put((u8)(PageNum));
	nSS_Inactive();
	W25N01GV_Wait_Busy();
	W25N01GV_Write_SR(0xb0, W25N01GV_ReadSR(0xb0) | 0x08); // Set the starting address to read the data

	//------ Get the BUFF data and write to pBuffer ------
	nSS_Active();
	SPI_Master_FIFO_Data_Put(0x03);
	SPI_Master_FIFO_Data_Put((PageAddr) >> 8);
	SPI_Master_FIFO_Data_Put(PageAddr);
	SPI_Master_FIFO_Data_Put(0x00);
	for (i = 0; i < NumByteToRead; i++)
	{
		pBuffer[i] = SPI_Master_FIFO_Data_Put(0XFF); // Read the data in loop
	}
	nSS_Active();
	W25N01GV_Wait_Busy();
}
void Init_Font(void)
{
 	Enable_SFlash_SPI();									             // 使能SPI功能
	Flash_Sel() 										 // 选择外挂的SPI0

	
	Memory_Linear_Mode();
	//Select_SFI_DMA_Mode();								             // 设置SPI的DMA模式
	
	SPI_Clock_Period(1);                             // SPI速率 
	SFI_DMA_Destination_Start_Address(FontMemoryAddr);  	 // 指定的内存的开始地址
	SFI_DMA_Transfer_Number(FontSize);                 // DMA传输的数量
	SFI_DMA_Source_Start_Address(0);          // flash地址
	Check_Busy_SFI_DMA(); 
	Start_SFI_DMA();
	Check_Busy_SFI_DMA();
	Memory_XY_Mode();
	CGRAM_Start_address(FontMemoryAddr+0x1F000+4); 

}
void Init_Font_1024Mb(void)
{		
Flash_Sel() 
   	Enable_SFlash_SPI();
    
    //Select_SFI_DMA_Mode();
    Select_SFI_24bit_Address();
//Select_SFI_8_DummyRead();
 Memory_Linear_Mode();
//Select_SFI_Waveform_Mode_3();
 //  Select_SFI_Dual_Mode0();

   SPI_Clock_Period(1);
SFI_DMA_Source_Start_Address(0);
	 
	 
  SFI_DMA_Transfer_Number(FontSize); 
SFI_DMA_Destination_Start_Address(FontMemoryAddr);

	  Start_SFI_DMA();
      Check_Busy_SFI_DMA();
	 CGRAM_Start_address(FontMemoryAddr+0x1F000+4); 
}
////////////////////////////////////////////////////////////////
unsigned short W25QXX_ReadID(void)
{
	u16 Temp = 0;

		nSS_Active();			
		SPI_Master_FIFO_Data_Put(0x90);//发送读取ID命令	    
		SPI_Master_FIFO_Data_Put(0x00); 	    
		SPI_Master_FIFO_Data_Put(0x00); 	    
		SPI_Master_FIFO_Data_Put(0x00); 	 			   
		Temp|=SPI_Master_FIFO_Data_Put(0xFF)<<8;  
		Temp|=SPI_Master_FIFO_Data_Put(0xFF);	 
		nSS_Inactive();				    
		return Temp;

}   		
unsigned int W25NXX_ReadID(void)
{
	uint8_t Mfr_id = 0;
	uint16_t Device_id = 0;
	uint32_t id = 0;
	

		nSS_Active();	
		SPI_Master_FIFO_Data_Put(0x9f);
		SPI_Master_FIFO_Data_Put(0x00);
		Mfr_id = SPI_Master_FIFO_Data_Put(0xff);
		Device_id = SPI_Master_FIFO_Data_Put(0xff);
		Device_id = Device_id<<8 | SPI_Master_FIFO_Data_Put(0xff);
		nSS_Inactive();
	
	id = Mfr_id;
	id = (id<<16)+Device_id;
	return id;
}  
void Flash_Init(void)
{
unsigned char Flash_ID;
Flash_Sel()
		Flash_ID = W25QXX_ReadID();
	

//Flash_ID = W25NXX_ReadID();
printf("Flash_ID:%x\r\n",Flash_ID);
if(Flash_ID==0x17)
{
Flash_Type=0;
}
if(Flash_ID==0x21)
{
Flash_Type=1;
LCD_Reg_Write(0xb7,LCD_Reg_Read(0xb7) | 0x40);
}




}
void Start_JPG_DMA(void)
{
	unsigned char temp;
	LCD_CmdWrite(0xB6);
	temp = LCD_DataRead();
	temp |= cSetD1;
	temp |= cSetD0;
	LCD_DataWrite(temp);
}
void Check_Busy_JPG_DMA(void)
{
	unsigned int retty = 0;
	LCD_CmdWrite(0xB6);
	do
	{
		retty++;
		delay_us(1);
		if(retty >= 1000000)
		{
			LCD_Reg_Write(0xB6,LCD_Reg_Read(0xb6)|0x01);		// Set back to regular DMA
			return;
		}
	} while ((LCD_DataRead() & 0x01) == 0x01);
}

void DMA_Read_03H(void)
{
/*
	REG_Page_0[B7h]:
	bit[3:0]

	Read Command code & behavior selection
	0H:03H
	4H:0BH
	8H:1BH		1x
	
	2H:3BH
	3H:BBH		2x

	7H:6BH
	AH:EBH		4x

	6H:0BH		4x w25q128 38
	BH:EBH		MXIC
	EH:EBH		Windbond
*/
	unsigned char temp;
	LCD_CmdWrite(0xB7);
	temp = LCD_DataRead();
	temp &= 0xF0;
	temp |= 0x00; 
	LCD_DataWrite(temp);	
}

void DMA_Read_0BH(void)
{
/*
	REG_Page_0[B7h]:
	bit[3:0]

	Read Command code & behavior selection
	0H:03H
	4H:0BH
	8H:1BH		1x
	
	2H:3BH
	3H:BBH		2x

	7H:6BH
	AH:EBH		4x

	6H:0BH		4x w25q128 38
	BH:EBH		MXIC
	EH:EBH		Windbond
*/
	unsigned char temp;
	LCD_CmdWrite(0xB7);
	temp = LCD_DataRead();
	temp &= 0xF0;
	temp |= 0x04; 
	LCD_DataWrite(temp);	
}

void DMA_Read_1BH(void)
{
/*
	REG_Page_0[B7h]:
	bit[3:0]

	Read Command code & behavior selection
	0H:03H
	4H:0BH
	8H:1BH		1x
	
	2H:3BH
	3H:BBH		2x

	7H:6BH
	AH:EBH		4x

	6H:0BH		4x w25q128 38
	BH:EBH		MXIC
	EH:EBH		Windbond
*/
	unsigned char temp;
	LCD_CmdWrite(0xB7);
	temp = LCD_DataRead();
	temp &= 0xF0;
	temp |= 0x08; 
	LCD_DataWrite(temp);	
}

void DMA_Read_3BH(void)
{
/*
	REG_Page_0[B7h]:
	bit[3:0]

	Read Command code & behavior selection
	0H:03H
	4H:0BH
	8H:1BH		1x
	
	2H:3BH
	3H:BBH		2x

	7H:6BH
	AH:EBH		4x

	6H:0BH		4x w25q128 38
	BH:EBH		MXIC
	EH:EBH		Windbond
*/
	unsigned char temp;
	LCD_CmdWrite(0xB7);
	temp = LCD_DataRead();
	temp &= 0xF0;
	temp |= 0x02; 
	LCD_DataWrite(temp);		
}

void DMA_Read_BBH(void)
{
/*
	REG_Page_0[B7h]:
	bit[3:0]

	Read Command code & behavior selection
	0H:03H
	4H:0BH
	8H:1BH		1x
	
	2H:3BH
	3H:BBH		2x

	7H:6BH
	AH:EBH		4x

	6H:0BH		4x w25q128 38
	BH:EBH		MXIC
	EH:EBH		Windbond
*/
	unsigned char temp;
	LCD_CmdWrite(0xB7);
	temp = LCD_DataRead();
	temp &= 0xF0;
	temp |= 0x03; 
	LCD_DataWrite(temp);	
}

void DMA_Read_6BH(void)
{
/*
	REG_Page_0[B7h]:
	bit[3:0]

	Read Command code & behavior selection
	0H:03H
	4H:0BH
	8H:1BH		1x
	
	2H:3BH
	3H:BBH		2x

	7H:6BH
	AH:EBH		4x

	6H:0BH		4x w25q128 38
	BH:EBH		MXIC
	EH:EBH		Windbond
*/
	unsigned char temp;
	LCD_CmdWrite(0xB7);
	temp = LCD_DataRead();
	temp &= 0xF0;
	temp |= 0x07; 
	LCD_DataWrite(temp);	
}

void DMA_Read_EBH(void)
{
/*
	REG_Page_0[B7h]:
	bit[3:0]

	Read Command code & behavior selection
	0H:03H
	4H:0BH
	8H:1BH		1x
	
	2H:3BH
	3H:BBH		2x

	7H:6BH
	AH:EBH		4x

	6H:0BH		4x w25q128 38
	BH:EBH		MXIC
	EH:EBH		Windbond
*/
	unsigned char temp;
	LCD_CmdWrite(0xB7);
	temp = LCD_DataRead();
	temp &= 0xF0;

	//if(Flash_Type)//1g为0x09
	//	temp |= 0x09; 
	//else
		temp |= 0x0a; 

	LCD_DataWrite(temp);	
}

void DMA_Read_0BH_38(void)
{
/*
	REG_Page_0[B7h]:
	bit[3:0]

	Read Command code & behavior selection
	0H:03H
	4H:0BH
	8H:1BH		1x
	
	2H:3BH
	3H:BBH		2x

	7H:6BH
	AH:EBH		4x

	6H:0BH		4x w25q128 38
	BH:EBH		MXIC
	EH:EBH		Windbond
*/
	unsigned char temp;
	LCD_CmdWrite(0xB7);
	temp = LCD_DataRead();
	temp &= 0xF0;
	temp |= 0x06; 
	LCD_DataWrite(temp);	
}

void DMA_Read_EBH_38_MX(void)
{
/*
	REG_Page_0[B7h]:
	bit[3:0]

	Read Command code & behavior selection
	0H:03H
	4H:0BH
	8H:1BH		1x
	
	2H:3BH
	3H:BBH		2x

	7H:6BH
	AH:EBH		4x

	6H:0BH		4x w25q128 38
	BH:EBH		MXIC
	EH:EBH		Windbond
*/
	unsigned char temp;
	LCD_CmdWrite(0xB7);
	temp = LCD_DataRead();
	temp &= 0xF0;
	temp |= 0x0B; 
	LCD_DataWrite(temp);	
}

void DMA_Read_EBH_38_Win(void)
{
/*
	REG_Page_0[B7h]:
	bit[3:0]

	Read Command code & behavior selection
	0H:03H
	4H:0BH
	8H:1BH		1x
	
	2H:3BH
	3H:BBH		2x

	7H:6BH
	AH:EBH		4x

	6H:0BH		4x w25q128 38
	BH:EBH		MXIC
	EH:EBH		Windbond
*/
	unsigned char temp;
	LCD_CmdWrite(0xB7);
	temp = LCD_DataRead();
	temp &= 0xF0;
	temp |= 0x0E; 
	LCD_DataWrite(temp);	
}



