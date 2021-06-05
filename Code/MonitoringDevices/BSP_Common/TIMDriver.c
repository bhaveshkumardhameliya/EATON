/**
  ******************************************************************************
  * File Name          : TIMDriver.c
  * Description        : The Timer initialization routines.
  ******************************************************************************
  ******************************************************************************
  */

//-------------------------------- Includes ------------------------------------
#include "stm32f0xx_hal.h"


#ifdef HAL_TIM_MODULE_ENABLED	// enable this macro from stm32f0xx_hal_conf.h file 
								// to include timer module in project.

//---------------------------- Defines & Structures ----------------------------

/* The counter of a timer instance is disabled only if all the CCx and CCxN
   channels have been disabled */
#define TIM_CCER_CCxE_MASK 		((uint32_t)(TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E))
#define TIM_CCER_CCxNE_MASK 	((uint32_t)(TIM_CCER_CC1NE | TIM_CCER_CC2NE | TIM_CCER_CC3NE))


/** @defgroup HAL_TIM_Aliased_Defines HAL TIM Aliased Defines maintained for legacy purpose
  * @{
  */
#define CCER_CCxE_MASK                   TIM_CCER_CCxE_MASK
#define CCER_CCxNE_MASK                  TIM_CCER_CCxNE_MASK

/**
  * @brief  Enable the TIM peripheral.
  * @param  __HANDLE__ TIM handle
  * @retval None
 */
#define __HAL_TIM_ENABLE(__HANDLE__)                 ((__HANDLE__)->Instance->CR1 |= (TIM_CR1_CEN))


/**
  * @brief  Disable the TIM peripheral.
  * @param  __HANDLE__ TIM handle
  * @retval None
  */
#define __HAL_TIM_DISABLE(__HANDLE__) \
                        do { \
                          if (((__HANDLE__)->Instance->CCER & TIM_CCER_CCxE_MASK) == 0) \
                            { \
                            if(((__HANDLE__)->Instance->CCER & TIM_CCER_CCxNE_MASK) == 0) \
                            { \
                              (__HANDLE__)->Instance->CR1 &= ~(TIM_CR1_CEN); \
                            } \
                          } \
                        } while(0)
						
						
/** @defgroup TIM_Flag_definition TIM Flag Definition
  * @{
  */
#define TIM_FLAG_UPDATE                    TIM_SR_UIF		/*!< Update interrupt flag         */
#define TIM_FLAG_CC1                       TIM_SR_CC1IF		/*!< Capture/Compare 1 interrupt flag */
#define TIM_FLAG_CC2                       TIM_SR_CC2IF		/*!< Capture/Compare 2 interrupt flag */
#define TIM_FLAG_CC3                       TIM_SR_CC3IF		/*!< Capture/Compare 3 interrupt flag */
#define TIM_FLAG_CC4                       TIM_SR_CC4IF		/*!< Capture/Compare 4 interrupt flag */
#define TIM_FLAG_COM                       TIM_SR_COMIF		/*!< Commutation interrupt flag    */
#define TIM_FLAG_TRIGGER                   TIM_SR_TIF		/*!< Trigger interrupt flag        */
#define TIM_FLAG_BREAK                     TIM_SR_BIF		/*!< Break interrupt flag          */
#define TIM_FLAG_CC1OF                     TIM_SR_CC1OF		/*!< Capture 1 overcapture flag    */
#define TIM_FLAG_CC2OF                     TIM_SR_CC2OF		/*!< Capture 2 overcapture flag    */
#define TIM_FLAG_CC3OF                     TIM_SR_CC3OF		/*!< Capture 3 overcapture flag    */
#define TIM_FLAG_CC4OF                     TIM_SR_CC4OF		/*!< Capture 4 overcapture flag    */

