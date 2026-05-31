#include "sys.h"

 #define CMD_RDY 0X90  //0B10010000即用差分方式读X坐标
#define CMD_RDX	0XD0  //0B11010000即用差分方式读Y坐标  

#define PEN  PCin(1)   //PA0  INT
#define DOUT PAin(6)   //PA6  MISO
#define TDIN PAout(7)  //PA7 MOSI
#define TCLK PAout(5)  //PA5  SCLK
#define TCS  PAout(4)  //PA4  CS    
 #define SKIP_DIF 30
#define SAMPLES 10
#define CALSIZE  20//三个校准点距离屏幕边的距离像素点
 #define READ_TIMES 8 //读取次数
#define LOST_VAL 2	  //丢弃值
void TOUCH_IO_Init(void);
