#include "sys.h"
//1£ºÒº¾§½Ó¿ÚÅäÖÃ
//#define SPI//SPI½Ó¿Ú
//#define P_8BIT//8Î»²¢¿Ú£¬ÓÃIOÄ£Äâ²Ù×÷
#define FSMC_16BIT//16Î»×ÜÏß½Ó¿Ú£¬ÅäÖÃ³ÉFSMC


//2£ºÒº¾§´¢´æÆ÷ÅäÖÃ,¾ßÌåÑ¡ÔñÇë×ÉÑ¯ÒµÎñÔ±Ëù¹ºÂòÊ±µÄÅäÖÃ£¬»òÕß²é¿´Òº¾§PCB±³ÃæµÄ´æ´¢Æ÷Ð¾Æ¬¡£
#define NOR_FLASH//´æ´¢Ð¾Æ¬ÐÍºÅ£ºw25q64»òÕßw25q128
//#define  NAND_FLASH//´æ´¢Ð¾Æ¬ÐÍºÅ£ºw25n01


//3£ºÅäÖÃ´¥ÃþÆÁ

//#define RTP//µç×èÆÁ
#define CTP_GT//GTÐ¾Æ¬µÄµçÈÝÆÁ
//#define CTP_FT//FTÐ¾Æ¬µÄµçÈÝÆÁ
//#define Touch_NC//²»´ø´¥ÃþÆÁ

//#define LCD_320240
//#define LCD43inch_480272
//#define LCD50inch_480272
//#define  LCD_640480
//#define  LCD50inch_800480
//#define  LCD70inch_800480
#define  LCD50inch_1024600
//#define  LCD70inch_1024600
//#define  LCD102inch_1024600

/*
	LCD_Horizontal_Non_Display(LCD_HBPD);
	LCD_HSYNC_Start_Position(LCD_HFPD);
	LCD_HSYNC_Pulse_Width(LCD_HSPW);
	LCD_Vertical_Non_Display(LCD_VBPD);
	LCD_VSYNC_Start_Position(LCD_VFPD);
	LCD_VSYNC_Pulse_Width(LCD_VSPW);
*/
#ifdef LCD_320240
#define lcdx    320
#define lcdy    240
#define LCD_VBPD		 		 4
#define LCD_VFPD	 	 		 15
#define LCD_VSPW		     3
#define LCD_HBPD		     38
#define LCD_HFPD		     20
#define LCD_HSPW	   	   20
#define OSC_FREQ         11	
#endif
#ifdef LCD43inch_480272
#define lcdx    480
#define lcdy    272
#define LCD_VBPD		 		 23
#define LCD_VFPD	 	 		 22
#define LCD_VSPW		     10
#define LCD_HBPD		     46
#define LCD_HFPD		     210
#define LCD_HSPW	   	   20
#define OSC_FREQ         11	
#endif
#ifdef LCD50inch_800480
#define lcdx    800
#define lcdy    480
#define LCD_VBPD		 23
#define LCD_VFPD	 	 22	
#define LCD_VSPW		 3
#define LCD_HBPD		 46
#define LCD_HFPD		 210
#define LCD_HSPW	   	 20
#endif
#ifdef LCD70inch_800480
#define lcdx    800
#define lcdy    480
#define LCD_VBPD		 23
#define LCD_VFPD	 	 22	
#define LCD_VSPW		 3
#define LCD_HBPD		 46
#define LCD_HFPD		 210
#define LCD_HSPW	   	 20
#endif
#ifdef LCD50inch_1024600
#define lcdx    1024
#define lcdy    600
#define LCD_VBPD		 20
#define LCD_VFPD	 	 12	
#define LCD_VSPW		 3
#define LCD_HBPD		 140
#define LCD_HFPD		 160
#define LCD_HSPW	   	 20
#endif

#ifdef LCD_640480
#define lcdx    640
#define lcdy    480
#define LCD_VBPD		 11
#define LCD_VFPD	 	 32
#define LCD_VSPW		 2
#define LCD_HBPD		 134
#define LCD_HFPD		 16
#define LCD_HSPW	   	 10
#endif


#define Flash_Sel()   Enable_SFlash_SPI();Select_SFI_0();Select_nSS_drive_on_xnsfcs0();
//#define Flash_Sel()   Enable_SFlash_SPI();Select_SFI_1();Select_nSS_drive_on_xnsfcs1();	



#define LcdReset   PEout(1) 
//SPIÅäÖÃ
#define SPI_CS	PBout(12)  
//8Î»²¢¿ÚioÅäÖÃ
#define RDATA      GPIOB->IDR//GPIOB¶ÁÊý¾Ý
#define DATA_BUS       GPIOB->ODR//¶¨ÒåGPIOBµÄ¶Ë¿ÚÉèÖÃ¼Ä´æÆ÷£¨¶Ë¿ÚÊä³öÊý¾Ý¼Ä´æÆ÷(GPIOB_ODR)£©
#define rd1   GPIOA->BSRR = GPIO_PIN_1
#define rd0   GPIOA->BRR = GPIO_PIN_1
#define wr1   GPIOA->BSRR = GPIO_PIN_2
#define wr0   GPIOA->BRR = GPIO_PIN_2
#define cs1   GPIOA->BSRR = GPIO_PIN_3
#define cs0   GPIOA->BRR = GPIO_PIN_3
#define rs1   GPIOA->BSRR = GPIO_PIN_8
#define rs0   GPIOA->BRR = GPIO_PIN_8
#define rst1   GPIOC->BSRR = GPIO_PIN_14
#define rst0   GPIOC->BRR = GPIO_PIN_14
//FSMCÅäÖÃ
 	#define LCD_BASE0        ((u32)0x60000000)
	#define LCD_BASE1        ((u32)0x60000002)
	#define LCD_CmdWrite_FSMC(cmd)	  *(vu16*) (LCD_BASE0)= (cmd);
	#define LCD_DataWrite_FSMC(data)   *(vu16*) (LCD_BASE1)= (data);
	#define	LCD_StatusRead_FSMC()	 *(vu16*) (LCD_BASE0) 
	#define	LCD_DataRead_FSMC()   	 *(vu16*) (LCD_BASE1) 










 #define  FontSize	2179072