/** @brief  Check whether the specified TIM interrupt flag is set or not.
  * @param  __HANDLE__ specifies the TIM Handle.
  * @param  __FLAG__ specifies the TIM interrupt flag to check.
  *        This parameter can be one of the following values:
  *            @arg TIM_FLAG_UPDATE: Update interrupt flag
  *            @arg TIM_FLAG_CC1: Capture/Compare 1 interrupt flag
  *            @arg TIM_FLAG_CC2: Capture/Compare 2 interrupt flag
  *            @arg TIM_FLAG_CC3: Capture/Compare 3 interrupt flag
  *            @arg TIM_FLAG_CC4: Capture/Compare 4 interrupt flag
  *            @arg TIM_FLAG_COM:  Commutation interrupt flag
  *            @arg TIM_FLAG_TRIGGER: Trigger interrupt flag
  *            @arg TIM_FLAG_BREAK: Break interrupt flag
  *            @arg TIM_FLAG_CC1OF: Capture/Compare 1 overcapture flag
  *            @arg TIM_FLAG_CC2OF: Capture/Compare 2 overcapture flag
  *            @arg TIM_FLAG_CC3OF: Capture/Compare 3 overcapture flag
  *            @arg TIM_FLAG_CC4OF: Capture/Compare 4 overcapture flag
  * @retval The new state of __FLAG__ (TRUE or FALSE).
  */
#define __HAL_TIM_GET_FLAG(__HANDLE__, __FLAG__)          (((__HANDLE__)->Instance->SR &(__FLAG__)) == (__FLAG__))

/** @brief  Clear the specified TIM interrupt flag.
  * @param  __HANDLE__ specifies the TIM Handle.
  * @param  __FLAG__ specifies the TIM interrupt flag to clear.
  *        This parameter can be one of the following values:
  *            @arg TIM_FLAG_UPDATE: Update interrupt flag
  *            @arg TIM_FLAG_CC1: Capture/Compare 1 interrupt flag
  *            @arg TIM_FLAG_CC2: Capture/Compare 2 interrupt flag
  *            @arg TIM_FLAG_CC3: Capture/Compare 3 interrupt flag
  *            @arg TIM_FLAG_CC4: Capture/Compare 4 interrupt flag
  *            @arg TIM_FLAG_COM:  Commutation interrupt flag
  *            @arg TIM_FLAG_TRIGGER: Trigger interrupt flag
  *            @arg TIM_FLAG_BREAK: Break interrupt flag
  *            @arg TIM_FLAG_CC1OF: Capture/Compare 1 overcapture flag
  *            @arg TIM_FLAG_CC2OF: Capture/Compare 2 overcapture flag
  *            @arg TIM_FLAG_CC3OF: Capture/Compare 3 overcapture flag
  *            @arg TIM_FLAG_CC4OF: Capture/Compare 4 overcapture flag
  * @retval The new state of __FLAG__ (TRUE or FALSE).
  */
#define __HAL_TIM_CLEAR_FLAG(__HANDLE__, __FLAG__)        ((__HANDLE__)->Instance->SR = ~(__FLAG__))

		

/** @defgroup TIM_Interrupt_definition TIM interrupt Definition
  * @{
  */
#define TIM_IT_UPDATE                      TIM_DIER_UIE		/*!< Update interrupt            */
#define TIM_IT_CC1                         TIM_DIER_CC1IE	/*!< Capture/Compare 1 interrupt */
#define TIM_IT_CC2                         TIM_DIER_CC2IE	/*!< Capture/Compare 2 interrupt */
#define TIM_IT_CC3                         TIM_DIER_CC3IE	/*!< Capture/Compare 3 interrupt */
#define TIM_IT_CC4                         TIM_DIER_CC4IE	/*!< Capture/Compare 4 interrupt */
#define TIM_IT_COM                         TIM_DIER_COMIE	/*!< Commutation interrupt       */
#define TIM_IT_TRIGGER                     TIM_DIER_TIE		/*!< Trigger interrupt           */
#define TIM_IT_BREAK                       TIM_DIER_BIE		/*!< Break interrupt             */

/**
  * @brief  Enables the specified TIM interrupt.
  * @param  __HANDLE__ specifies the TIM Handle.
  * @param  __INTERRUPT__ specifies the TIM interrupt source to enable.
  *          This parameter can be one of the following values:
  *            @arg TIM_IT_UPDATE: Update interrupt
  *            @arg TIM_IT_CC1:   Capture/Compare 1 interrupt
  *            @arg TIM_IT_CC2:  Capture/Compare 2 interrupt
  *            @arg TIM_IT_CC3:  Capture/Compare 3 interrupt
  *            @arg TIM_IT_CC4:  Capture/Compare 4 interrupt
  *            @arg TIM_IT_COM:   Commutation interrupt
  *            @arg TIM_IT_TRIGGER: Trigger interrupt
  *            @arg TIM_IT_BREAK: Break interrupt
  * @retval None
  */
