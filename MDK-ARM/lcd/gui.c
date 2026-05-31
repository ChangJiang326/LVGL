#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "gui.h"
#include "ctp.h"

extern  unsigned long Bcolor;
extern unsigned long Fcolor;


extern unsigned long Current_Layer,Draw_Layer;
extern unsigned char Lcd_Fre;

/*******************************************************************************
* 函数名  : 设置背光亮度
* 描述    : 设置液晶背光亮度
* 输入          : 设定背光亮度 范围0~100           
* 输出         : None
* 返回         : None
*******************************************************************************/
 void Set_Backlight(uchar bl) //设置背光亮度，范围0~100
{
Set_PWM_Prescaler_1_to_256(1);
Select_PWM1_Clock_Divided_By_1();
Select_PWM1();								
Set_Timer1_Count_Buffer(6000); 
Set_Timer1_Compare_Buffer(bl*60);
Start_PWM1();  							
}

void GUI_CleanScreen(void) //用背景色清屏
{
Foreground_color_16M(Bcolor);//
	Line_Start_XY(0,0);
	Line_End_XY(lcdx-1,lcdy-1);
	Start_Square_Fill();
Check_2D_Busy();
	Foreground_color_16M(Fcolor);	
}
/*******************************************************************************
* 函数名  : 设置前景色和背景色
* 描述    : 设置液晶前景色和背景色
* 输入    : 设置前景色BC 背景色FC                  
* 输出    : 前景色Fcolor 背景色Bcolor
* 返回    : None
*******************************************************************************/
void Set_Color(unsigned long bc,unsigned long fc)//设置bc为背景色，fc为前景色
{
Foreground_color_16M(fc);
Background_color_16M(bc);	
Bcolor=bc,Fcolor=fc;
}

/*******************************************************************************
* 函数名  : 以前景色显示一个点
* 描述    : 在液晶屏上显示一个点 ,坐标范围不超液晶分辨率
* 输入    : 设定点的坐标位置X Y                                
* 输出    : None
* 返回    : None
*******************************************************************************/
void GUI_Point(uint x,uint y)//画一点，x、y为坐标值
{
    LCD_SetCursor(x, y);
    LCD_WriteRAM_Prepare(); 

LCD_DataWrite(Fcolor&0x00ff);
LCD_DataWrite(Fcolor>>8);	


	
}
/*******************************************************************************
* 函数名  : 读取一个点颜色数据
* 描述    : 读取液晶屏上一个点的颜色数据 ,坐标范围不超液晶分辨率
* 输入          : 设定读取坐标点的位置X Y 
* 输出         : None
* 返回         :  颜色数据
*******************************************************************************/
uint GUI_ReadPoint(uint x,uint y)//读一点，x、y为坐标值
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
* 函数名  : 写入一个点颜色数据
* 描述    : 写入液晶屏上一个点颜色数据 ,坐标范围不超液晶分辨率
* 输入    : 设定写入坐标点的位置X Y 
* 输出    : None
* 返回    : None
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
* 函数名  : 显示空心圆
* 描述    : 在液晶屏上显示一个空心圆 ,坐标范围不超液晶分辨率
* 输入          : 设定空心圆圆心坐标 X Y 
*                 设定空心圆半径R
*                 
* 输出         : None
* 返回         : None
*******************************************************************************/

