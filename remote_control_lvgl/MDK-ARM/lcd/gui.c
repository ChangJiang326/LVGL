#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "gui.h"
#include "ctp.h"
#include <stdio.h>

extern  unsigned long Bcolor;
extern unsigned long Fcolor;


extern unsigned long Current_Layer,Draw_Layer;
extern unsigned char Lcd_Fre;

/*******************************************************************************
* ������  : ���ñ�������
* ����    : ����Һ����������
* ����          : �趨�������� ��Χ0~100           
* ���         : None
* ����         : None
*******************************************************************************/
 void Set_Backlight(uchar bl) //���ñ������ȣ���Χ0~100
{
Set_PWM_Prescaler_1_to_256(1);
Select_PWM1_Clock_Divided_By_1();
Select_PWM1();								
Set_Timer1_Count_Buffer(6000); 
Set_Timer1_Compare_Buffer(bl*60);
Start_PWM1();  							
}

void GUI_CleanScreen(void) //�ñ���ɫ����
{
Foreground_color_16M(Bcolor);//
	Line_Start_XY(0,0);
	Line_End_XY(lcdx-1,lcdy-1);
	Start_Square_Fill();
Check_2D_Busy();
	Foreground_color_16M(Fcolor);	
}
/*******************************************************************************
* ������  : ����ǰ��ɫ�ͱ���ɫ
* ����    : ����Һ��ǰ��ɫ�ͱ���ɫ
* ����    : ����ǰ��ɫBC ����ɫFC                  
* ���    : ǰ��ɫFcolor ����ɫBcolor
* ����    : None
*******************************************************************************/
void Set_Color(unsigned long bc,unsigned long fc)//����bcΪ����ɫ��fcΪǰ��ɫ
{
Foreground_color_16M(fc);
Background_color_16M(bc);	
Bcolor=bc,Fcolor=fc;
}

/*******************************************************************************
* ������  : ��ǰ��ɫ��ʾһ����
* ����    : ��Һ��������ʾһ���� ,���귶Χ����Һ���ֱ���
* ����    : �趨�������λ��X Y                                
* ���    : None
* ����    : None
*******************************************************************************/
void GUI_Point(uint x,uint y)//��һ�㣬x��yΪ����ֵ
{
    LCD_SetCursor(x, y);
    LCD_WriteRAM_Prepare(); 

LCD_DataWrite(Fcolor&0x00ff);
LCD_DataWrite(Fcolor>>8);	


	
}
/*******************************************************************************
* ������  : ��ȡһ������ɫ����
* ����    : ��ȡҺ������һ�������ɫ���� ,���귶Χ����Һ���ֱ���
* ����          : �趨��ȡ������λ��X Y 
* ���         : None
* ����         :  ��ɫ����
*******************************************************************************/
uint GUI_ReadPoint(uint x,uint y)//��һ�㣬x��yΪ����ֵ
{	uint readcolor;
	  LCD_SetCursor(x, y);
    LCD_WriteRAM_Prepare(); 





		#ifdef FSMC_16BIT
	LCD_DataRead();
readcolor=LCD_DataRead();
	#endif		
			#ifdef P_8BIT
LCD_DataRead();LCD_DataRead();
readcolor= LCD_DataRead();	
readcolor=(readcolor<<8)+LCD_DataRead();
	#endif		
			#ifdef SPI
LCD_DataRead();LCD_DataRead();
readcolor= LCD_DataRead();	
readcolor=(readcolor<<8)+LCD_DataRead();
	#endif			


return readcolor;	
}
/*******************************************************************************
* ������  : д��һ������ɫ����
* ����    : д��Һ������һ������ɫ���� ,���귶Χ����Һ���ֱ���
* ����    : �趨д��������λ��X Y 
* ���    : None
* ����    : None
*******************************************************************************/
void GUI_WritePoint(uint x,uint y,uint color)
{	
    LCD_SetCursor(x, y);
    LCD_WriteRAM_Prepare(); 
		#ifdef FSMC_16BIT
		LCD_DataWrite(color);
	#endif		
			#ifdef P_8BIT
		LCD_DataWrite(color&0x00ff);
LCD_DataWrite(color>>8);	
	#endif		
			#ifdef SPI
		LCD_DataWrite(color&0x00ff);
    LCD_DataWrite(color>>8);	
	#endif		



}
/*******************************************************************************
* ������  : ��ʾ����Բ
* ����    : ��Һ��������ʾһ������Բ ,���귶Χ����Һ���ֱ���
* ����          : �趨����ԲԲ������ X Y 
*                 �趨����Բ�뾶R
*                 
* ���         : None
* ����         : None
*******************************************************************************/