#define __HAL_TIM_ENABLE_IT(__HANDLE__, __INTERRUPT__)    ((__HANDLE__)->Instance->DIER |= (__INTERRUPT__))

/**
  * @brief  Disables the specified TIM interrupt.
  * @param  __HANDLE__ specifies the TIM Handle.
  * @param  __INTERRUPT__ specifies the TIM interrupt source to disable.
  *          This parameter can be one of the following values:
  *            @arg TIM_IT_UPDATE: Update interrupt
  *            @arg TIM_IT_CC1:   Capture/Compare 1 interrupt
  *            @arg TIM_IT_CC2:  Capture/Compare 2 interrupt
  *            @arg TIM_IT_CC3:  Capture/Compare 3 interrupt
  *            @arg TIM_IT_CC4:  Capture/Compare 4 interrupt
  *            @arg TIM_IT_COM:   Commutation interrupt
  *            @arg TIM_IT_TRIGGER: Trigger interrupt
  *            @arg TIM_IT_BREAK: Break interrupt
  * @retval None
  */
#define __HAL_TIM_DISABLE_IT(__HANDLE__, __INTERRUPT__)   ((__HANDLE__)->Instance->DIER &= ~(__INTERRUPT__))


/**
  * @brief  Check whether the specified TIM interrupt source is enabled or not.
  * @param  __HANDLE__ TIM handle
  * @param  __INTERRUPT__ specifies the TIM interrupt source to check.
  *          This parameter can be one of the following values:
  *            @arg TIM_IT_UPDATE: Update interrupt
  *            @arg TIM_IT_CC1:   Capture/Compare 1 interrupt
  *            @arg TIM_IT_CC2:  Capture/Compare 2 interrupt
  *            @arg TIM_IT_CC3:  Capture/Compare 3 interrupt
  *            @arg TIM_IT_CC4:  Capture/Compare 4 interrupt
  *            @arg TIM_IT_COM:   Commutation interrupt
  *            @arg TIM_IT_TRIGGER: Trigger interrupt
  *            @arg TIM_IT_BREAK: Break interrupt
  * @retval The state of TIM_IT (SET or RESET).
  */
#define __HAL_TIM_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__) ((((__HANDLE__)->Instance->DIER & (__INTERRUPT__)) \
                                                             == (__INTERRUPT__)) ? SET : RESET)
															 

//---------------------------- Static Variables --------------------------------


//---------------------------- Global Variables --------------------------------
TIM_HandleTypeDef Timer2Handle;		// Timer 2 handle 
TIM_HandleTypeDef Timer3Handle;		// Timer 3 handle

//------------------------- Extern Global Variables ----------------------------


//--------------------------- Private function prototypes ----------------------
static void TIM_Base_SetConfig(TIM_TypeDef *TIMx, TIM_Base_InitTypeDef *timerConfig, 
										TIM_ClockConfigTypeDef * tmrClockConfig);