void GUI_Circle   (uint x,uint y,uint r)	  //显示空心圆
{
	Circle_Center_XY(x,y);		
		Circle_Radius_R(r);			
Start_Circle_or_Ellipse();	

}
/*******************************************************************************
* 函数名  : 显示实心圆
* 描述    : 在液晶屏上显示一个实心圆 ,坐标范围不超液晶分辨率
* 输入          : 设定实心圆圆心坐标 X Y
*                 设定实心圆半径R
*                 
* 输出         : None
* 返回         : None
*******************************************************************************/
void GUI_CircleFill (uint x,uint y,uint r)	  //显示实心圆
{
Circle_Center_XY(x,y);			
Circle_Radius_R(r);					
Start_Circle_or_Ellipse_Fill();	
}
/*******************************************************************************
* 函数名  : 显示空心椭圆
* 描述    : 在液晶屏上显示一个空心椭圆 ,坐标范围不超液晶分辨率
* 输入          : 设定空心椭圆圆心坐标 X Y 
*                 设定空心椭圆半径R1，半径R2
*                 
* 输出         : None
* 返回         : None
*******************************************************************************/
void GUI_Ellipse  (uint x,uint y,uint r1,uint r2)	  //显示空心椭圆
{
Circle_Center_XY(x,y);			
Ellipse_Radius_RxRy(r1, r2);
Start_Circle_or_Ellipse();
}
/*******************************************************************************
* 函数名  : 显示实心椭圆
* 描述    : 在液晶屏上显示一个实心椭圆
* 输入          : 设定实心椭圆圆心坐标 X Y ,坐标范围不超液晶分辨率
*                设置实心椭圆半径R1，半径R2
*                 
* 输出         : None
* 返回         : None
*******************************************************************************/
void GUI_EllipseFill  (uint x,uint y,uint r1,uint r2)	  //显示实心椭圆
{
Circle_Center_XY(x,y);			
Ellipse_Radius_RxRy(r1,r2);	
Start_Circle_or_Ellipse_Fill();	
}
/*******************************************************************************
* 函数名  : 显示空心三角形
* 描述    : 在液晶屏上显示一个实心三角形 ,坐标范围不超液晶分辨率
* 输入          : 设置空心三角形第一个点坐标X1 Y1
*               ：设置空心三角形第二个点坐标X2 Y2
*								：设置空心三角形第二个点坐标X3 Y3
*                 
* 输出         : None
* 返回         : None
*******************************************************************************/
void GUI_Triangle (uint x1,uint y1,uint x2,uint y2,uint x3,uint y3)	 //显示空心三角形
{
Triangle_Point1_XY(x1,y1);	
Triangle_Point2_XY(x2,y2);	
Triangle_Point3_XY(x3,y3);	
Start_Triangle();	
}
/*******************************************************************************
* 函数名  : 显示实心三角形
* 描述    : 在液晶屏上显示一个实心三角形 ,坐标范围不超液晶分辨率
* 输入          : 设置实心三角形第一个点坐标X1 Y1
*               ：设置实心三角形第二个点坐标X2 Y2
*								：设置实心三角形第二个点坐标X3 Y3
*                 
* 输出         : None
* 返回         : None
*******************************************************************************/
void GUI_Fill_Triangle (uint x1,uint y1,uint x2,uint y2,uint x3,uint y3)  //显示实心三角形
{
Triangle_Point1_XY(x1,y1);	
Triangle_Point2_XY(x2,y2);	
Triangle_Point3_XY(x3,y3);	
Start_Triangle_Fill();
}
/*******************************************************************************
* 函数名  : 显示空心矩形
* 描述    : 在液晶屏上显示一个空心矩形 ,坐标范围不超液晶分辨率
* 输入          : 设置空心矩形左上角第一个点坐标
*									设置矩形的长W和宽H
*                 
* 输出         : None
* 返回         : None
*******************************************************************************/
 void GUI_Rectangle (uint xs,uint ys,uint w,uint h)  //显示空心矩形
 {
	Line_Start_XY(xs,ys);
	Line_End_XY(xs+w-1,ys+h-1);
	Start_Square();	
}
/*******************************************************************************
* 函数名  : 显示实心矩形
* 描述    : 在液晶屏上显示一个实心矩形 ,坐标范围不超液晶分辨率
* 输入          : 设置实心矩形左上角第一个点坐标
*									设置矩形的长W和宽H
*                 
* 输出         : None
* 返回         : None
*******************************************************************************/
 void GUI_RectangleFill (uint xs,uint ys,uint w,uint h)   //显示实心矩形
 {
	Line_Start_XY(xs,ys);
	Line_End_XY(xs+w-1,ys+h-1);
	Start_Square_Fill();
}
/*******************************************************************************
* 函数名  : 显示空心圆角矩形
* 描述    : 在液晶屏上显示一个空心圆角矩形 ，坐标范围不超液晶分辨率
* 输入          : 设置显示空心圆角矩形左上角第一个点坐标
*									设置空心圆角矩形的长和宽
*                 设置圆角R ,R圆角不能少于长W或者宽H的一半
* 输出         : None
* 返回         : None
*******************************************************************************/
 void GUI_Rectangle_R (uint xs,uint ys,uint w,uint h,uint r)  //显示空心圆角矩形，r圆角不能少于w或者h的一半
 {
 		Line_Start_XY(xs,ys);
	Line_End_XY(xs+w-1,ys+h-1);
Ellipse_Radius_RxRy(r,r);
Start_Circle_Square();	
}
/*******************************************************************************
* 函数名  : 显示实心圆角矩形
* 描述    : 在液晶屏上显示一个空心圆角矩形 ，坐标范围不超液晶分辨率
* 输入          : 设置显示实心圆角矩形左上角第一个点坐标
*									设置实心圆角矩形的长和宽
*                 设置圆角R ,R圆角不能少于长W或者宽H的一半
* 输出         : None
* 返回         : None
*******************************************************************************/
 void GUI_RectangleFill_R  (uint xs,uint ys,uint w,uint h,uint r)  //显示实心圆角矩形，r圆角不能少于w或者h的一半
 {
 			Line_Start_XY(xs,ys);
Line_End_XY(xs+w-1,ys+h-1);
Ellipse_Radius_RxRy(r,r);
Start_Circle_Square_Fill();	
}
/*******************************************************************************
* 函数名  : 显直线
* 描述    : 在液晶屏上显示一条直线 ，坐标范围不超液晶分辨率
* 输入          : 设置直线的第一个点的坐标X1 Y1
*									设置直线的第二个点的坐标X2 Y2
*                
* 输出         : None
* 返回         : None
*******************************************************************************/
void GUI_Line(uint x1,uint y1,uint x2,uint y2)//显示直线
{
	Line_Start_XY(x1,y1);
	Line_End_XY(x2,y2);
	Start_Line();
}
/*******************************************************************************
* 函数名  : 显示128MB内的图片
* 描述    : 在液晶屏上显示一幅图片 ，坐标范围不超液晶分辨率
* 输入          :picnum为烧录到储存器里面的图片序号，好烧录软件左边列表对应，X,Y为在改位置显示的坐标             
* 输出         : None
* 返回         : None
*******************************************************************************/

 void Dis_Image_Mix_128Mb(uint picnum,uint x,uint y)	//128Mb储存器，显示图片到当前屏幕
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
  void Dis_Image_Mix_Big(uint picnum,uint x,uint y)	//128Mb储存器，显示图片到当前屏幕
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
* 函数名  : 显示128MB内的图片到制定图层
* 描述    : 在液晶屏上指定图层显示一幅图片， 坐标范围不超液晶分辨率
* 输入          :picnum为烧录到储存器里面的图片序号，好烧录软件左边列表对应，X,Y为在改位置显示的坐标 ，layer为指定的图层
* 输出         : None
* 返回         : None
*******************************************************************************/
    void Dis_Png_mix_128Mb(uint picnum,uint x,uint y,uint layer)	//128Mb储存器，显示图片到指定图层
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
  Enable_SFlash_SPI();									          // 使能SPI功能
 Select_SFI_0();										// 选择外挂的SPI0
 //Select_SFI_1();									  // 选择外挂的SPI1
 
										   
  Select_SFI_DMA_Mode();								          // 设置SPI的DMA模式
  SPI_Clock_Period(1);                          // 设置SPI的分频系数

  Goto_Pixel_XY(0,0);									          // 在图形模式中设置内存的位置
  SFI_DMA_Destination_Upper_Left_Corner(0,0);		// DMA传输的目的地（内存的位置）
  SFI_DMA_Transfer_Width_Height(480,272);				  // 设置块数据的宽度和高度
  SFI_DMA_Source_Width(480);							        // 设置源数据的宽度
  SFI_DMA_Source_Start_Address(0); 					  // 设置源数据在Flash的地址

  Start_SFI_DMA();									              // 开始DMA传输
  Check_Busy_SFI_DMA();	
	
 }