void GUI_Circle   (uint x,uint y,uint r)	  //��ʾ����Բ
{
	Circle_Center_XY(x,y);		
		Circle_Radius_R(r);			
Start_Circle_or_Ellipse();	

}
/*******************************************************************************
* ������  : ��ʾʵ��Բ
* ����    : ��Һ��������ʾһ��ʵ��Բ ,���귶Χ����Һ���ֱ���
* ����          : �趨ʵ��ԲԲ������ X Y
*                 �趨ʵ��Բ�뾶R
*                 
* ���         : None
* ����         : None
*******************************************************************************/
void GUI_CircleFill (uint x,uint y,uint r)	  //��ʾʵ��Բ
{
Circle_Center_XY(x,y);			
Circle_Radius_R(r);					
Start_Circle_or_Ellipse_Fill();	
}
/*******************************************************************************
* ������  : ��ʾ������Բ
* ����    : ��Һ��������ʾһ��������Բ ,���귶Χ����Һ���ֱ���
* ����          : �趨������ԲԲ������ X Y 
*                 �趨������Բ�뾶R1���뾶R2
*                 
* ���         : None
* ����         : None
*******************************************************************************/
void GUI_Ellipse  (uint x,uint y,uint r1,uint r2)	  //��ʾ������Բ
{
Circle_Center_XY(x,y);			
Ellipse_Radius_RxRy(r1, r2);
Start_Circle_or_Ellipse();
}
/*******************************************************************************
* ������  : ��ʾʵ����Բ
* ����    : ��Һ��������ʾһ��ʵ����Բ
* ����          : �趨ʵ����ԲԲ������ X Y ,���귶Χ����Һ���ֱ���
*                ����ʵ����Բ�뾶R1���뾶R2
*                 
* ���         : None
* ����         : None
*******************************************************************************/
void GUI_EllipseFill  (uint x,uint y,uint r1,uint r2)	  //��ʾʵ����Բ
{
Circle_Center_XY(x,y);			
Ellipse_Radius_RxRy(r1,r2);	
Start_Circle_or_Ellipse_Fill();	
}
/*******************************************************************************
* ������  : ��ʾ����������
* ����    : ��Һ��������ʾһ��ʵ�������� ,���귶Χ����Һ���ֱ���
* ����          : ���ÿ��������ε�һ��������X1 Y1
*               �����ÿ��������εڶ���������X2 Y2
*								�����ÿ��������εڶ���������X3 Y3
*                 
* ���         : None
* ����         : None
*******************************************************************************/
void GUI_Triangle (uint x1,uint y1,uint x2,uint y2,uint x3,uint y3)	 //��ʾ����������
{
Triangle_Point1_XY(x1,y1);	
Triangle_Point2_XY(x2,y2);	
Triangle_Point3_XY(x3,y3);	
Start_Triangle();	
}
/*******************************************************************************
* ������  : ��ʾʵ��������
* ����    : ��Һ��������ʾһ��ʵ�������� ,���귶Χ����Һ���ֱ���
* ����          : ����ʵ�������ε�һ��������X1 Y1
*               ������ʵ�������εڶ���������X2 Y2
*								������ʵ�������εڶ���������X3 Y3
*                 
* ���         : None
* ����         : None
*******************************************************************************/
void GUI_Fill_Triangle (uint x1,uint y1,uint x2,uint y2,uint x3,uint y3)  //��ʾʵ��������
{
Triangle_Point1_XY(x1,y1);	
Triangle_Point2_XY(x2,y2);	
Triangle_Point3_XY(x3,y3);	
Start_Triangle_Fill();
}
/*******************************************************************************
* ������  : ��ʾ���ľ���
* ����    : ��Һ��������ʾһ�����ľ��� ,���귶Χ����Һ���ֱ���
* ����          : ���ÿ��ľ������Ͻǵ�һ��������
*									���þ��εĳ�W�Ϳ�H
*                 
* ���         : None
* ����         : None
*******************************************************************************/
 void GUI_Rectangle (uint xs,uint ys,uint w,uint h)  //��ʾ���ľ���
 {
	Line_Start_XY(xs,ys);
	Line_End_XY(xs+w-1,ys+h-1);
	Start_Square();	
}
/*******************************************************************************
* ������  : ��ʾʵ�ľ���
* ����    : ��Һ��������ʾһ��ʵ�ľ��� ,���귶Χ����Һ���ֱ���
* ����          : ����ʵ�ľ������Ͻǵ�һ��������
*									���þ��εĳ�W�Ϳ�H
*                 
* ���         : None
* ����         : None
*******************************************************************************/
 void GUI_RectangleFill (uint xs,uint ys,uint w,uint h)   //��ʾʵ�ľ���
 {
	Line_Start_XY(xs,ys);
	Line_End_XY(xs+w-1,ys+h-1);
	Start_Square_Fill();
}
/*******************************************************************************
* ������  : ��ʾ����Բ�Ǿ���
* ����    : ��Һ��������ʾһ������Բ�Ǿ��� �����귶Χ����Һ���ֱ���
* ����          : ������ʾ����Բ�Ǿ������Ͻǵ�һ��������
*									���ÿ���Բ�Ǿ��εĳ��Ϳ�
*                 ����Բ��R ,RԲ�ǲ������ڳ�W���߿�H��һ��
* ���         : None
* ����         : None
*******************************************************************************/
 void GUI_Rectangle_R (uint xs,uint ys,uint w,uint h,uint r)  //��ʾ����Բ�Ǿ��Σ�rԲ�ǲ�������w����h��һ��
 {
 		Line_Start_XY(xs,ys);
	Line_End_XY(xs+w-1,ys+h-1);
Ellipse_Radius_RxRy(r,r);
Start_Circle_Square();	
}
/*******************************************************************************
* ������  : ��ʾʵ��Բ�Ǿ���
* ����    : ��Һ��������ʾһ������Բ�Ǿ��� �����귶Χ����Һ���ֱ���
* ����          : ������ʾʵ��Բ�Ǿ������Ͻǵ�һ��������
*									����ʵ��Բ�Ǿ��εĳ��Ϳ�
*                 ����Բ��R ,RԲ�ǲ������ڳ�W���߿�H��һ��
* ���         : None
* ����         : None
*******************************************************************************/
 void GUI_RectangleFill_R  (uint xs,uint ys,uint w,uint h,uint r)  //��ʾʵ��Բ�Ǿ��Σ�rԲ�ǲ�������w����h��һ��
 {
 			Line_Start_XY(xs,ys);
Line_End_XY(xs+w-1,ys+h-1);
Ellipse_Radius_RxRy(r,r);
Start_Circle_Square_Fill();	
}
/*******************************************************************************
* ������  : ��ֱ��
* ����    : ��Һ��������ʾһ��ֱ�� �����귶Χ����Һ���ֱ���
* ����          : ����ֱ�ߵĵ�һ���������X1 Y1
*									����ֱ�ߵĵڶ����������X2 Y2
*                
* ���         : None
* ����         : None
*******************************************************************************/
void GUI_Line(uint x1,uint y1,uint x2,uint y2)//��ʾֱ��
{
	Line_Start_XY(x1,y1);
	Line_End_XY(x2,y2);
	Start_Line();
}
/*******************************************************************************
* ������  : ��ʾ128MB�ڵ�ͼƬ
* ����    : ��Һ��������ʾһ��ͼƬ �����귶Χ����Һ���ֱ���
* ����          :picnumΪ��¼�������������ͼƬ��ţ�����¼��������б���Ӧ��X,YΪ�ڸ�λ����ʾ������             
* ���         : None
* ����         : None
*******************************************************************************/

 void Dis_Image_Mix_128Mb(uint picnum,uint x,uint y)	//128Mb����������ʾͼƬ����ǰ��Ļ
 {	

ulong picx=0,picy=0,fadd=0;unsigned char buf3[12];//uchar buf3[12];
  FLASH_Read_128Mb(buf3, 0x220000+picnum*12+16, 12);

  fadd=(ulong)buf3[0];
  fadd=(fadd<<8)+buf3[1];
  fadd=(fadd<<8)+buf3[2];
  fadd=(fadd<<8)+buf3[3];
  fadd=fadd+0x220000;

  picx=(ulong)buf3[4];
  picx=(picx<<8)+buf3[5];
  picx=(picx<<8)+buf3[6];
  picx=(picx<<8)+buf3[7];

  picy=(ulong)buf3[8];
  picy=(picy<<8)+buf3[9];
  picy=(picy<<8)+buf3[10];
  picy=(picy<<8)+buf3[11];


 if((picx<1025)&(picy<1025))
 {
//Enable_SFlash_SPI(); // Enable SPI interface
Flash_Sel()
	Graphic_Mode();		 // Set as Graphic Mode

	DMA_Read_6BH(); // Set as Fast Read Quard Output

	Reset_CPOL(); // Set SPI operation mode: Mode 0
	Reset_CPHA(); // Set SPI operation mode: Mode 0

	SPI_Clock_Period(1); // Set SPI clock cycle
Canvas_Image_Start_address(Layer1);
	Memory_XY_Mode(); // Set as Block mode

	SFI_DMA_Destination_Upper_Left_Corner(x, y);

	/******************* Key code for JPG **************************/

	SFI_DMA_Source_Start_Address(fadd); // Starting address of the JPG key code2228288
	Start_JPG_DMA();
	Check_Busy_JPG_DMA();
	/******************* Key code for JPG **************************/

	SFI_DMA_Source_Start_Address(fadd+16); // Starting address of the JPG picture
	Start_JPG_DMA();
Check_Busy_JPG_DMA();

	LCD_Reg_Write(0xB6, LCD_Reg_Read(0xB6) & 0xFD); // Set back to regular DMA
	
	DMA_Read_3BH();
LCD_Reg_Read(0xB6);

  BTE_Destination_Color_24bpp();  
  BTE_Destination_Memory_Start_Address(Draw_Layer);
  BTE_Destination_Image_Width(lcdx);
  BTE_Destination_Window_Start_XY(x,y);  
  BTE_Window_Size(picx,picy);

  BTE_S0_Color_24bpp();
  BTE_S0_Memory_Start_Address(Layer1);//BTE_S0_Memory_Start_Address(Layer2);
  BTE_S0_Image_Width(lcdx);
  BTE_S0_Window_Start_XY(x,y);

  BTE_ROP_Code(12);
  BTE_Operation_Code(2); //BTE move  
  BTE_Enable(); Check_BTE_Busy();
  Canvas_Image_Start_address(Draw_Layer); 

  }

 }
  void Dis_Image_Mix_Big(uint picnum,uint x,uint y)	//128Mb����������ʾͼƬ����ǰ��Ļ
 {	

ulong picx=0,picy=0,fadd=0;uchar buf3[12];
	   FLASH_Read_128Mb(buf3, 0x220000+picnum*12, 12);
	
	   fadd=(ulong)buf3[0];
	   fadd=(fadd<<8)+buf3[1];
	   fadd=(fadd<<8)+buf3[2];
	   fadd=(fadd<<8)+buf3[3];
       fadd=fadd+0x220000;

	   picx=(ulong)buf3[4];
	   picx=(picx<<8)+buf3[5];
	   picx=(picx<<8)+buf3[6];
	   picx=(picx<<8)+buf3[7];

	   picy=(ulong)buf3[8];
	   picy=(picy<<8)+buf3[9];
	   picy=(picy<<8)+buf3[10];
	   picy=(picy<<8)+buf3[11];


BTE_Destination_Color_16bpp();  
Enable_SFlash_SPI();
Select_SFI_0();
Select_SFI_DMA_Mode();
Select_SFI_24bit_Address();
Select_SFI_Waveform_Mode_3();
Select_SFI_Dual_Mode0();
SPI_Clock_Period(0);
SFI_DMA_Destination_Start_Address(Draw_Layer);//	Layer3
Memory_XY_Mode(); 
SFI_DMA_Source_Width(800); 
SFI_DMA_Transfer_Width_Height(800,800);
SFI_DMA_Destination_Upper_Left_Corner(x,y); 
SFI_DMA_Source_Start_Address(fadd);
Start_SFI_DMA();
  Check_Busy_SFI_DMA();
 }
 /*******************************************************************************
* ������  : ��ʾ128MB�ڵ�ͼƬ���ƶ�ͼ��
* ����    : ��Һ������ָ��ͼ����ʾһ��ͼƬ�� ���귶Χ����Һ���ֱ���
* ����          :picnumΪ��¼�������������ͼƬ��ţ�����¼��������б���Ӧ��X,YΪ�ڸ�λ����ʾ������ ��layerΪָ����ͼ��
* ���         : None
* ����         : None
*******************************************************************************/
    void Dis_Png_mix_128Mb(uint picnum,uint x,uint y,uint layer)	//128Mb����������ʾͼƬ��ָ��ͼ��
 {
 /*
ulong picx=0,picy=0,fadd=0;uchar buf3[12];
FLASH_Read_128Mb(buf3, picnum*12, 12);
	
	   fadd=(ulong)buf3[0];
	   fadd=(fadd<<8)+buf3[1];
	   fadd=(fadd<<8)+buf3[2];
	   fadd=(fadd<<8)+buf3[3];


	   picx=(ulong)buf3[4];
	   picx=(picx<<8)+buf3[5];
	   picx=(picx<<8)+buf3[6];
	   picx=(picx<<8)+buf3[7];

	   picy=(ulong)buf3[8];
	   picy=(picy<<8)+buf3[9];
	   picy=(picy<<8)+buf3[10];
	   picy=(picy<<8)+buf3[11];
	  // fadd=2048;picx=480;picy=272;

	 if((picx!=0)&(picy!=0)&(picnum<200))
	 {
 	 BTE_Destination_Color_16bpp();  
				Enable_SFlash_SPI();
    Select_SFI_0();
    Select_SFI_DMA_Mode();
    Select_SFI_24bit_Address();
		Select_SFI_Waveform_Mode_3();
		    Select_SFI_Dual_Mode0();
    SPI_Clock_Period(0);
Canvas_Image_Start_address(layer);
	
	SFI_DMA_Destination_Start_Address(layer);
Memory_XY_Mode(); 
	   SFI_DMA_Source_Width(picx); 
	   SFI_DMA_Transfer_Width_Height(picx,picy);
	   SFI_DMA_Destination_Upper_Left_Corner(x,y); 
	   SFI_DMA_Source_Start_Address(fadd);

 Start_SFI_DMA();
  Check_Busy_SFI_DMA();
  
	//Canvas_Image_Start_address(Layer1);
}  */
  Enable_SFlash_SPI();									          // ʹ��SPI����
 Select_SFI_0();										// ѡ����ҵ�SPI0
 //Select_SFI_1();									  // ѡ����ҵ�SPI1
 
										   
  Select_SFI_DMA_Mode();								          // ����SPI��DMAģʽ
  SPI_Clock_Period(1);                          // ����SPI�ķ�Ƶϵ��

  Goto_Pixel_XY(0,0);									          // ��ͼ��ģʽ�������ڴ��λ��
  SFI_DMA_Destination_Upper_Left_Corner(0,0);		// DMA�����Ŀ�ĵأ��ڴ��λ�ã�
  SFI_DMA_Transfer_Width_Height(480,272);				  // ���ÿ����ݵĿ��Ⱥ͸߶�
  SFI_DMA_Source_Width(480);							        // ����Դ���ݵĿ���
  SFI_DMA_Source_Start_Address(0); 					  // ����Դ������Flash�ĵ�ַ

  Start_SFI_DMA();									              // ��ʼDMA����
  Check_Busy_SFI_DMA();	
	
 }
