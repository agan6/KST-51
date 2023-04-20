#ifndef _IIC_H_
#define _IIC_H_

void I2CStart();
void I2CStop();
bit I2CWrite(unsigned char dat);
unsigned char I2CReadNAK();
unsigned char I2CReadACK();

#endif