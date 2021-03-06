//********************************************************* 
/* 文件名：Test_62F21X_io.c
*	 功能：  FT62F21X-IO功能演示
*   IC:    FT62F21X SOP8
*   晶振：  16M/4T                    
*   说明：  当DemoPortIn悬空或者高电平时,
*		   DemoPortOut输出50Hz占空比50%的波形
*		   当DemoPortIn接地时,DemoPortOut输出高电平
* Memory: Flash 1KX14b, EEPROM 128X8b, SRAM 64X8b
*                  							FT62F21X SOP8 
*                 								----------------
*  DemoPortOut ------------|1(PA4)    		 	(PA3)8 |-------------NC 
*  NC------------|2(TKCAP)   	 					(PA0)7 |-------------NC
*  NC------------|3(VDD)								(PA1)6 |-------------NC 
*  NC------------|4(VSS)   	 						(PA2)5	|-------------DemoPortIn
*			     								 ----------------
*/
//===========================================================
//===========================================================
#include	"SYSCFG.h";
#include 	"FT62F21X.h";

#define  DemoPortOut	RA4   
#define  DemoPortIn		RA2

//===========================================================
//
//	系统时钟
//===========================================================
#define OSC_16M  0X70
#define OSC_8M   0X60
#define OSC_4M   0X50
#define OSC_2M   0X40
#define OSC_1M   0X30
#define OSC_500K 0X20
#define OSC_250K 0X10
#define OSC_32K  0X00

//===========================================================
//变量定义
//===========================================================

/*-------------------------------------------------
 * 函数名: nterrupt ISR 
 *	功能：  中断服务函数
 * 输入：  无
 * 输出：  无
 --------------------------------------------------*/
void interrupt ISR(void)
{
    
}

/*-------------------------------------------------
 * 函数名称：DelayUs
 * 功能：   短延时函数 --16M-4T--大概快1%左右.
 * 输入参数：Time延时时间长度 延时时长Time*2Us
 * 返回参数：无 
 -------------------------------------------------*/
void DelayUs(unsigned char Time)
{
	unsigned char a;
	for(a=0;a<Time;a++)
	{
		NOP();
	}
} 

/*------------------------------------------------- 
 * 函数名称：DelayMs
 * 功能：   短延时函数
 * 输入参数：Time延时时间长度 延时时长Time ms
 * 返回参数：无 
 -------------------------------------------------*/
void DelayMs(unsigned char Time)
{
	unsigned char a,b;
	for(a=0;a<Time;a++)
	{
		for(b=0;b<5;b++)
		{
		 	DelayUs(98); //快1%
		}
	}
}

/*------------------------------------------------- 
 * 函数名称：DelayS
 * 功能：   短延时函数
 * 输入参数：Time 延时时间长度 延时时长Time S
 * 返回参数：无 
 -------------------------------------------------*/
void DelayS(unsigned char Time)
{
	unsigned char a,b;
	for(a=0;a<Time;a++)
	{
		for(b=0;b<10;b++)
		{
		 	DelayMs(100); 
		}
	}
}

/*-------------------------------------------------
 * 函数名：POWER_INITIAL
 *	功能：  上电系统初始化
 * 输入：  无
 * 输出：  无
 --------------------------------------------------*/	
void POWER_INITIAL (void) 
{ 
	OSCCON = OSC_16M;	//	bit7	Timer2 选择LIRC为时钟源时 LIRC的频率选择  0:32KHz	1:256KHz
										//bit[6:4]	系统频率选择	
										//bit[2]		高速内部时钟状态	1:ready	0:not ready
										//bit[1]		低速内部时钟状态	1:ready	0:not ready

	INTCON = 0;  				//暂禁止所有中断
	OPTION = 0;
    TRISA	= 1<<2;			//1:输入	0:输出			PA2为输入模式	PA4输出模式
    PSRCA 	= 0;					//00:	4mA		01/10:	8mA		11:	28mA		bit[3:2]控制PA5源电流	bit[1:0]控制PA4源电流
    PSINKA	= 0;					//bit[1:0]	控制PA5和PA4		0:灌电流最小	1:灌电流最大
    PORTA 	= 0;					//1:PAx输出高电平	0:PAx输出低电平	PA4输出低电平
 	WPUA 	= 1<<2;			//1:	使能PA口上拉	0:关闭PA口上拉   	PA2 上拉
}
/*-------------------------------------------------
 *  函数名: main 
 *	功能：  主函数
 *  输入：  无
 *  输出：  无
 --------------------------------------------------*/
void main()
{
 	POWER_INITIAL();				//系统初始化
    
	while(1)
	{
		DemoPortOut = 1; 			
		DelayMs(10);     				//10ms
		if(DemoPortIn == 1) 		//判断输入是否为高电平 
		{
			DemoPortOut = 0;
		}
		DelayMs(10); 
	}   
}
//===========================================================