/*******************************************************************************
* ������  : ��ʾ1024MB�ڵ�ͼƬ
* ����    : ��Һ��������ʾһ��ͼƬ
 * ����          :picnumΪ��¼�������������ͼƬ��ţ�����¼��������б���Ӧ��X,YΪ�ڸ�λ����ʾ������
* ���         : None
* ����         : None
*******************************************************************************/
  void Dis_Image_mix_1024Mb(uint picnum,uint x,uint y)	//1024Mb����������ʾͼƬ����ǰ��Ļ
 {  uchar buf3[12]; 
ulong picx=0,picy=0,fadd=0;
FLASH_Read_1024GMb(buf3, 0x220000,(picnum)*12+16);

  
	   fadd=(ulong)buf3[0];
	   fadd=(fadd<<8)+buf3[1];
	   fadd=(fadd<<8)+buf3[2];
	   fadd=(fadd<<8)+buf3[3];


	   picx=(ulong)buf3[4];
	   picx=(picx<<8)+buf3[5];
	   picx=(picx<<8)+buf3[6];
	   picx=(picx<<8)+buf3[7];

	   picy=(ulong)buf3[8];
	   picy=(picy<<8)+buf3[9];
	   picy=(picy<<8)+buf3[10];
	   picy=(picy<<8)+buf3[11];
fadd=fadd+0x220000; 
printf("fadd=%d,picx=%d,picy=%d\r\n",fadd-0x220000,picx,picy);

FLASH_Read_1024GMb_1(buf3,0x220000+picnum*12+16,12);
	   fadd=(ulong)buf3[0];
	   fadd=(fadd<<8)+buf3[1];
	   fadd=(fadd<<8)+buf3[2];
	   fadd=(fadd<<8)+buf3[3];


	   picx=(ulong)buf3[4];
	   picx=(picx<<8)+buf3[5];
	   picx=(picx<<8)+buf3[6];
	   picx=(picx<<8)+buf3[7];

	   picy=(ulong)buf3[8];
	   picy=(picy<<8)+buf3[9];
	   picy=(picy<<8)+buf3[10];
	   picy=(picy<<8)+buf3[11];
fadd=fadd+0x220000; 
printf("fadd1=%d,picx1=%d,picy1=%d\r\n",fadd-0x220000,picx,picy);



	if((picx!=0)&(picy!=0)&(picnum<1000))
	 {
		
Enable_SFlash_SPI(); // Enable SPI interface
Flash_Sel()
	Graphic_Mode();		 // Set as Graphic Mode

	DMA_Read_6BH(); // Set as Fast Read Quard Output

	Reset_CPOL(); // Set SPI operation mode: Mode 0
	Reset_CPHA(); // Set SPI operation mode: Mode 0

	SPI_Clock_Period(1); // Set SPI clock cycle
//Canvas_Image_Start_address(Layer1);
	Memory_XY_Mode(); // Set as Block mode

	SFI_DMA_Destination_Upper_Left_Corner(x, y);

	/******************* Key code for JPG **************************/

	SFI_DMA_Source_Start_Address(fadd); // Starting address of the JPG key code2228288
	Start_JPG_DMA();
	Check_Busy_JPG_DMA();
	/******************* Key code for JPG **************************/

	SFI_DMA_Source_Start_Address(fadd+16); // Starting address of the JPG picture
	Start_JPG_DMA();
Check_Busy_JPG_DMA();

	LCD_Reg_Write(0xB6, LCD_Reg_Read(0xB6) & 0xFD); // Set back to regular DMA
	
	DMA_Read_3BH();
LCD_Reg_Read(0xB6);
}
 }


  /*******************************************************************************
* ������  : ���ݲ�ͬ�Ĵ�����������ѡ��ͬ����ʾͼƬ����
* ����    : ��Һ��������ʾһ��ͼƬ ,���귶Χ����Һ���ֱ���
**����          :picnumΪ��¼�������������ͼƬ��ţ�����¼��������б���Ӧ��X,YΪ�ڸ�λ����ʾ������ 
* ���         : None
* ����         : None
*******************************************************************************/
 void Dis_Image_Mix(uint picnum,uint x,uint y)
 {	
	#ifdef NOR_FLASH
Dis_Image_Mix_128Mb(picnum,x,y);	
	#endif	
		#ifdef NAND_FLASH
Dis_Image_mix_1024Mb(picnum,x,y);	
	#endif	

 }
 
 /*******************************************************************************
* ������  : �Թ���ɫ��ʾ1024MB�ڵ�ͼƬ
* ����    : ��Һ������ָ��ͼ����ʾһ��ͼƬ ,���귶Χ����Һ���ֱ���
* ����    ��picnumΪ��¼�������������ͼƬ��ţ�����¼��������б���Ӧ��X,YΪ�ڸ�λ����ʾ������ ��colorΪ����ɫ
* ���         : None
* ����         : None
*******************************************************************************/
    void Dis_Png_Mix(uint picnum,uint x,uint y,uint color)
 {
	
Dis_Png_mix_128Mb(picnum,x,y,color);	
 }