/*******************************************************************************
* 函数名  : 显示1024MB内的图片
* 描述    : 在液晶屏上显示一幅图片
 * 输入          :picnum为烧录到储存器里面的图片序号，好烧录软件左边列表对应，X,Y为在改位置显示的坐标
* 输出         : None
* 返回         : None
*******************************************************************************/
  void Dis_Image_mix_1024Mb(uint picnum,uint x,uint y)	//1024Mb储存器，显示图片到当前屏幕
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
* 函数名  : 根据不同的储存器容量，选择不同的显示图片函数
* 描述    : 在液晶屏上显示一幅图片 ,坐标范围不超液晶分辨率
**输入          :picnum为烧录到储存器里面的图片序号，好烧录软件左边列表对应，X,Y为在改位置显示的坐标 
* 输出         : None
* 返回         : None
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
* 函数名  : 以过滤色显示1024MB内的图片
* 描述    : 在液晶屏上指定图层显示一幅图片 ,坐标范围不超液晶分辨率
* 输入    ：picnum为烧录到储存器里面的图片序号，好烧录软件左边列表对应，X,Y为在改位置显示的坐标 ，color为过滤色
* 输出         : None
* 返回         : None
*******************************************************************************/
    void Dis_Png_Mix(uint picnum,uint x,uint y,uint color)
 {
	
Dis_Png_mix_128Mb(picnum,x,y,color);	
 }
