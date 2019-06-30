//#ifndef _Use_LCD1602B
//# define _Use_LCD1602B

/***********************
*    ϵ ͳ �� �� ��    *
***********************/

/*---------------------*
*    �� �� �� �� ��    *
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
*    �� �� �� �� ��    *
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
*    ȫ�ֱ���������    *
***********************/
const char CHR[16] = {'0','1','2','3','4','5','6','7','8'
                     ,'9','a','b','c','d','e','f'};
extern unsigned int FlashTimeCounter;
extern unsigned int RunTimeCounter;
extern unsigned int FlashGIFStringCounter;
/***********************
*    ϵͳ����������    *
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
*  ����˵����LCD�����������ʱ����                      *
*  ���룺    ��Ҫ��ʱ�Ĵ��������                       *
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
*  ����˵����LCD����ָ��������ʱ����                    *
*  ���룺    ��Ҫ������ʱUs��                           *
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
*  ����˵����LCD��ʼ������                              *
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
*  ����˵������LCD����ָ���                          *
*  ���룺    ��Ҫ���͵�ָ��                             *
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
*  ����˵������LCD�������ݺ���                          *
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
*  ����˵�����ȴ�LCD����״̬����                        *
********************************************************/
void LCDWaitForReady(void)
{
#ifdef _Use_LCDOnlyWrite
	LCDDelayUs(30);
#else
	
	SetRead;
	SetCommand;
   
    LCD_E = Enable;   
    
	while (LCD_BF == Enable);   //RW=1,��PD7��Ϊ0��ʾ���У�
   
    LCD_E = Disable;
	
#endif
}

/********************************************************
*  ����˵����������ʾ���꺯��                           *
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
*  ����˵����LCD�ַ�����ʾ����                          *
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
*  ����˵������ֵ��ʾ����(HEX)                          *
*  ���룺    ��Ҫ��ʾ�����֣��޷��ų����Σ�             *
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
*  ����˵����������ĻЧ��                               *
*  ���룺    ��Ҫ�������ַ��� ���� λ��                 *
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
*  ����˵�����ַ������Ȳ��Ժ���                         *
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
*  ����˵������˸��ʾ����                               *
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
*  ����˵������ĻGIF����                                *
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