#define  FontMemoryAddr	 0//1827840// 0x003E0D760	


//²ÎÊý
/*
#define LCD_VBPD		 18
#define LCD_VFPD	 	 2
#define LCD_VSPW		 2
#define LCD_HBPD		  68
#define LCD_HFPD		 18
#define LCD_HSPW	   	  2
  */





#define  Layer0	       2181120+0*lcdx*lcdy*3
#define  Layer1	       2181120+1*lcdx*lcdy*3
#define  Layer2	       2181120+2*lcdx*lcdy*3
#define  Layer3	       2181120+3*lcdx*lcdy*3
#define  Layer4	       2181120+4*lcdx*lcdy*3											  													  
											  


#define color_black   0x00000000
#define color_white   0x00ffffff
#define color_red     0x00ff0000
#define color_green   0x0000ff00
#define color_blue    0x000000FF
#define color_yellow  color_red|color_green
#define color_cyan    color_green|color_blue
#define color_purple  color_red|color_blue





#define	cSetD0		     0x01
#define	cSetD1		     0x02
#define	cSetD2		     0x04
#define	cSetD3		     0x08
#define	cSetD4		     0x10
#define	cSetD5		     0x20
#define	cSetD6		     0x40
#define	cSetD7		     0x80

#define	cClrD0		     0xfe
#define	cClrD1		     0xfd
#define	cClrD2		     0xfb
#define	cClrD3		     0xf7
#define	cClrD4		     0xef
#define	cClrD5		     0xdf
#define	cClrD6		     0xbf
#define	cClrD7		     0x7f

#define cClrD1D0         0xfc
#define cClrD1SetD0	     0x01
#define cSetD1ClrD0	     0x02
#define cSetD1D0		 0x03
#define cClrD3D2         0xf3
#define cClrD3SetD2      0x04
#define cSetD3ClrD2      0x08
#define cSetD3D2		 0x0c

#define cClrD2D1D0		 0xf8
#define cClrD2D1SetD0	 0x01
#define cClrD2SetD1ClrD0 0x02
#define cClrD2SetD1D0	 0x03
#define cSetD2ClrD1D0	 0x04
#define cSetD2ClrD1SetD0 0x05

#define cClrD6D5D4	     0x8f
#define cClrD6D5SetD4    0x10
#define cClrD6SetD5ClrD4 0x20
#define cClrD6SetD5D4	 0x30
#define cSetD6ClrD5D4	 0x40
#define cSetD6ClrD5SetD4 0x50
#define cSetD6D5ClrD4	 0x60
#define cSetD6D5D4		 0x70
#define LCDXSIZE lcdx
#define LCDYSIZE lcdy
#define LCD_DMA_CTRL  0xB6
#define LCD_DMA_START  1
#define LCD_SFL_CTRL  0xB7
#define LCD_SERIAL_FLASH_SELECT0    0
#define LCD_SERIAL_FLASH_SELECT1    1
#define LCD_SERIAL_FLASH_FONT_MODE  0
#define LCD_SERIAL_FLASH_DMA_MODE   1
#define LCD_SERIAL_FLASH_ADDR_24BIT  0
#define LCD_SERIAL_FLASH_ADDR_32BIT  1
#define LCD_STANDARD_SPI_MODE  0
#define LCD_FOLLOW_LCD_MODE  1
#define LCD_SPI_NORMAL_READ   0   //COMMAND 03h
#define LCD_SPI_FAST_READ_8DUMMY   4   //COMMAND 0Bh
#define LCD_SPI_FAST_READ_16DUMMY  8   //COMMAND 1Bh
#define LCD_SPI_DUAL_READ_8DUMMY   2   //COMMAND 3Bh
#define LCD_2XIO_READ_MODE_4DUMMY  3   //COMMAND BBh



	#define SPI_WRITE_DATA	0x80
	#define SPI_READ_DATA	0xC0
	#define SPI_WRITE_CMD	0x00
	#define SPI_READ_STATUS	0x40

extern const unsigned char gImage_100X100_RGB888[30000]; 
extern const unsigned char gImage_100X100_RGB565[20000]; 
extern const unsigned char gImage_100X100_RGB332[10000]; 

	void Chk_Busy(void);


void LCD_Select(uchar lcdnum);




uchar SPI_ShiftByte(uchar _ucByte);
void  Write_Dir(uchar _ucRegAddr, uchar _ucRegValue);


 void FontWrite_Position(uint _usX, uint _usY);


 void BTE_SetOperateCode(uchar _ucOperate);
 void BTE_Start(void);

  void LCD_CmdWrite(uchar _ucRegAddr);
  void LCD_DataWrite(uint _ucRegValue);
 void screeninit(void);

