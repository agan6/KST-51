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
	ConfigTimer0(1);  //����T0��ʱ1ms
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
  {                             //ÿ��200ms��δ��������״̬ʱ��
    flag200ms = 0;
    GetRealTime(&bufTime);    //��ȡ��ǰʱ��
    if (psec != bufTime.sec)  //��⵽ʱ���б仯ʱˢ����ʾ
    {
      if((mode != 2)&&(mode != 3))
			{
			  RefreshTimeShow();
			}
      psec = bufTime.sec;   //�õ�ǰֵ�����ϴ�����
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

/* �����������������ݼ���ִ����Ӧ�Ĳ�����keycode-�������� */
void KeyAction(unsigned char keycode)
{
  if(mode == 0)
	{
	  if  ((keycode>='0') && (keycode<='9'))  //�����в���Ӧ�ַ���
    {
    }
    else if (keycode == 0x26)  //���ϼ���������ǰ����λ��ֵ
    {
        IncSetTime();
    }
    else if (keycode == 0x28)  //���¼����ݼ���ǰ����λ��ֵ
    {
        DecSetTime();
    }
    else if (keycode == 0x25)  //������������л�����λ
    {
        LeftShiftTimeSet();
    }
    else if (keycode == 0x27)  //���Ҽ��������л�����λ
    {
        RightShiftTimeSet();
    }
    else if (keycode == 0x0D)  //�س�������������ģʽ/���õ�ǰ����ֵ
    {  
			if (setIndex == 0)  //����������״̬ʱ����������״̬
        {
            EnterTimeSet();
        }
        else                //�Ѵ�������״̬ʱ������ʱ�䲢�˳�����״̬
        {
            ExitTimeSet(1);
        }
		}
    else if (keycode == 0x1B)  //Esc����ȡ����ǰ����
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
    unsigned long tmp;  //��ʱ����
    
    tmp = 11059200 / 12;      //��ʱ������Ƶ��
    tmp = (tmp * ms) / 1000;  //��������ļ���ֵ
    tmp = 65536 - tmp;        //���㶨ʱ������ֵ
    tmp = tmp + 12;           //�����ж���Ӧ��ʱ��ɵ����
    T0RH = (unsigned char)(tmp>>8);  //��ʱ������ֵ���Ϊ�ߵ��ֽ�
    T0RL = (unsigned char)tmp;
    TMOD &= 0xF0;   //����T0�Ŀ���λ
    TMOD |= 0x01;   //����T0Ϊģʽ1
    TH0 = T0RH;     //����T0����ֵ
    TL0 = T0RL;
    ET0 = 1;        //ʹ��T0�ж�
    TR0 = 1;        //����T0
}

void InterruptTimer0() interrupt 1
{
 	  static unsigned char tmr200ms = 0;
	  static unsigned int tmr1000ms = 0;
    
    TH0 = T0RH;  //���¼�������ֵ
    TL0 = T0RL;
    KeyScan();   //����ɨ��
    tmr200ms++;
    if (tmr200ms >= 200)  //��ʱ200ms
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