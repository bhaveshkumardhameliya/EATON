/**
  ******************************************************************************
  * File Name          : COMPDriver.c
  * Description        : The Comparator initialization routines.
  ******************************************************************************
  ******************************************************************************
  
    Additional Tables:

    Table 1. COMP Inputs for the STM32F05x, STM32F07x and STM32F09x devices
    +--------------------------------------------------+    
    |                 |                | COMP1 | COMP2 |
    |-----------------|----------------|---------------|
    |                 | 1/4 VREFINT    |  OK   |  OK   |
    |                 | 1/2 VREFINT    |  OK   |  OK   |
    |                 | 3/4 VREFINT    |  OK   |  OK   |
    | Inverting Input | VREFINT        |  OK   |  OK   |
    |                 | DAC1 OUT (PA4) |  OK   |  OK   |
    |                 | DAC2 OUT (PA5) |  OK   |  OK   |
    |                 | IO1            |  PA0  |  PA2  |
    |-----------------|----------------|-------|-------|
    |  Non Inverting  |                |  PA1  |  PA3  |
    |    Input        |                |       |       |
    +--------------------------------------------------+  

    Table 2. COMP Outputs for the STM32F05x, STM32F07x and STM32F09x devices
    +---------------+     
    | COMP1 | COMP2 |
    |-------|-------|
    |  PA0  |  PA2  |
    |  PA6  |  PA7  |
    |  PA11 |  PA12 |
    +---------------+

    Table 3. COMP Outputs redirection to embedded timers for the STM32F05x, STM32F07x and STM32F09x devices
    +---------------------------------+     
    |     COMP1      |     COMP2      |
    |----------------|----------------|
    |  TIM1 BKIN     |  TIM1 BKIN     |
    |                |                |
    |  TIM1 OCREFCLR |  TIM1 OCREFCLR |
    |                |                |
    |  TIM1 IC1      |  TIM1 IC1      |
    |                |                |
    |  TIM2 IC4      |  TIM2 IC4      |
    |                |                |
    |  TIM2 OCREFCLR |  TIM2 OCREFCLR |
    |                |                |
    |  TIM3 IC1      |  TIM3 IC1      |
    |                |                |
    |  TIM3 OCREFCLR |  TIM3 OCREFCLR |
    +---------------------------------+

**/

//-------------------------------- Includes ------------------------------------
#include "stm32f0xx_hal.h"


#ifdef HAL_COMP_MODULE_ENABLED	// enable this macro from stm32f0xx_hal_conf.h file 
								// to include comparator Module module in project.

//---------------------------- Defines & Structures ----------------------------

/* Delay for COMP startup time.                                               */
/* Note: Delay required to reach propagation delay specification.             */
/* Literal set to maximum value (refer to device datasheet,                   */
/* parameter "tSTART").                                                       */
/* Unit: us                                                                   */
#define COMP_DELAY_STARTUP_US           (60U)  /*!< Delay for COMP startup time */

/* CSR register reset value */ 
#define COMP_CSR_RESET_VALUE            (0x00000000U)
/* CSR register masks */ 
#define COMP_CSR_RESET_PARAMETERS_MASK   (0x00003FFFU)
#define COMP_CSR_UPDATE_PARAMETERS_MASK  (0x00003FFEU)
/* CSR COMPx non inverting input mask */ 
#define COMP_CSR_COMPxNONINSEL_MASK      ((uint16_t)COMP_CSR_COMP1SW1)
/* CSR COMP2 shift */ 
#define COMP_CSR_COMP1_SHIFT             0U
#define COMP_CSR_COMP2_SHIFT             16U


/** @defgroup COMP_ExtiLine COMP EXTI Lines
  *        Elements values convention: XXXX0000
  *           - XXXX : Interrupt mask in the EMR/IMR/RTSR/FTSR register
  * @{   
  */  
#define COMP_EXTI_LINE_COMP1             ((uint32_t)EXTI_IMR_MR21)  /*!< EXTI line 21 connected to COMP1 output */
#define COMP_EXTI_LINE_COMP2             ((uint32_t)EXTI_IMR_MR22)  /*!< EXTI line 22 connected to COMP2 output */


/**
  * @brief  Get the specified EXTI line for a comparator instance.
  * @param  __INSTANCE__ specifies the COMP instance.
  * @retval value of @ref COMP_ExtiLine
  */
#define COMP_GET_EXTI_LINE(__INSTANCE__)             (((__INSTANCE__) == COMP1) ? COMP_EXTI_LINE_COMP1 : \
                                                      COMP_EXTI_LINE_COMP2)


//---------------------------- Static Variables --------------------------------



//---------------------------- Global Variables --------------------------------
COMP_HandleTypeDef comp1Handle;
COMP_HandleTypeDef comp2Handle;

const COMP_HandleTypeDef *compArray[MAX_COMPARATOR_INSTANCE] = 
{
	&comp1Handle,
	&comp2Handle,
};
//------------------------- Extern Global Variables ----------------------------



//--------------------------- Private function prototypes ----------------------

