/**
  ******************************************************************************
  * File Name          : IWDGDriver.h
  * Description        : The independent watchdog timer initialization routines.
  ******************************************************************************
 
  ******************************************************************************
  */

#ifndef __BSP_COMMON_IWDG_H
#define __BSP_COMMON_IWDG_H

#ifdef __cplusplus
 extern "C" {
#endif
	 
	 
//-------------------------------- Includes ------------------------------------
#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_iwdg.h"

//---------------------------- Defines & Structures ----------------------------


//---------------------------- Global Variables --------------------------------


//--------------------------- Private function prototypes ----------------------
/*
+------------------------------------------------------------------------------
| Function : IWDG_Init(...)
+------------------------------------------------------------------------------
| Purpose: This function initializes the independent watchdog timer at power ON.
+------------------------------------------------------------------------------
| Algorithms: 
|           
|			
|	@note: Add Note if any 
|
+------------------------------------------------------------------------------
| Parameters:  
|  		None
+------------------------------------------------------------------------------
| Return Value: 
|		None
|  
+------------------------------------------------------------------------------
*/
int IWDG_Init(void);

/*
+------------------------------------------------------------------------------
| Function : IWDG_Disable(...)
+------------------------------------------------------------------------------
| Purpose: This function disables IWDG timer at power ON.
+------------------------------------------------------------------------------
| Algorithms: 
|           
|			
|	@note: Add Note if any 
|
+------------------------------------------------------------------------------
| Parameters:  
|  		None
+------------------------------------------------------------------------------
| Return Value: 
|		None
|  
+------------------------------------------------------------------------------
*/
void IWDG_Disable(void);

/*
+------------------------------------------------------------------------------
| Function : IWDG_Refresh(...)
+------------------------------------------------------------------------------
| Purpose: This function refresh watchdog timer.
+------------------------------------------------------------------------------
| Algorithms: 
|           
|			
|	@note: Always call from main loop
|
+------------------------------------------------------------------------------
| Parameters:  
|  		None
+------------------------------------------------------------------------------
| Return Value: 
|		None
|  
+------------------------------------------------------------------------------
*/
void IWDG_Refresh(void);

#ifdef __cplusplus
}
#endif

#endif // __BSP_COMMON_IWDG_H