void Text_Foreground_Color1(uint _usColor);
void Text_Background_Color1(uint _usColor);
 void Active_Window(uint XL,uint XR ,uint YT ,uint YB);
 void Chk_DMA_Busy(void);
 void  Draw_Circle(uint X,uint Y,uint R);
 void  Draw_Ellipse(uint X,uint Y,uint R1,uint R2);
 void Draw_Line(uint XS,uint XE ,uint YS,uint YE);
 void Draw_Triangle(uint X3,uint Y3);
 void MemoryWrite_Position(uint X,uint Y);
  void DMA_Start_address_setting(ulong set_address);
  void DMA_block_mode_size_setting(uint BWR,uint BHR,uint SPWR);
void Spi_Init(void);
 void RA8875_Init_8bit(void);
 void XY_Coordinate(uint X,uint Y);
 void Serial_ROM_select0(void);//bit7
void Serial_ROM_select1(void);//bit7

void Serial_ROM_Address_set_24bit(void);//bit6
void Serial_ROM_Address_set_32bit(void);//bit6

void Select_Serial_Waveform_mode0(void);//bit5
void Select_Serial_Waveform_mode3(void);//bit5

void SERIAL_ROM_Read_Cycle_4bus(void); //bit4~3
void SERIAL_ROM_Read_Cycle_5bus(void); //bit4~3
void SERIAL_ROM_Read_Cycle_6bus(void); //bit4~3

void SERIAL_ROM_Font_mode(void); //bit2
void SERIAL_ROM_DMA_mode(void);  //bit2

void SERIAL_ROM_Signal_mode(void); //bit1~0
void SERIAL_ROM_Dual_mode0(void);  //bit1~0
void SERIAL_ROM_Dual_mode1(void);  //bit1~0

  
void SROM_CLK_DIV(uchar CLK_DIV);
void CGROM_Font(void);
void CGRAM_Font(void);
void Internal_CGROM(void);
void External_CGROM(void);
void ISO8859_1(void);
void ISO8859_2(void);
void ISO8859_3(void);
void ISO8859_4(void);
void Line_distance(uchar setx);

//REG[2Ah]~REG[2Dh]
void Font_Coordinate(uint X,uint Y);

//REG[2Eh]
void Font_size_16x16_8x16(void);
void Font_size_24x24_12x24(void);
void Font_size_32x32_16x32(void);
void Font_spacing_set(uchar setx); //bit[5:0]

//REG[2Fh]
void GT_serial_ROM_select_GT21L16T1W(void);
void GT_serial_ROM_select_GT23L16U2W(void);
void GT_serial_ROM_select_GT23L24T3Y(void);
void GT_serial_ROM_select_GT23L24M1Z(void);
void GT_serial_ROM_select_GT23L32S4W(void);
void Font_code_GB2312(void);
void Font_code_GB12345(void);
void Font_code_BIG5(void);
void Font_code_UNICODE(void);
void Font_code_ASCII(void);
void Font_code_UNIJIS(void);
void Font_code_JIS0208(void);
void Font_code_LATIN(void);
void Font_Standard(void);
void Font_Arial(void);
void Font_Roman(void);
void Font_Bold(void); //for ASCII
void Graphic_Mode(void);
void Font_with_BackgroundTransparency(void);
void Font_with_BackgroundColor(void);
void Show_String(uchar *str);
void Set_Font(uchar font);
void LCDTEST(void);
void LCDTEST70(void);
 void Chk_Busy_BTE(void);
void LCD_initial(void);
//Set PLL
void LCD_PLL_Initial(void); 
//Set SDRAM 
void LCD_SDRAM_initail(unsigned char mclk);




//**Staus**//
void Check_Mem_WR_FIFO_not_Full(void);
void Check_Mem_WR_FIFO_Empty(void);
void Check_Mem_RD_FIFO_not_Full(void);
void Check_Mem_RD_FIFO_not_Empty(void);
void Check_2D_Busy(void);
void Check_SDRAM_Ready(void);
unsigned char Power_Saving_Status(void);
void Check_Power_is_Normal(void);//¬O Normal mode ´NÂ÷¶}
void Check_Power_is_Saving(void);//¬O Saving mode ´NÂ÷¶}
void Check_NO_Interrupt(void);//¨S¦³ Interrupt ´NÂ÷¶}
void Check_Interrupt_Occur(void);//¦³ Interrupt ´NÂ÷¶}

void Check_Busy_Draw(void);
void Check_Busy_SFI_DMA(void);

//**[00h]**//
void LCD_SW_Reset(void);
//**[01h]**//
void Enable_PLL(void);
void LCD_Sleep(void);
void LCD_WakeUp(void);
void TFT_24bit(void);
void TFT_18bit(void);
void TFT_16bit(void);
void TFT_LVDS(void);
void Key_Scan_Enable(void);
void Key_Scan_Disable(void);
void LCD_I2CM_Enable(void);
void LCD_I2CM_Disable(void);
void Enable_SFlash_SPI(void);
void Disable_SFlash_SPI(void);
void Host_Bus_8bit(void);
void Host_Bus_16bit(void);
//**[02h]**//

void RGB_8b_8bpp(void);
void RGB_8b_16bpp(void);
void RGB_8b_24bpp(void);

void RGB_16b_8bpp(void);
void RGB_16b_16bpp(void);
void RGB_16b_24bpp_mode1(void);
void RGB_16b_24bpp_mode2(void);

void MemRead_Left_Right_Top_Down(void);
void MemRead_Right_Left_Top_Down(void);
void MemRead_Top_Down_Left_Right(void);
void MemRead_Down_Top_Left_Right(void);