/*******************************************************************************
* 函数名  : 初始化文字功能
* 描述    : 初始化文字功能
* 输入          : NONE

* 输出         : None
* 返回         : None
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
* 函数名  : 设置字体
* 描述    : 设置字体
* 输入          : 1代表16点阵，2代表24点阵，3代表32点阵
*        				
* 输出         : None
* 返回         : None
*******************************************************************************/
void Set_Font(uchar font) //设置字体
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
* 函数名  : 设置文字间距
* 描述    : 设置文字间的间距
* 输入          : 设置文字间左右间距X
*									设置文字间上下间距Y
*        				
* 输出         : None
* 返回         : None
*******************************************************************************/
void Set_Font_Distance(uint x,uint y)//设置文字间距
{
Font_Line_Distance(x);	
Set_Font_to_Font_Width(y);	

}
/*******************************************************************************
* 函数名  : 指定屏幕位置反色显示
* 描述    : 在液晶指定位置反色显示
* 输入          : 设置左上角坐标X1 Y1
*									设置右下角坐标X2 Y2
*        				
* 输出         : None
* 返回         : None
*******************************************************************************/
 void inverse(uint xs,uint ys,uint xe,uint ye)//屏幕区域反显	
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

void Nandflash_Config(void)//针对1024Mb 图片储存的设置，128Mb的不需要理会
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
显示自定义图片
x:X坐标地址
x:Y坐标地址
w:显示图片的宽度
h:显示图片的高度
numbers:显示图片的字节数
*datap：图片数组的指针
注意：推荐用“image2lcd”软件生成图片数组，数组格式是低位在前，RGB565格式
*/
void Draw_Picture(uint x,uint y,uint w,uint h,unsigned long numbers,const unsigned char *datap)
{		unsigned short i, j,temp;
	Graphic_Mode();
//Sel_Layer(Current_Layer,Draw_Layer);//clayer:当前图层  dlayer：写入图层
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


// Check_Mem_WR_FIFO_not_Full(); //如果MCU速度比较快，要加上这个测满函数，一般72M的STM32，无论用什么接口，都不需要加这段。 
   
   }
   Active_Window_WH(lcdx,lcdy);Active_Window_XY(0,0);
}

void Draw_Picture_SPI_Fast1(uint x,uint y,uint w,uint h,unsigned long numbers,const unsigned char *datap)
{		unsigned short i, j;
	Graphic_Mode();
//Sel_Layer(Current_Layer,Draw_Layer);//clayer:当前图层  dlayer：写入图层
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
Sel_Layer(Current_Layer,Layer1);//clayer:当前图层  dlayer：写入图层
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
void Disp_FloatNum(u8 mode,u32 x,u32 y,u8 fontsize,u8 w,double value,uchar *format,uint B_color,uint F_color ) //显示浮点数，客户可以根据自己的需求修改程序
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
* 函数名  : 显示128MB储存器的图片
* 描述    : 以过滤色的方式在液晶屏上显示一幅图片 ，坐标范围不超液晶分辨率，改功能可以用于显示图标，按键等图片，显示非方形的图片
* 输入          :picnum为烧录到储存器里面的图片序号，好烧录软件左边列表对应，X,Y为在改位置显示的坐标 ，Filter_Color为过滤色            
* 输出         : None
* 返回         : None
*******************************************************************************/
void Dis_PicButton_128Mb(uint picnum,uint x,uint y,uint Filter_Color)	
 {
ulong picx=0,picy=0,fadd=0;uchar buf3[12];
uint temp_color=0x0000;
temp_color=Read_Background_color_65k();//保存目前背景色
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
    Background_color_65k(temp_color);//将之前保存的背景色重新写进背景色寄存器
}
	
 }
 //选择不同的图层进行操作