/*
+------------------------------------------------------------------------------
| Function : TIM_ConfigureTimeBase(...)
+------------------------------------------------------------------------------
| Purpose: This is initializes the timer module base on instant as parameter
+------------------------------------------------------------------------------
| Algorithms: 
|   	- Enable clock of Timer module from RCC
|		- Enable IRQ channel and its priority
|		- Clock selection
|		- Enable global interrupt for Timer 
|		- Start Timer
|	
+------------------------------------------------------------------------------
| Parameters:  
|		TIMER_INSTANCE_e - Timer instance number
|		TIM_Base_InitTypeDef - Timer Basic parameters for time base
|		TIM_ClockConfigTypeDef - Timer clock configuration
|
+------------------------------------------------------------------------------
| Return Value: 
|		None
|  
+------------------------------------------------------------------------------
*/
void TIM_ConfigureTimeBase(TIMER_INSTANCE_e timerInst, 
							TIM_Base_InitTypeDef * basicTmrInfo,
							TIM_ClockConfigTypeDef * tmrClockConfig)							
{
	TIM_HandleTypeDef *pLocalInstance;
	
	// take a local copy of Timer instance
	if(timerInst == TIMER_2_INSTANCE)
	{
		// Enable clock source for Timer module from RCC block.
		__HAL_RCC_TIM2_CLK_ENABLE();
		
		// Enable Timer interrupts
		HAL_NVIC_EnableIRQ(TIM2_IRQn);
		HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
		
		// Initialize timer 2 register array
		Timer2Handle.Instance = TIM2;
		// take local pointer copy of timer handle
		pLocalInstance = &Timer2Handle;
	}
	else if(timerInst == TIMER_3_INSTANCE)
	{
		// Enable clock source for Timer module from RCC block.
		__HAL_RCC_TIM3_CLK_ENABLE();
		
		// Enable Timer Receive interrupts
		HAL_NVIC_EnableIRQ(TIM3_IRQn);
		HAL_NVIC_SetPriority(TIM3_IRQn, 0, 0);
		
		// Initialize timer 2 register array
		Timer3Handle.Instance = TIM3;
		// take local pointer copy of timer handle
		pLocalInstance = &Timer3Handle;		
	}
	
	// Set Timer basic configuration
	TIM_Base_SetConfig(pLocalInstance->Instance, basicTmrInfo, tmrClockConfig);
	
	// Disable slave mode to clock the prescaler directly with the internal clock
    pLocalInstance->Instance->SMCR &= ~TIM_SMCR_SMS;
	
	// Clear the Interrupt Flags
	__HAL_TIM_CLEAR_FLAG(pLocalInstance, TIM_FLAG_UPDATE);
	
	// Enable TIM Interrupts 
	__HAL_TIM_ENABLE_IT(pLocalInstance, TIM_IT_UPDATE);

	// checking timer should start or not
	if(basicTmrInfo->runTimer)
	{
		// Start the timer
		__HAL_TIM_ENABLE(pLocalInstance);
	}	
}

/*
+------------------------------------------------------------------------------
| Function : TIM_Base_SetConfig(...)
+------------------------------------------------------------------------------
| Purpose: This function set basic parameters of Timer into its register base.
+------------------------------------------------------------------------------
| Algorithms: 
|   	- Select Mode
|		- Select Clock division
|		- Select Auto reload, prescaler, repetation counter
|		- Interrupt source selection
|	
+------------------------------------------------------------------------------
| Parameters:  
|		TIMER_INSTANCE_e - Timer instance number
|		TIM_Base_InitTypeDef - Timer Basic parameters for time base
|		TIM_ClockConfigTypeDef - Timer clock configuration
|
+------------------------------------------------------------------------------
| Return Value: 
|		None
|  
+------------------------------------------------------------------------------
*/
static void TIM_Base_SetConfig(TIM_TypeDef *TIMx, TIM_Base_InitTypeDef *timerConfig, 
										TIM_ClockConfigTypeDef * tmrClockConfig)
{
	uint32_t tmpcr1 = 0U;
	tmpcr1 = TIMx->CR1;

	/* Set TIM Time Base Unit parameters ---------------------------------------*/
	if (IS_TIM_COUNTER_MODE_SELECT_INSTANCE(TIMx))
	{
		/* Select the Counter Mode */
		tmpcr1 &= ~(TIM_CR1_DIR | TIM_CR1_CMS);
		tmpcr1 |= timerConfig->CounterMode;
	}

	if(IS_TIM_CLOCK_DIVISION_INSTANCE(TIMx))
	{
		/* Set the clock division */
		tmpcr1 &= ~TIM_CR1_CKD;
		tmpcr1 |= (uint32_t)timerConfig->ClockDivision;
	}

	/* Set the auto-reload preload */
	MODIFY_REG(tmpcr1, TIM_CR1_ARPE, timerConfig->AutoReloadPreload);

	TIMx->CR1 = tmpcr1;

	/* Set the Autoreload value */
	TIMx->ARR = (uint32_t)timerConfig->Period;

	/* Set the Prescaler value */
	TIMx->PSC = (uint32_t)timerConfig->Prescaler;

	if (IS_TIM_REPETITION_COUNTER_INSTANCE(TIMx))
	{
		/* Set the Repetition Counter value */
		TIMx->RCR = timerConfig->RepetitionCounter;
	}

	// Clock Selection
	if(tmrClockConfig->ClockSource == TIM_CLOCKSOURCE_INTERNAL)
	{
		/* Disable slave mode to clock the prescaler directly with the internal clock */
		TIMx->SMCR &= ~TIM_SMCR_SMS;
	}
	else
	{
		// TODO: Add external clock source selection here
	}
	
	/* Generate an update event to reload the Prescaler 
	 and the repetition counter(only for TIM1 and TIM8) value immediatly */
	TIMx->EGR = TIM_EGR_UG;
}