void MemWrite_Left_Right_Top_Down(void);
void MemWrite_Right_Left_Top_Down(void);
void MemWrite_Top_Down_Left_Right(void);
void MemWrite_Down_Top_Left_Right(void);
//**[03h]**//
void Interrupt_Active_Low(void);
void Interrupt_Active_High(void);
void ExtInterrupt_Debounce(void);
void ExtInterrupt_Nodebounce(void);
void ExtInterrupt_Input_Low_Level_Trigger(void);
void ExtInterrupt_Input_High_Level_Trigger(void);
void ExtInterrupt_Input_Falling_Edge_Trigger(void);
void ExtInterrupt_Input_Rising_Edge_Trigger(void);
void LVDS_Format1(void);
void LVDS_Format2(void);
void Graphic_Mode_8876(void);
void Text_Mode_8876(void);
void Memory_Select_SDRAM(void);
void Memory_Select_Graphic_Cursor_RAM(void);
void Memory_Select_Color_Palette_RAM(void);
//**[05h]**//
//**[06h]**//
//**[07h]**//
//**[09h]**//
//**[0Ah]**//
//**[0Bh]**//
void Enable_Resume_Interrupt(void);
void Disable_Resume_Interrupt(void);
void Enable_ExtInterrupt_Input(void);
void Disable_ExtInterrupt_Input(void);
void Enable_I2CM_Interrupt(void);
void Disable_I2CM_Interrupt(void);
void Enable_Vsync_Interrupt(void);
void Disable_Vsync_Interrupt(void);
void Enable_KeyScan_Interrupt(void);
void Disable_KeyScan_Interrupt(void);
void Enable_DMA_Draw_BTE_Interrupt(void);
void Disable_DMA_Draw_BTE_Interrupt(void);
void Enable_PWM1_Interrupt(void);
void Disable_PWM1_Interrupt(void);
void Enable_PWM0_Interrupt(void);
void Disable_PWM0_Interrupt(void);
//**[0Ch]**//
unsigned char Read_Interrupt_status(void);
void Clear_Resume_Interrupt_Flag(void);
void Clear_ExtInterrupt_Input_Flag(void);
void Clear_I2CM_Interrupt_Flag(void);
void Clear_Vsync_Interrupt_Flag(void);
void Clear_KeyScan_Interrupt_Flag(void);
void Clear_DMA_Draw_BTE_Interrupt_Flag(void);
void Clear_PWM1_Interrupt_Flag(void);
void Clear_PWM0_Interrupt_Flag(void);
//**[0Dh]**//
void Mask_Resume_Interrupt_Flag(void);
void Mask_ExtInterrupt_Input_Flag(void);
void Mask_I2CM_Interrupt_Flag(void);
void Mask_Vsync_Interrupt_Flag(void);
void Mask_KeyScan_Interrupt_Flag(void);
void Mask_DMA_Draw_BTE_Interrupt_Flag(void);
void Mask_PWM1_Interrupt_Flag(void);
void Mask_PWM0_Interrupt_Flag(void);
//
void Enable_Resume_Interrupt_Flag(void);
void Enable_ExtInterrupt_Input_Flag(void);
void Enable_I2CM_Interrupt_Flag(void);
void Enable_Vsync_Interrupt_Flag(void);
void Enable_KeyScan_Interrupt_Flag(void);
void Enable_DMA_Draw_BTE_Interrupt_Flag(void);
void Enable_PWM1_Interrupt_Flag(void);
void Enable_PWM0_Interrupt_Flag(void);
//**[0Eh]**//
void Enable_GPIOF_PullUp(void);
void Enable_GPIOE_PullUp(void);
void Enable_GPIOD_PullUp(void);
void Enable_GPIOC_PullUp(void);
void Enable_XDB15_8_PullUp(void);
void Enable_XDB7_0_PullUp(void);
void Disable_GPIOF_PullUp(void);
void Disable_GPIOE_PullUp(void);
void Disable_GPIOD_PullUp(void);
void Disable_GPIOC_PullUp(void);
void Disable_XDB15_8_PullUp(void);
void Disable_XDB7_0_PullUp(void);

//**[0Fh]**//
void XPDAT18_Set_GPIO_D7(void);
void XPDAT18_Set_KOUT4(void);
void XPDAT17_Set_GPIO_D5(void);
void XPDAT17_Set_KOUT2(void);
void XPDAT16_Set_GPIO_D4(void);
void XPDAT16_Set_KOUT1(void);
void XPDAT9_Set_GPIO_D3(void);
void XPDAT9_Set_KOUT3(void);
void XPDAT8_Set_GPIO_D2(void);
void XPDAT8_Set_KIN3(void);
void XPDAT2_Set_GPIO_D6(void);
void XPDAT2_Set_KIN4(void);
void XPDAT1_Set_GPIO_D1(void);
void XPDAT1_Set_KIN2(void);
void XPDAT0_Set_GPIO_D0(void);
void XPDAT0_Set_KIN1(void);

