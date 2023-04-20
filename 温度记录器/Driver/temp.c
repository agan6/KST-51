#include "config.h"
#include "ds18b20.h"
#include "lcd1602.h"

unsigned char IntToString(unsigned char *str, int dat);

void RefreshTemp()
{
      bit res;  
	    int temp;        //��ȡ���ĵ�ǰ�¶�ֵ
      int intT, decT,decTH,decTL;  //�¶�ֵ��������С������
      unsigned char len;
      unsigned char str[10];
           
      res = Get18B20Temp(&temp);  //��ȡ��ǰ�¶�
      if (res)                    //��ȡ�ɹ�ʱ��ˢ�µ�ǰ�¶���ʾ
      {
        intT = temp >> 4;             //������¶�ֵ��������
        decT = temp & 0xF;            //������¶�ֵС������
        len = IntToString(str, intT); //��������ת��Ϊ�ַ���
        str[len++] = '.';             //���С����
        decTH = (decT*10) / 16;        //�����Ƶ�С������ת��Ϊ1λʮ����λ
        str[len++] = decTH + '0';      //ʮ����С��λ��ת��ΪASCII�ַ�
				decTL = (decT*100) / 16 % 10;        
        str[len++] = decTL + '0';
        while (len < 8)               //�ÿո��뵽6���ַ�����
        {
          str[len++] = ' ';
        }
        str[len] = '\0';              //����ַ���������
        LcdShowStr(3, 0, str);        //��ʾ��Һ������
       }
        else
			 {
          LcdShowStr(3, 0, "error!");
        }
        Start18B20();               //����������һ��ת��       
}

unsigned char IntToString(unsigned char *str, int dat)
{
    signed char i = 0;
    unsigned char len = 0;
    unsigned char buf[6];
    
    if (dat < 0)  //���Ϊ����������ȡ����ֵ������ָ������Ӹ���
    {
        dat = -dat;
        *str++ = '-';
        len++;
    }
    do {          //��ת��Ϊ��λ��ǰ��ʮ��������
        buf[i++] = dat % 10;
        dat /= 10;
    } while (dat > 0);
    len += i;     //i����ֵ������Ч�ַ��ĸ���
    while (i-- > 0)   //������ֵת��ΪASCII�뷴�򿽱�������ָ����
    {
        *str++ = buf[i] + '0';
    }
    *str = '\0';  //����ַ���������
    
    return len;   //�����ַ�������
}