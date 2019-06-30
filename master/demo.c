#include <intrins.h>
#include <reg52.h>
#define uchar unsigned char
#define uint unsigned int

//定义使能位
sbit Slave1En = P1^3;
sbit Slave2En = P1^4;

//sbit SpeakEn1 = P1^5;
//sbit SpeakEn2 = P1^6;
//sbit SpeakEn3 = P1^7;
sbit LCD_RS=P1^2;//定义引脚
sbit LCD_RW=P1^1;
sbit LCD_E=P1^0;
//sbit PSB =P3^6;		//PSB脚为12864-12系列的串、并通讯功能切换，我们使用8位并行接口，PSB=1
#define LCD_Data P2



#define Busy    0x80 //用于检测LCD状态字中的Busy标识
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

/////////////////////////串口部分///////////////
uchar Uart_flag=0,recData=0;

void UartInit(void);
void Txd_Data(char Data);
void PutString(unsigned char *TXStr); 
//------------------串口通信协议-----------------//
/*
  客户端数据包格式解释(长度恒为8):
  例如:A1_T225#	 	  设备1的温度是22.5
  A	    	  -----数据包的开始标记(可以为A到Z,意味着数据包可以有26种)
  1		      -----设备代号
  T225        --------指令(长度恒为4)，指令的前1个人字符是指令头部，指令的后3个字符是指令尾部
  #           ---------数据包的结束标记

  服务器端数据包格式解释(长度恒为15):
  例如:A2_SenT010250#
  A--------数据包的开始标记(可以为A到Z,意味着数据包可以有26种)
  02-----设备代号
  SenT010250--------指令(长度恒为10)，指令的前4个人字符是指令头部，指令的后6个字符是指令尾部
  #---------数据包的结束标记
*/
#define DataPacketLength 13
uchar buf_string[DataPacketLength];  //定义数据包长度为8个字符
#define deviceID '0'        //用于串口通信时，定义本地设备ID的第1位
#define datapackage_headflag 'A' //用于串口通信时，定义数据包头部的验证标记

uchar DataPackage[DataPacketLength]={datapackage_headflag,deviceID,'_','T','X','X','X','H','X','X','S','X','#'};

uchar slave1_temp_disp[4]={'T','X','X','X'};
uchar slave2_temp_disp[4]={'T','X','X','X'};
uchar slave1_hum_disp[3]={'H','X','X'};
uchar slave2_hum_disp[3]={'H','X','X'};
uchar slave1_s_disp[2]={'S','X'};
uchar slave2_s_disp[2]={'S','X'};

//uchar code disp1[]={"位置01的监控如下"};
//uchar code disp2[]={"位置02的监控如下"};

/////////////////////////////////////////////////////////////////////////////////////////////