/*******************************************************************************
* ������  : ��ʼ�����ֹ���
* ����    : ��ʼ�����ֹ���
* ����          : NONE

* ���         : None
* ����         : None
*******************************************************************************/
void cnconfig(void)
{
			Enable_SFlash_SPI();
    Select_SFI_1();
   
    Select_SFI_24bit_Address();
		Select_SFI_Waveform_Mode_3();
		    Select_SFI_Dual_Mode1();
    SPI_Clock_Period(0);	
	 }


/*******************************************************************************
* ������  : ��������
* ����    : ��������
* ����          : 1����16����2����24����3����32����
*        				
* ���         : None
* ����         : None
*******************************************************************************/
void Set_Font(uchar font) //��������
{
switch (font)
 {
	 case 0x01:	 
       {
	   Font_Select_8x16_16x16();
	   	CGRAM_Start_address(FontMemoryAddr+0x1F000+4);
       }
       break;
	 case 0x02:	 
       {
	   Font_Select_12x24_24x24();
	   	CGRAM_Start_address(FontMemoryAddr+0x64080+4); 
       }
       break;
	 case 0x03:	 
       {
	   Font_Select_16x32_32x32();
	   	CGRAM_Start_address(FontMemoryAddr+0xFF5A0+4); 
       }
       break;

}
}
/*******************************************************************************
* ������  : �������ּ��
* ����    : �������ּ�ļ��
* ����          : �������ּ����Ҽ��X
*									�������ּ����¼��Y
*        				
* ���         : None
* ����         : None
*******************************************************************************/
void Set_Font_Distance(uint x,uint y)//�������ּ��
{
Font_Line_Distance(x);	
Set_Font_to_Font_Width(y);	

}
/*******************************************************************************
* ������  : ָ����Ļλ�÷�ɫ��ʾ
* ����    : ��Һ��ָ��λ�÷�ɫ��ʾ
* ����          : �������Ͻ�����X1 Y1
*									�������½�����X2 Y2
*        				
* ���         : None
* ����         : None
*******************************************************************************/
 void inverse(uint xs,uint ys,uint xe,uint ye)//��Ļ������	
 {
   Memory_XY_Mode();
  	 BTE_Destination_Color_16bpp();  
     BTE_Destination_Memory_Start_Address(Draw_Layer);
     BTE_Destination_Image_Width(lcdx);
     BTE_Destination_Window_Start_XY(xs,ys);  
     BTE_Window_Size(xe-xs,ye-ys);

     BTE_S0_Color_16bpp();
     BTE_S0_Memory_Start_Address(Draw_Layer);
     BTE_S0_Image_Width(lcdx);
     BTE_S0_Window_Start_XY(xs,ys);

	BTE_ROP_Code(3);
    BTE_Operation_Code(2); //BTE move  
	BTE_Enable(); Check_BTE_Busy();
 }