//**[10h]**//
void Enable_PIP1(void);
void Disable_PIP1(void);
void Enable_PIP2(void);
void Disable_PIP2(void);
void Select_PIP1_Parameter(void);
void Select_PIP2_Parameter(void);
void Select_Main_Window_8bpp(void);
void Select_Main_Window_16bpp(void);
void Select_Main_Window_24bpp(void);
//**[11h]**//
void Select_PIP1_Window_8bpp(void);
void Select_PIP1_Window_16bpp(void);
void Select_PIP1_Window_24bpp(void);
void Select_PIP2_Window_8bpp(void);
void Select_PIP2_Window_16bpp(void);
void Select_PIP2_Window_24bpp(void);
//**[12h]**//
void PCLK_Rising(void);
void PCLK_Falling(void);
void Display_ON(void);
void Display_OFF(void);
void Color_Bar_ON(void);
void Color_Bar_OFF(void);
void HSCAN_L_to_R(void);
void HSCAN_R_to_L(void);
void VSCAN_T_to_B(void);
void VSCAN_B_to_T(void);
void PDATA_Set_RGB(void);
void PDATA_Set_RBG(void);
void PDATA_Set_GRB(void);
void PDATA_Set_GBR(void);
void PDATA_Set_BRG(void);
void PDATA_Set_BGR(void);
void PDATA_IDLE_STATE(void);

//**[13h]**//
void HSYNC_Low_Active(void);
void HSYNC_High_Active(void);
void VSYNC_Low_Active(void);
void VSYNC_High_Active(void);
void DE_Low_Active(void);
void DE_High_Active(void);
void Idle_DE_Low(void);
void Idle_DE_High(void);
void Idle_PCLK_Low(void);
void Idle_PCLK_High(void);
void Idle_PDAT_Low(void);
void Idle_PDAT_High(void);
void Idle_HSYNC_Low(void);
void Idle_HSYNC_High(void);
void Idle_VSYNC_Low(void);
void Idle_VSYNC_High(void);
//**[14h][15h][1Ah][1Bh]**//
void LCD_HorizontalWidth_VerticalHeight(unsigned short WX,unsigned short HY);
//**[16h][17h]**//
void LCD_Horizontal_Non_Display(unsigned short WX);
//**[18h]**//
void LCD_HSYNC_Start_Position(unsigned short WX);
//**[19h]**//
void LCD_HSYNC_Pulse_Width(unsigned short WX);
//**[1Ch][1Dh]**//
void LCD_Vertical_Non_Display(unsigned short HY);
//**[1Eh]**//
void LCD_VSYNC_Start_Position(unsigned short HY);
//**[1Fh]**//
void LCD_VSYNC_Pulse_Width(unsigned short HY);
//**[20h][21h][22h][23h]**//
void Main_Image_Start_Address(unsigned long Addr);
//**[24h][25h]**//					
void Main_Image_Width(unsigned short WX);							
//**[26h][27h][28h][29h]**//
void Main_Window_Start_XY(unsigned short WX,unsigned short HY);	
//**[2Ah][2Bh][2Ch][2Dh]**//
void PIP_Display_Start_XY(unsigned short WX,unsigned short HY);
//**[2Eh][2Fh][30h][31h]**//
void PIP_Image_Start_Address(unsigned long Addr);
//**[32h][33h]**//
void PIP_Image_Width(unsigned short WX);
//**[34h][35h][36h][37h]**//
void PIP_Window_Image_Start_XY(unsigned short WX,unsigned short HY);
//**[38h][39h][3Ah][3Bh]**//
void PIP_Window_Width_Height(unsigned short WX,unsigned short HY);
//**[3C]**//
void Enable_Graphic_Cursor(void);
void Disable_Graphic_Cursor(void);
void Select_Graphic_Cursor_1(void);
void Select_Graphic_Cursor_2(void);
void Select_Graphic_Cursor_3(void);
void Select_Graphic_Cursor_4(void);
void Enable_Text_Cursor(void);
void Disable_Text_Cursor(void);
void Enable_Text_Cursor_Blinking(void);
void Disable_Text_Cursor_Blinking(void);
//**[3D]**//
void Blinking_Time_Frames(unsigned char temp);
//**[3E][3Fh]**//
void Text_Cursor_H_V(unsigned short WX,unsigned short HY);
//**[40h][41h][42h][43h]**//
void Graphic_Cursor_XY(unsigned short WX,unsigned short HY);
//**[44]**//
void Set_Graphic_Cursor_Color_1(unsigned char temp);
//**[45]**//
void Set_Graphic_Cursor_Color_2(unsigned char temp);
//**[50h][51h][52h][53h]**//
void Canvas_Image_Start_address(unsigned long Addr);
//**[54h][55h]**//
void Canvas_image_width(unsigned short WX);
//**[56h][57h][58h][59h]**//
void Active_Window_XY(unsigned short WX,unsigned short HY);
//**[5Ah][5Bh][5Ch][5Dh]**//
void Active_Window_WH(unsigned short WX,unsigned short HY);
//**[5E]**//
void Select_Write_Data_Position(void);
void Select_Read_Data_Position(void);
void Memory_XY_Mode(void);
void Memory_Linear_Mode(void);
void Memory_8bpp_Mode(void);
void Memory_16bpp_Mode(void);
void Memory_24bpp_Mode(void);
//**[5Fh][60h][61h][62h]**//
void Goto_Pixel_XY(unsigned short WX,unsigned short HY);
void Goto_Linear_Addr(unsigned long Addr);
//**[63h][64h][65h][66h]**//
void Goto_Text_XY(unsigned short WX,unsigned short HY);

