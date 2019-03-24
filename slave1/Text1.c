//#ifndef _Use_LCD1602B
//# define _Use_LCD1602B

/***********************
*    系 统 宏 定 义    *
***********************/

/*---------------------*
*    常 数 宏 定 义    *
*---------------------*/
#ifndef True
    # define True           0x01
#endif
#ifndef False
    # define False          0x00
#endif

#ifndef Enable
    # define Enable         0x01
#endif
#ifndef Disable
    # define Disable        0x00
#endif

#ifndef RunStringSpeed
    # define RunStringSpeed 100
#endif

#ifndef FlashTimeOut
    # define FlashTimeOut   500
#endif
#ifndef FlashGIFTimeOut
    # define FlashGIFTimeOut  1000
#endif

# define LCD_Write          0x00
# define LCD_Read           0x01
# define LCD_Command        0x00
# define LCD_Data           0x01
# define LCD_CMD_Init       0x28
# define LCD_CMD_DispCtr    0x0c
# define LCD_CMD_CLS        0x01
# define LCD_CMD_EnterSet   0x06
# define LCD_CMD_IconShow   0x0f
# define LCD_CMD_IconHide   0x0c
# define LCD_CMD_NotMove    0b00010100
/*---------------------*
*    动 作 宏 定 义    *
*---------------------*/
# define SetReadState       LCD_SetReadData;LCD_RS = LCD_Command;LCD_RW = LCD_Read;
# define SetRead            LCD_SetReadData;LCD_RW = LCD_Read;
# define SetWrite           LCD_SetWriteData;LCD_RW = LCD_Write;
# define SetCommand         LCD_RS = LCD_Command;
# define SetData            LCD_RS = LCD_Data;

# define Print(a)           LCDDisplayString(a);
# define Locate(x,y)        LCDSetXY(x-1,y-1);
# define CLS                LCDWaitForReady();LCDSendCommand(LCD_CMD_CLS);
# define PrintN(a,b)        LCDDisplayNum((unsigned long)a,b);
# define ShowIcon           LCDWaitForReady();LCDSendCommand(LCD_CMD_IconShow);
# define HideIcon           LCDWaitForReady();LCDSendCommand(LCD_CMD_IconHide);
/***********************
*    全局变量声明区    *
***********************/
const char CHR[16] = {'0','1','2','3','4','5','6','7','8'
                     ,'9','a','b','c','d','e','f'};
extern unsigned int FlashTimeCounter;
extern unsigned int RunTimeCounter;
extern unsigned int FlashGIFStringCounter;
/***********************
*    系统函数声明区    *
***********************/
void LCDInit(void);
void LCDSendCommand(char Command);
void LCDSendData(char Data);
void LCDWaitForReady(void);
void LCDSetXY(char X,char Y);
void LCDDisplayString(char *String);
void LCDDisplayNum(unsigned long Num,char BitCount);
void LCDDelay(unsigned int Time);
void LCDDelayUs(unsigned int Time);
void RunString(char *String,char Direction,char Y,char StartX,char EndX);
void Flash(char *String,char Icon,char X,char Y);
char StringLength(char *String);
void FlashStringGroup(char String[][17],char StringCounter,char X,char Y);
/********************************************************
*  函数说明：LCD驱动类毫秒延时函数                      *
*  输入：    需要延时的大体毫秒数                       *
********************************************************/
void LCDDelay(unsigned int Time)
{
    unsigned int TimeCounter = 0;
	
	for (TimeCounter = 0;TimeCounter < Time;TimeCounter ++)
	{
	    LCDDelayUs(255);
	}
}

/********************************************************
*  函数说明：LCD驱动指令周期延时函数                    *
*  输入：    需要大体延时Us数                           *
********************************************************/
void LCDDelayUs(unsigned int Time)
{
     unsigned int TimeCounter = 0;
	 for (TimeCounter = 0;TimeCounter < Time;TimeCounter ++)
	 {
	     asm("nop");
	 }
}

/********************************************************
*  函数说明：LCD初始化函数                              *
********************************************************/
void LCDInit(void)
{
    LCDDelay(15);
	
	LCDWaitForReady();
	LCDSendCommand(LCD_CMD_Init);
	
	LCDWaitForReady();
	LCDSendCommand(LCD_CMD_DispCtr);
	
	LCDWaitForReady();
	LCDSendCommand(LCD_CMD_CLS);
	LCDDelay(2);
	LCDSendCommand(LCD_CMD_EnterSet);
	
}

/********************************************************
*  函数说明：向LCD发送指令函数                          *
*  输入：    需要发送的指令                             *
********************************************************/
void LCDSendCommand(char Command)
{
	SetWrite;
	SetCommand;
	{
	    LCD_E = Enable;
		
		LCD_SendHalfCharHigh(Command);
		
	    LCD_E = Disable;
	}
	{
	    LCD_E = Enable;
		
        LCD_SendHalfCharLow(Command);	
		
		LCD_E = Disable;
	}
    SetRead;		
	SetCommand;
}