void Nandflash_Config(void)//���1024Mb ͼƬ��������ã�128Mb�Ĳ���Ҫ����
 {
  unsigned char temp;
  

   Select_nSS_drive_on_xnsfcs0();

 
  nSS_Active();
  SPI_Master_FIFO_Data_Put(0xff);//spi nand reset
  nSS_Inactive();
  
  
  //configuration register/status register-2 
  //read status register-2
  nSS_Active();
  SPI_Master_FIFO_Data_Put(0x0F);//
  SPI_Master_FIFO_Data_Put(0xB0);//	
  temp = SPI_Master_FIFO_Data_Put(0xff);
  nSS_Inactive(); 
  temp&=0xf7; //clear bit 3 to set BUF = 0
  temp|=0x10;//set bit 4 to ECC-Enable
  //To write the status register bits,a standard write enable(06h) 
  //instruction must have previously been executed 
  nSS_Active();
  SPI_Master_FIFO_Data_Put(temp);//
  nSS_Inactive();

  nSS_Active();
  
  SPI_Master_FIFO_Data_Put(0x1F);//
  SPI_Master_FIFO_Data_Put(0xB0);//
  SPI_Master_FIFO_Data_Put(temp);//
  nSS_Inactive();
  delay_ms(10);
  nSS_Active();
  SPI_Master_FIFO_Data_Put(0x0F);//
  SPI_Master_FIFO_Data_Put(0xB0);//
  temp = SPI_Master_FIFO_Data_Put(0xff);
  nSS_Inactive();

 Select_nSS_drive_on_xnsfcs0();	


 }

/*
��ʾ�Զ���ͼƬ
x:X�����ַ
x:Y�����ַ
w:��ʾͼƬ�Ŀ���
h:��ʾͼƬ�ĸ߶�
numbers:��ʾͼƬ���ֽ���
*datap��ͼƬ�����ָ��
ע�⣺�Ƽ��á�image2lcd����������ͼƬ���飬�����ʽ�ǵ�λ��ǰ��RGB565��ʽ
*/
void Draw_Picture(uint x,uint y,uint w,uint h,unsigned long numbers,const unsigned char *datap)
{		unsigned short i, j,temp;
	Graphic_Mode();
//Sel_Layer(Current_Layer,Draw_Layer);//clayer:��ǰͼ��  dlayer��д��ͼ��
	Active_Window_XY(x, y);
	Active_Window_WH(w, h);
	Goto_Pixel_XY(x, y);
	LCD_CmdWrite(0x04);


  for(i=0;i<(numbers);)
   {
 

		
		#ifdef FSMC_16BIT

		  temp=(uint)datap[i];
   temp=(uint)(temp<<8)+datap[i+1];
		 LCD_DataWrite(temp);
	i=i+2;
	#endif		
			#ifdef P_8BIT
 LCD_DataWrite(datap[i]);
	LCD_DataWrite(datap[i+1]);
LCD_DataWrite(datap[i+2]);
		 i=i+3;
	#endif		
			#ifdef SPI





 LCD_DataWrite(datap[i]);
	LCD_DataWrite(datap[i+1]);
	LCD_DataWrite(datap[i+2]);
		 i=i+3;
	#endif		


// Check_Mem_WR_FIFO_not_Full(); //���MCU�ٶȱȽϿ죬Ҫ�����������������һ��72M��STM32��������ʲô�ӿڣ�������Ҫ����Ρ� 
   
   }
   Active_Window_WH(lcdx,lcdy);Active_Window_XY(0,0);
}

