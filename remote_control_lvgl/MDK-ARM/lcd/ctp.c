
#include "CTP.h"
#include "sys.h"
#include "stdio.h"	
#include <math.h>
#include "delay.h"
#include "gui.h"
#include "i2c.h"

//#define  Touch_Mode_ContinuedDown_OnceUp //����ģʽΪ����������Ӧ���ɿ���Ӧһ��
#define  Touch_Mode_OnceDown_OnceUP		  //����ģʽΪ������Ӧһ�Σ��ɿ���Ӧһ��
   uchar Touch_State=TOUCH_NC;
volatile u16 X0,Y0,X_S,Y_S;
 uint releasex=0,releasey=0;
volatile u8 CTP_Debug_Mode=0;
volatile u8 CTP_Ready=0;
volatile u8 CTP_Debug_Addr=0;
volatile u8 CTP_Debug_ProbeAck=0xff;
volatile u8 CTP_Debug_PID[5]={0};
volatile u8 CTP_Debug_RdAckWr=0xff;
volatile u8 CTP_Debug_RdAckRegH=0xff;
volatile u8 CTP_Debug_RdAckRegL=0xff;
volatile u8 CTP_Debug_RdAckRd=0xff;
volatile u8 CTP_Debug_PID_BA[5]={0};
volatile u8 CTP_Debug_PID_28[5]={0};
volatile u8 CTP_Debug_Mode_BA=0;
volatile u8 CTP_Debug_Mode_28=0;
volatile u8 CTP_Debug_DirectAckWr=0xff;
volatile u8 CTP_Debug_DirectAckRegH=0xff;
volatile u8 CTP_Debug_DirectAckRegL=0xff;
volatile u8 CTP_Debug_DirectAckRd=0xff;
volatile u8 CTP_Debug_INT_Level=0;
volatile u8 CTP_Debug_SCL_Level=0;
volatile u8 CTP_Debug_SDA_Level=0;
#define CTP_USE_HW_I2C 1
#define CTP_I2C_PROBE_TIMEOUT_MS 20U
#define CTP_I2C_XFER_TIMEOUT_MS 5U
volatile u8 CTP_Debug_HwReadyBA=0xff;
volatile u8 CTP_Debug_HwReady28=0xff;
volatile u8 CTP_Debug_HwReadRet=0xff;
volatile u8 CTP_Debug_HwWriteRet=0xff;
volatile u32 CTP_Debug_HwError=0;
volatile u32 CTP_Debug_HwReadCount=0;
volatile u32 CTP_Debug_HwWriteCount=0;
volatile u32 CTP_Debug_HwErrorCount=0;
volatile u32 CTP_Debug_LastReg=0;
volatile u32 CTP_Debug_LastLen=0;
static u8 gt_cmd_wr=GT_CMD_WR;
static u8 gt_cmd_rd=GT_CMD_RD;


 extern volatile unsigned long Bcolor,Fcolor;
  extern volatile uint drx,dry;
  extern volatile uchar fun;
   extern const uint Touch_Coordinate[8][6];
   extern uint Button_State[4];
 const u16 GT9147_TPX_TBL[5]={GT_TP1_REG,GT_TP2_REG,GT_TP3_REG,GT_TP4_REG,GT_TP5_REG};
   
 
 void CTP_Prepare_Reset(void)
 {
     GPIO_InitTypeDef GPIO_Initure;

     GTP_I2C_RST_OPEN();
     GPIO_Initure.Pin=GTP_RST_GPIO_PIN;
     GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;
     GPIO_Initure.Pull=GPIO_PULLUP;
     GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;
     HAL_GPIO_Init(GTP_RST_GPIO_PORT,&GPIO_Initure);
     GT_RST=0;

     GTP_I2C_INT_OPEN();
     GPIO_Initure.Pin=GTP_INT_GPIO_PIN;
     GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;
     GPIO_Initure.Pull=GPIO_NOPULL;
     GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;
     HAL_GPIO_Init(GTP_INT_GPIO_PORT,&GPIO_Initure);
     GT_INT=0;
     delay_ms(10);
 }
 static u8 CTP_Probe_Address(u8 wr_cmd, u8 rd_cmd)
{
#if CTP_USE_HW_I2C
    HAL_StatusTypeDef ret;

    (void)rd_cmd;
    ret = HAL_I2C_IsDeviceReady(&hi2c3, wr_cmd, 2, CTP_I2C_PROBE_TIMEOUT_MS);
    CTP_Debug_ProbeAck = (ret == HAL_OK) ? 0 : 1;
    CTP_Debug_HwError = HAL_I2C_GetError(&hi2c3);
    if(wr_cmd == 0xBA) CTP_Debug_HwReadyBA = (u8)ret;
    if(wr_cmd == 0x28) CTP_Debug_HwReady28 = (u8)ret;
    if(ret == HAL_OK)
    {
        gt_cmd_wr = wr_cmd;
        gt_cmd_rd = rd_cmd;
        CTP_Debug_Addr = wr_cmd;
        return 1;
    }
    return 0;
#else
    u8 ack;

    CT_IIC_Start();
    CT_IIC_Send_Byte(wr_cmd);
    ack=CT_IIC_Wait_Ack();
    CT_IIC_Stop();

    CTP_Debug_ProbeAck=ack;
    if(ack==0)
    {
        gt_cmd_wr=wr_cmd;
        gt_cmd_rd=rd_cmd;
        CTP_Debug_Addr=wr_cmd;
        return 1;
    }
    return 0;
#endif
}

 static u8 CTP_Detect_Address(void)
 {
     CTP_Debug_Addr=0;
     if(CTP_Probe_Address(0xBA,0xBB)) return 1;
     return CTP_Probe_Address(0x28,0x29);
 }


