#include <reg52.h>
#include <intrins.h>
#define uchar unsigned char
#define uint unsigned int
//
typedef unsigned char  U8;       /* defined for unsigned 8-bits integer variable 	  �޷���8λ���ͱ���  */
typedef signed   char  S8;       /* defined for signed 8-bits integer variable		  �з���8λ���ͱ���  */
typedef unsigned int   U16;      /* defined for unsigned 16-bits integer variable 	  �޷���16λ���ͱ��� */
typedef signed   int   S16;      /* defined for signed 16-bits integer variable 	  �з���16λ���ͱ��� */
typedef unsigned long  U32;      /* defined for unsigned 32-bits integer variable 	  �޷���32λ���ͱ��� */
typedef signed   long  S32;      /* defined for signed 32-bits integer variable 	  �з���32λ���ͱ��� */
typedef float          F32;      /* single precision floating point variable (32bits) �����ȸ�������32λ���ȣ� */
typedef double         F64;      /* double precision floating point variable (64bits) ˫���ȸ�������64λ���ȣ� */
//
#define uchar unsigned char
#define uint unsigned int
#define   Data_0_time    4

sbit  P2_0= P1^0 ;
sbit LCD_RS=P3^5;//��������
sbit LCD_RW=P3^6;
sbit LCD_E=P3^7;
#define LCD_Data P2
#define Busy    0x80 //���ڼ��LCD״̬���е�Busy��ʶ

//DHT11
U8  U8FLAG,k;
U8  U8count,U8temp;
U8  U8T_data_H,U8T_data_L,U8RH_data_H,U8RH_data_L,U8checkdata;
U8  U8T_data_H_temp,U8T_data_L_temp,U8RH_data_H_temp,U8RH_data_L_temp,U8checkdata_temp;
U8  U8comdata;
U8  outdata[5];  //���巢�͵��ֽ���	   
U8  indata[5];
U8  count, count_r=0;
U8  str[5]={"RS232"};
U16 U16temp1,U16temp2;

/////////////////////////���ڲ���///////////////
uchar Uart_flag=0,recData=0;
void UartInit(void);
void Txd_Data(char Data);
//------------------����ͨ��Э��-----------------//
/*
  �ͻ������ݰ���ʽ����(���Ⱥ�Ϊ13):
  ����:A1_T225H32X1#	 	  �豸1���¶���22.5	ʪ����32 ������
  A	    	  -----���ݰ��Ŀ�ʼ���(����ΪA��Z,��ζ�����ݰ�������26��)
  1		      -----�豸����

  ָ��ָ���ǰ1�����ַ���ָ��ͷ����ָ��ĺ�n��3��2��1�����ַ���ָ��β��
  T225        -----�¶�Ϊ22.5
  H32		  -----ʪ����32
  X1		  -----�����У��ޣ�
  #           -----���ݰ��Ľ������
*/
#define DataPacketLength 13

#define deviceID '1'        //���ڴ���ͨ��ʱ�����屾���豸ID�ĵ�1λ
#define datapackage_headflag 'A' //���ڴ���ͨ��ʱ���������ݰ�ͷ������֤���
uchar buf_string[DataPacketLength];  //�������ݰ�����ΪDataPacketLength���ַ�
uchar DataPackage[DataPacketLength]={datapackage_headflag,deviceID,'_','T','X','X','X','H','X','X','S','X','#'};


uchar slave1_temp_disp[4]={'T','X','X','X'};
uchar slave2_temp_disp[4]={'T','X','X','X'};
uchar slave1_hum_disp[3]={'H','X','X'};
uchar slave2_hum_disp[3]={'H','X','X'};
uchar slave1_s_disp[2]={'H','X'};
uchar slave2_s_disp[2]={'H','X'};

void PutString(unsigned char *TXStr); 




