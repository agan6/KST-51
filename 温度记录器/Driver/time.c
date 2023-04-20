#include "config.h"
#include "ds1302.h"
#include "keyboard.h"
#include "lcd1602.h"

struct sTime bufTime;
unsigned char setIndex = 0;
unsigned char psec = 0xAA;

void RefreshTimeShow();

void ShowBcdByte(unsigned char x, unsigned char y, unsigned char bcd)
{
    unsigned char str[4];
    
    str[0] = (bcd >> 4) + '0';
    str[1] = (bcd&0x0F) + '0';
    str[2] = '\0';
    LcdShowStr(x, y, str);
}

void RefreshTimeShow()
{
  LcdShowStr(3, 0, "20");
	ShowBcdByte(5,  0, bufTime.year);
	LcdShowChar(7, 0, '-');
  ShowBcdByte(8,  0, bufTime.mon);
	LcdShowChar(10, 0, '-');
  ShowBcdByte(11, 0, bufTime.day);
  ShowBcdByte(4,  1, bufTime.hour);
	LcdShowChar(6, 1, ':');
  ShowBcdByte(7,  1, bufTime.min);
	LcdShowChar(9, 1, ':');
  ShowBcdByte(10, 1, bufTime.sec);
}

void RefreshSetShow()
{
    switch (setIndex)
    {
        case 1:  LcdSetCursor(5,  0); break;
        case 2:  LcdSetCursor(6,  0); break;
        case 3:  LcdSetCursor(8,  0); break;
        case 4:  LcdSetCursor(9,  0); break;
        case 5:  LcdSetCursor(11, 0); break;
        case 6:  LcdSetCursor(12, 0); break;
        case 7:  LcdSetCursor(4,  1); break;
        case 8:  LcdSetCursor(5,  1); break;
        case 9:  LcdSetCursor(7,  1); break;
        case 10: LcdSetCursor(8,  1); break;
        case 11: LcdSetCursor(10, 1); break;
        case 12: LcdSetCursor(11, 1); break;
        default:  break;
    }
}

unsigned char IncBcdHigh(unsigned char bcd)
{
    if ((bcd&0xF0) < 0x90)
        bcd += 0x10;
    else
        bcd &= 0x0F;
    
    return bcd;
}

unsigned char IncBcdLow(unsigned char bcd)
{
    if ((bcd&0x0F) < 0x09)
        bcd += 0x01;
    else
        bcd &= 0xF0;
    
    return bcd;
}

unsigned char DecBcdHigh(unsigned char bcd)
{
    if ((bcd&0xF0) > 0x00)
        bcd -= 0x10;
    else
        bcd |= 0x90;
    
    return bcd;
}

unsigned char DecBcdLow(unsigned char bcd)
{
    if ((bcd&0x0F) > 0x00)
        bcd -= 0x01;
    else
        bcd |= 0x09;
    
    return bcd;
}

void IncSetTime()
{
  switch (setIndex)
  {
    case 1:  bufTime.year = IncBcdHigh(bufTime.year); break;
    case 2:  bufTime.year = IncBcdLow(bufTime.year);  break;
    case 3:  bufTime.mon  = IncBcdHigh(bufTime.mon);  break;
    case 4:  bufTime.mon  = IncBcdLow(bufTime.mon);   break;
    case 5:  bufTime.day  = IncBcdHigh(bufTime.day);  break;
    case 6:  bufTime.day  = IncBcdLow(bufTime.day);   break;
    case 7:  bufTime.hour = IncBcdHigh(bufTime.hour); break;
    case 8:  bufTime.hour = IncBcdLow(bufTime.hour);  break;
    case 9:  bufTime.min  = IncBcdHigh(bufTime.min);  break;
    case 10: bufTime.min  = IncBcdLow(bufTime.min);   break;
    case 11: bufTime.sec  = IncBcdHigh(bufTime.sec);  break;
    case 12: bufTime.sec  = IncBcdLow(bufTime.sec);   break;
    default:  break;
  }
  RefreshTimeShow();
  RefreshSetShow();
}

void DecSetTime()
{
  switch (setIndex)
  {
    case 1:  bufTime.year = DecBcdHigh(bufTime.year); break;
    case 2:  bufTime.year = DecBcdLow(bufTime.year);  break;
    case 3:  bufTime.mon  = DecBcdHigh(bufTime.mon);  break;
    case 4:  bufTime.mon  = DecBcdLow(bufTime.mon);   break;
    case 5:  bufTime.day  = DecBcdHigh(bufTime.day);  break;
    case 6:  bufTime.day  = DecBcdLow(bufTime.day);   break;
    case 7:  bufTime.hour = DecBcdHigh(bufTime.hour); break;
    case 8:  bufTime.hour = DecBcdLow(bufTime.hour);  break;
    case 9:  bufTime.min  = DecBcdHigh(bufTime.min);  break;
    case 10: bufTime.min  = DecBcdLow(bufTime.min);   break;
    case 11: bufTime.sec  = DecBcdHigh(bufTime.sec);  break;
    case 12: bufTime.sec  = DecBcdLow(bufTime.sec);   break;
    default:  break;
  }
  RefreshTimeShow();
  RefreshSetShow();
}

void RightShiftTimeSet()
{
  if (setIndex != 0)
  {
    if(setIndex < 12)
        setIndex++;
    else
        setIndex = 1;
    RefreshSetShow();
  }
}

void LeftShiftTimeSet()
{
  if(setIndex != 0)
  { 
    if(setIndex > 1)
       setIndex--;
    else
       setIndex = 12;
       RefreshSetShow();
  }
}

void EnterTimeSet()
{
  setIndex = 2;       
  LeftShiftTimeSet(); 
  LcdOpenCursor();   
}

void ExitTimeSet(bit save)
{
  setIndex = 0;    
  if(save)     
  {
    SetRealTime(&bufTime);
  }
  LcdCloseCursor(); 
}