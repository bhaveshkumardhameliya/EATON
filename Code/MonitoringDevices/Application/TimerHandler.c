/*
---------------------------------------------------------------------------------
File Name : 									TimerHandler.c
---------------------------------------------------------------------------------

 Program Description    : Provides functionality of timer in system
 Author                 : Bhavesh Dhameliya
 Revision History       : 

---------------------------------------------------------------------------------
| Effeced Date |    Person        |        Description                          |
|--------------|------------------|---------------------------------------------|
---------------------------------------------------------------------------------
*/

//-----------------------------------------------------------------------------
// Including the modified definations and header files
//-----------------------------------------------------------------------------
#include "stm32f0xx_hal_conf.h"
#include "TimerHandler.h"
#include "GPIODriver.h"
#include "debugger.h"


//---------------------------- Defines & Structures ----------------------------
#define TIMER_2_TIME_BASE					1000		
#define TIMER_3_TIME_BASE					250			

#define ONE_SEC_IN_CNT_FOR_1MSEC_BASE		1000
#define HUNDREAD_SEC_IN_CNT_FOR_1MSEC_BASE	100

//---------------------------- Static Variables --------------------------------
static uint16_t timerbaseCnter = 0;

//---------------------------- Global Variables --------------------------------
uint8_t gPerformOneMsecJobs = 0;
uint8_t gPerformHundreadMsecJobs = 0;
uint8_t gPerformOneSecJobs = 0;

//------------------------- Extern Global Variables ----------------------------


//---------------------------- Static Variables --------------------------------


//--------------------------- Private function prototypes ----------------------
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
void Timers_Initialization(void)
{
	TIM_Base_InitTypeDef	timerParams;
	TIM_ClockConfigTypeDef	timerClockParams;
	
	/* Set Time Basic parameters */
  	timerParams.CounterMode = TIM_COUNTERMODE_UP;
	timerParams.Prescaler = 0;
	/* Compute the value to be set in ARR regiter to generate signal frequency at 10Khz */
	timerParams.Period = (SystemCoreClock / (TIMER_2_TIME_BASE) ) - 1;
	timerParams.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	timerParams.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	timerParams.RepetitionCounter = 0;
	timerParams.runTimer = 1;
	
	/* Set Timer 2 clock parameters */
	timerClockParams.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	timerClockParams.ClockPrescaler = TIM_CLOCKPRESCALER_DIV1; 
	timerClockParams.ClockPolarity = TIM_CLOCKPOLARITY_INVERTED;
	timerClockParams.ClockFilter = 0;
	
	TIM_ConfigureTimeBase(TIMER_2_INSTANCE, &timerParams, &timerClockParams);
	
	/* Set Time Basic parameters */
  	timerParams.CounterMode = TIM_COUNTERMODE_UP;
	timerParams.Prescaler = 0;
	timerParams.Period = (SystemCoreClock / (TIMER_3_TIME_BASE) ) - 1;
	timerParams.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	timerParams.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	timerParams.RepetitionCounter = 0;
	timerParams.runTimer = 0;
	
	/* Set Timer 3 clock parameters */
	timerClockParams.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	timerClockParams.ClockPrescaler = TIM_CLOCKPRESCALER_DIV1; 
	timerClockParams.ClockPolarity = TIM_CLOCKPOLARITY_INVERTED;
	timerClockParams.ClockFilter = 0;
	TIM_ConfigureTimeBase(TIMER_3_INSTANCE, &timerParams, &timerClockParams);
}

/*
+------------------------------------------------------------------------------
| Function : TIMER_2_IRQ_Handler(...)
+------------------------------------------------------------------------------
| Purpose: This is IRQ Handler function for Timer 2
+------------------------------------------------------------------------------
| Algorithms: 
|   	- calculates 100msec time and sets 100msec job flag to 1
|		- calculates 1sec time and sets 1sec job flag to 1
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
void TIMER_2_IRQ_Handler(void)
{
	/* Need to add relevant application for 1 msec time base */
	timerbaseCnter ++;
	gPerformOneMsecJobs = 1;
	
	if(!(timerbaseCnter % HUNDREAD_SEC_IN_CNT_FOR_1MSEC_BASE))
	{
		gPerformHundreadMsecJobs = 1;
		
		
	}
	
	if(timerbaseCnter >= ONE_SEC_IN_CNT_FOR_1MSEC_BASE)
	{
		timerbaseCnter = 0;
		HAL_GPIO_TogglePin(LED_Port, LED_RED_Pin);
		gPerformOneSecJobs = 1;
	}
}

void HundreadMiliSecJobs(void)
{
	
}

void OneSecJobs(void)
{
	
}