#ifdef SPI
void Draw_Picture_SPI_Fast1(uint x,uint y,uint w,uint h,unsigned long numbers,const unsigned char *datap)
{		unsigned short i, j;
	Graphic_Mode();
//Sel_Layer(Current_Layer,Draw_Layer);//clayer:��ǰͼ��  dlayer��д��ͼ��
	Active_Window_XY(x, y);
	Active_Window_WH(w, h);
	Goto_Pixel_XY(x, y);
	LCD_CmdWrite(0x04);
SPI_CS=0;
SPI_ShiftByte(SPI_WRITE_DATA);
  for(i=0;i<(numbers);i++)
   {
 
SPI_ShiftByte(datap[i]);

	
 
   }SPI_CS=1;
   Active_Window_WH(lcdx,lcdy);Active_Window_XY(0,0);
}
void Draw_Picture_SPI_Fast(uint x,uint y,uint w,uint h,unsigned long numbers,const unsigned char *datap)
{		unsigned short i, j;
	Graphic_Mode();
Sel_Layer(Current_Layer,Layer1);//clayer:��ǰͼ��  dlayer��д��ͼ��
Memory_8bpp_Mode();
	Active_Window_XY(x, y);
	Active_Window_WH(w, h);
	Goto_Pixel_XY(x, y);
	LCD_CmdWrite(0x04);
SPI_CS=0;
SPI_ShiftByte(SPI_WRITE_DATA);
  for(i=0;i<(numbers);i++)
   {
 
SPI_ShiftByte(datap[i]);

	
 
   }SPI_CS=1;
//Layer_Copy(Layer1,Current_Layer,x,y,x,y,w,h);


 	 BTE_Destination_Color_24bpp();  
     BTE_Destination_Memory_Start_Address(Current_Layer);
     BTE_Destination_Image_Width(lcdx);
     BTE_Destination_Window_Start_XY(x,y);  
     BTE_Window_Size(w,h);

     BTE_S0_Color_8bpp();
     BTE_S0_Memory_Start_Address(Layer1);
     BTE_S0_Image_Width(lcdx);
     BTE_S0_Window_Start_XY(x,y);

	BTE_ROP_Code(12);
    BTE_Operation_Code(2); //BTE move  
	BTE_Enable(); Check_BTE_Busy();
Memory_24bpp_Mode();
Sel_Layer(Current_Layer,Current_Layer);

   Active_Window_WH(lcdx,lcdy);Active_Window_XY(0,0);
}
#endif
void Disp_FloatNum(u8 mode,u32 x,u32 y,u8 fontsize,u8 w,double value,uchar *format,uint B_color,uint F_color ) //��ʾ���������ͻ����Ը����Լ��������޸ĳ���
{

	 uchar temp[10];
	  uint tempb,tempf;
	 tempf=Read_Foreground_color_65k();
	tempb= Read_Background_color_65k();

	Background_color_65k(B_color);
	if(mode==0)
	Font_Background_select_Color();
	if(mode==1)
	Font_Background_select_Transparency();
	
	if(fontsize==16)
 Font_Select_8x16_16x16();
	if(fontsize==24)
 Font_Select_12x24_24x24();
	if(fontsize==32)
 Font_Select_16x32_32x32();
 	

sprintf((char*)temp,format,value);
Foreground_color_65k(B_color);
 GUI_RectangleFill(x,y,w,fontsize); 
Foreground_color_65k(F_color);

Display_Text(x,y,mode ,temp);
	
 Foreground_color_65k(tempf);
 Foreground_color_65k(tempb);




} 

 /*******************************************************************************
* ������  : ��ʾ128MB��������ͼƬ
* ����    : �Թ���ɫ�ķ�ʽ��Һ��������ʾһ��ͼƬ �����귶Χ����Һ���ֱ��ʣ��Ĺ��ܿ���������ʾͼ�꣬������ͼƬ����ʾ�Ƿ��ε�ͼƬ
* ����          :picnumΪ��¼�������������ͼƬ��ţ�����¼��������б���Ӧ��X,YΪ�ڸ�λ����ʾ������ ��Filter_ColorΪ����ɫ            
* ���         : None
* ����         : None
*******************************************************************************/
void Dis_PicButton_128Mb(uint picnum,uint x,uint y,uint Filter_Color)	
 {
ulong picx=0,picy=0,fadd=0;uchar buf3[12];
uint temp_color=0x0000;
temp_color=Read_Background_color_65k();//����Ŀǰ����ɫ
FLASH_Read_128Mb(buf3, picnum*12, 12);
	
	   fadd=(ulong)buf3[0];
	   fadd=(fadd<<8)+buf3[1];
	   fadd=(fadd<<8)+buf3[2];
	   fadd=(fadd<<8)+buf3[3];


	   picx=(ulong)buf3[4];
	   picx=(picx<<8)+buf3[5];
	   picx=(picx<<8)+buf3[6];
	   picx=(picx<<8)+buf3[7];

	   picy=(ulong)buf3[8];
	   picy=(picy<<8)+buf3[9];
	   picy=(picy<<8)+buf3[10];
	   picy=(picy<<8)+buf3[11];

	 if((picx!=0)&(picy!=0)&(picnum<170))
	 {
 	 BTE_Destination_Color_16bpp();  
				Enable_SFlash_SPI();
    Select_SFI_0();
    Select_SFI_DMA_Mode();
    Select_SFI_24bit_Address();
		Select_SFI_Waveform_Mode_3();
		    Select_SFI_Dual_Mode0();
    SPI_Clock_Period(0);
Canvas_Image_Start_address(Layer2);
	
	SFI_DMA_Destination_Start_Address(Layer2);
Memory_XY_Mode(); 
	   SFI_DMA_Source_Width(picx); 
	   SFI_DMA_Transfer_Width_Height(picx,picy);
	   SFI_DMA_Destination_Upper_Left_Corner(x,y); 
	   SFI_DMA_Source_Start_Address(fadd);

 Start_SFI_DMA();
  Check_Busy_SFI_DMA();
 	 BTE_Destination_Color_16bpp();  
     BTE_Destination_Memory_Start_Address(Layer1);
     BTE_Destination_Image_Width(lcdx);
     BTE_Destination_Window_Start_XY(x,y);  
     BTE_Window_Size(picx,picy);

     BTE_S0_Color_16bpp();
     BTE_S0_Memory_Start_Address(Layer2);
     BTE_S0_Image_Width(lcdx);
     BTE_S0_Window_Start_XY(x,y);

	 BTE_S1_Color_16bpp();
     BTE_S1_Memory_Start_Address(Layer1);
     BTE_S1_Image_Width(lcdx);
     BTE_S1_Window_Start_XY(x,y);

    Background_color_65k(Filter_Color);
	BTE_ROP_Code(14);
    BTE_Operation_Code(5); //BTE move  
	BTE_Enable(); Check_BTE_Busy();
	Canvas_Image_Start_address(Layer1);
    Background_color_65k(temp_color);//��֮ǰ����ı���ɫ����д������ɫ�Ĵ���
}
	
 }
 //ѡ��ͬ��ͼ����в���
