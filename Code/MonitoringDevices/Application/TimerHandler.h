

#ifndef __TIMER_HANDLER_H_
#define __TIMER_HANDLER_H_

#include "CommonConstDefine.h"
#include "TIMDriver.h"

extern uint8_t gPerformOneMsecJobs;
extern uint8_t gPerformHundreadMsecJobs;
extern uint8_t gPerformOneSecJobs;

/*
+------------------------------------------------------------------------------
| Function : Timers_Initialization(...)
+------------------------------------------------------------------------------
| Purpose: This Function initializes all timer with time base.
+------------------------------------------------------------------------------
| Algorithms: 
|   - If user wants to modify the time base of any timer, this the function where 
|	  user can chane timer related parameters accordingly.
|	
+------------------------------------------------------------------------------
| Parameters:  
|		None
|
+------------------------------------------------------------------------------
| Return Value: 
|		None
|  
+------------------------------------------------------------------------------
*/
void Timers_Initialization(void);

/*
+------------------------------------------------------------------------------
| Function : Timer_Reload(...)
+------------------------------------------------------------------------------
| Purpose: This Function reloads timer base with new value for new timing
+------------------------------------------------------------------------------
| Algorithms: 
|   - Pass instance and new timer value to low level driver
|	
+------------------------------------------------------------------------------
| Parameters:  
|		TIMER_INSTANCE_e - Timer instance number
|		uint32_t - New Period
|
+------------------------------------------------------------------------------
| Return Value: 
|		None
|  
+------------------------------------------------------------------------------
*/
inline void Timer_Reload(TIMER_INSTANCE_e timerInst, uint32_t newPeriod)
{
	TIM_Reload(timerInst, newPeriod);
}

/*
+------------------------------------------------------------------------------
| Function : Timer_Reload(...)
+------------------------------------------------------------------------------
| Purpose: Start and stops timer
+------------------------------------------------------------------------------
| Algorithms: 
|   - Pass instance and value to low level driver
|	
+------------------------------------------------------------------------------
| Parameters:  
|		TIMER_INSTANCE_e - Timer instance number
|		uint8_t - start stop indicator
|
+------------------------------------------------------------------------------
| Return Value: 
|		None
|  
+------------------------------------------------------------------------------
*/
inline void Timer_StartStop(TIMER_INSTANCE_e timerInst, uint8_t startStopFlg)
{
	TIM_StartStop(timerInst, startStopFlg);
}

void HundreadMiliSecJobs(void);

void OneSecJobs(void);

#endif /*#ifndef __TIMER_HANDLER_H_*/
