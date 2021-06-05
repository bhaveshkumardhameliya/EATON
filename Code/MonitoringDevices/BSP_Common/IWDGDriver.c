/**
  ******************************************************************************
  * File Name          : IWDGDriver.c
  * Description        : The independent watchdog timer initialization routines.
  ******************************************************************************
 
  ******************************************************************************
  */

//-------------------------------- Includes ------------------------------------
#include "IWDGDriver.h"

//---------------------------- Defines & Structures ----------------------------


//---------------------------- Static Variables --------------------------------



//---------------------------- Global Variables --------------------------------
IWDG_HandleTypeDef iwdg_handle;



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
|		0 = Success
|		1 = fail
|  
+------------------------------------------------------------------------------
*/
int IWDG_Init(void)
{
	iwdg_handle.Instance = IWDG;
	iwdg_handle.Init.Prescaler = IWDG_PRESCALER_16;
	// Set counter reload value to obtain 250ms IWDG TimeOut.
	// Counter Reload Value = 250ms/IWDG counter clock period
	//                      = 250ms / (LSI/16)
	//                      = 0.25s / (LsiFreq/16)
	//                      = LsiFreq/(16 * 4)
	//                      = LsiFreq/64 = 625
	iwdg_handle.Init.Reload = 625;
	iwdg_handle.Init.Window = 0xFFF;
	
	// Initialize Watchdog timer
	if(HAL_IWDG_Init(&iwdg_handle) == HAL_OK)
	{
		return 0;
	}

	return 1;
}

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
void IWDG_Disable(void)
{
	iwdg_handle.Instance->KR = IWDG_KEY_DISABLE;
}

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
void IWDG_Refresh(void)
{
	HAL_IWDG_Refresh(&iwdg_handle);
}