void main(void)
{
	//Delay400Ms(); //启动等待，等LCD讲入工作状态
	LCDInit(); //LCM初始化
	Delay5Ms(); //延时片刻(可不要)
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

//写数据
void WriteDataLCD(unsigned char WDLCD)
{
	ReadStatusLCD(); //检测忙 
	LCD_RS = 1;
	LCD_RW = 0;
	LCD_Data = WDLCD;
	LCD_E = 1;
	LCD_E = 1;
	LCD_E = 1;
	LCD_E = 0;
}

//写指令
void WriteCommandLCD(unsigned char WCLCD,BuysC) //BuysC为0时忽略忙检测
{
	if (BuysC) ReadStatusLCD(); //根据需要检测忙 
	LCD_RS = 0;
	LCD_RW = 0; 
	LCD_Data = WCLCD;
	LCD_E = 1; 
	LCD_E = 1;
	LCD_E = 1;
	LCD_E = 0;  
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
	LCD_E = 1;
	while (LCD_Data & Busy); //检测忙信号
	LCD_E = 0;
	return(LCD_Data);
}

void LCDInit(void) //LCM初始化
{
	WriteCommandLCD(0x30,1); //显示模式设置,开始要求每次检测忙信号
	WriteCommandLCD(0x01,1); //显示清屏
	WriteCommandLCD(0x06,1); // 显示光标移动设置
	WriteCommandLCD(0x0C,1); // 显示开及光标设置
}
void LCDClear(void) //清屏
{
	WriteCommandLCD(0x01,1); //显示清屏
	WriteCommandLCD(0x34,1); // 显示光标移动设置
	WriteCommandLCD(0x30,1); // 显示开及光标设置
}
void LCDFlash(void)	//闪烁效果
{
	WriteCommandLCD(0x08,1); //显示清屏
	Delay400Ms();
	WriteCommandLCD(0x0c,1); // 显示开及光标设置
	Delay400Ms();
	WriteCommandLCD(0x08,1); //显示清屏
	Delay400Ms();
	WriteCommandLCD(0x0c,1); // 显示开及光标设置
	Delay400Ms();
	WriteCommandLCD(0x08,1); //显示清屏
	Delay400Ms();
}
//按指定位置显示一个字符
void DisplayOneChar(unsigned char X, unsigned char Y, unsigned char DData)
{
	if(Y<1)
	  Y=1;
	if(Y>4)
	  Y=4;
	  X &= 0x0F; //限制X不能大于16，Y不能大于1
	switch(Y){
	case 1:X|=0X80;break;
	case 2:X|=0X90;break;
	case 3:X|=0X88;break;
	case 4:X|=0X98;break;
	}
	WriteCommandLCD(X, 0); //这里不检测忙信号，发送地址码
	WriteDataLCD(DData);
}

//按指定位置显示一串字符
void DisplayListChar(unsigned char X, unsigned char Y, unsigned char code *DData)
{
    unsigned char ListLength,X2;
    ListLength = 0;
    X2=X;
    if(Y<1)
   	   Y=1;
    if(Y>4)
 	   Y=4;
    X &= 0x0F; //限制X不能大于16，Y在1-4之内
    switch(Y){
		case 1:X2|=0X80;break;	//根据行数来选择相应地址
		case 2:X2|=0X90;break;
		case 3:X2|=0X88;break;
		case 4:X2|=0X98;break;
	}
  WriteCommandLCD(X2, 1); //发送地址码
  while (DData[ListLength]>=0x20) //若到达字串尾则退出
  {
     if (X <= 0x0F) //X坐标应小于0xF
     {
		WriteDataLCD(DData[ListLength]); //
		ListLength++;
		X++;
		Delay5Ms();
    }
  }
}

//图形显示122*32
//void DisplayImage (unsigned char code *DData){
//	unsigned char x,y,i;
//	unsigned int tmp=0;
//	for(i=0;i<9;){		//分两屏，上半屏和下半屏，因为起始地址不同，需要分开
//	for(x=0;x<32;x++){				//32行
//		WriteCommandLCD(0x34,1);
//		WriteCommandLCD((0x80+x),1);//列地址
//		WriteCommandLCD((0x80+i),1);	//行地址，下半屏，即第三行地址0X88
//		WriteCommandLCD(0x30,1);		
//		for(y=0;y<16;y++)	
//			WriteDataLCD(DData[tmp+y]);//读取数据写入LCD
//		tmp+=16;		
//	}
//	i+=8;
//	}
//	WriteCommandLCD(0x36,1);	//扩充功能设定
//	WriteCommandLCD(0x30,1);
//}

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

void UartInit()
{
	SCON=0x50;   //串口方式1，使能接收
	TMOD=0x20;  //定时器1工作方式2（8位自动重装初值）
	
	TH1=0xfd;    //计数器初始值设置 9600bps
	TL1=0xfa;  
	//PCON|=0x80;  //定义PCON寄存器中的SMOD=1,波特率加倍
	SM0=0;
	SM1=1;

	TR1=1;
	TI=0;
	RI=0;
	//PS=1;   //提高串口中断优先级
	ES=1;  //开启串口中断使能
	EA=1;
//    SCON=0x50; //串口工作方式1，8位UART，波特率可变  
//	TH2=0xFF;           
//	TL2=0xFD;    //波特率:115200 晶振=11.0592MHz 
//	RCAP2H=0xFF;   
//	RCAP2L=0xFD; //16位自动再装入值
//	
//	TCLK=1;   
//	RCLK=1;   
//	C_T2=0;   
//	EXEN2=0; //波特率发生器工作方式
//
//    TR2=1 ; //定时器2开始            //
//    TI = 1;
//
//    REN=1;       //使能串口允许接收
//    SM0=0;      //工作在串口的工作方式1
//    SM1=1;
//    ES=1;       //串口中断关闭，待初始化后再打开
//    EA=1;       //总中断打开  
}
//
//void Txd_Data(char Data)    //串口发送数据
//{
//	SBUF = Data;
//	while(!TI);
//	TI=0;
//}

//串口发送函数
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
	    if(num < DataPacketLength -1)  //数据包长度为8个字符,尝试连续接收8个字符
	    {
	        num++;
	        RecStr++;    
	        while(!RI);
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
     if(buf_string[0]==datapackage_headflag&&buf_string[DataPacketLength -1]=='#')  //进行数据包头尾标记验证
     {        
         switch(buf_string[1])        //识别发送者设备ID的第1位数字
         {
             case '1':
			 	{
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
                 switch(buf_string[2])        //识别发送者设备ID的第2位数字
                 {
                     case '_':
                         if(buf_string[3]=='T')    //判断指令头部是否为'T'
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

