#ifndef _KEY_H
#define _KEY_H
#include "sys.h"


#define KEY1     PEin(3)
#define KEY2     PEin(4)
#define KEY3     PEin(5)
#define KEY4     PEin(6)


#define KEY1_PRES 1
#define KEY2_PRES 2
#define KEY3_PRES 3
#define KEY4_PRES 4

void KEY_Init(void);
u8 KEY_Read(u8 mode);

#endif

