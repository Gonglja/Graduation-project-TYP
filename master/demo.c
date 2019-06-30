#include <intrins.h>
#include <reg52.h>
#define uchar unsigned char
#define uint unsigned int

//����ʹ��λ
sbit Slave1En = P1^3;
sbit Slave2En = P1^4;

//sbit SpeakEn1 = P1^5;
//sbit SpeakEn2 = P1^6;
//sbit SpeakEn3 = P1^7;
sbit LCD_RS=P1^2;//��������
sbit LCD_RW=P1^1;
sbit LCD_E=P1^0;
//sbit PSB =P3^6;		//PSB��Ϊ12864-12ϵ�еĴ�����ͨѶ�����л�������ʹ��8λ���нӿڣ�PSB=1
#define LCD_Data P2



#define Busy    0x80 //���ڼ��LCD״̬���е�Busy��ʶ
void WriteDataLCD(unsigned char WDLCD);
void WriteCommandLCD(unsigned char WCLCD,BuysC);
unsigned char ReadDataLCD(void);
unsigned char ReadStatusLCD(void);
void LCDInit(void);
void LCDClear(void);
void LCDFlash(void);
void DisplayOneChar(unsigned char X, unsigned char Y, unsigned char DData);
void DisplayListChar(unsigned char X, unsigned char Y, unsigned char code *DData);
//void DisplayImage (unsigned char code *DData);
void Delay5Ms(void);
void Delay400Ms(void);

/////////////////////////���ڲ���///////////////
uchar Uart_flag=0,recData=0;

void UartInit(void);
void Txd_Data(char Data);
void PutString(unsigned char *TXStr); 
//------------------����ͨ��Э��-----------------//
/*
  �ͻ������ݰ���ʽ����(���Ⱥ�Ϊ8):
  ����:A1_T225#	 	  �豸1���¶���22.5
  A	    	  -----���ݰ��Ŀ�ʼ���(����ΪA��Z,��ζ�����ݰ�������26��)
  1		      -----�豸����
  T225        --------ָ��(���Ⱥ�Ϊ4)��ָ���ǰ1�����ַ���ָ��ͷ����ָ��ĺ�3���ַ���ָ��β��
  #           ---------���ݰ��Ľ������

  �����������ݰ���ʽ����(���Ⱥ�Ϊ15):
  ����:A2_SenT010250#
  A--------���ݰ��Ŀ�ʼ���(����ΪA��Z,��ζ�����ݰ�������26��)
  02-----�豸����
  SenT010250--------ָ��(���Ⱥ�Ϊ10)��ָ���ǰ4�����ַ���ָ��ͷ����ָ��ĺ�6���ַ���ָ��β��
  #---------���ݰ��Ľ������
*/
#define DataPacketLength 13
uchar buf_string[DataPacketLength];  //�������ݰ�����Ϊ8���ַ�
#define deviceID '0'        //���ڴ���ͨ��ʱ�����屾���豸ID�ĵ�1λ
#define datapackage_headflag 'A' //���ڴ���ͨ��ʱ���������ݰ�ͷ������֤���

uchar DataPackage[DataPacketLength]={datapackage_headflag,deviceID,'_','T','X','X','X','H','X','X','S','X','#'};

uchar slave1_temp_disp[4]={'T','X','X','X'};
uchar slave2_temp_disp[4]={'T','X','X','X'};
uchar slave1_hum_disp[3]={'H','X','X'};
uchar slave2_hum_disp[3]={'H','X','X'};
uchar slave1_s_disp[2]={'S','X'};
uchar slave2_s_disp[2]={'S','X'};

//uchar code disp1[]={"λ��01�ļ������"};
//uchar code disp2[]={"λ��02�ļ������"};

/////////////////////////////////////////////////////////////////////////////////////////////