void Sel_Layer(unsigned long clayer,unsigned long dlayer)//clayer:��ǰͼ��  dlayer��д��ͼ��
{
Canvas_Image_Start_address(dlayer);
Draw_Layer=dlayer;
Main_Image_Start_Address(clayer);
Current_Layer=clayer;
}
  /*******************************************************************************
* ������  : ����ͼ��
* ����    : ����ͼ��
* ����          :slayerΪԴͼ�㣬dlayerΪĿ��ͼ�㣬sx,syΪԴͼ������Ͻ����꣬dx,dyΪĿ��ͼ������Ͻ����꣬w,hΪҪ��������Ŀ��͸߶�           
* ���         : None
* ����         : None
*******************************************************************************/
  void Layer_Copy(unsigned long slayer,unsigned long dlayer,uint sx,uint sy,uint dx,uint dy,uint w,uint h)
 {


 	 BTE_Destination_Color_16bpp();  
     BTE_Destination_Memory_Start_Address(dlayer);
     BTE_Destination_Image_Width(lcdx);
     BTE_Destination_Window_Start_XY(dx,dy);  
     BTE_Window_Size(w,h);

     BTE_S0_Color_16bpp();
     BTE_S0_Memory_Start_Address(slayer);
     BTE_S0_Image_Width(lcdx);
     BTE_S0_Window_Start_XY(sx,sy);

	BTE_ROP_Code(12);
    BTE_Operation_Code(2); //BTE move  
	BTE_Enable(); Check_BTE_Busy();
	Canvas_Image_Start_address(Layer1);

	
 }
   /*******************************************************************************
* ������  : ���л�����1
* ����    : ��ʾ���л�����1
* ����          :sx,syΪԴͼ������Ͻ����꣬dx,dyΪĿ��ͼ������Ͻ����꣬w,hΪҪ���л��Ŀ��͸߶�           
* ���         : None
* ����         : None
*******************************************************************************/
void PIP1(unsigned long layer,uint dx,uint dy,uint sx,uint sy,uint w,uint h)
{ 
Select_PIP1_Parameter();
PIP_Image_Start_Address(layer);
PIP_Image_Width(lcdx);
PIP_Window_Image_Start_XY(sx,sy);
PIP_Window_Width_Height(w,h);
PIP_Display_Start_XY(dx,dy);
Enable_PIP1();
}
   /*******************************************************************************
* ������  : ���л�����2
* ����    : ��ʾ���л�����2
* ����          :sx,syΪԴͼ������Ͻ����꣬dx,dyΪĿ��ͼ������Ͻ����꣬w,hΪҪ���л��Ŀ��͸߶�           
* ���         : None
* ����         : None
*******************************************************************************/
void PIP2(unsigned long layer,uint dx,uint dy,uint sx,uint sy,uint w,uint h)
{ 
Select_PIP2_Parameter();
PIP_Image_Start_Address(layer);
PIP_Image_Width(lcdx);
PIP_Window_Image_Start_XY(sx,sy);
PIP_Window_Width_Height(w,h);
PIP_Display_Start_XY(dx,dy);
Enable_PIP2();
}