static void CTP_I2C_ReleaseBus(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    u8 i;

    GTP_I2C_PORT_OPEN();
    HAL_GPIO_WritePin(GTP_I2C_SCL_PORT, GTP_I2C_SCL_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GTP_I2C_SDA_PORT, GTP_I2C_SDA_PIN, GPIO_PIN_SET);

    GPIO_Initure.Pin = GTP_I2C_SCL_PIN;
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_Initure.Pull = GPIO_PULLUP;
    GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GTP_I2C_SCL_PORT, &GPIO_Initure);

    GPIO_Initure.Pin = GTP_I2C_SDA_PIN;
    HAL_GPIO_Init(GTP_I2C_SDA_PORT, &GPIO_Initure);

    CT_IIC_SCL = 1;
    CT_IIC_SDA = 1;
    delay_us(20);

    for(i = 0; (i < 9) && (HAL_GPIO_ReadPin(GTP_I2C_SDA_PORT, GTP_I2C_SDA_PIN) == GPIO_PIN_RESET); i++)
    {
        CT_IIC_SCL = 0;
        delay_us(20);
        CT_IIC_SCL = 1;
        delay_us(20);
    }

    CT_IIC_SDA = 1;
    CT_IIC_SCL = 1;
    delay_us(20);

    CTP_Debug_SCL_Level = HAL_GPIO_ReadPin(GTP_I2C_SCL_PORT, GTP_I2C_SCL_PIN);
    CTP_Debug_SDA_Level = HAL_GPIO_ReadPin(GTP_I2C_SDA_PORT, GTP_I2C_SDA_PIN);
}

