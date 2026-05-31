#include "rtp.h"
#include "sys.h"
#include "stdio.h"	
#include <math.h>
#include "delay.h"
#include "gui.h"

  extern volatile u16 X0,Y0;
  extern volatile uint Bcolor;
extern volatile uint Fcolor;
 volatile u8 calflag=0,touchcal=1;
typedef          struct POINT 
          {
          long double  x;
         long  double y;
          }coordinate;

  typedef struct Matrix {
                                                
                       long double    An,  
                                 Bn,     
                                 Cn,   
                                 Dn,    
                                 En,    
                                 Fn,     
                                 Divider ;
                     } matrix ;


coordinate  Screen ;
coordinate  display ;
matrix mat ;
coordinate ScreenSample[3];	//液晶屏上的采样点/AD值  
coordinate DisplaySample[3];//LCD上 对应的点

coordinate DisplaySample[3] =   {
                                            { CALSIZE, CALSIZE },	 //左上角的校准点
                                            { CALSIZE, 480-CALSIZE},	  //左下角的校准点
                                            { 800-CALSIZE,480-CALSIZE}	  //右下角的校准点
                                    } ;
									 unsigned char setCalibrationMatrix( coordinate * displayPtr,coordinate * screenPtr,matrix * matrixPtr)	//用AD读取值和三个点的值，计算出公式所需的参数
{

    unsigned char  retValue = 0 ;
 
    matrixPtr->Divider = ((screenPtr[0].x - screenPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)) - 
                         ((screenPtr[1].x - screenPtr[2].x) * (screenPtr[0].y - screenPtr[2].y)) ;
   
    if( matrixPtr->Divider == 0 )

    {
        retValue = 1;
    }
    else
    {	
        matrixPtr->An = ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)) - 
                        ((displayPtr[1].x - displayPtr[2].x) * (screenPtr[0].y - screenPtr[2].y)) ;

        matrixPtr->Bn = ((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].x - displayPtr[2].x)) - 
                        ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].x - screenPtr[2].x)) ;

        matrixPtr->Cn = (screenPtr[2].x * displayPtr[1].x - screenPtr[1].x * displayPtr[2].x) * screenPtr[0].y +
                        (screenPtr[0].x * displayPtr[2].x - screenPtr[2].x * displayPtr[0].x) * screenPtr[1].y +
                        (screenPtr[1].x * displayPtr[0].x - screenPtr[0].x * displayPtr[1].x) * screenPtr[2].y ;

        matrixPtr->Dn = ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].y - screenPtr[2].y)) - 
                        ((displayPtr[1].y - displayPtr[2].y) * (screenPtr[0].y - screenPtr[2].y)) ;
    
        matrixPtr->En = ((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].y - displayPtr[2].y)) - 
                        ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].x - screenPtr[2].x)) ;

        matrixPtr->Fn = (screenPtr[2].x * displayPtr[1].y - screenPtr[1].x * displayPtr[2].y) * screenPtr[0].y +
                        (screenPtr[0].x * displayPtr[2].y - screenPtr[2].x * displayPtr[0].y) * screenPtr[1].y +
                        (screenPtr[1].x * displayPtr[0].y - screenPtr[0].x * displayPtr[1].y) * screenPtr[2].y ;
					


    }

    return( retValue ) ;
}


void getDisplayPoint(coordinate * displayPtr,matrix * matrixPtr ) 	//这个是校准公式，用AD读取值和之前计算出来的参数值代入公式，得出对应的屏幕坐标
{
        display.x = ( (matrixPtr->An * Screen.x) + 
                          (matrixPtr->Bn * Screen.y) + 
                           matrixPtr->Cn 
                        ) / matrixPtr->Divider ;

       display.y= ( (matrixPtr->Dn * Screen.x) + 
                          (matrixPtr->En * Screen.y) + 
                           matrixPtr->Fn 
                        ) / matrixPtr->Divider ;
}
 void SPI_Delay(void)
{
	uint i;

	for (i = 0; i < 100; i++);	/* 延迟, 不准 */
}
void TOUCH_IO_Init(void)
{


	    GPIO_InitTypeDef GPIO_Initure;
    
 __HAL_RCC_GPIOA_CLK_ENABLE();  
 __HAL_RCC_GPIOC_CLK_ENABLE() ; 

    GPIO_Initure.Pin=GPIO_PIN_5|GPIO_PIN_7;  
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP ;  
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;  
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);     
	 
	     GPIO_Initure.Pin=GPIO_PIN_6;  
    GPIO_Initure.Mode=GPIO_MODE_INPUT  ;  
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;     
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);    
	 
	 	     GPIO_Initure.Pin=GPIO_PIN_4;  
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP ;  
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;   
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);   




	 	     GPIO_Initure.Pin=GPIO_PIN_1;  
    GPIO_Initure.Mode=GPIO_MODE_INPUT;  
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;   
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);   
	 
  PEN=1;
