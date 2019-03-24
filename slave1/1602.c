#include <reg52.h>
#include <intrins.h>
#define uchar unsigned char
#define uint unsigned int
//
typedef unsigned char  U8;       /* defined for unsigned 8-bits integer variable 	  无符号8位整型变量  */
typedef signed   char  S8;       /* defined for signed 8-bits integer variable		  有符号8位整型变量  */
typedef unsigned int   U16;      /* defined for unsigned 16-bits integer variable 	  无符号16位整型变量 */
typedef signed   int   S16;      /* defined for signed 16-bits integer variable 	  有符号16位整型变量 */
typedef unsigned long  U32;      /* defined for unsigned 32-bits integer variable 	  无符号32位整型变量 */
typedef signed   long  S32;      /* defined for signed 32-bits integer variable 	  有符号32位整型变量 */
typedef float          F32;      /* single precision floating point variable (32bits) 单精度浮点数（32位长度） */
typedef double         F64;      /* double precision floating point variable (64bits) 双精度浮点数（64位长度） */
//
#define uchar unsigned char
#define uint unsigned int
#define   Data_0_time    4

sbit  P2_0= P1^0 ;
sbit LCD_RS=P3^5;//定义引脚
sbit LCD_RW=P3^6;
sbit LCD_E=P3^7;
#define LCD_Data P2
#define Busy    0x80 //用于检测LCD状态字中的Busy标识

//DHT11
U8  U8FLAG,k;
U8  U8count,U8temp;
U8  U8T_data_H,U8T_data_L,U8RH_data_H,U8RH_data_L,U8checkdata;
U8  U8T_data_H_temp,U8T_data_L_temp,U8RH_data_H_temp,U8RH_data_L_temp,U8checkdata_temp;
U8  U8comdata;
U8  outdata[5];  //定义发送的字节数	   
U8  indata[5];
U8  count, count_r=0;
U8  str[5]={"RS232"};
U16 U16temp1,U16temp2;

/////////////////////////串口部分///////////////
uchar Uart_flag=0,recData=0;
void UartInit(void);
void Txd_Data(char Data);
//------------------串口通信协议-----------------//
/*
  客户端数据包格式解释(长度恒为13):
  例如:A1_T225H32X1#	 	  设备1的温度是22.5	湿度是32 烟雾有
  A	    	  -----数据包的开始标记(可以为A到Z,意味着数据包可以有26种)
  1		      -----设备代号

  指令指令的前1个人字符是指令头部，指令的后n（3、2、1）个字符是指令尾部
  T225        -----温度为22.5
  H32		  -----湿度是32
  X1		  -----烟雾有（无）
  #           -----数据包的结束标记
*/
#define DataPacketLength 13

#define deviceID '1'        //用于串口通信时，定义本地设备ID的第1位
#define datapackage_headflag 'A' //用于串口通信时，定义数据包头部的验证标记
uchar buf_string[DataPacketLength];  //定义数据包长度为DataPacketLength个字符
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
 Delay400Ms(); //启动等待，等LCD讲入工作状态
 LCDInit(); //LCM初始化
 UartInit();
 Delay5Ms(); //延时片刻(可不要)

 //DisplayListChar(0, 0, uctech);
 //DisplayListChar(0, 5, net);
 //ReadDataLCD();//测试用句无意义
 while(1){
	Delay400Ms();
	//Delay400Ms();
	//Delay400Ms();
	//Delay400Ms();
	//------------------------
   //调用温湿度读取子程序 
    RH();
   //读取模块数据周期不易小于 2S 
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

//写数据
void WriteDataLCD(unsigned char WDLCD)
{
	ReadStatusLCD(); //检测忙
	LCD_Data = WDLCD;
	LCD_RS = 1;
	LCD_RW = 0;
	LCD_E = 0; //若晶振速度太高可以在这后加小的延时
	LCD_E = 0; //延时
	LCD_E = 1;
}

//写指令
void WriteCommandLCD(unsigned char WCLCD,BuysC) //BuysC为0时忽略忙检测
{
	if (BuysC) ReadStatusLCD(); //根据需要检测忙
	LCD_Data = WCLCD;
	LCD_RS = 0;
	LCD_RW = 0; 
	LCD_E = 0;
	LCD_E = 0;
	LCD_E = 1; 
}

//读数据
unsigned char ReadDataLCD(void)
{
 LCD_RS = 1; 
 LCD_RW = 1;
 LCD_E = 0;
 LCD_E = 0;
 LCD_E = 1;
 return(LCD_Data);
}

//读状态
unsigned char ReadStatusLCD(void)
{
 LCD_Data = 0xFF; 
 LCD_RS = 0;
 LCD_RW = 1;
 LCD_E = 0;
 LCD_E = 0;
 LCD_E = 1;
 while (LCD_Data & Busy); //检测忙信号
 return(LCD_Data);
}

void LCDInit(void) //LCM初始化
{
 LCD_Data = 0;
 WriteCommandLCD(0x38,0); //三次显示模式设置，不检测忙信号
 Delay5Ms(); 
 WriteCommandLCD(0x38,0);
 Delay5Ms(); 
 WriteCommandLCD(0x38,0);
 Delay5Ms(); 

 WriteCommandLCD(0x38,1); //显示模式设置,开始要求每次检测忙信号
 WriteCommandLCD(0x08,1); //关闭显示
 WriteCommandLCD(0x01,1); //显示清屏
 WriteCommandLCD(0x06,1); // 显示光标移动设置
 WriteCommandLCD(0x0C,1); // 显示开及光标设置
}

//按指定位置显示一个字符
void DisplayOneChar(unsigned char X, unsigned char Y, unsigned char DData)
{
 Y &= 0x1;
 X &= 0xF; //限制X不能大于15，Y不能大于1
 if (Y) X |= 0x40; //当要显示第二行时地址码+0x40;
 X |= 0x80; // 算出指令码
 WriteCommandLCD(X, 0); //这里不检测忙信号，发送地址码
 WriteDataLCD(DData);
}

//按指定位置显示一串字符
void DisplayListChar(unsigned char X, unsigned char Y, unsigned char code *DData)
{
 unsigned char ListLength;

  ListLength = 0;
 Y &= 0x1;
 X &= 0xF; //限制X不能大于15，Y不能大于1
 while (DData[ListLength]>=0x20) //若到达字串尾则退出
  {
   if (X <= 0xF) //X坐标应小于0xF
    {
     DisplayOneChar(X, Y, DData[ListLength]); //显示单个字符
     ListLength++;
     X++;
    }
  }
}

//5ms延时
void Delay5Ms(void)
{
 unsigned int TempCyc = 5552;
 while(TempCyc--);
}

//400ms延时
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
	   	//超时则跳出for循环		  
	   	if(U8FLAG==1)break;
	   	//判断数据位是0还是1	 
	   	   
		// 如果高电平高过预定0高电平值则数据位为 1 
	   	 
		U8comdata<<=1;
	   	U8comdata|=U8temp;        //0
	}//rof
	   
}

	//--------------------------------
	//-----湿度读取子程序 ------------
	//--------------------------------
	//----以下变量均为全局变量--------
	//----温度高8位== U8T_data_H------
	//----温度低8位== U8T_data_L------
	//----湿度高8位== U8RH_data_H-----
	//----湿度低8位== U8RH_data_L-----
	//----校验 8位 == U8checkdata-----
	//----调用相关子程序如下----------
	//---- Delay();, Delay_10us();,COM(); 
	//--------------------------------