/*
+------------------------------------------------------------------------------
| Function : COMP_HAL_Init(...)
+------------------------------------------------------------------------------
| Purpose: This function initialiazes Comparator parameters in its SFR 
+------------------------------------------------------------------------------
| Algorithms: 
|   	- Initialize comparator register base and take local copy 
|		- Set priority and IRQ channel for comparator
|		- Enable interrupt
|		- Disable windoe comparision, set edge, set interrupt source
|		- Enable comparator Module 
|
+------------------------------------------------------------------------------
| Parameters:  
|		COMPARATOR_INSTANCE_e - comparator instance number
|		COMP_InitTypeDef - comparator initialization parameters
|
+------------------------------------------------------------------------------
| Return Value: 
|		None
|  
+------------------------------------------------------------------------------
*/
void COMP_HAL_Init(COMPARATOR_INSTANCE_e compInstance, COMP_InitTypeDef *compPrams)
{
	uint32_t 			regshift = COMP_CSR_COMP1_SHIFT;
	uint32_t 			extiLine = 0U;
	COMP_HandleTypeDef 	*localCompHandle;
	
	/* Init SYSCFG and the low level hardware to access comparators */
    __HAL_RCC_SYSCFG_CLK_ENABLE(); 
	
	/* Initialize the comparator register base based on compInstance */
	if(compInstance == COMPARATOR_1)
	{
		comp1Handle.Instance = COMP1;
		localCompHandle = &comp1Handle;		/* take local pointer copy */
	}
	else
	{
		comp2Handle.Instance = COMP2;
		localCompHandle = &comp2Handle;		/* take local pointer copy */
	}
	
	/* COMP1 & COMP2 interrupt Init, For both comparator same interrupt. */
	/* COMP1 EXTI line 21 & COMP2 EXTI line 22. */
    HAL_NVIC_SetPriority(ADC1_COMP_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(ADC1_COMP_IRQn); 
	
	/* Set COMP parameters */
    /*	Set COMPxINSEL bits according to hcomp->Init.InvertingInput value	*/
    /*	Set COMPxOUTSEL bits according to hcomp->Init.Output value			*/
    /*	Set COMPxPOL bit according to hcomp->Init.OutputPol value			*/
    /*	Set COMPxHYST bits according to hcomp->Init.Hysteresis value		*/
    /*	Set COMPxMODE bits according to hcomp->Init.Mode value				*/
    if(localCompHandle->Instance == COMP2)
    {
		regshift = COMP_CSR_COMP2_SHIFT;
    }
    MODIFY_REG(COMP->CSR, 
               (COMP_CSR_COMPxINSEL  | COMP_CSR_COMPxNONINSEL_MASK | \
                COMP_CSR_COMPxOUTSEL | COMP_CSR_COMPxPOL           | \
                COMP_CSR_COMPxHYST   | COMP_CSR_COMPxMODE) << regshift,
               (compPrams->InvertingInput    | \
                compPrams->NonInvertingInput | \
                compPrams->Output            | \
                compPrams->OutputPol         | \
                compPrams->Hysteresis        | \
                compPrams->Mode) << regshift);   
	
	/* TODO: mostly we are not using window comparator, so below line can be removed */
	if(COMP_WINDOWMODE_DISABLE != compPrams->WindowMode)
    {
		COMP->CSR |= (uint32_t)COMP_CSR_WNDWEN;
    }
	
	/*----------- Enable Interrupt for comparator ------------*/
	
	/* Check the Exti Line output configuration */
    extiLine = COMP_GET_EXTI_LINE(localCompHandle->Instance);
	
	/* Configure the rising edge */
	if((compPrams->TriggerMode & COMP_TRIGGERMODE_IT_RISING) != RESET)
	{
		SET_BIT(EXTI->RTSR, extiLine);
	}
	else
	{
		CLEAR_BIT(EXTI->RTSR, extiLine);
	}
	
	/* Configure the falling edge */
	if((compPrams->TriggerMode & COMP_TRIGGERMODE_IT_FALLING) != RESET)
	{
		SET_BIT(EXTI->FTSR, extiLine);
	}
	else
	{
		CLEAR_BIT(EXTI->FTSR, extiLine);
	}
	
	// Clear COMP EXTI pending bit, writing 1 to bit will clear the flag.
    WRITE_REG(EXTI->PR, extiLine);
    
    // Enable EXTI interrupt mode
    SET_BIT(EXTI->IMR, extiLine);
	
	// Enable the selected comparator
	SET_BIT(COMP->CSR, COMP_CSR_COMPxEN << regshift);
	
	/* After starting comaparator module, it has propogation delay time of 60usec
		which is handled in STM driver file but as we are using in interrupt mode,
		so no need to wait till 60usec. */
}

/*
+------------------------------------------------------------------------------
| Function : ADC1_COMP_IRQHandler(...)
+------------------------------------------------------------------------------
| Purpose: This is IRQ Handler function for Comparator and ADC
+------------------------------------------------------------------------------
| Algorithms: 
|   	- The IRQ channel for Comparator 1, 2 and ADC are common.
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
void ADC1_COMP_IRQHandler(void)
{
	COMPARATOR_INSTANCE_e		cnt;
	uint32_t					extiLine;

	for(cnt = COMPARATOR_1; cnt < MAX_COMPARATOR_INSTANCE; cnt ++)
	{
		extiLine = COMP_GET_EXTI_LINE(compArray[cnt]->Instance);
		// Check COMP Exti flag
		if(READ_BIT(EXTI->PR, extiLine) != RESET)
		{
			// Clear COMP Exti pending bit
			WRITE_REG(EXTI->PR, extiLine);
			
			// Note: STM does not have any register from where we can check, 
			// interrupt edge, we need to manual read the IO pin.
			COMP_IRQ_Handler(cnt);
		}	
	}

}


/*
+------------------------------------------------------------------------------
| Function : COMP_IRQ_Handler(...)
+------------------------------------------------------------------------------
| Purpose: This is IRQ Handler function for Comparator
+------------------------------------------------------------------------------
| Algorithms: 
|   	- This function must be implemented in User file to handle 
|			comparator 1, 2 ISR
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
__weak void COMP_IRQ_Handler(COMPARATOR_INSTANCE_e compInstance)
{
  	/* NOTE : This function Should not be modified, when the callback is needed,
	the COMP_IRQ_Handler could be implemented in the user file
	*/
}



#endif //HAL_COMP_MODULE_ENABLED