////////////////////////////////////////////////////////////////////////
////**** [ Function : Draw ] ****////
//**[67h]**//
void Start_Line(void);
void Start_Triangle(void);
void Start_Triangle_Fill(void);
//**[68h]~[73h]**//
void Line_Start_XY(unsigned short WX,unsigned short HY);		//½u°_ÂI
void Line_End_XY(unsigned short WX,unsigned short HY);			//½u²×ÂI
void Triangle_Point1_XY(unsigned short WX,unsigned short HY);	//¤T¨¤-ÂI1
void Triangle_Point2_XY(unsigned short WX,unsigned short HY);	//¤T¨¤-ÂI2
void Triangle_Point3_XY (unsigned short WX,unsigned short HY);	//¤T¨¤-ÂI3
void Square_Start_XY(unsigned short WX,unsigned short HY);		//¤è°_ÂI
void Square_End_XY(unsigned short WX,unsigned short HY);		//¤è²×ÂI
//**[76h]**//
void Start_Circle_or_Ellipse(void);
void Start_Circle_or_Ellipse_Fill(void);
void Start_Left_Down_Curve(void);
void Start_Left_Up_Curve(void);
void Start_Right_Up_Curve(void);
void Start_Right_Down_Curve(void);
void Start_Left_Down_Curve_Fill(void);
void Start_Left_Up_Curve_Fill(void);
void Start_Right_Up_Curve_Fill(void);
void Start_Right_Down_Curve_Fill(void);
void Start_Square(void);
void Start_Square_Fill(void);
void Start_Circle_Square(void);
void Start_Circle_Square_Fill(void);
//**[77h]~[7Eh]**//
void Circle_Center_XY(unsigned short WX,unsigned short HY);				//¶ê¤¤¤ß
void Ellipse_Center_XY(unsigned short WX,unsigned short HY);			//¾ò¶ê¤¤¤ß
void Circle_Radius_R(unsigned short WX);								//¶ê¥b®|
void Ellipse_Radius_RxRy(unsigned short WX,unsigned short HY);			//¾ò¶ê¥b®|
void Circle_Square_Radius_RxRy(unsigned short WX,unsigned short HY);	//¤è§ÎÂà¨¤¥b®|

////////////////////////////////////////////////////////////////////////
////**** [ Function : PWM ] ****////
//**[84h]**//
void Set_PWM_Prescaler_1_to_256(unsigned short WX);
//**[85h]**//
void Select_PWM1_Clock_Divided_By_1(void);
void Select_PWM1_Clock_Divided_By_2(void);
void Select_PWM1_Clock_Divided_By_4(void);
void Select_PWM1_Clock_Divided_By_8(void);
void Select_PWM0_Clock_Divided_By_1(void);
void Select_PWM0_Clock_Divided_By_2(void);
void Select_PWM0_Clock_Divided_By_4(void);
void Select_PWM0_Clock_Divided_By_8(void);
//[85h].[bit3][bit2]
void Select_PWM1_is_ErrorFlag(void);
void Select_PWM1(void);
void Select_PWM1_is_Osc_Clock(void);
//[85h].[bit1][bit0]
void Select_PWM0_is_GPIO_C7(void);
void Select_PWM0(void);
void Select_PWM0_is_Core_Clock(void);
//**[86h]**//
//[86h]PWM1
void Enable_PWM1_Inverter(void);
void Disable_PWM1_Inverter(void);
void Auto_Reload_PWM1(void);
void One_Shot_PWM1(void);
void Start_PWM1(void);
void Stop_PWM1(void);
//[86h]PWM0
void Enable_PWM0_Dead_Zone(void);
void Disable_PWM0_Dead_Zone(void);
void Enable_PWM0_Inverter(void);
void Disable_PWM0_Inverter(void);
void Auto_Reload_PWM0(void);
void One_Shot_PWM0(void);
void Start_PWM0(void);
void Stop_PWM0(void);
//**[87h]**//
void Set_Timer0_Dead_Zone_Length(unsigned char temp);
//**[88h][89h]**//
void Set_Timer0_Compare_Buffer(unsigned short WX);
//**[8Ah][8Bh]**//
void Set_Timer0_Count_Buffer(unsigned short WX);
//**[8Ch][8Dh]**//
void Set_Timer1_Compare_Buffer(unsigned short WX);
//**[8Eh][8Fh]**//
void Set_Timer1_Count_Buffer(unsigned short WX);

////////////////////////////////////////////////////////////////////////
////**** [ Function : BTE ] ****////
//**[90h]**//

//[90h]=========================================================================
void BTE_Enable(void);
void BTE_Disable(void);

void Check_BTE_Busy(void);

void Pattern_Format_8X8(void);
void Pattern_Format_16X16(void);

//[91h]=========================================================================
void BTE_ROP_Code(unsigned char setx);
void BTE_Operation_Code(unsigned char setx);

//[92h]=========================================================================
void BTE_S0_Color_8bpp(void);
void BTE_S0_Color_16bpp(void);
void BTE_S0_Color_24bpp(void);

void BTE_S1_Color_8bpp(void);
void BTE_S1_Color_16bpp(void);
void BTE_S1_Color_24bpp(void);
void BTE_S1_Color_Constant(void);
void BTE_S1_Color_8bit_Alpha(void);
void BTE_S1_Color_16bit_Alpha(void);

void BTE_Destination_Color_8bpp(void);
void BTE_Destination_Color_16bpp(void);
void BTE_Destination_Color_24bpp(void);

//[93h][94h][95h][96h]=========================================================================
void BTE_S0_Memory_Start_Address(unsigned long Addr);	

//[97h][98h]=========================================================================
void BTE_S0_Image_Width(unsigned short WX);	

//[99h][9Ah][9Bh][9Ch]=========================================================================
void BTE_S0_Window_Start_XY(unsigned short WX,unsigned short HY);	

//[9Dh][9Eh][9Fh][A0h]=========================================================================
void BTE_S1_Memory_Start_Address(unsigned long Addr);
void S1_Constant_color_256(unsigned char temp);
void S1_Constant_color_65k(unsigned short temp);
void S1_Constant_color_16M(unsigned long temp);	