void Display_Text(uint x,uint y,uchar tongtou ,uchar *c)
{
 	unsigned short temp_H = 0;
	unsigned short temp_L = 0;
	unsigned short temp = 0;
	unsigned long i = 0;
	Memory_XY_Mode();
	Text_Mode();
	Font_Select_UserDefine_Mode();
  Foreground_color_16M(Fcolor);
	Background_color_16M(Bcolor);

		if(tongtou==0)
	Font_Background_select_Color();
	if(tongtou==1)
	Font_Background_select_Transparency();
	Goto_Text_XY(x,y);
	
	while(c[i] != '\0')
  { 
		if(c[i] < 0xa1)
		{
			CGROM_Select_Internal_CGROM();   // �ڲ�CGROMΪ�ַ���Դ
			LCD_CmdWrite(0x04);
			LCD_DataWrite(c[i]);
			Check_Mem_WR_FIFO_not_Full();  
			i += 1;
		}
		else
		{
			Font_Select_UserDefine_Mode();   // �Զ����ֿ�
			LCD_CmdWrite(0x04);
			temp_H = ((c[i] - 0xa1) & 0x00ff) * 94;
			temp_L = c[i+1] - 0xa1;
			temp = (temp_H + temp_L +0x8000);
			LCD_DataWrite((temp>>8)&0xff);
			Check_Mem_WR_FIFO_not_Full();
			LCD_DataWrite(temp&0xff);
			Check_Mem_WR_FIFO_not_Full();
			i += 2;		
		}
	}
	
  Check_2D_Busy();

  Graphic_Mode(); //back to graphic mode;ͼ��ģʽ


}
void Set_Text_Cursor(uchar enable, uint x,uint y,uchar w,uchar h,uchar freq)
{
 		if(enable==1)	 
			{
				 Enable_Text_Cursor();
			Enable_Text_Cursor_Blinking();
     Blinking_Time_Frames(freq);
			Enable_Text_Cursor_Blinking();				
             Goto_Text_XY(x,y);
				
			Text_Cursor_H_V(w,h);	
			}
				else
				{
Disable_Text_Cursor();
Disable_Text_Cursor_Blinking();
				}	
}
void Graphic_Cursor_Init
(
 unsigned char Cursor_N                  // ѡ����   1:���1   2:���2   3:���3  4:���4
,unsigned char Color1                    // ��ɫ1
,unsigned char Color2                    // ��ɫ2
,unsigned short X_Pos                    // ��ʾ����X
,unsigned short Y_Pos                    // ��ʾ����Y
,unsigned char *Cursor_Buf               // ������ݵĻ����׵�ַ
)
{
	unsigned int i ;
	
	Memory_Select_Graphic_Cursor_RAM(); 
	Graphic_Mode();
	
	switch(Cursor_N)
	{
		case 1:	Select_Graphic_Cursor_1();	break;
		case 2:	Select_Graphic_Cursor_2();	break;
		case 3:	Select_Graphic_Cursor_3();	break;
		case 4:	Select_Graphic_Cursor_4();	break;
		default:														break;
	}
	
	LCD_CmdWrite(0x04);
  for(i=0;i<256;i++)
  {					 
		LCD_DataWrite(Cursor_Buf[i]);
  }
	
	Memory_Select_SDRAM();//д����л�SDRAM
	Set_Graphic_Cursor_Color_1(Color1);
  Set_Graphic_Cursor_Color_2(Color2);
  Graphic_Cursor_XY(X_Pos,Y_Pos);
	
	Enable_Graphic_Cursor();

}
void Set_Graphic_Cursor_Pos(
 unsigned char Cursor_N                  // ѡ����   1:���1   2:���2   3:���3  4:���4
,unsigned short X_Pos                    // ��ʾ����X
,unsigned short Y_Pos                    // ��ʾ����Y
)
{
	Graphic_Cursor_XY(X_Pos,Y_Pos);
	switch(Cursor_N)
	{
		case 1:	Select_Graphic_Cursor_1();	break;
		case 2:	Select_Graphic_Cursor_2();	break;
		case 3:	Select_Graphic_Cursor_3();	break;
		case 4:	Select_Graphic_Cursor_4();	break;
		default:														break;
	}
}

void LCDTEST(void)//
{
unsigned long i,j=0;
Bcolor=color_blue; //�Զ���ǰ��ɫ�ͱ���ɫ����
Fcolor=color_red;//�Զ���ǰ��ɫ�ͱ���ɫ����

	
	



Display_ON(); //����ʾ
//Color_Bar_ON();
Foreground_color_16M(Fcolor);//����ǰ��ɫ
Background_color_16M(Bcolor); //���ñ���ɫ




	#ifdef NOR_FLASH
Init_Font();
	#endif	
		#ifdef NAND_FLASH
Init_Font_1024Mb();//��ʼ���ֿ�

	#endif	
GUI_CleanScreen();//�ñ���ɫ����  
 Set_Backlight(100);
	Set_Font(3);


   	for(i=0;i<32;i++)
	{
		Set_Color(color_yellow,(j<<16));
		GUI_RectangleFill (i*(lcdx/32),0,lcdx/32,lcdy/3);	

		Set_Color(color_yellow,j<<8);	
		GUI_RectangleFill (i*(lcdx/32),lcdy/3,lcdx/32,lcdy/3);
		Set_Color(color_yellow,j);	
		GUI_RectangleFill (i*(lcdx/32),lcdy/3*2,lcdx/32,lcdy-lcdy/3*2); 
j=j+8;
	}

	delay_ms(1000);		



Dis_Image_Mix(0,0,0);
delay_ms(1000);		
//Draw_Picture_SPI_Fast(0,0,100,100,10000,gImage_100X100_RGB332);//��SPI�ӿڵķ�ʽ�£���ʾrgb232��ʽ��ͼƬ�����ķ�ʽ,




	Set_Font(3);
Display_Text(200,  0,1,"TFTҺ����");//��ʾ�ַ�



  



}
extern uchar Touch_State;
extern volatile u16 X0,Y0;
void Read_Coordinate(u16 *x,u16 *y,u8 *state)
{
*x=	X0;
*y=	Y0;
*state=	Touch_State;
}

/* EXTI_Init: touch interrupt EXTI config - stub, GPIO already init in CTP_Init */
void EXTI_Init(void)
{
}

void Touch_Test(void)
{
	u16 x,y;u8 state;
	EXTI_Init();
		Set_Color(color_white,color_black);
	GUI_CleanScreen();
	Set_Font(3);
	 Display_Text(0,0,0,"���º�ɫԲ�㣬�ɿ���ɫԲ��");

  	while(1)//
	{

Read_Coordinate(&x,&y,&state);
		
		switch(state)
		{
	
			case TOUCH_UP:
				delay_ms(300);//Ϊ����ʾ�ɿ�״̬������ʱ
			HAL_NVIC_DisableIRQ(EXTI1_IRQn);
				Set_Color(color_white,color_red);
	GUI_CircleFill (x,y,7);
	Display_Text(  432,  0,0,"״̬���ͷ�");
	Touch_State=TOUCH_NC;		
	HAL_NVIC_EnableIRQ(EXTI1_IRQn);   		
			break;
			case  TOUCH_DOWN:
				HAL_NVIC_DisableIRQ(EXTI1_IRQn);
		Set_Color(color_white,color_black);
			 GUI_CircleFill (x,y,3);
			Display_Text(  432,  0,0,"״̬������");
			HAL_NVIC_EnableIRQ(EXTI1_IRQn);   
			break;
			case  TOUCH_NC:
			
			break;
		}
	
/*
	if(PCin(1)==0)	
	{  
	
	// Readtouch_RTP();//�����������������������

	Readtouch_CTP(); //�����������������������
	 }
	*/
	}
	
}
