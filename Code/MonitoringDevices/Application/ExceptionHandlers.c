/**
  ******************************************************************************
  * File Name          : ExceptionHandler.c
  * Description        : 
  ******************************************************************************
 
  ******************************************************************************
  */

//-------------------------------- Includes ------------------------------------
#include "stm32f0xx_hal.h"


//---------------------------- Static Variables --------------------------------


//---------------------------- Global Variables --------------------------------
extern __IO uint32_t TimingDelay;

//--------------------------- Private function prototypes ----------------------

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/*
+------------------------------------------------------------------------------
| Function : SysTick_Handler(...)
+------------------------------------------------------------------------------
| Purpose: This function handles SysTick Handler of CMSIS core of 1msec
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
void SysTick_Handler(void)
{
	TimingDelay--;
}


/******************************************************************************/
/*                 STM32F0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f0xx.s).                                               */
/******************************************************************************/

/*
+------------------------------------------------------------------------------
| Function : HAL_RCC_CSSCallback(...)
+------------------------------------------------------------------------------
| Purpose: When HSE fails, NMI Handler will call this function and we need to 
|          handle error related to Clock failure @Standby, @during Cycle and @RUN
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
void HAL_RCC_CSSCallback(void)
{
	
}