void WriteDataLCD(unsigned char WDLCD);
void WriteCommandLCD(unsigned char WCLCD,BuysC);
unsigned char ReadDataLCD(void);
unsigned char ReadStatusLCD(void);
void LCDInit(void);
void DisplayOneChar(unsigned char X, unsigned char Y, unsigned char DData);
void DisplayListChar(unsigned char X, unsigned char Y, unsigned char code *DData);
void Delay5Ms(void);
void Delay400Ms(void);
void Delay(U16 j);
void Delay_10us(void);
void RH(void);

unsigned char code uctech[] = {"ic2012.taobao.co"};
unsigned char code net[] = {"Happy every day!"};

void main(void)
{
 Delay400Ms(); //�����ȴ�����LCD���빤��״̬
 LCDInit(); //LCM��ʼ��
 UartInit();
 Delay5Ms(); //��ʱƬ��(�ɲ�Ҫ)

 //DisplayListChar(0, 0, uctech);
 //DisplayListChar(0, 5, net);
 //ReadDataLCD();//�����þ�������
 while(1){
	Delay400Ms();
	//Delay400Ms();
	//Delay400Ms();
	//Delay400Ms();
	//------------------------
   //������ʪ�ȶ�ȡ�ӳ��� 
    RH();
   //��ȡģ���������ڲ���С�� 2S 
    DisplayOneChar(1,1,'H');
	DisplayOneChar(2,1,':');
    DisplayOneChar(3,1,U8RH_data_H%100/10+0x30);
	DisplayOneChar(4,1,U8RH_data_H%10+0x30);
	//DisplayOneChar(2,0,U8RH_data_L%100/10+0x30);
	//DisplayOneChar(3,0,U8RH_data_L%10+0x30);
	DisplayOneChar(6,1,'T');
	DisplayOneChar(7,1,':');
	DisplayOneChar(8,1,U8T_data_H%100/10+0x30); 
	DisplayOneChar(9,1,U8T_data_H%10+0x30); 
	DisplayOneChar(10,1,'.');
	//DisplayOneChar(11,0,U8T_data_L%100/10+0x30);
	DisplayOneChar(11,1,U8T_data_L%10+0x30);
	//DisplayOneChar(8,0,U8checkdata);
	DisplayOneChar(13,1,'S');
	DisplayOneChar(14,1,':');
	DisplayOneChar(15,1,'X');
	DataPackage[3] =  'T';
	DataPackage[4] =  U8T_data_H%100/10+0x30;
	DataPackage[5] =  U8T_data_H%10+0x30;
	DataPackage[6] =  U8T_data_L%10+0x30;

	DataPackage[7] =  'H';
	DataPackage[8] =  U8RH_data_H%100/10+0x30;
	DataPackage[9] =  U8RH_data_H%10+0x30;

    DataPackage[10]=  'S';
    DataPackage[11]=  '1';
	PutString(DataPackage);
	Delay(10000);
 
 
 };
}

//д����
void WriteDataLCD(unsigned char WDLCD)
{
	ReadStatusLCD(); //���æ
	LCD_Data = WDLCD;
	LCD_RS = 1;
	LCD_RW = 0;
	LCD_E = 0; //�������ٶ�̫�߿���������С����ʱ
	LCD_E = 0; //��ʱ
	LCD_E = 1;
}

//дָ��
void WriteCommandLCD(unsigned char WCLCD,BuysC) //BuysCΪ0ʱ����æ���
{
	if (BuysC) ReadStatusLCD(); //������Ҫ���æ
	LCD_Data = WCLCD;
	LCD_RS = 0;
	LCD_RW = 0; 
	LCD_E = 0;
	LCD_E = 0;
	LCD_E = 1; 
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
 LCD_E = 0;
 LCD_E = 0;
 LCD_E = 1;
 while (LCD_Data & Busy); //���æ�ź�
 return(LCD_Data);
}