//[A1h][A2h]=========================================================================
void BTE_S1_Image_Width(unsigned short WX);	

//[A3h][A4h][A5h][A6h]=========================================================================
void BTE_S1_Window_Start_XY(unsigned short WX,unsigned short HY);	

//[A7h][A8h][A9h][AAh]=========================================================================
void BTE_Destination_Memory_Start_Address(unsigned long Addr);	

//[ABh][ACh]=========================================================================
void BTE_Destination_Image_Width(unsigned short WX);	

//[ADh][AEh][AFh][B0h]=========================================================================
void BTE_Destination_Window_Start_XY(unsigned short WX,unsigned short HY);	

//[B1h][B2h][B3h][B4h]=========================================================================
void BTE_Window_Size(unsigned short WX, unsigned short WY);

//[B5h]=========================================================================
void BTE_Alpha_Blending_Effect(unsigned char temp);


//**[B5h]**//

////////////////////////////////////////////////////////////////////////
////**** [ Function : Serial Flash ] ****////


//REG[B6h] Serial flash DMA Controller REG (DMA_CTRL) 
void Start_SFI_DMA(void);
void Check_Busy_SFI_DMA(void);

//REG[B7h] Serial Flash/ROM Controller Register (SFL_CTRL) 
void Select_SFI_0(void);
void Select_SFI_1(void);
void Select_SFI_Font_Mode(void);
void Select_SFI_DMA_Mode(void);
void Select_SFI_24bit_Address(void);
void Select_SFI_32bit_Address(void);
void Select_SFI_Waveform_Mode_0(void);
void Select_SFI_Waveform_Mode_3(void);
void Select_SFI_0_DummyRead(void);
void Select_SFI_8_DummyRead(void);
void Select_SFI_16_DummyRead(void);
void Select_SFI_24_DummyRead(void);
void Select_SFI_Single_Mode(void);
void Select_SFI_Dual_Mode0(void);
void Select_SFI_Dual_Mode1(void);

//REG[B8h] SPI master Tx /Rx FIFO Data Register (SPIDR) 
unsigned char SPI_Master_FIFO_Data_Put(unsigned char Data);
unsigned char SPI_Master_FIFO_Data_Get(void);

//REG[B9h] SPI master Control Register (SPIMCR2) 
void Mask_SPI_Master_Interrupt_Flag(void);
void Select_nSS_drive_on_xnsfcs0(void);
void Select_nSS_drive_on_xnsfcs1(void);
void nSS_Inactive(void);
void nSS_Active(void);
void OVFIRQEN_Enable(void);
void EMTIRQEN_Enable(void);
void Reset_CPOL(void);
void Set_CPOL(void);
void Reset_CPHA(void);
void Set_CPHA(void);

//REG[BAh] SPI master Status Register (SPIMSR)
unsigned char Tx_FIFO_Empty_Flag(void);
unsigned char Tx_FIFO_Full_Flag(void);
unsigned char Rx_FIFO_Empty_Flag(void);
unsigned char Rx_FIFO_full_flag(void);
unsigned char OVFI_Flag(void);
void Clear_OVFI_Flag(void);
unsigned char EMTI_Flag(void);
void Clear_EMTI_Flag(void);

//REG[BB] SPI Clock period (SPIDIV) 
void SPI_Clock_Period(unsigned char temp);


//**[BCh][BDh][BEh][BFh]**//
void SFI_DMA_Source_Start_Address(unsigned long Addr);
//**[C0h][C1h][C2h][C3h]**//
void SFI_DMA_Destination_Start_Address(unsigned long Addr);
void SFI_DMA_Destination_Upper_Left_Corner(unsigned short WX,unsigned short HY);
//**[C4h][C5h]**//
void SFI_DMA_Destination_Width(unsigned short WX);
//**[C6h][C7h][C8h][C9h]**//
void SFI_DMA_Transfer_Number(unsigned long Addr);
void SFI_DMA_Transfer_Width_Height(unsigned short WX,unsigned short HY);
//**[CAh][CBh]**//
void SFI_DMA_Source_Width(unsigned short WX);

////////////////////////////////////////////////////////////////////////
////**** [ Function : Font ] ****////
//**[CCh]**//

