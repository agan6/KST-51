#ifndef _TIME_H_
#define _TIME_H_

extern unsigned char psec;
extern struct sTime bufTime;

void RefreshTimeShow();
void IncSetTime();
void DecSetTime();
void LeftShiftTimeSet();
void RightShiftTimeSet();
void EnterTimeSet();
void ExitTimeSet(bit save);

#endif