void RH(void)
{
  //主机拉低18ms 
   P2_0=0;
   Delay(180);
   P2_0=1;
 //总线由上拉电阻拉高 主机延时20us
   Delay_10us();
   Delay_10us();
   Delay_10us();
   Delay_10us();
 //主机设为输入 判断从机响应信号 
   P2_0=1;
 //判断从机是否有低电平响应信号 如不响应则跳出，响应则向下运行	  
   if(!P2_0)		 //T !	  
   {
   U8FLAG=2;
 //判断从机是否发出 80us 的低电平响应信号是否结束	 
   while((!P2_0)&&U8FLAG++);
   U8FLAG=2;
 //判断从机是否发出 80us 的高电平，如发出则进入数据接收状态
   while((P2_0)&&U8FLAG++);
 //数据接收状态		 
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
 //数据校验 
 
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
	SCON=0x50;   //串口方式1，使能接收
	TMOD=0x20;  //定时器1工作方式2（8位自动重装初值）
	
	TH1=0xfa;    //计数器初始值设置 9600bps
	TL1=0xfa;  
	PCON|=0x80;  //定义PCON寄存器中的SMOD=1,波特率加倍
	
	TR1=1;
	TI=0;
	RI=0;
	//PS=1;   //提高串口中断优先级
	ES=1;  //开启串口中断使能
	EA=1;
}
//
//void Txd_Data(char Data)    //串口发送数据
//{
//	SBUF = Data;
//	while(!TI);
//	TI=0;
//}

//串口发送函数
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
//串口接收函数
bit ReceiveString()    
{
    char * RecStr=buf_string;
    char num=0;
    unsigned char count=0;
    loop:    
	    *RecStr=SBUF;
	    count=0;
	    RI=0;    
	    if(num<7)  //数据包长度为8个字符,尝试连续接收8个字符
	    {
	        num++;
	        RecStr++;    
	        while(!RI)
	        {
	            count++;
	            if(count>130)return 0;    //接收数据等待延迟，等待时间太久会导致CPU运算闲置，太短会出现"数据包被分割",默认count=130
	        }
	        goto loop;
	    }
    return 1;
}
////比较指令头部
//bit CompareCMD_head(char CMD_head[])    
//{
//     unsigned char CharNum;
//     for(CharNum=0;CharNum<1;CharNum++)  //指令长度为8个字符
//     {
//         if(!(buf_string[CharNum+3]==CMD_head[CharNum]))
//         {
//             return 0;  //指令头部匹配失败
//         }
//     }
//     return 1;        //指令头部匹配成功
//}
////比较指令尾部(start:从哪里开始比较，quality:比较多少个字符，CMD_tail[]：要比较的字符串)
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

bit Deal_UART_RecData()   //处理串口接收数据包函数（成功处理数据包则返回1，否则返回0）
 {
     //PutString(buf_string);
     if(buf_string[0]==datapackage_headflag&&buf_string[7]=='#')  //进行数据包头尾标记验证
     {        
         switch(buf_string[1])        //识别发送者设备ID的第1位数字
         {
             case '1':
                 switch(buf_string[2])        //识别发送者设备ID的第2位数字
                 {
                     case '_':
                         if(buf_string[3]=='T')    //判断指令头部是否为'T'
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
                 switch(buf_string[2])        //识别发送者设备ID的第2位数字
                 {
                     case '_':
                         if(buf_string[3]=='T')    //判断指令头部是否为'T'
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
void ser() interrupt 4		//串口中断
{
	if(ReceiveString())    
    {
        //数据包长度正确则执行以下代码
        Deal_UART_RecData();   
    }
    else
    {
        //数据包长度错误则执行以下代码
        //LED1=~LED1;                
    }
    RI=0;  //接收并处理一次数据后把接收中断标志清除一下，拒绝响应在中断接收忙的时候发来的请求
}

