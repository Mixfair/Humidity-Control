
#ifndef __MAIN_H
#define	__MAIN_H

#include "stm32f10x.h"  

void initButtons(void);
void initTIM6(void);
void initUsart(void);
void initPWM(void);
void setPwm(void);
void initADC(void);
void send_to_uart(char *str);
void reverseStr(char *str);
int getVal(char *str);
void delay(uint32_t timevs);
void ExecuteCommand(void);
bool checkCmd(char *cmd_in);
void moveCursor(void);
#endif