/*
+------------------------------------------------------------------------------
| Function : TIM_StartStop(...)
+------------------------------------------------------------------------------
| Purpose: This function start and stops function based on input parameter.
+------------------------------------------------------------------------------
| Algorithms: 
|   	- 1 as startStopFlg, starts timer
|		- 0 as startStopFlg, stops timer
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
void TIM_StartStop(TIMER_INSTANCE_e timerInst, uint8_t startStopFlg)
{
	TIM_HandleTypeDef *pLocalInstance;

	if(timerInst == TIMER_2_INSTANCE)
	{
		pLocalInstance = &Timer2Handle;
	}
	else if(timerInst == TIMER_3_INSTANCE)
	{
		pLocalInstance = &Timer3Handle;
	}	
	
	if(startStopFlg)
	{
		/* Start the timer */
		__HAL_TIM_ENABLE(pLocalInstance);
	}
	else
	{
		/* Start the timer */
		__HAL_TIM_DISABLE(pLocalInstance);
	}
}

/*
+------------------------------------------------------------------------------
| Function : TIM2_IRQHandler(...)
+------------------------------------------------------------------------------
| Purpose: This is IRQ function for Timer 2
+------------------------------------------------------------------------------
| Algorithms: 
|   	- Check selected source and Intrrupt source matching?
|		- call weak function of Timer 2 ISR handler
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
void TIM2_IRQHandler(void)  
{
	// Check interrupt flag and source of current interrupt.
	// Interrupt due to time base functionality
	if( __HAL_TIM_GET_FLAG(&Timer2Handle, TIM_FLAG_UPDATE) && 
		__HAL_TIM_GET_IT_SOURCE(&Timer2Handle, TIM_IT_UPDATE) )
	{
		// Clear timer base interrupt : Update event interrupt
		__HAL_TIM_CLEAR_FLAG(&Timer2Handle, TIM_FLAG_UPDATE);
		
		// User function to handle Timer 2 ISR 
		TIMER_2_IRQ_Handler();
	}
}

/*
+------------------------------------------------------------------------------
| Function : TIMER_2_IRQ_Handler(...)
+------------------------------------------------------------------------------
| Purpose: This is IRQ Handler function for Timer 2
+------------------------------------------------------------------------------
| Algorithms: 
|   	- This function must be implemented in User file to handle Timer 2 ISR
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
__weak void TIMER_2_IRQ_Handler(void)
{
	/* NOTE : This function Should not be modified, when the callback is needed,
	the TIMER_2_IRQ_Handler could be implemented in the user file
	*/
}


/*
+------------------------------------------------------------------------------
| Function : TIM3_IRQHandler(...)
+------------------------------------------------------------------------------
| Purpose: This is IRQ function for Timer 3
+------------------------------------------------------------------------------
| Algorithms: 
|   	- Check selected source and Intrrupt source matching?
|		- call weak function of Timer 3 ISR handler
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
void TIM3_IRQHandler(void)  
{
	// Check interrupt flag and source of current interrupt.
	// Interrupt due to time base functionality
	if( __HAL_TIM_GET_FLAG(&Timer3Handle, TIM_FLAG_UPDATE) && 
		__HAL_TIM_GET_IT_SOURCE(&Timer3Handle, TIM_IT_UPDATE) )
	{
		// Clear timer base interrupt : Update event interrupt
		__HAL_TIM_CLEAR_FLAG(&Timer3Handle, TIM_FLAG_UPDATE);
		
		// User function to handle Timer 3 ISR 
		TIMER_3_IRQ_Handler();
		
		TIM_StartStop(TIMER_3_INSTANCE, 0);
	}
}

/*
+------------------------------------------------------------------------------
| Function : TIMER_3_IRQ_Handler(...)
+------------------------------------------------------------------------------
| Purpose: This is IRQ Handler function for Timer 3
+------------------------------------------------------------------------------
| Algorithms: 
|   	- This function must be implemented in User file to handle Timer 3 ISR
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
__weak void TIMER_3_IRQ_Handler(void)
{
	/* NOTE : This function Should not be modified, when the callback is needed,
	the TIMER_3_IRQ_Handler could be implemented in the user file
	*/
}
#endif //HAL_TIM_MODULE_ENABLED
