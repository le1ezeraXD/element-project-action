#ifndef __SEARCH_H_
#define __SEARCH_H_

#include "stm32f10x.h"
void SearchInit(void);
void searchrun(void);

#define  L GPIO_Pin_1  
#define  M GPIO_Pin_2  
#define  R GPIO_Pin_3  


#define black 1
#define white 0

#endif