void main(void)
{
	//Delay400Ms(); //�����ȴ�����LCD���빤��״̬
	LCDInit(); //LCM��ʼ��
	Delay5Ms(); //��ʱƬ��(�ɲ�Ҫ)
	UartInit();
	Slave1En=1;
	Slave2En=0;

 	while(1)
	{

		WriteCommandLCD(0x80);	   //80 90 88 98
		WriteDataLCD(0x31);
		WriteCommandLCD(0x90);	   //80 90 88 98
		WriteDataLCD(slave1_temp_disp[0]);
		WriteDataLCD(':');
		WriteDataLCD(slave1_temp_disp[1]);
		WriteDataLCD(slave1_temp_disp[2]);
		WriteDataLCD('.');
		WriteDataLCD(slave1_temp_disp[3]);


		WriteDataLCD(' ');
		WriteDataLCD(slave1_hum_disp[0]);
		WriteDataLCD(':');
		WriteDataLCD(slave1_hum_disp[1]);
		WriteDataLCD(slave1_hum_disp[2]);
		WriteDataLCD(' ');
		WriteDataLCD(slave1_s_disp[0]);
		WriteDataLCD(':');
		WriteDataLCD(slave1_s_disp[1]);

		
		//DisplayListChar(0,3,disp2);
		WriteCommandLCD(0x88);	   //80 90 88 98
		WriteDataLCD(0x32);
		WriteCommandLCD(0x98);	   //80 90 88 98
		WriteDataLCD(slave2_temp_disp[0]);
		WriteDataLCD(':');
		WriteDataLCD(slave2_temp_disp[1]);
		WriteDataLCD(slave2_temp_disp[2]);
		WriteDataLCD('.');
		WriteDataLCD(slave2_temp_disp[3]);
		WriteDataLCD(' ');
		WriteDataLCD(slave2_hum_disp[0]);
		WriteDataLCD(':');
		WriteDataLCD(slave2_hum_disp[1]);
		WriteDataLCD(slave2_hum_disp[2]);
		WriteDataLCD(' ');
		WriteDataLCD(slave2_s_disp[0]);
		WriteDataLCD(':');
		WriteDataLCD(slave2_s_disp[1]);

	}
}

//д����
void WriteDataLCD(unsigned char WDLCD)
{
	ReadStatusLCD(); //���æ 
	LCD_RS = 1;
	LCD_RW = 0;
	LCD_Data = WDLCD;
	LCD_E = 1;
	LCD_E = 1;
	LCD_E = 1;
	LCD_E = 0;
}

//дָ��
void WriteCommandLCD(unsigned char WCLCD,BuysC) //BuysCΪ0ʱ����æ���
{
	if (BuysC) ReadStatusLCD(); //������Ҫ���æ 
	LCD_RS = 0;
	LCD_RW = 0; 
	LCD_Data = WCLCD;
	LCD_E = 1; 
	LCD_E = 1;
	LCD_E = 1;
	LCD_E = 0;  
}

//������
unsigned char ReadDataLCD(void)
{
	LCD_RS = 1; 
	LCD_RW = 1;
	LCD_E = 0;
	LCD_E = 0;
	LCD_E = 1;
	return(LCD_Data);
}

//��״̬
unsigned char ReadStatusLCD(void)
{
	LCD_Data = 0xFF; 
	LCD_RS = 0;
	LCD_RW = 1; 
	LCD_E = 1;
	while (LCD_Data & Busy); //���æ�ź�
	LCD_E = 0;
	return(LCD_Data);
}

void LCDInit(void) //LCM��ʼ��
{
	WriteCommandLCD(0x30,1); //��ʾģʽ����,��ʼҪ��ÿ�μ��æ�ź�
	WriteCommandLCD(0x01,1); //��ʾ����
	WriteCommandLCD(0x06,1); // ��ʾ����ƶ�����
	WriteCommandLCD(0x0C,1); // ��ʾ�����������
}
void LCDClear(void) //����
{
	WriteCommandLCD(0x01,1); //��ʾ����
	WriteCommandLCD(0x34,1); // ��ʾ����ƶ�����
	WriteCommandLCD(0x30,1); // ��ʾ�����������
}
void LCDFlash(void)	//��˸Ч��
{
	WriteCommandLCD(0x08,1); //��ʾ����
	Delay400Ms();
	WriteCommandLCD(0x0c,1); // ��ʾ�����������
	Delay400Ms();
	WriteCommandLCD(0x08,1); //��ʾ����
	Delay400Ms();
	WriteCommandLCD(0x0c,1); // ��ʾ�����������
	Delay400Ms();
	WriteCommandLCD(0x08,1); //��ʾ����
	Delay400Ms();
}
//��ָ��λ����ʾһ���ַ�
void DisplayOneChar(unsigned char X, unsigned char Y, unsigned char DData)
{
	if(Y<1)
	  Y=1;
	if(Y>4)
	  Y=4;
	  X &= 0x0F; //����X���ܴ���16��Y���ܴ���1
	switch(Y){
	case 1:X|=0X80;break;
	case 2:X|=0X90;break;
	case 3:X|=0X88;break;
	case 4:X|=0X98;break;
	}
	WriteCommandLCD(X, 0); //���ﲻ���æ�źţ����͵�ַ��
	WriteDataLCD(DData);
}

