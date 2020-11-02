//********************************************************* 
/*	�ļ�����TEST_FT62F21x_IR_Receive.c
*	���ܣ�	 FT62F21x-������� ������ʾ
*	IC:		 FT62F211 SOP8
*	����   16M/4T                    
*	˵����   ��ʾ�����У�IR�����ǲ���6122Э�飬��ʼ�ź���9ms�͵�ƽ����4.5ms�ߵ�ƽ���ٵ���8λ
*           	 �û�ʶ���룬����8λ���û�ʶ���룬8λ�����룬8λ������ķ��롣RXIO��PA2��ÿ���յ�
*           	 ң���������������ݺ�������ݺϷ������Բ��룬���������жϣ���LED��PA4������״̬
*		    	 �͸ı�һ�Ρ�
*
*                  FT62F211 SOP8 
*                 ----------------
*  LED-----------|1(PA4)     (PA3)8|------------NC     
*  NC-----------|2(TKCAP)  (PA0)7|------------NC
*  VDD----------|3(VDD)     (PA1)6|------------NC
*  GND----------|4(VSS)     (PA2)5|------------IRRIO
*			      ----------------
*/
//*********************************************************
#include	"SYSCFG.h";
#include 	"FT62F21X.h";
//***********************�궨��*****************************
#define  uchar		unsigned char 
#define  uint			unsigned int
#define  ulong		unsigned long

#define  LED			RA4		//LEDָʾ�Ƶ�IO  
#define  IRRIO		RA2		//IR�Ľ��ս� 
 
uchar IRbitNum = 0;		    //���ڼ�¼���յ��ڼ�λ������
uchar IRbitTime = 0;			//���ڼ�ʱһλ��ʱ�䳤��
uchar IRDataTimer[4];		//�������4������
uchar bitdata=0x01;			//���ڰ�λ���λ����
uchar ReceiveFinish = 0;	//���ڼ�¼�������
uchar ReadAPin = 0;			//���ڶ�ȡIO��״̬����ƽ�仯�жϱ�־���
uchar rdata1,rdata2;
/*-------------------------------------------------
 *	��������interrupt ISR
 *	���ܣ� 	 ��ʱ��0�жϺ�PA��ƽ�仯�ж�
 *	���룺	 ��
 *	����� 	 ��
 --------------------------------------------------*/
void interrupt ISR(void)			//PIC_HI-TECHʹ��
{ 
   
  //��ʱ��0���жϴ���**********************
	if(T0IE && T0IF)					//104us
	{
		TMR0 = 140;                	//ע��:��TMR0���¸�ֵTMR0�����������ڲ��仯
		 
		T0IF = 0;
        IRbitTime++;
        if(IRbitTime > 50)
        {
        	T0IE = 0;
            IRbitTime = 0;
        }
	} 
    
    //PA��ƽ�仯�ж�**********************
	if(PAIE && PAIF)		
    {
		ReadAPin = PORTA; 			//��ȡPORTA������PAIF��־
		PAIF = 0; 
        if(IRRIO == 0)
        {
        	T0IE = 1;
        	if(IRbitTime > 21)
            {
            	IRDataTimer[0] = 0;
                IRDataTimer[1] = 0;
                IRDataTimer[2] = 0;
                IRDataTimer[3] = 0;
                IRbitNum = 0;
                bitdata = 0x00;
            }
            else if(IRbitTime > 3)
            {
            	IRDataTimer[IRbitNum-1] |= bitdata;
            }
            IRbitTime = 0;
            bitdata<<=1;
            if(bitdata == 0)
            {
            	bitdata = 0x01;
                IRbitNum++;
            }
            if(IRbitNum > 4)
            {
            	IRbitNum = 0;
                T0IE = 0;  
                ReceiveFinish = 1;		
            }

        }
	}
}
 /*-------------------------------------------------
 *	��������POWER_INITIAL
 *	���ܣ�  �ϵ�ϵͳ��ʼ��
 *	���룺  ��
 *	�����  ��
 --------------------------------------------------*/	
void POWER_INITIAL (void) 
{	
    OSCCON = 0B01110000;			//WDT 32KHZ IRCF=111=16MHZ/4=4MHZ,0.25US/T

	INTCON = 0;  							//�ݽ�ֹ�����ж�

	PORTA = 0B00000000;				
	TRISA = 0B00000100;				//PA������� 0-��� 1-����  
	WPUA = 0B00000100;     		//PA�˿��������� 1-������ 0-������

	OPTION = 0B00001000;			//Bit3=1 WDT MODE,PS=000=1:1 WDT RATE
													//Bit7(PAPU)=0 ��WPUA�����Ƿ�����
	MSCON = 0B00000000;		                             
}
/*----------------------------------------------------
 *	�������ƣ�TIMER0_INITIAL
 *	���ܣ���ʼ�����ö�ʱ��
 *	��ؼĴ�����T0CS T0CS T0SE PSA 
 *	����TMR0��ʱʱ��560us=(1/16000000)*4*16*140(16M-2T-PSA 1:16- TMR0=255���)	                      
 ----------------------------------------------------*/
void TIMER0_INITIAL (void)  
{
	OPTION = 0B00000011; 		//Ԥ��Ƶ�������Timer0��Ԥ��Ƶ��Ϊ1:16��������   
	//Bit5 T0CS Timer0ʱ��Դѡ�� 
	//1-�ⲿ���ŵ�ƽ�仯T0CKI 0-�ڲ�ʱ��(FOSC/2)
	//Bit4 T0CKI���Ŵ�����ʽ 1-�½��� 0-������
	//Bit3 PSA Ԥ��Ƶ������λ 0-Timer0 1-WDT 
	//Bit2:0 PS2 8��Ԥ��Ƶ�� 011 - 1:16
    						
	TMR0 = 140; 
    T0IF = 0;								//���T0�����ж�
}
/*-------------------------------------------------
 *	������: PA2_Level_Change_INITIAL
 *	���ܣ�  PA�˿�(PA2)��ƽ�仯�жϳ�ʼ��
 *	���룺  ��
 *	�����  ��
--------------------------------------------------*/
void PA2_Level_Change_INITIAL(void)
{ 
	TRISA2 =1; 			     //SET PA2 INPUT
	ReadAPin = PORTA;	 //��PA��ƽ�仯�ж�
	PAIF =0;   			     //��PA INT�жϱ�־λ
    IOCA2 =1;  			     //ʹ��PA2��ƽ�仯�ж�
	PAIE =1;   			     //ʹ��PA INT�ж�
    //GIE =1;    				 //ʹ��ȫ���ж�
}
/*-------------------------------------------------
 *	������: main 
 *	���ܣ�  ������
 *	���룺  ��
 *	�����  ��
 --------------------------------------------------*/
void main()
{
    
	POWER_INITIAL();						//ϵͳ��ʼ��
    TIMER0_INITIAL();
    PA2_Level_Change_INITIAL();		//��ʼ��PA�˿ڵ�ƽ�ж�
	GIE = 1;										//�����ж�
	while(1)
	{
		if(ReceiveFinish)
        {
        	ReceiveFinish = 0;
            rdata1 = 0xFF - IRDataTimer[0];
            rdata2 = 0xFF - IRDataTimer[2];
            if((rdata1 == IRDataTimer[1])&&(rdata2 == IRDataTimer[3]))
            {
            	LED = ~LED; 		//��ת��ƽ
            }
        }
	}
}