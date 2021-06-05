/**
  ******************************************************************************
  * File Name          : GPIODriver.h
  * Description        : GPIO related defines, data structure and functions.
  ******************************************************************************
 
  ******************************************************************************
  */

#ifndef __GPIO_DRIVER__H
#define __GPIO_DRIVER__H

#ifdef __cplusplus
 extern "C" {
#endif
	 
	 
//-------------------------------- Includes ------------------------------------
#include "stm32f0xx_hal.h"

//---------------------------- Defines & Structures ----------------------------
#define MODBUS_TX_Pin				GPIO_PIN_9
#define MODBUS_RX_Pin				GPIO_PIN_10
#define MODBUS_TX_RX_GPIO_Port		GPIOA
#define MODBUS_RX_GPIO_Port			GPIOA


#define DEBUGG_TX_Pin				GPIO_PIN_10
#define DEBUGG_RX_Pin				GPIO_PIN_11
#define DEBUGG_TX_RX_GPIO_Port		GPIOC

//Digital Input Port Defination
#define INPUT_PortB					GPIOB
#define INPUT_PortE					GPIOE
#define INPUT_PortF					GPIOF

#define LED_RED_Pin					GPIO_PIN_6
#define LED_BLUE_Pin				GPIO_PIN_7
#define LED_ORANGE_Pin				GPIO_PIN_8
#define LED_GREEN_Pin				GPIO_PIN_9
#define LED_Port					GPIOC



//---------------------------- Global Variables --------------------------------


//--------------------------- Private function prototypes ----------------------
/*
+------------------------------------------------------------------------------
| Function : GPIO_Init(...)
+------------------------------------------------------------------------------
| Purpose: This function initializes all IO, Alternate function, Analog Pins
+------------------------------------------------------------------------------
| Algorithms: 
|           
|			
|	@note: Add Details for all pin muxing information here
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
void GPIO_Init(void);

#ifdef __cplusplus
}
#endif

#endif // __GPIO_DRIVER__H
