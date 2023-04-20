#include "main.h"

bit flag200ms = 1;
bit flag500ms = 0;
bit flag1s = 0;
bit flag18b20 = 0;
unsigned char T0RH = 0;
unsigned char T0RL = 0;
extern unsigned char setIndex;
unsigned char mode = 0;
unsigned char Temp_index = 0;
unsigned char flagtemp = 0;
static unsigned char i = 0;
unsigned char code LcdBuff[] = {
	'0','1','2','3','4','5','6','7','8','9',
};
unsigned char showbuf[3] = {
	'0','0','\0'
};
int tempbuf[10] = {
	0,0,0,0,0,0,0,0,0,0
};
unsigned char zhengshu[10];
unsigned char xiaoshu[10];

void timeworking();
void ConfigTimer0(unsigned int ms);
void tempworking();
void RefreshLED();
void StopLED();
void DS18B20_Read();

void main()
{
	EA = 1;
	ConfigTimer0(1);  //配置T0定时1ms
	InitDS1302();
	InitLcd1602();
	
	while(1)
	{
		KeyDriver();
		timeworking();
		tempworking();
		RefreshLED();
		StopLED();
	}
}

void timeworking()
{
	unsigned char psec = 0xAA;
	if (flag200ms && (setIndex == 0))
  {                             //每隔200ms且未处于设置状态时，
    flag200ms = 0;
    GetRealTime(&bufTime);    //获取当前时间
    if (psec != bufTime.sec)  //检测到时间有变化时刷新显示
    {
      if((mode != 2)&&(mode != 3))
			{
			  RefreshTimeShow();
			}
      psec = bufTime.sec;   //用当前值更新上次秒数
    }
  }
}

void tempworking()
{
	if((mode == 1)&&flag1s)
	{
		Temp_index++;
		DS18B20_Read();
		if(Temp_index >= 10)
		  {
			  mode = 2;
				Temp_index = 0;
			}
	}	
}

void RefreshLED()
{
	if((mode == 2)&&flag1s)
	{
		LcdClearScreen();
		RefreshTemp();
		LEDworking();
	}
	flag1s = 0;
}

void StopLED()
{
	if(mode == 3)
	{
		LEDnoworking();
	}
}

void DS18B20_Read()
{
	unsigned char buf[2] = {
		'0','\0'	
	};
	
	if(flag18b20 == 1)
	{
		EA = 0;
		Start18B20();
		Get18B20Temp(tempbuf+i);
		EA = 1;
		buf[0] = LcdBuff[i];
		i++;
	}
}

void DS18B20_Math()
{
	unsigned char i = 0;	

	for(i=0;i<=9;i++)
	{
		xiaoshu[i] = ((tempbuf[i] & 0xF)*100)/16;
		zhengshu[i] = tempbuf[i] >> 4;
	}
}

void DS18B20_Show(unsigned char i)
{		
	unsigned char pdata wendubuf[8] = {
		'0',' ','0','0','.','0','0','\0'
	};
	
	DS18B20_Math();
	
	wendubuf[0] =  LcdBuff[flagtemp];
	wendubuf[2] =  LcdBuff[(zhengshu[i]%100)/10];
	wendubuf[3] =  LcdBuff[zhengshu[i]%10];
	wendubuf[5] =  LcdBuff[(xiaoshu[i]%100)/10];
	wendubuf[6] =  LcdBuff[xiaoshu[i]%10];
	
	LcdWriteCmd(0x01);
	
	LcdShowStr(0,0,"0");
	LcdShowStr(1,0,wendubuf);
}

/* 按键动作函数，根据键码执行相应的操作，keycode-按键键码 */
void KeyAction(unsigned char keycode)
{
  if(mode == 0)
	{
	  if  ((keycode>='0') && (keycode<='9'))  //本例中不响应字符键
    {
    }
    else if (keycode == 0x26)  //向上键，递增当前设置位的值
    {
        IncSetTime();
    }
    else if (keycode == 0x28)  //向下键，递减当前设置位的值
    {
        DecSetTime();
    }
    else if (keycode == 0x25)  //向左键，向左切换设置位
    {
        LeftShiftTimeSet();
    }
    else if (keycode == 0x27)  //向右键，向右切换设置位
    {
        RightShiftTimeSet();
    }
    else if (keycode == 0x0D)  //回车键，进入设置模式/启用当前设置值
    {  
			if (setIndex == 0)  //不处于设置状态时，进入设置状态
        {
            EnterTimeSet();
        }
        else                //已处于设置状态时，保存时间并退出设置状态
        {
            ExitTimeSet(1);
        }
		}
    else if (keycode == 0x1B)  //Esc键，取消当前设置
    {
      ExitTimeSet(1);
		  mode = 1;
    }
	}
	if(mode == 2)
	{
		if(keycode == '1')
		{
			mode = 3;
			DS18B20_Show(flagtemp);
			if(flagtemp > 9)
			{
				flagtemp = 0;
			}
		}
	}
	if(mode == 3)
	{
		if(keycode == '1')
		{
			DS18B20_Show(flagtemp);
			flagtemp++;
			if(flagtemp >= 10)
			{
				flagtemp = 0;
			}
		}
		else if(keycode == '2')
		{
			i = 0;
			mode = 1;
			flagtemp = 0;
			LcdWriteCmd(0x01);
		}
	}
}

void ConfigTimer0(unsigned int ms)
{
    unsigned long tmp;  //临时变量
    
    tmp = 11059200 / 12;      //定时器计数频率
    tmp = (tmp * ms) / 1000;  //计算所需的计数值
    tmp = 65536 - tmp;        //计算定时器重载值
    tmp = tmp + 12;           //补偿中断响应延时造成的误差
    T0RH = (unsigned char)(tmp>>8);  //定时器重载值拆分为高低字节
    T0RL = (unsigned char)tmp;
    TMOD &= 0xF0;   //清零T0的控制位
    TMOD |= 0x01;   //配置T0为模式1
    TH0 = T0RH;     //加载T0重载值
    TL0 = T0RL;
    ET0 = 1;        //使能T0中断
    TR0 = 1;        //启动T0
}

void InterruptTimer0() interrupt 1
{
 	  static unsigned char tmr200ms = 0;
	  static unsigned int tmr1000ms = 0;
    
    TH0 = T0RH;  //重新加载重载值
    TL0 = T0RL;
    KeyScan();   //按键扫描
    tmr200ms++;
    if (tmr200ms >= 200)  //定时200ms
    {
        tmr200ms = 0;
        flag200ms = 1;
    }
		tmr1000ms++;
		if(tmr1000ms >= 1000)
		{
			tmr1000ms = 0;
			flag1s = 1;
			flag18b20 = 1;
		}
}