/********************************************************
*  函数说明：向LCD发送数据函数                          *
********************************************************/
void LCDSendData(char Data)
{
    SetWrite;
	SetData;
	{
	    LCD_E = Enable;
		
		LCD_SendHalfCharHigh(Data);
		
	    LCD_E = Disable;
	}

	{
	    LCD_E = Enable;
		
        LCD_SendHalfCharLow(Data);	
		
		LCD_E = Disable;
	}
    SetRead;		
	SetCommand;
}

/********************************************************
*  函数说明：等待LCD空闲状态函数                        *
********************************************************/
void LCDWaitForReady(void)
{
#ifdef _Use_LCDOnlyWrite
	LCDDelayUs(30);
#else
	
	SetRead;
	SetCommand;
   
    LCD_E = Enable;   
    
	while (LCD_BF == Enable);   //RW=1,读PD7，为0表示空闲；
   
    LCD_E = Disable;
	
#endif
}

/********************************************************
*  函数说明：设置显示坐标函数                           *
********************************************************/
void LCDSetXY(char X,char Y)
{
    char Address;
    if (Y == 0)
	{
	    Address = 0x80 + X;
	}
    else
	{ 
        Address = 0xc0 + X;
	}
	
	LCDWaitForReady();
    LCDSendCommand(Address);
}

/********************************************************
*  函数说明：LCD字符串显示函数                          *
********************************************************/
void LCDDisplayString(char *String)
{
    while(*String)
	{
	    LCDWaitForReady();
	    LCDSendData(*String);
		String++;
	}
}

/********************************************************
*  函数说明：数值显示函数(HEX)                          *
*  输入：    需要显示的数字（无符号长整形）             *
********************************************************/
void LCDDisplayNum(unsigned long Num,char BitCount)
{
    char a = 0;
	for (a = 8-BitCount ;a<8;a++)
	{
	    LCDSendData(CHR[(Num<<(a<<2))>>28]);
	}
}

/********************************************************
*  函数说明：滚屏字幕效果                               *
*  输入：    需要滚屏的字符串 长度 位置                 *
********************************************************/
void RunString(char *String,char Direction,char Y,char StartX,char EndX)
{
    static char StringHead = 0;
	char SCREEN[17];
	char a = 0;
	char Point = StringHead;
	char StringLong = StringLength(String);
	static int RunTimeCounter = 0;
	
	for (a = 0;a<EndX - StartX + 1;a++)
	{
	     SCREEN[a] = String[Point];
		 Point ++;
		 if (Point == StringLong)
		 {
		     Point = 0;
		 }
	}
	
	for (;a < 17;a++)
	{
	    SCREEN[a] =' ';
	}
	
	RunTimeCounter ++;
	if (RunTimeCounter >RunStringSpeed)
	{
	    StringHead ++;
		RunTimeCounter = 0;
		if (StringHead == StringLong)
	    {
	        StringHead = 0;
	    }
	}
	
	Locate(StartX,Y)
	Print(SCREEN)
}

/********************************************************
*  函数说明：字符串长度测试函数                         *
********************************************************/
char StringLength(char *String)
{
    char n = 0;
	while (*String)
	{
	    n++;
		String ++;
	}
	
	return n;
}

/********************************************************
*  函数说明：闪烁显示函数                               *
********************************************************/
void Flash(char *String,char Icon,char X,char Y)
{
    char a = 0;
	char StringLong = StringLength(String);
	
    if (FlashTimeCounter % FlashTimeOut > (FlashTimeOut >> 1))
	{
	    Locate(X,Y)
		Print(String)    
	}
	else
	{
	    for (a = X ;a < X+StringLong;a++)
		{
		    Locate(a,Y)
			LCDWaitForReady();
			LCDSendData(Icon);
		}
	}
}

/********************************************************
*  函数说明：字幕GIF函数                                *
********************************************************/
void FlashStringGroup(char String[][17],char StringCounter,char X,char Y)
{
    static char Pictures = 0;
	static char DispState = 0;
    if (FlashGIFStringCounter % FlashGIFTimeOut > (FlashGIFTimeOut >> 1))
	{
	    if (DispState == 0)
		{
	        Pictures ++;
		    if (Pictures == StringCounter)
		    {
		        Pictures = 0; 
		    }
			DispState = 1;
		}
	}
	else
	{
	    if (DispState == 1)
		{
		    Pictures ++;
			if (Pictures == StringCounter)
			{
			    Pictures = 0;
			}
			DispState = 0;
		}
	}
	
	Locate(X,Y);
	Print(String[Pictures]);
}



