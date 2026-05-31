#include "sys.h"
#include "lcd.h"
void Set_Backlight(uchar bl);//设置背光亮度
void GUI_CleanScreen(void);//用颜色清屏
void Set_Color(unsigned long bc,unsigned long fc);  //设置颜色
void GUI_Point(uint x,uint y);  //显示一个点
void GUI_Circle   (uint x,uint y,uint r);	  //显示空心圆
void GUI_CircleFill (uint x,uint y,uint r);	  //显示实心圆
void GUI_Ellipse  (uint x,uint y,uint r1,uint r2);	  //显示空心椭圆
void GUI_EllipseFill  (uint x,uint y,uint r1,uint r2);	  //显示实心椭圆，功能07
void GUI_Triangle (uint x1,uint y1,uint x2,uint y2,uint x3,uint y3);	 //显示空心三角形，功能08
void GUI_Fill_Triangle (uint x1,uint y1,uint x2,uint y2,uint x3,uint y3);  //显示实心三角形，功能09
void GUI_Rectangle (uint xs,uint ys,uint w,uint h);  //显示空心矩形，功能10
void GUI_RectangleFill (uint xs,uint ys,uint w,uint h) ;  //显示实心矩形，功能11
void GUI_Rectangle_R (uint xs,uint ys,uint w,uint h,uint r);  //显示空心圆角矩形，功能12
void GUI_RectangleFill_R  (uint xs,uint ys,uint w,uint h,uint r);  //显示实心圆角矩形
void GUI_Line(uint x1,uint y1,uint x2,uint y2);//显示直线	 
 void Dis_Image_Mix_1024Mb(uint picnum,uint x,uint y);	
 void Dis_Image_Mix_128Mb(uint picnum,uint x,uint y);	
 void Dis_Image_Mix(uint picnum,uint x,uint y);	
void Draw_Picture(uint x,uint y,uint w,uint h,unsigned long numbers,const unsigned char *datap);
void Disp_FloatNum(u8 mode,u32 x,u32 y,u8 fontsize,u8 w,double value,uchar *format,uint B_color,uint F_color );

 void selectlcd_mode(uchar lcdmode,uchar commode);
   void Set_Backlight(uchar bl);

void cnconfig(void);
void Display_Text(uint x,uint y,uchar tongtou ,uchar *c);
void GUI_Point(uint x,uint y);
void Set_Font_Distance(uint x,uint y);


 void Set_Font(uchar font);
 void sentvalue(uchar tm);
	 void inverse(uint xs,uint ys,uint xe,uint ye);
	void Sel_Layer(unsigned long clayer,unsigned long dlayer);//clayer:当前图层  dlayer：写入图层

	 void  Nandflash_Config(void);



   void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead);
   void STMFLASH_Write_NoCheck(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite) ;  
   void Geometric_Drawing(void);
   void W25N01_Set_BUF(void);
   void W25N01_Reset(void);
	void W25N01_Clear_BUF(void);
   void GUI_WritePoint(uint x,uint y,uint color);
	uint GUI_ReadPoint(uint x,uint y);
	void Touch_Test(void);
	void EXTI_Init(void);
	
	  void Layer_Copy(unsigned long slayer,unsigned long dlayer,uint sx,uint sy,uint dx,uint dy,uint w,uint h);

