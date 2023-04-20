#include "config.h"
#include "ds18b20.h"
#include "lcd1602.h"

unsigned char IntToString(unsigned char *str, int dat);

void RefreshTemp()
{
      bit res;  
	    int temp;        //读取到的当前温度值
      int intT, decT,decTH,decTL;  //温度值的整数和小数部分
      unsigned char len;
      unsigned char str[10];
           
      res = Get18B20Temp(&temp);  //读取当前温度
      if (res)                    //读取成功时，刷新当前温度显示
      {
        intT = temp >> 4;             //分离出温度值整数部分
        decT = temp & 0xF;            //分离出温度值小数部分
        len = IntToString(str, intT); //整数部分转换为字符串
        str[len++] = '.';             //添加小数点
        decTH = (decT*10) / 16;        //二进制的小数部分转换为1位十进制位
        str[len++] = decTH + '0';      //十进制小数位再转换为ASCII字符
				decTL = (decT*100) / 16 % 10;        
        str[len++] = decTL + '0';
        while (len < 8)               //用空格补齐到6个字符长度
        {
          str[len++] = ' ';
        }
        str[len] = '\0';              //添加字符串结束符
        LcdShowStr(3, 0, str);        //显示到液晶屏上
       }
        else
			 {
          LcdShowStr(3, 0, "error!");
        }
        Start18B20();               //重新启动下一次转换       
}

unsigned char IntToString(unsigned char *str, int dat)
{
    signed char i = 0;
    unsigned char len = 0;
    unsigned char buf[6];
    
    if (dat < 0)  //如果为负数，首先取绝对值，并在指针上添加负号
    {
        dat = -dat;
        *str++ = '-';
        len++;
    }
    do {          //先转换为低位在前的十进制数组
        buf[i++] = dat % 10;
        dat /= 10;
    } while (dat > 0);
    len += i;     //i最后的值就是有效字符的个数
    while (i-- > 0)   //将数组值转换为ASCII码反向拷贝到接收指针上
    {
        *str++ = buf[i] + '0';
    }
    *str = '\0';  //添加字符串结束符
    
    return len;   //返回字符串长度
}