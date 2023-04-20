#ifndef _LCD1602_H_
#define _LCD1602_H_

void InitLcd1602();
void LcdClearScreen();
void LcdOpenCursor();
void LcdCloseCursor();
void LcdSetCursor(unsigned char x, unsigned char y);
void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);
void LcdShowChar(unsigned char x, unsigned char y, unsigned char chr);
void LcdWriteCmd(unsigned char cmd);

#endif