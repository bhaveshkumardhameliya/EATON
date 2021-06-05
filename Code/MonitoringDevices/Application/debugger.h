

#ifndef __DEBUGGER_H_
#define __DEBUGGER_H_

#include "CommonConstDefine.h"

void UART0_SendWelcomeMsg(void);

void ProcessDebuggCommand(void);

void PrintBuffer(const char *buff,...);

#endif /*#ifndef __DEBUGGER_H_*/