static void CTP_I2C_ConfigHardwarePins(void)
{
    GPIO_InitTypeDef GPIO_Initure;

    GTP_I2C_PORT_OPEN();

    GPIO_Initure.Pin = GTP_I2C_SCL_PIN;
    GPIO_Initure.Mode = GPIO_MODE_AF_OD;
    GPIO_Initure.Pull = GPIO_PULLUP;
    GPIO_Initure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_Initure.Alternate = GPIO_AF4_I2C3;
    HAL_GPIO_Init(GTP_I2C_SCL_PORT, &GPIO_Initure);

    GPIO_Initure.Pin = GTP_I2C_SDA_PIN;
    HAL_GPIO_Init(GTP_I2C_SDA_PORT, &GPIO_Initure);

    CTP_Debug_SCL_Level = HAL_GPIO_ReadPin(GTP_I2C_SCL_PORT, GTP_I2C_SCL_PIN);
    CTP_Debug_SDA_Level = HAL_GPIO_ReadPin(GTP_I2C_SDA_PORT, GTP_I2C_SDA_PIN);
}
static void CTP_Debug_Read_Reg_With_Cmd(u8 wr_cmd, u8 rd_cmd, u16 reg, volatile u8 *buf, u8 len)
{
#if CTP_USE_HW_I2C
    HAL_StatusTypeDef ret;

    (void)rd_cmd;
    ret = HAL_I2C_Mem_Read(&hi2c3, wr_cmd, reg, I2C_MEMADD_SIZE_16BIT, (u8 *)buf, len, CTP_I2C_PROBE_TIMEOUT_MS);
    CTP_Debug_HwReadRet = (u8)ret;
    CTP_Debug_HwError = HAL_I2C_GetError(&hi2c3);
    CTP_Debug_DirectAckWr = (ret == HAL_OK) ? 0 : 1;
    CTP_Debug_DirectAckRegH = (ret == HAL_OK) ? 0 : 1;
    CTP_Debug_DirectAckRegL = (ret == HAL_OK) ? 0 : 1;
    CTP_Debug_DirectAckRd = (ret == HAL_OK) ? 0 : 1;
#else
    u8 i;

    CT_IIC_Start();
    CT_IIC_Send_Byte(wr_cmd);
    CTP_Debug_DirectAckWr = CT_IIC_Wait_Ack();
    CT_IIC_Send_Byte(reg >> 8);
    CTP_Debug_DirectAckRegH = CT_IIC_Wait_Ack();
    CT_IIC_Send_Byte(reg & 0XFF);
    CTP_Debug_DirectAckRegL = CT_IIC_Wait_Ack();
    CT_IIC_Start();
    CT_IIC_Send_Byte(rd_cmd);
    CTP_Debug_DirectAckRd = CT_IIC_Wait_Ack();
    for(i = 0; i < len; i++)
    {
        buf[i] = CT_IIC_Read_Byte(i == (len - 1) ? 0 : 1);
    }
    CT_IIC_Stop();
#endif
}

 void CTP_Init(void)
 {

	 	    GPIO_InitTypeDef GPIO_Initure;
    Touch_State=TOUCH_NC;
    CTP_Ready=0;
    GTP_I2C_INT_OPEN();
    HAL_I2C_DeInit(&hi2c3);
    CTP_I2C_ReleaseBus();
    MX_I2C3_Init();
    CTP_I2C_ConfigHardwarePins();
GPIO_Initure.Pin=GTP_INT_GPIO_PIN;  
    GPIO_Initure.Mode=GPIO_MODE_INPUT ;  
    GPIO_Initure.Pull=GPIO_PULLUP;
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;   
    HAL_GPIO_Init(GTP_INT_GPIO_PORT,&GPIO_Initure);
	delay_ms(10); 
	/*
	for(i=0;i<3;i++)
	{

		GT9147_WR_Reg(0X814E ,&temp,1);//����?
		delay_ms(50);
	}
	 */
	 	 	     GPIO_Initure.Pin=GTP_INT_GPIO_PIN;  
    GPIO_Initure.Mode=GPIO_MODE_INPUT ;  
    GPIO_Initure.Pull=GPIO_PULLUP;
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;//����       
    HAL_GPIO_Init(GTP_INT_GPIO_PORT,&GPIO_Initure);     //��ʼ��	
    CTP_Debug_INT_Level = HAL_GPIO_ReadPin(GTP_INT_GPIO_PORT, GTP_INT_GPIO_PIN);
    CTP_Debug_SCL_Level = HAL_GPIO_ReadPin(GTP_I2C_SCL_PORT, GTP_I2C_SCL_PIN);
    CTP_Debug_SDA_Level = HAL_GPIO_ReadPin(GTP_I2C_SDA_PORT, GTP_I2C_SDA_PIN);
	 
	delay_ms(100); 
	CTP_Ready=CTP_Detect_Address();
	if(CTP_Ready)
    {
        if(gt_cmd_wr == 0xBA)
        {
            CTP_Debug_Read_Reg_With_Cmd(gt_cmd_wr, gt_cmd_rd, GT_PID_REG, CTP_Debug_PID_BA, 4);
            CTP_Debug_Read_Reg_With_Cmd(gt_cmd_wr, gt_cmd_rd, GT_GSTID_REG, &CTP_Debug_Mode_BA, 1);
        }
        else
        {
            CTP_Debug_Read_Reg_With_Cmd(gt_cmd_wr, gt_cmd_rd, GT_PID_REG, CTP_Debug_PID_28, 4);
            CTP_Debug_Read_Reg_With_Cmd(gt_cmd_wr, gt_cmd_rd, GT_GSTID_REG, &CTP_Debug_Mode_28, 1);
        }
        GT9147_RD_Reg(GT_PID_REG,(u8 *)CTP_Debug_PID,4);
    }
 }



  void CT_Delay(void)
{
delay_us(30);
} 
//����IIC��ʼ�ź�
void CT_IIC_Start(void)
{
	CT_SDA_OUT();     //sda�����?
	CT_IIC_SDA=1;	  	  
	CT_IIC_SCL=1;
	
 	delay_us(30);
 	CT_IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	CT_Delay();
	CT_IIC_SCL=0;//ǯסI2C���ߣ�׼�����ͻ��������?
}	  
//����IICֹͣ�ź�
void CT_IIC_Stop(void)
{
    CT_SDA_OUT();
    CT_IIC_SCL=0;
    CT_IIC_SDA=0;
    delay_us(30);
    CT_IIC_SCL=1;
    CT_Delay();
    CT_IIC_SDA=1;
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�?
u8 CT_IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	CT_SDA_IN();      //SDA����Ϊ����  
	CT_IIC_SDA=1;	   
	CT_IIC_SCL=1;
	CT_Delay();
	while(CT_READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			CT_IIC_Stop();
			return 1;
		} 
		CT_Delay();
	}
	CT_IIC_SCL=0;//ʱ�����? 	   
	return 0;  
} 
//����ACKӦ��
void CT_IIC_Ack(void)
{
	CT_IIC_SCL=0;
	CT_SDA_OUT();
	CT_Delay();
	CT_IIC_SDA=0;
	CT_Delay();
	CT_IIC_SCL=1;
	CT_Delay();
	CT_IIC_SCL=0;
}
//������ACKӦ��		    
void CT_IIC_NAck(void)
{
	CT_IIC_SCL=0;
	CT_SDA_OUT();
	CT_Delay();
	CT_IIC_SDA=1;
	CT_Delay();
	CT_IIC_SCL=1;
	CT_Delay();
	CT_IIC_SCL=0;
}					 				     
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void CT_IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	CT_SDA_OUT(); 	    
    CT_IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
	CT_Delay();
	for(t=0;t<8;t++)
    {              
        CT_IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	      
		CT_IIC_SCL=1; 
		CT_Delay();
		CT_IIC_SCL=0;	
		CT_Delay();
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 CT_IIC_Read_Byte(unsigned char ack)
{
	u8 i,receive=0;
 	CT_SDA_IN();//SDA����Ϊ����
	delay_us(30);
	for(i=0;i<8;i++ )
	{ 
		CT_IIC_SCL=0; 	    	   
		CT_Delay();
		CT_IIC_SCL=1;	 
		receive<<=1;
		if(CT_READ_SDA)receive++;   
	}	  				 
	if (!ack)CT_IIC_NAck();//����nACK
	else CT_IIC_Ack(); //����ACK   
 	return receive;
}

 u8 GT9147_WR_Reg(u16 reg,u8 *buf,u8 len)
{
#if CTP_USE_HW_I2C
    HAL_StatusTypeDef ret;

    CTP_Debug_HwWriteCount++;
    CTP_Debug_LastReg = reg;
    CTP_Debug_LastLen = len;
    ret = HAL_I2C_Mem_Write(&hi2c3, gt_cmd_wr, reg, I2C_MEMADD_SIZE_16BIT, buf, len, CTP_I2C_XFER_TIMEOUT_MS);
    CTP_Debug_HwWriteRet = (u8)ret;
    CTP_Debug_HwError = HAL_I2C_GetError(&hi2c3);
    if(ret != HAL_OK) CTP_Debug_HwErrorCount++;
    return (ret == HAL_OK) ? 0 : 1;
#else
    u8 i;
    u8 ret=0;
    CT_IIC_Start();
    CT_IIC_Send_Byte(gt_cmd_wr);
    CT_IIC_Wait_Ack();
    CT_IIC_Send_Byte(reg>>8);
    CT_IIC_Wait_Ack();
    CT_IIC_Send_Byte(reg&0XFF);
    CT_IIC_Wait_Ack();
    for(i=0;i<len;i++)
    {
        CT_IIC_Send_Byte(buf[i]);
        ret=CT_IIC_Wait_Ack();
        if(ret)break;
    }
    CT_IIC_Stop();
    return ret;
#endif
}

 void GT9147_RD_Reg(u16 reg,u8 *buf,u8 len)
{
#if CTP_USE_HW_I2C
    HAL_StatusTypeDef ret;

    CTP_Debug_HwReadCount++;
    CTP_Debug_LastReg = reg;
    CTP_Debug_LastLen = len;
    ret = HAL_I2C_Mem_Read(&hi2c3, gt_cmd_wr, reg, I2C_MEMADD_SIZE_16BIT, buf, len, CTP_I2C_XFER_TIMEOUT_MS);
    CTP_Debug_HwReadRet = (u8)ret;
    CTP_Debug_HwError = HAL_I2C_GetError(&hi2c3);
    if(ret != HAL_OK) CTP_Debug_HwErrorCount++;
    CTP_Debug_RdAckWr = (ret == HAL_OK) ? 0 : 1;
    CTP_Debug_RdAckRegH = (ret == HAL_OK) ? 0 : 1;
    CTP_Debug_RdAckRegL = (ret == HAL_OK) ? 0 : 1;
    CTP_Debug_RdAckRd = (ret == HAL_OK) ? 0 : 1;
#else
    u8 i;
    CT_IIC_Start();
    CT_IIC_Send_Byte(gt_cmd_wr);
    CTP_Debug_RdAckWr = CT_IIC_Wait_Ack();
    CT_IIC_Send_Byte(reg>>8);
    CTP_Debug_RdAckRegH = CT_IIC_Wait_Ack();
    CT_IIC_Send_Byte(reg&0XFF);
    CTP_Debug_RdAckRegL = CT_IIC_Wait_Ack();
    CT_IIC_Start();
    CT_IIC_Send_Byte(gt_cmd_rd);
    CTP_Debug_RdAckRd = CT_IIC_Wait_Ack();
    for(i=0;i<len;i++)
    {
        buf[i]=CT_IIC_Read_Byte(i==(len-1)?0:1);
    }
    CT_IIC_Stop();
#endif
} 


void GTP_Touch_Down(int x,int y)
{  
			// printf("X=%d\n",input_x ); printf("Y=%d\r\n",input_y ); 
			Set_Color(color_white,color_black);
			  GUI_CircleFill (x,y,3);
			Display_Text(  240,  0,0,(uchar *)"DOWN");
}

 void GTP_Touch_Up(int x,int y)
{  
//printf("Release_X=%d\n",x ); printf("Release_Y=%d\r\n",y ); 
	
	Set_Color(color_white,color_red);
	GUI_CircleFill (x,y,3);
	Display_Text(  240,  0,0,(uchar *)"UP");
}
#define TOUCH_ADD	0x70 
void TOUCH_RdParFrPCTPFun(u8 *PCTP_Par,u8 ValFlag)
{
	int i=0;

		CT_IIC_Start();	
	CT_IIC_Send_Byte(TOUCH_ADD);
	CT_IIC_Wait_Ack();	
		CT_IIC_Send_Byte(0);
	CT_IIC_Wait_Ack();	
		CT_IIC_Start();
	CT_IIC_Send_Byte(TOUCH_ADD+1);
	CT_IIC_Wait_Ack();	
	for(i=0;i<ValFlag;i++)
	{
		if(i==(ValFlag-1))  *(PCTP_Par+i)=CT_IIC_Read_Byte(0);
		else                *(PCTP_Par+i)=CT_IIC_Read_Byte(1);
	}		
	CT_IIC_Stop();
}

char  Readtouch_CTP_FT(void) //��ȡ���������꣬���FTϵ�е���������ic
{
	 int input_x = 0;
    int input_y = 0;
	u8 buf[7];
TOUCH_RdParFrPCTPFun(buf, 7);	
	
			if((buf[3]&0X40)==0X00)
		{  

	 input_x= (u16)(buf[3] & 0x0F)<<8 | (u16)buf[4];
 input_y = (u16)(buf[5] & 0x0F)<<8 | (u16)buf[6];
			releasex=input_x;releasey=input_y;

			X0=  input_x;Y0=input_y;

				Touch_State=TOUCH_DOWN;		
		}

		if((buf[3]&0X40)==0X40)
		{
		if(Touch_State==TOUCH_DOWN)
		 {
		 Touch_State=TOUCH_UP;
		// GTP_Touch_Up(releasex,releasey);
		 
		 }


		 if(Touch_State==TOUCH_UP)
		 {
		// Touch_State=TOUCH_NC;
		 }


		}	
	
	return 0;
}
char  Readtouch_CTP_GT(void) //��ȡ���������꣬���GTϵ�е���������ic
{
    int input_x = 0;
    int input_y = 0;

	 u8 mode=0;
 	u8 buf[4]={0};

    if(!CTP_Ready)
    {
        Touch_State=TOUCH_NC;
        return 0;
    }
	u8 res=0;
	u8 temp;

	GT9147_RD_Reg(0X814E ,&mode,1);
	CTP_Debug_Mode=mode;// printf("%d\n",mode);//��ȡ�������״�? 
		
		if((mode&0XF)&&((mode&0XF)<2))
		{  

					GT9147_RD_Reg(GT9147_TPX_TBL[0],buf,4);	//��ȡXY����ֵ
	
            input_x  = (((u16)buf[1]<<8)+buf[0]);	//x����
            input_y  = (((u16)buf[3]<<8)+buf[2]);	//y����

			releasex=input_x;releasey=input_y;
			//if(X0!=0&Y0!=0)	
			//{ 
			X0=  input_x;Y0=input_y;
		//	}
          // GTP_Touch_Down(input_x, input_y);//���ݴ���

				Touch_State=TOUCH_DOWN;			
					
			
	
			res=1;

		}
		if(((mode&0X8F)==0X80) || ((mode&0X0F)==0X00))
		{
		if(Touch_State==TOUCH_DOWN)
		 {
		 Touch_State=TOUCH_UP;
		// GTP_Touch_Up(releasex,releasey);
		 
		 }


		 if(Touch_State==TOUCH_UP)
		 {
		// Touch_State=TOUCH_NC;
		 }


		}
 		if(mode&0X80&&((mode&0XF)<6))
		{
			temp=0;
			GT9147_WR_Reg(0X814E ,&temp,1);//����?		
		}

	return res;

}