//��ָ��λ����ʾһ���ַ�
void DisplayListChar(unsigned char X, unsigned char Y, unsigned char code *DData)
{
    unsigned char ListLength,X2;
    ListLength = 0;
    X2=X;
    if(Y<1)
   	   Y=1;
    if(Y>4)
 	   Y=4;
    X &= 0x0F; //����X���ܴ���16��Y��1-4֮��
    switch(Y){
		case 1:X2|=0X80;break;	//����������ѡ����Ӧ��ַ
		case 2:X2|=0X90;break;
		case 3:X2|=0X88;break;
		case 4:X2|=0X98;break;
	}
  WriteCommandLCD(X2, 1); //���͵�ַ��
  while (DData[ListLength]>=0x20) //�������ִ�β���˳�
  {
     if (X <= 0x0F) //X����ӦС��0xF
     {
		WriteDataLCD(DData[ListLength]); //
		ListLength++;
		X++;
		Delay5Ms();
    }
  }
}

//ͼ����ʾ122*32
//void DisplayImage (unsigned char code *DData){
//	unsigned char x,y,i;
//	unsigned int tmp=0;
//	for(i=0;i<9;){		//���������ϰ������°�������Ϊ��ʼ��ַ��ͬ����Ҫ�ֿ�
//	for(x=0;x<32;x++){				//32��
//		WriteCommandLCD(0x34,1);
//		WriteCommandLCD((0x80+x),1);//�е�ַ
//		WriteCommandLCD((0x80+i),1);	//�е�ַ���°������������е�ַ0X88
//		WriteCommandLCD(0x30,1);		
//		for(y=0;y<16;y++)	
//			WriteDataLCD(DData[tmp+y]);//��ȡ����д��LCD
//		tmp+=16;		
//	}
//	i+=8;
//	}
//	WriteCommandLCD(0x36,1);	//���书���趨
//	WriteCommandLCD(0x30,1);
//}

//5ms��ʱ
void Delay5Ms(void)
{
 unsigned int TempCyc = 5552;
 while(TempCyc--);
}

//400ms��ʱ
void Delay400Ms(void)
{
 unsigned char TempCycA = 5;
 unsigned int TempCycB;
 while(TempCycA--)
 {
  TempCycB=7269;
  while(TempCycB--);
 };
}

void UartInit()
{
	SCON=0x50;   //���ڷ�ʽ1��ʹ�ܽ���
	TMOD=0x20;  //��ʱ��1������ʽ2��8λ�Զ���װ��ֵ��
	
	TH1=0xfd;    //��������ʼֵ���� 9600bps
	TL1=0xfa;  
	//PCON|=0x80;  //����PCON�Ĵ����е�SMOD=1,�����ʼӱ�
	SM0=0;
	SM1=1;

	TR1=1;
	TI=0;
	RI=0;
	//PS=1;   //��ߴ����ж����ȼ�
	ES=1;  //���������ж�ʹ��
	EA=1;
//    SCON=0x50; //���ڹ�����ʽ1��8λUART�������ʿɱ�  
//	TH2=0xFF;           
//	TL2=0xFD;    //������:115200 ����=11.0592MHz 
//	RCAP2H=0xFF;   
//	RCAP2L=0xFD; //16λ�Զ���װ��ֵ
//	
//	TCLK=1;   
//	RCLK=1;   
//	C_T2=0;   
//	EXEN2=0; //�����ʷ�����������ʽ
//
//    TR2=1 ; //��ʱ��2��ʼ            //
//    TI = 1;
//
//    REN=1;       //ʹ�ܴ����������
//    SM0=0;      //�����ڴ��ڵĹ�����ʽ1
//    SM1=1;
//    ES=1;       //�����жϹرգ�����ʼ�����ٴ�
//    EA=1;       //���жϴ�  
}
//
//void Txd_Data(char Data)    //���ڷ�������
//{
//	SBUF = Data;
//	while(!TI);
//	TI=0;
//}