DOUT=1; 
TDIN=1; 
TCLK=1; 
TCS=1; 

 DisplaySample[0].x=CALSIZE;
DisplaySample[0].y=CALSIZE;
DisplaySample[1].x=CALSIZE;
DisplaySample[1].y=lcdy-CALSIZE;
DisplaySample[2].x=lcdx-CALSIZE;
DisplaySample[2].y=lcdy-CALSIZE;  
}
void ADS_Write_Byte(u8 num)    
{  
	u8 count=0;   
	for(count=0;count<8;count++)  
	{ 	  
		if(num&0x80)TDIN=1;  
		else TDIN=0;   
		num<<=1; SPI_Delay();    
		TCLK=0;//上升沿有效	 
		SPI_Delay(); 	 
		TCLK=1;      
	} 			    
} 		 

u16 ADS_Read_AD(u8 CMD)	  
{ 	 
	u16 i;
	u8 count=0; 	  
	u16 Num=0; 
	TCLK=0;//先拉低时钟 	 
	TCS=0; //选中ADS7843	 
	ADS_Write_Byte(CMD);//发送命令字
	for(i=500;i>0;i--);
//	Delay1_us(6);//ADS7846的转换时间最长为6us
	TCLK=1;//给1个时钟，清除BUSY  
	SPI_Delay();  	    
	TCLK=0; 	 
	for(count=0;count<16;count++)  
	{ 				  
		Num<<=1; 
		SPI_Delay(); 	 
		TCLK=0;//下降沿有效 
		SPI_Delay();  	    	   
		TCLK=1;
		if(DOUT)Num++; 		 
	}  	
	Num>>=4;   //只有高12位有效.
	TCS=1;//释放ADS7843	 
	return(Num);   
}

 u16 ADS_Read_XY(u8 xy)
{
	u16 i, j;
	u16 buf[READ_TIMES];
	u16 sum=0;
	u16 temp;
	for(i=0;i<READ_TIMES;i++)
	{				 
		buf[i]=ADS_Read_AD(xy);	    
	}				    
	for(i=0;i<READ_TIMES-1; i++)//排序
	{
		for(j=i+1;j<READ_TIMES;j++)
		{
			if(buf[i]>buf[j])//升序排列
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}	  
	sum=0;
	for(i=LOST_VAL;i<READ_TIMES-LOST_VAL;i++)sum+=buf[i];
	temp=sum/(READ_TIMES-2*LOST_VAL);
	return temp;   
} 
 u8 Read_ADS(u16 *x,u16 *y)
{
	u16 xtemp,ytemp;
  //	DelayMS_sys(5);			 	 		  
	xtemp=ADS_Read_XY(CMD_RDX);
	// DelayMS_sys(5);
	ytemp=ADS_Read_XY(CMD_RDY);	  												   
	if(xtemp<10||ytemp<10)return 0;//读数失败
	*x=xtemp;
	*y=ytemp;
	return 1;//读数成功
}
//2次读取ADS7846,连续读取2次有效的AD值,且这两次的偏差不能超过
//50,满足条件,则认为读数正确,否则读数错误.	   
//该函数能大大提高准确度
#define ERR_RANGE 50 //误差范围 
u8 Read_ADS2(u16 *x,u16 *y) 
{
	u16 x1,y1;
 	u16 x2,y2;
 	u8 flag;   
    flag=Read_ADS(&x1,&y1);   
    if(flag==0)return(0);
    flag=Read_ADS(&x2,&y2);	   
    if(flag==0)return(0);   
    if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))//前后两次采样在+-50内
    &&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE)))
    {
        *x=(x1+x2)/2;
        *y=(y1+y2)/2;
	//	printf("X：%d \r\n ", (x1+x2)/2);
    //   printf("Y：%d \r\n ", (y1+y2)/2);


        return 1;
    }else return 0;	  
} 