void LCDInit(void) //LCM��ʼ��
{
 LCD_Data = 0;
 WriteCommandLCD(0x38,0); //������ʾģʽ���ã������æ�ź�
 Delay5Ms(); 
 WriteCommandLCD(0x38,0);
 Delay5Ms(); 
 WriteCommandLCD(0x38,0);
 Delay5Ms(); 

 WriteCommandLCD(0x38,1); //��ʾģʽ����,��ʼҪ��ÿ�μ��æ�ź�
 WriteCommandLCD(0x08,1); //�ر���ʾ
 WriteCommandLCD(0x01,1); //��ʾ����
 WriteCommandLCD(0x06,1); // ��ʾ����ƶ�����
 WriteCommandLCD(0x0C,1); // ��ʾ�����������
}

//��ָ��λ����ʾһ���ַ�
void DisplayOneChar(unsigned char X, unsigned char Y, unsigned char DData)
{
 Y &= 0x1;
 X &= 0xF; //����X���ܴ���15��Y���ܴ���1
 if (Y) X |= 0x40; //��Ҫ��ʾ�ڶ���ʱ��ַ��+0x40;
 X |= 0x80; // ���ָ����
 WriteCommandLCD(X, 0); //���ﲻ���æ�źţ����͵�ַ��
 WriteDataLCD(DData);
}

//��ָ��λ����ʾһ���ַ�
void DisplayListChar(unsigned char X, unsigned char Y, unsigned char code *DData)
{
 unsigned char ListLength;

  ListLength = 0;
 Y &= 0x1;
 X &= 0xF; //����X���ܴ���15��Y���ܴ���1
 while (DData[ListLength]>=0x20) //�������ִ�β���˳�
  {
   if (X <= 0xF) //X����ӦС��0xF
    {
     DisplayOneChar(X, Y, DData[ListLength]); //��ʾ�����ַ�
     ListLength++;
     X++;
    }
  }
}

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

///////////////////////////////////////
void Delay(U16 j)
{     U8 i;
	  for(;j>0;j--)
	  { 	
		 for(i=0;i<27;i++);
	  }
}

void  Delay_10us(void)
{
	U8 i;
	i--;
	i--;
	i--;
	i--;
	i--;
	i--;
}
void COM(void)
{
     U8 i;     
     for(i=0;i<8;i++)	   
	 {
		
	   	U8FLAG=2;	
	   	while((!P2_0)&&U8FLAG++);
		Delay_10us();
	    Delay_10us();
		Delay_10us();
	  	U8temp=0;
	    if(P2_0)U8temp=1;
		   U8FLAG=2;
		while((P2_0)&&U8FLAG++);
	   	//��ʱ������forѭ��		  
	   	if(U8FLAG==1)break;
	   	//�ж�����λ��0����1	 
	   	   
		// ����ߵ�ƽ�߹�Ԥ��0�ߵ�ƽֵ������λΪ 1 
	   	 
		U8comdata<<=1;
	   	U8comdata|=U8temp;        //0
	}//rof
	   
}

	//--------------------------------
	//-----ʪ�ȶ�ȡ�ӳ��� ------------
	//--------------------------------
	//----���±�����Ϊȫ�ֱ���--------
	//----�¶ȸ�8λ== U8T_data_H------
	//----�¶ȵ�8λ== U8T_data_L------
	//----ʪ�ȸ�8λ== U8RH_data_H-----
	//----ʪ�ȵ�8λ== U8RH_data_L-----
	//----У�� 8λ == U8checkdata-----
	//----��������ӳ�������----------
	//---- Delay();, Delay_10us();,COM(); 
	//--------------------------------