//���ڷ��ͺ���
//void PutString(unsigned char *TXStr)  
//{                
//    ES=0;     
//    while(*TXStr!=0) 
//    {                      
//        SBUF=*TXStr;
//        while(TI==0);
//        TI=0;    
//        TXStr++;
//    }
//    ES=1; 
//}                                                     
//���ڽ��պ���
bit ReceiveString()    
{
    char * RecStr=buf_string;
    char num=0;
    unsigned char count=0;
    loop:    
	    *RecStr=SBUF;
	    count=0;
	    RI=0;    
	    if(num < DataPacketLength -1)  //���ݰ�����Ϊ8���ַ�,������������8���ַ�
	    {
	        num++;
	        RecStr++;    
	        while(!RI);
	        {
	           count++;
	           if(count>130)return 0;    //�������ݵȴ��ӳ٣��ȴ�ʱ��̫�ûᵼ��CPU�������ã�̫�̻����"���ݰ����ָ�",Ĭ��count=130
	        }
	        goto loop;
	    }
    return 1;
}
////�Ƚ�ָ��ͷ��
//bit CompareCMD_head(char CMD_head[])    
//{
//     unsigned char CharNum;
//     for(CharNum=0;CharNum<1;CharNum++)  //ָ���Ϊ8���ַ�
//     {
//         if(!(buf_string[CharNum+3]==CMD_head[CharNum]))
//         {
//             return 0;  //ָ��ͷ��ƥ��ʧ��
//         }
//     }
//     return 1;        //ָ��ͷ��ƥ��ɹ�
//}
////�Ƚ�ָ��β��(start:�����￪ʼ�Ƚϣ�quality:�Ƚ϶��ٸ��ַ���CMD_tail[]��Ҫ�Ƚϵ��ַ���)
//bit CompareCMD_tail(unsigned char start,unsigned char quality,char CMD_tail[]) 
//{
//     unsigned char CharNum;
//     for(CharNum=0;CharNum<quality;CharNum++)
//     {
//         if(!(buf_string[start+CharNum]==CMD_tail[CharNum]))
//         {
//             return 0; 
//         }
//     }
//     return 1;
// }

bit Deal_UART_RecData()   //�����ڽ������ݰ��������ɹ��������ݰ��򷵻�1�����򷵻�0��
 {
     //PutString(buf_string);
     if(buf_string[0]==datapackage_headflag&&buf_string[DataPacketLength -1]=='#')  //�������ݰ�ͷβ�����֤
     {        
         switch(buf_string[1])        //ʶ�������豸ID�ĵ�1λ����
         {
             case '1':
			 	{
                 switch(buf_string[2])        //ʶ�������豸ID�ĵ�2λ����
                 {
                     case '_':
                         if(buf_string[3]=='T')    //�ж�ָ��ͷ���Ƿ�Ϊ'T'
                         {
                         	 //temp =  buf_string[4]*100+buf_string[5]*10+buf_string[6]; 
                         	slave1_temp_disp[1]=buf_string[4];
							slave1_temp_disp[2]=buf_string[5];
							slave1_temp_disp[3]=buf_string[6];
						 }
                         if(buf_string[7]=='H')  
                         {
                             slave1_hum_disp[1]= buf_string[8]; 
							 slave1_hum_disp[2]= buf_string[9];           
                             
                         }
                         if(buf_string[10]=='S')  
                         {
                             slave1_s_disp[1]= buf_string[11]; 
                             
                         }
						 Slave1En=0;
				         Slave2En=1;
                         return 0;
                     default:
                         return 0;
                 }
				 
				 }
			 case '2':
			 {
                 switch(buf_string[2])        //ʶ�������豸ID�ĵ�2λ����
                 {
                     case '_':
                         if(buf_string[3]=='T')    //�ж�ָ��ͷ���Ƿ�Ϊ'T'
                         {
                         	slave2_temp_disp[1]=buf_string[4];
							slave2_temp_disp[2]=buf_string[5];
							slave2_temp_disp[3]=buf_string[6];
						 }
                         if(buf_string[7]=='H')  
                         {
                             slave2_hum_disp[1]= buf_string[8]; 
							 slave2_hum_disp[2]= buf_string[9];             
                             
                         }
                         if(buf_string[10]=='S')  
                         {
                            slave2_s_disp[1]= buf_string[11];  
                         }
						 Slave1En=1;
				         Slave2En=0;
                         return 0;
                     
                     default:
                         return 0;
                 }
				  
			}	  
             default:
                 return 0;
         }
     }
     return 0;
 }
void ser() interrupt 4		//�����ж�
{
	if(ReceiveString())    
    {
        //���ݰ�������ȷ��ִ�����´���
        Deal_UART_RecData();   
    }
    else
    {
        //���ݰ����ȴ�����ִ�����´���
        //LED1=~LED1;                
    }
    RI=0;  //���ղ�����һ�����ݺ�ѽ����жϱ�־���һ�£��ܾ���Ӧ���жϽ���æ��ʱ����������
}