char Readtouch_RTP(void)
{
  	if(PEN==0)
{ 	    
//  printf("下拉 \r\n ");
if(Read_ADS2(&X0,&Y0))
 {

 	Screen.x=X0;
	Screen.y=Y0;
//printf("X：%d \r\n ", (uint)Screen.x);
//printf("Y：%d \r\n ", (uint)Screen.y);	

	if(touchcal==0)
	{
 getDisplayPoint(&display, &mat ) ;
printf("校准后X：%d \r\n ", (uint)display.x);
printf("校准后Y：%d \r\n ", (uint)display.y);	
 }

 	if(touchcal==1)
{
  	display.x=X0;
	display.y=Y0;
printf("原始值X：%d \r\n ", (uint)display.x);
printf("原始值Y：%d \r\n ", (uint)display.y);	
}
calflag=1; 


return 1;
}
else
{
return 0;
}
}
else
return 0;


}
void Adjustdisplay(void)
{ 


//Background_color_65k(0x001f);
begin:;
	Set_Font(1);
 touchcal=1;
 calflag=0;		
 Set_Color(color_blue,color_white);
GUI_CleanScreen();

	    // Display_Text(156,0,0,"触摸屏校准程序");
	Display_Text(lcdx/2-146,lcdy/2,0,"Press The Cross Dot In Left_Top"); 
		//DisplayMix(1,0,lcdx/2-146,lcdy/2,"Press The Cross Dot In Left_Top"); 
delay_ms(500);

  GUI_Line(CALSIZE, CALSIZE-10, CALSIZE, CALSIZE+10); 
  GUI_Line(CALSIZE-10, CALSIZE, CALSIZE+10, CALSIZE);

delay_ms(1);  

  while(Readtouch_RTP()!=1)
  {
delay_ms(1); 
 }	delay_ms(500);

 //USART_Send(0x88);
 calflag=0;	//printf("X：%d \r\n ", (uint)display.x); printf("y：%d \r\n ", (uint)display.y);
ScreenSample[0].x=display.x;	
 ScreenSample[0].y=display.y;




	   	//Display_Text(120,32,0,"请准确点击左下角圆点"); 	
 Display_Text(lcdx/2-146,lcdy/2,0,"Press The Cross Dot In Left_Bottom"); 
		//DisplayMix(1,0,lcdx/2-146,lcdy/2,"Press The Cross Dot In Left_Bottom"); 

delay_ms(500);

   GUI_Line(CALSIZE, lcdy-CALSIZE-10, CALSIZE, lcdy-CALSIZE+10); 
  GUI_Line(CALSIZE-10, lcdy-CALSIZE, CALSIZE+10, lcdy-CALSIZE);

  while(Readtouch_RTP()!=1)
  {
delay_ms(1); 
 }	delay_ms(500);
	delay_ms(500);	delay_ms(500);

 calflag=0;	

//EXTI1_SET(0);	 

 calflag=0;

 ScreenSample[1].x=display.x;	
 ScreenSample[1].y=display.y;


   Display_Text(lcdx/2-146,lcdy/2,0,"Press The Cross Dot In Right_Bottom"); 


delay_ms(500);
 
    GUI_Line(lcdx-CALSIZE, lcdy-CALSIZE-10, lcdx-CALSIZE, lcdy-CALSIZE+10); 
  GUI_Line(lcdx-CALSIZE-10, lcdy-CALSIZE, lcdx-CALSIZE+10, lcdy-CALSIZE);

//EXTI1_SET(1);
  while(Readtouch_RTP()!=1)
  {
delay_ms(1); 
 }	delay_ms(500);
	delay_ms(500);

 calflag=0;	//printf("X：%d \r\n ", (uint)display.x); printf("y：%d \r\n ", (uint)display.y);



 calflag=0;


 calflag=0;
 ScreenSample[2].x=display.x;	
 ScreenSample[2].y=display.y;
touchcal=0;
setCalibrationMatrix( &DisplaySample[0],&ScreenSample[0],&mat ) ;


Foreground_color_65k(Fcolor);
Background_color_65k(Bcolor);
 GUI_CleanScreen();


			Display_Text(lcdx/2-88,156,0,"请点击屏幕测试是否准确"); 
	  


delay_ms(500); 
 Set_Font(2);
GUI_Rectangle_R(26, lcdy-42, 55, 28,5);	 
Display_Text(29,lcdy-39,0,"重做"); 

GUI_Rectangle_R(lcdx-68, lcdy-42, 55, 28,5);	 
Display_Text(lcdx-65,lcdy-39,0,"确认"); 
 touchcal=0;


  while(1)
{
			
	if(PCin(1)==0)	
	{

	  Readtouch_RTP();
	  GUI_CircleFill (display.x,display.y,2);
	 }
if(display.x>(lcdx-65)&display.y>(lcdy-39)&display.x<(lcdx-65+49)&display.y<(lcdy-39+25))
 break;
 if(display.x>29&display.y>(lcdy-39)&display.x<75&display.y<(lcdy-39+25))
 goto begin;		

	
} 




GUI_CleanScreen();
Display_Text(156,130,1,"触摸屏校准完成"); 

 touchcal=0;

			 
}