void Sel_Layer(unsigned long clayer,unsigned long dlayer)//clayer:当前图层  dlayer：写入图层
{
Canvas_Image_Start_address(dlayer);
Draw_Layer=dlayer;
Main_Image_Start_Address(clayer);
Current_Layer=clayer;
}
  /*******************************************************************************
* 函数名  : 复制图层
* 描述    : 复制图层
* 输入          :slayer为源图层，dlayer为目标图层，sx,sy为源图层的左上角坐标，dx,dy为目标图层的左上角坐标，w,h为要复制区域的宽和高度           
* 输出         : None
* 返回         : None
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
* 函数名  : 画中画窗口1
* 描述    : 显示画中画窗口1
* 输入          :sx,sy为源图层的左上角坐标，dx,dy为目标图层的左上角坐标，w,h为要画中画的宽和高度           
* 输出         : None
* 返回         : None
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
* 函数名  : 画中画窗口2
* 描述    : 显示画中画窗口2
* 输入          :sx,sy为源图层的左上角坐标，dx,dy为目标图层的左上角坐标，w,h为要画中画的宽和高度           
* 输出         : None
* 返回         : None
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
			CGROM_Select_Internal_CGROM();   // 内部CGROM为字符来源
			LCD_CmdWrite(0x04);
			LCD_DataWrite(c[i]);
			Check_Mem_WR_FIFO_not_Full();  
			i += 1;
		}
		else
		{
			Font_Select_UserDefine_Mode();   // 自定义字库
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

  Graphic_Mode(); //back to graphic mode;图形模式


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
 unsigned char Cursor_N                  // 选择光标   1:光标1   2:光标2   3:光标3  4:光标4
,unsigned char Color1                    // 颜色1
,unsigned char Color2                    // 颜色2
,unsigned short X_Pos                    // 显示坐标X
,unsigned short Y_Pos                    // 显示坐标Y
,unsigned char *Cursor_Buf               // 光标数据的缓冲首地址
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
	
	Memory_Select_SDRAM();//写完后切回SDRAM
	Set_Graphic_Cursor_Color_1(Color1);
  Set_Graphic_Cursor_Color_2(Color2);
  Graphic_Cursor_XY(X_Pos,Y_Pos);
	
	Enable_Graphic_Cursor();

}
void Set_Graphic_Cursor_Pos(
 unsigned char Cursor_N                  // 选择光标   1:光标1   2:光标2   3:光标3  4:光标4
,unsigned short X_Pos                    // 显示坐标X
,unsigned short Y_Pos                    // 显示坐标Y
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
Bcolor=color_blue; //自定义前景色和背景色变量
Fcolor=color_red;//自定义前景色和背景色变量

	
	



Display_ON(); //开显示
//Color_Bar_ON();
Foreground_color_16M(Fcolor);//设置前景色
Background_color_16M(Bcolor); //设置背景色




	#ifdef NOR_FLASH
Init_Font();
	#endif	
		#ifdef NAND_FLASH
Init_Font_1024Mb();//初始化字库

	#endif	
GUI_CleanScreen();//用背景色清屏  
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
//Draw_Picture_SPI_Fast(0,0,100,100,10000,gImage_100X100_RGB332);//在SPI接口的方式下，显示rgb232格式的图片，打点的方式,




	Set_Font(3);
Display_Text(200,  0,1,"TFT液晶屏");//显示字符



  



}
extern uchar Touch_State;
extern volatile u16 X0,Y0;
void Read_Coordinate(u16 *x,u16 *y,u8 *state)
{
*x=	X0;
*y=	Y0;
*state=	Touch_State;
}
void Touch_Test(void)
{
	u16 x,y;u8 state;
	EXTI_Init();
		Set_Color(color_white,color_black);
	GUI_CleanScreen();
	Set_Font(3);
	 Display_Text(0,0,0,"按下黑色圆点，松开红色圆点");

  	while(1)//
	{

Read_Coordinate(&x,&y,&state);
		
		switch(state)
		{
	
			case TOUCH_UP:
				delay_ms(300);//为了演示松开状态，加延时
			HAL_NVIC_DisableIRQ(EXTI1_IRQn);
				Set_Color(color_white,color_red);
	GUI_CircleFill (x,y,7);
	Display_Text(  432,  0,0,"状态：释放");
	Touch_State=TOUCH_NC;		
	HAL_NVIC_EnableIRQ(EXTI1_IRQn);   		
			break;
			case  TOUCH_DOWN:
				HAL_NVIC_DisableIRQ(EXTI1_IRQn);
		Set_Color(color_white,color_black);
			 GUI_CircleFill (x,y,3);
			Display_Text(  432,  0,0,"状态：按下");
			HAL_NVIC_EnableIRQ(EXTI1_IRQn);   
			break;
			case  TOUCH_NC:
			
			break;
		}
	
/*
	if(PCin(1)==0)	
	{  
	
	// Readtouch_RTP();//电容屏测试请运行这个函数

	Readtouch_CTP(); //电阻屏测试请运行这个函数
	 }
	*/
	}
	
}