void Font_Select_UserDefine_Mode(void);
void CGROM_Select_Internal_CGROM(void);
void CGROM_Select_Genitop_FontROM(void);
void Font_Select_8x16_16x16(void);
void Font_Select_12x24_24x24(void);
void Font_Select_16x32_32x32(void);
void Internal_CGROM_Select_ISOIEC8859_1(void);
void Internal_CGROM_Select_ISOIEC8859_2(void);
void Internal_CGROM_Select_ISOIEC8859_3(void);
void Internal_CGROM_Select_ISOIEC8859_4(void);
//**[CDh]**//
void Enable_Font_Alignment(void);
void Disable_Font_Alignment(void);
void Font_Background_select_Transparency(void);
void Font_Background_select_Color(void);
void Font_0_degree(void);
void Font_90_degree(void);
void Font_Width_X1(void);
void Font_Width_X2(void);
void Font_Width_X3(void);
void Font_Width_X4(void);
void Font_Height_X1(void);
void Font_Height_X2(void);
void Font_Height_X3(void);
void Font_Height_X4(void);
//**[CEh]**//
void GTFont_Select_GT21L16TW_GT21H16T1W(void);
void GTFont_Select_GT23L16U2W(void);
void GTFont_Select_GT23L24T3Y_GT23H24T3Y(void);
void GTFont_Select_GT23L24M1Z(void);
void GTFont_Select_GT23L32S4W_GT23H32S4W(void);
void GTFont_Select_GT20L24F6Y(void);
void GTFont_Select_GT21L24S1W(void);
void GTFont_Select_GT22L16A1Y(void);
//**[CFh]**//
void Set_GTFont_Decoder(unsigned char temp);
//**[D0h]**//
void Font_Line_Distance(unsigned char temp);
//**[D1h]**//
void Set_Font_to_Font_Width(unsigned char temp);
//**[D2h]~[D4h]**//
void Foreground_RGB(unsigned char RED,unsigned char GREEN,unsigned char BLUE);
void Foreground_color_256(unsigned char temp);
void Foreground_color_65k(unsigned short temp);
void Foreground_color_16M(unsigned long temp);
//**[D5h]~[D7h]**//
void Background_RGB(unsigned char RED,unsigned char GREEN,unsigned char BLUE);
void Background_color_256(unsigned char temp);
void Background_color_65k(unsigned short temp);
void Background_color_16M(unsigned long temp);
//**[DBh]~[DEh]**//
void CGRAM_Start_address(unsigned long Addr);
//**[DFh]**//
void Power_Normal_Mode(void);
void Power_Saving_Standby_Mode(void);
void Power_Saving_Suspend_Mode(void);
void Power_Saving_Sleep_Mode(void);


void LCD_I2CM_Clock_Prescale(unsigned short WX);
//**[E7h]**//
void LCD_I2CM_Transmit_Data(unsigned char temp);
//**[E8h]**//
unsigned char LCD_I2CM_Receiver_Data(void);
//**[E9h]**//
 
void LCD_I2CM_Read_With_Ack(void);
void LCD_I2CM_Read_With_Nack(void);
void LCD_I2CM_Write_With_Start(void);
void LCD_I2CM_Write(void);
void LCD_I2CM_Stop(void);
 

//**[EAh]**//
unsigned char LCD_I2CM_Check_Slave_ACK(void);
unsigned char LCD_I2CM_Bus_Busy(void);
unsigned char LCD_I2CM_transmit_Progress(void);
unsigned char LCD_I2CM_Arbitration(void);



////////////////////////////////////////////////////////////////////////
////**** [ Function : GPIO ] ****////
//[F0h][F1h]
void Set_GPIO_A_In_Out(unsigned char temp);
void Write_GPIO_A_7_0(unsigned char temp);
unsigned char Read_GPIO_A_7_0(void);
//[F2h]
void Write_GPIO_B_7_4(unsigned char temp);
unsigned char Read_GPIO_B_7_0(void);
//[F3h][F4h]
void Set_GPIO_C_In_Out(unsigned char temp);
void Write_GPIO_C_7_0(unsigned char temp);
unsigned char Read_GPIO_C_7_0(void);
//[F5h][F6h]
void Set_GPIO_D_In_Out(unsigned char temp);
void Write_GPIO_D_7_0(unsigned char temp);
unsigned char Read_GPIO_D_7_0(void);
//[F7h][F8h]
void Set_GPIO_E_In_Out(unsigned char temp);
void Write_GPIO_E_7_0(unsigned char temp);
unsigned char Read_GPIO_E_7_0(void);
//[F9h][FAh]
void Set_GPIO_F_In_Out(unsigned char temp);
void Write_GPIO_F_7_0(unsigned char temp);
unsigned char Read_GPIO_F_7_0(void);


////////////////////////////////////////////////////////////////////////
////**** [ Function : Key ] ****////
//**[FBh]~[FFh]**//
//[FBh]
void Long_Key_enable(void);
void Key_Scan_Freg(unsigned char temp);	//set bit2~0 

//[FCh]
void Key_Scan_Wakeup_Function_Enable(void);
void Long_Key_Timing_Adjustment(unsigned char setx);//set bit5~3
unsigned char Numbers_of_Key_Hit(void);

//[FDh][FEh][FFh]
unsigned char Read_Key_Strobe_Data_0(void);
unsigned char Read_Key_Strobe_Data_1(void);
unsigned char Read_Key_Strobe_Data_2(void);

void Show_String(uchar *str);
void Show_picture(unsigned long numbers,const unsigned short *data);
void LCDTEST_LCD(void);
 void LCD_SetCursor(unsigned short  Xpos, unsigned short Ypos);
void LCD_WriteRAM_Prepare(void);

 void  Set_Bound(unsigned char data);
 void Font_Init(void);
  uchar LCD_StatusRead(void);
  uchar LCD_DataRead(void);
   uchar LCD_DataRead_Fast(void);
void LCD_HW_Reset(void);
  void System_Check_Temp(void);
 void FLASH_Read_128Mb(unsigned char* pBuffer, unsigned int ReadAddr, unsigned int NumByteToRead);
 void FLASH_Read_1024GMb(unsigned char *UpdataValue,unsigned int add,unsigned short ReadAddr);
	 uint Read_Background_color_65k(void);
uint Read_Foreground_color_65k(void);
 void W25N01_Set_ECC(void);
 void W25N01_Clear_BUF(void);
 void W25N01_Set_BUF(void);
 void Init_Font(void);
 void LCD_Init(void);
void Text_Mode(void);
void Init_Font_1024Mb(void);

void W25N01GV_ReadPageAddr_Data(u8 *pBuffer, u32 PageNum, u32 PageAddr, u16 NumByteToRead);

