#ifndef _DS1302_H_
#define _DS1302_H_

struct sTime {
	unsigned int year;
	unsigned char mon;
	unsigned char day;
	unsigned char hour;
	unsigned char min;
	unsigned char sec;
	unsigned char week;
};

void DS1302ByteWrite(unsigned char dat);
unsigned char DS1302ByteRead();
void DS1302SingleWrite(unsigned char reg, unsigned char dat);
unsigned char DS1302SingleRead(unsigned char reg);
void DS1302BurstWrite(unsigned char *dat);
void GetRealTime(struct sTime *time);
void SetRealTime(struct sTime *time);
void InitDS1302();

#endif