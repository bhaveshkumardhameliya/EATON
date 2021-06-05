/**
  ******************************************************************************
  * File Name          : COMPDriver.h
  * Description        : Comparator Module related defines, data structure and 
  *							functions.
  ******************************************************************************
 
  ******************************************************************************
  */

#ifndef __COMPARATOR_DRIVER__H
#define __COMPARATOR_DRIVER__H

#ifdef __cplusplus
 extern "C" {
#endif
	 
	 
//-------------------------------- Includes ------------------------------------
#include "stm32f0xx_hal.h"

//---------------------------- Defines & Structures ----------------------------

typedef enum
{
	COMPARATOR_1,
	COMPARATOR_2,
	MAX_COMPARATOR_INSTANCE,
	
}COMPARATOR_INSTANCE_e;

/** @defgroup COMP_OutputPolarity COMP OutputPolarity
  * @{
  */
#define COMP_OUTPUTPOL_NONINVERTED             (0x00000000U)  /*!< COMP output on GPIO isn't inverted */
#define COMP_OUTPUTPOL_INVERTED                COMP_CSR_COMP1POL       /*!< COMP output on GPIO is inverted  */

/** @defgroup COMP_Hysteresis COMP Hysteresis
  * @{
  */
#define COMP_HYSTERESIS_NONE                   (0x00000000U)  /*!< No hysteresis */
#define COMP_HYSTERESIS_LOW                    COMP_CSR_COMP1HYST_0    /*!< Hysteresis level low */
#define COMP_HYSTERESIS_MEDIUM                 COMP_CSR_COMP1HYST_1    /*!< Hysteresis level medium */
#define COMP_HYSTERESIS_HIGH                   COMP_CSR_COMP1HYST      /*!< Hysteresis level high */


/** @defgroup COMP_Mode COMP Mode
  * @{
  */
/* Please refer to the electrical characteristics in the device datasheet for
   the power consumption values */
#define COMP_MODE_HIGHSPEED                    (0x00000000U) /*!< High Speed */
#define COMP_MODE_MEDIUMSPEED                  COMP_CSR_COMP1MODE_0   /*!< Medium Speed */
#define COMP_MODE_LOWPOWER                     COMP_CSR_COMP1MODE_1   /*!< Low power mode */
#define COMP_MODE_ULTRALOWPOWER                COMP_CSR_COMP1MODE     /*!< Ultra-low power mode */


/** @defgroup COMP_InvertingInput COMP InvertingInput
  * @{
  */

#define COMP_INVERTINGINPUT_1_4VREFINT         (0x00000000U) /*!< 1/4 VREFINT connected to comparator inverting input */
#define COMP_INVERTINGINPUT_1_2VREFINT         COMP_CSR_COMP1INSEL_0                         /*!< 1/2 VREFINT connected to comparator inverting input    */
#define COMP_INVERTINGINPUT_3_4VREFINT         COMP_CSR_COMP1INSEL_1                         /*!< 3/4 VREFINT connected to comparator inverting input    */
#define COMP_INVERTINGINPUT_VREFINT            (COMP_CSR_COMP1INSEL_1|COMP_CSR_COMP1INSEL_0) /*!< VREFINT connected to comparator inverting input        */
#define COMP_INVERTINGINPUT_DAC1               COMP_CSR_COMP1INSEL_2                         /*!< DAC_OUT1 (PA4) connected to comparator inverting input */
#define COMP_INVERTINGINPUT_DAC1SWITCHCLOSED   (COMP_CSR_COMP1INSEL_2|COMP_CSR_COMP1SW1)     /*!< DAC_OUT1 (PA4) connected to comparator inverting input 
																								  and close switch (PA0 for COMP1 only) */
#define COMP_INVERTINGINPUT_DAC2               (COMP_CSR_COMP1INSEL_2|COMP_CSR_COMP1INSEL_0) /*!< DAC_OUT2 (PA5) connected to comparator inverting input */
#define COMP_INVERTINGINPUT_IO1                (COMP_CSR_COMP1INSEL_2|COMP_CSR_COMP1INSEL_1) /*!< IO (PA0 for COMP1 and PA2 for COMP2) connected to comparator inverting input */


/** @defgroup COMP_NonInvertingInput COMP NonInvertingInput
  * @{
  */
#define COMP_NONINVERTINGINPUT_IO1               (0x00000000U) /*!< I/O1 (PA1 for COMP1, PA3 for COMP2) 
																			 connected to comparator non inverting input */
#define COMP_NONINVERTINGINPUT_DAC1SWITCHCLOSED  COMP_CSR_COMP1SW1  /*!< DAC ouput connected to comparator COMP1 non inverting input */


/** @defgroup COMP_Output COMP Output
  * @{
  */

/* Output Redirection common for COMP1 and COMP2 */
#define COMP_OUTPUT_NONE                       (0x00000000U)                          /*!< COMP output isn't connected to other peripherals */
#define COMP_OUTPUT_TIM1BKIN                   COMP_CSR_COMP1OUTSEL_0                          /*!< COMP output connected to TIM1 Break Input (BKIN) */
#define COMP_OUTPUT_TIM1IC1                    COMP_CSR_COMP1OUTSEL_1                          /*!< COMP output connected to TIM1 Input Capture 1 */
#define COMP_OUTPUT_TIM1OCREFCLR               (COMP_CSR_COMP1OUTSEL_1|COMP_CSR_COMP1OUTSEL_0) /*!< COMP output connected to TIM1 OCREF Clear */
#define COMP_OUTPUT_TIM2IC4                    COMP_CSR_COMP1OUTSEL_2                          /*!< COMP output connected to TIM2 Input Capture 4 */
#define COMP_OUTPUT_TIM2OCREFCLR               (COMP_CSR_COMP1OUTSEL_2|COMP_CSR_COMP1OUTSEL_0) /*!< COMP output connected to TIM2 OCREF Clear */
#define COMP_OUTPUT_TIM3IC1                    (COMP_CSR_COMP1OUTSEL_2|COMP_CSR_COMP1OUTSEL_1) /*!< COMP output connected to TIM3 Input Capture 1 */
#define COMP_OUTPUT_TIM3OCREFCLR               COMP_CSR_COMP1OUTSEL                            /*!< COMP output connected to TIM3 OCREF Clear */
 

/** @defgroup COMP_OutputLevel COMP OutputLevel
  * @{
  */ 
/* When output polarity is not inverted, comparator output is low when
   the non-inverting input is at a lower voltage than the inverting input*/
#define COMP_OUTPUTLEVEL_LOW                   (0x00000000U)
/* When output polarity is not inverted, comparator output is high when
   the non-inverting input is at a higher voltage than the inverting input */
#define COMP_OUTPUTLEVEL_HIGH                  COMP_CSR_COMP1OUT


/** @defgroup COMP_TriggerMode COMP TriggerMode
  * @{
  */
#define COMP_TRIGGERMODE_NONE                  (0x00000000U)  /*!< No External Interrupt trigger detection */
#define COMP_TRIGGERMODE_IT_RISING             (0x00000001U)  /*!< External Interrupt Mode with Rising edge trigger detection */
#define COMP_TRIGGERMODE_IT_FALLING            (0x00000002U)  /*!< External Interrupt Mode with Falling edge trigger detection */
#define COMP_TRIGGERMODE_IT_RISING_FALLING     (0x00000003U)  /*!< External Interrupt Mode with Rising/Falling edge trigger detection */
#define COMP_TRIGGERMODE_EVENT_RISING          (0x00000010U)  /*!< Event Mode with Rising edge trigger detection */
#define COMP_TRIGGERMODE_EVENT_FALLING         (0x00000020U)  /*!< Event Mode with Falling edge trigger detection */
#define COMP_TRIGGERMODE_EVENT_RISING_FALLING  (0x00000030U)  /*!< Event Mode with Rising/Falling edge trigger detection */


/** @defgroup COMP_WindowMode COMP WindowMode
  * @{
  */
#define COMP_WINDOWMODE_DISABLE                (0x00000000U)  	/*!< Window mode disabled */
#define COMP_WINDOWMODE_ENABLE                 COMP_CSR_WNDWEN	/*!< Window mode enabled: non inverting input of comparator 2 */


/** 
  * @brief  COMP Init structure definition  
  */ 
typedef struct
{
	uint32_t InvertingInput;     /*!< Selects the inverting input of the comparator.
									This parameter can be a value of @ref COMP_InvertingInput */

	uint32_t NonInvertingInput;  /*!< Selects the non inverting input of the comparator.
									This parameter can be a value of @ref COMP_NonInvertingInput */

	uint32_t Output;             /*!< Selects the output redirection of the comparator.
									This parameter can be a value of @ref COMP_Output */

	uint32_t OutputPol;          /*!< Selects the output polarity of the comparator.
									This parameter can be a value of @ref COMP_OutputPolarity */

	uint32_t Hysteresis;         /*!< Selects the hysteresis voltage of the comparator.
									This parameter can be a value of @ref COMP_Hysteresis */

	uint32_t Mode;               /*!< Selects the operating comsumption mode of the comparator
									to adjust the speed/consumption.
									This parameter can be a value of @ref COMP_Mode */

	uint32_t WindowMode;         /*!< Selects the window mode of the comparator 1 & 2.
									This parameter can be a value of @ref COMP_WindowMode */

	uint32_t TriggerMode;        /*!< Selects the trigger mode of the comparator (interrupt mode).
									This parameter can be a value of @ref COMP_TriggerMode */
}COMP_InitTypeDef;

/** 
  * @brief  COMP Handle Structure definition  
  */ 
typedef struct
{
	COMP_TypeDef                *Instance; /*!< Register base address    */

}COMP_HandleTypeDef;

//---------------------------- Global Variables --------------------------------


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
void COMP_HAL_Init(COMPARATOR_INSTANCE_e compInstance, COMP_InitTypeDef *compPrams);

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
__weak void COMP_IRQ_Handler(COMPARATOR_INSTANCE_e compInstance);


#ifdef __cplusplus
}
#endif

#endif // __COMPARATOR_DRIVER__H
