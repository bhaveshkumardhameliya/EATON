/**
  ******************************************************************************
  * File Name          : TIMDriver.h
  * Description        : Timer Module related defines, data structure and functions.
  ******************************************************************************
 
  ******************************************************************************
  */

#ifndef __TIMER_DRIVER__H
#define __TIMER_DRIVER__H

#ifdef __cplusplus
 extern "C" {
#endif
	 
	 
//-------------------------------- Includes ------------------------------------
#include "stm32f0xx_hal.h"

//---------------------------- Defines & Structures ----------------------------

typedef enum
{
	TIMER_1_INSTANCE,
	TIMER_2_INSTANCE,
	TIMER_3_INSTANCE,
	TIMER_4_INSTANCE,
	
}TIMER_INSTANCE_e;

/** @defgroup TIM_Counter_Mode TIM Counter Mode
  * @{
  */
#define TIM_COUNTERMODE_UP                 (0x0000U)
#define TIM_COUNTERMODE_DOWN               TIM_CR1_DIR
#define TIM_COUNTERMODE_CENTERALIGNED1     TIM_CR1_CMS_0
#define TIM_COUNTERMODE_CENTERALIGNED2     TIM_CR1_CMS_1
#define TIM_COUNTERMODE_CENTERALIGNED3     TIM_CR1_CMS

/** @defgroup TIM_ClockDivision TIM Clock Division
  * @{
  */
#define TIM_CLOCKDIVISION_DIV1                       (0x0000U)
#define TIM_CLOCKDIVISION_DIV2                       (TIM_CR1_CKD_0)
#define TIM_CLOCKDIVISION_DIV4                       (TIM_CR1_CKD_1)

/** @defgroup TIM_AutoReloadPreload TIM Auto-Reload Preload
  * @{
  */
#define TIM_AUTORELOAD_PRELOAD_DISABLE                (0x0000U)       /*!< TIMx_ARR register is not buffered */
#define TIM_AUTORELOAD_PRELOAD_ENABLE                 (TIM_CR1_ARPE)  /*!< TIMx_ARR register is buffered */

/** @defgroup TIM_Clock_Source TIM Clock Source
  * @{
  */
#define	TIM_CLOCKSOURCE_ETRMODE2    (TIM_SMCR_ETPS_1)
#define	TIM_CLOCKSOURCE_INTERNAL    (TIM_SMCR_ETPS_0)
#define	TIM_CLOCKSOURCE_ITR0        (0x0000U)
#define	TIM_CLOCKSOURCE_ITR1        (TIM_SMCR_TS_0)
#define	TIM_CLOCKSOURCE_ITR2        (TIM_SMCR_TS_1)
#define	TIM_CLOCKSOURCE_ITR3        (TIM_SMCR_TS_0 | TIM_SMCR_TS_1)
#define	TIM_CLOCKSOURCE_TI1ED       (TIM_SMCR_TS_2)
#define	TIM_CLOCKSOURCE_TI1         (TIM_SMCR_TS_0 | TIM_SMCR_TS_2)
#define	TIM_CLOCKSOURCE_TI2         (TIM_SMCR_TS_1 | TIM_SMCR_TS_2)
#define	TIM_CLOCKSOURCE_ETRMODE1    (TIM_SMCR_TS)

/** @defgroup TIM_ETR_Polarity TIM ETR Polarity
  * @{
  */
#define TIM_ETRPOLARITY_INVERTED              (TIM_SMCR_ETP)	/*!< Polarity for ETR source */
#define TIM_ETRPOLARITY_NONINVERTED           (0x0000U)			/*!< Polarity for ETR source */

/** @defgroup TIM_Clock_Polarity TIM Clock Polarity
  * @{
  */
#define TIM_CLOCKPOLARITY_INVERTED           TIM_ETRPOLARITY_INVERTED          /*!< Polarity for ETRx clock sources */
#define TIM_CLOCKPOLARITY_NONINVERTED        TIM_ETRPOLARITY_NONINVERTED       /*!< Polarity for ETRx clock sources */
#define TIM_CLOCKPOLARITY_RISING             TIM_INPUTCHANNELPOLARITY_RISING   /*!< Polarity for TIx clock sources */
#define TIM_CLOCKPOLARITY_FALLING            TIM_INPUTCHANNELPOLARITY_FALLING   /*!< Polarity for TIx clock sources */
#define TIM_CLOCKPOLARITY_BOTHEDGE           TIM_INPUTCHANNELPOLARITY_BOTHEDGE  /*!< Polarity for TIx clock sources */

/** @defgroup TIM_ETR_Prescaler TIM ETR Prescaler
  * @{
  */
#define TIM_ETRPRESCALER_DIV1                 (0x0000U)                /*!< No prescaler is used */
#define TIM_ETRPRESCALER_DIV2                 (TIM_SMCR_ETPS_0)                 /*!< ETR input source is divided by 2 */
#define TIM_ETRPRESCALER_DIV4                 (TIM_SMCR_ETPS_1)                 /*!< ETR input source is divided by 4 */
#define TIM_ETRPRESCALER_DIV8                 (TIM_SMCR_ETPS)                   /*!< ETR input source is divided by 8 */

/** @defgroup TIM_Clock_Prescaler TIM Clock Prescaler
  * @{
  */
#define TIM_CLOCKPRESCALER_DIV1                 TIM_ETRPRESCALER_DIV1     /*!< No prescaler is used */
#define TIM_CLOCKPRESCALER_DIV2                 TIM_ETRPRESCALER_DIV2     /*!< Prescaler for External ETR Clock: Capture performed once every 2 events. */
#define TIM_CLOCKPRESCALER_DIV4                 TIM_ETRPRESCALER_DIV4     /*!< Prescaler for External ETR Clock: Capture performed once every 4 events. */
#define TIM_CLOCKPRESCALER_DIV8                 TIM_ETRPRESCALER_DIV8     /*!< Prescaler for External ETR Clock: Capture performed once every 8 events. */

/**
  * @brief  TIM Clock Configuration Handle Structure definition
  */ 
typedef struct
{
  uint32_t ClockSource;     /*!< TIM clock sources 
                                 This parameter can be a value of @ref TIM_Clock_Source */
  uint32_t ClockPolarity;   /*!< TIM clock polarity 
                                 This parameter can be a value of @ref TIM_Clock_Polarity */
  uint32_t ClockPrescaler;  /*!< TIM clock prescaler 
                                 This parameter can be a value of @ref TIM_Clock_Prescaler */
  uint32_t ClockFilter;    /*!< TIM clock filter 
                                This parameter can be a number between Min_Data = 0x0 and Max_Data = 0xF */
}TIM_ClockConfigTypeDef;

/**
  * @brief  TIM Time base Configuration Structure definition 
  */
typedef struct
{
	uint32_t Prescaler;         /*!< Specifies the prescaler value used to divide the TIM clock.
								   This parameter can be a number between Min_Data = 0x0000 and Max_Data = 0xFFFF */

	uint32_t CounterMode;       /*!< Specifies the counter mode.
								   This parameter can be a value of @ref TIM_Counter_Mode */

	uint32_t Period;            /*!< Specifies the period value to be loaded into the active
								   Auto-Reload Register at the next update event.
								   This parameter can be a number between Min_Data = 0x0000 and Max_Data = 0xFFFF.  */

	uint32_t ClockDivision;     /*!< Specifies the clock division.
								   This parameter can be a value of @ref TIM_ClockDivision */

	uint32_t RepetitionCounter;  /*!< Specifies the repetition counter value. Each time the RCR downcounter
									reaches zero, an update event is generated and counting restarts
									from the RCR value (N).
									This means in PWM mode that (N+1) corresponds to:
										- the number of PWM periods in edge-aligned mode
										- the number of half PWM period in center-aligned mode
									 This parameter must be a number between Min_Data = 0x00 and Max_Data = 0xFF.
									 @note This parameter is valid only for TIM1 and TIM8. */

	uint32_t AutoReloadPreload;  /*!< Specifies the auto-reload preload.
								   This parameter can be a value of @ref TIM_AutoReloadPreload */
								   
	uint8_t runTimer;			/*!< Specifies timer should start from initialization or it's just configuration
									 for Timer */
}TIM_Base_InitTypeDef;


/**
  * @brief  HAL Active channel structures definition 
  */ 
typedef enum
{
  HAL_TIM_ACTIVE_CHANNEL_1        = 0x01U,    /*!< The active channel is 1     */
  HAL_TIM_ACTIVE_CHANNEL_2        = 0x02U,    /*!< The active channel is 2     */
  HAL_TIM_ACTIVE_CHANNEL_3        = 0x04U,    /*!< The active channel is 3     */
  HAL_TIM_ACTIVE_CHANNEL_4        = 0x08U,    /*!< The active channel is 4     */
  HAL_TIM_ACTIVE_CHANNEL_CLEARED  = 0x00U    /*!< All active channels cleared */
}HAL_TIM_ActiveChannel;

/**
  * @brief  TIM Time Base Handle Structure definition 
  */ 
typedef struct
{
	TIM_TypeDef					*Instance;     /*!< Register base address             */
	HAL_TIM_ActiveChannel    	Channel;       /*!< Active channel                    */

}TIM_HandleTypeDef;

//---------------------------- Global Variables --------------------------------

//--------------------------- Private function prototypes ----------------------

/*
+------------------------------------------------------------------------------
| Function : TIM_ConfigureTimeBase(...)
+------------------------------------------------------------------------------
| Purpose: This is initializes the timer module base on instant as parameter
+------------------------------------------------------------------------------
| Algorithms: 
|   	- Enable clock of Timer module from RCC
|		- Enable IRQ channel and its priority
|		- Set parameters from base timer in timer register base
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
							TIM_ClockConfigTypeDef * tmrClockConfig);
	
/*
+------------------------------------------------------------------------------
| Function : TIM_Reload(...)
+------------------------------------------------------------------------------
| Purpose: This function reloads time base with new value.
+------------------------------------------------------------------------------
| Algorithms: 
|   	- Sets New Period Value
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
inline void TIM_Reload(TIMER_INSTANCE_e timerInst, uint32_t newPeriod)
{
	if(timerInst == TIMER_2_INSTANCE)
	{
		/* Set the Autoreload value */
		TIM2->CNT = 0;
	}
	else if(timerInst == TIMER_3_INSTANCE)
	{
		/* Set the Autoreload value */
		TIM3->CNT = 0;
	}
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
void TIM_StartStop(TIMER_INSTANCE_e timerInst, uint8_t startStopFlg);


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
__weak void TIMER_2_IRQ_Handler(void);

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
__weak void TIMER_3_IRQ_Handler(void);

#ifdef __cplusplus
}
#endif

#endif // __TIMER_DRIVER__H