void RH(void)
{
  //��������18ms 
   P2_0=0;
   Delay(180);
   P2_0=1;
 //������������������ ������ʱ20us
   Delay_10us();
   Delay_10us();
   Delay_10us();
   Delay_10us();
 //������Ϊ���� �жϴӻ���Ӧ�ź� 
   P2_0=1;
 //�жϴӻ��Ƿ��е͵�ƽ��Ӧ�ź� �粻��Ӧ����������Ӧ����������	  
   if(!P2_0)		 //T !	  
   {
   U8FLAG=2;
 //�жϴӻ��Ƿ񷢳� 80us �ĵ͵�ƽ��Ӧ�ź��Ƿ����	 
   while((!P2_0)&&U8FLAG++);
   U8FLAG=2;
 //�жϴӻ��Ƿ񷢳� 80us �ĸߵ�ƽ���緢����������ݽ���״̬
   while((P2_0)&&U8FLAG++);
 //���ݽ���״̬		 
   COM();
   U8RH_data_H_temp=U8comdata;
   COM();
   U8RH_data_L_temp=U8comdata;
   COM();
   U8T_data_H_temp=U8comdata;
   COM();
   U8T_data_L_temp=U8comdata;
   COM();
   U8checkdata_temp=U8comdata;
   P2_0=1;
 //����У�� 
 
   U8temp=(U8T_data_H_temp+U8T_data_L_temp+U8RH_data_H_temp+U8RH_data_L_temp);
   if(U8temp==U8checkdata_temp)
   {
   	  U8RH_data_H=U8RH_data_H_temp;
   	  U8RH_data_L=U8RH_data_L_temp;
	  U8T_data_H=U8T_data_H_temp;
   	  U8T_data_L=U8T_data_L_temp;
   	  U8checkdata=U8checkdata_temp;
    }//fi
	}//fi

}
	


void UartInit()
{
	SCON=0x50;   //���ڷ�ʽ1��ʹ�ܽ���
	TMOD=0x20;  //��ʱ��1������ʽ2��8λ�Զ���װ��ֵ��
	
	TH1=0xfa;    //��������ʼֵ���� 9600bps
	TL1=0xfa;  
	PCON|=0x80;  //����PCON�Ĵ����е�SMOD=1,�����ʼӱ�
	
	TR1=1;
	TI=0;
	RI=0;
	//PS=1;   //��ߴ����ж����ȼ�
	ES=1;  //���������ж�ʹ��
	EA=1;
}
//
//void Txd_Data(char Data)    //���ڷ�������
//{
//	SBUF = Data;
//	while(!TI);
//	TI=0;
//}

//���ڷ��ͺ���
void PutString(unsigned char *TXStr)  
{                
    ES=0;     
    while(*TXStr!=0) 
    {                      
        SBUF=*TXStr;
        while(TI==0);
        TI=0;    
        TXStr++;
    }
    ES=1; 
}                                                     
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
	    if(num<7)  //���ݰ�����Ϊ8���ַ�,������������8���ַ�
	    {
	        num++;
	        RecStr++;    
	        while(!RI)
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
     if(buf_string[0]==datapackage_headflag&&buf_string[7]=='#')  //�������ݰ�ͷβ�����֤
     {        
         switch(buf_string[1])        //ʶ�������豸ID�ĵ�1λ����
         {
             case '1':
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
                         if(buf_string[3]=='H')  
                         {
                                         
                             
                         }
                         if(buf_string[3]=='Q')  
                         {
                             
                             
                         }
                         if(buf_string[3]=='W')  
                         {
                             
                             
                         }
                         return 0;
                     
                     default:
                         return 0;
                 }

			 case '2':
                 switch(buf_string[2])        //ʶ�������豸ID�ĵ�2λ����
                 {
                     case '_':
                         if(buf_string[3]=='T')    //�ж�ָ��ͷ���Ƿ�Ϊ'T'
                         {
                         	 //temp =  buf_string[4]*100+buf_string[5]*10+buf_string[6]; 
                         	slave2_temp_disp[1]=buf_string[4];
							slave2_temp_disp[2]=buf_string[5];
							slave2_temp_disp[3]=buf_string[6];
						 }
                         if(buf_string[3]=='H')  
                         {
                                         
                             
                         }
                         if(buf_string[3]=='Q')  
                         {
                             
                             
                         }
                         if(buf_string[3]=='W')  
                         {
                             
                             
                         }
                         return 0;
                     
                     default:
                         return 0;
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

