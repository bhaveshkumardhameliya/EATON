/**
  ******************************************************************************
  * File Name          : GPIODriver.c
  * Description        : The GPIO initialization routines.
  ******************************************************************************
 
  ******************************************************************************
  */

//-------------------------------- Includes ------------------------------------
#include <string.h>
#include "GPIODriver.h"
	


//---------------------------- Defines & Structures ----------------------------



//---------------------------- Static Variables --------------------------------



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
void GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	// GPIO Ports Clock Enable
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();

	// Select on board LED_RED  
	GPIO_InitStruct.Pin = (LED_RED_Pin | LED_GREEN_Pin | LED_BLUE_Pin | LED_ORANGE_Pin);
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LED_Port, &GPIO_InitStruct);
	HAL_GPIO_WritePin(LED_Port, LED_RED_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED_Port, LED_GREEN_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED_Port, LED_BLUE_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED_Port, LED_ORANGE_Pin, GPIO_PIN_RESET);
	
	// Select Modbus Tx Pin 
	GPIO_InitStruct.Pin = MODBUS_TX_Pin | MODBUS_RX_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF1_USART1;
	HAL_GPIO_Init(MODBUS_TX_RX_GPIO_Port, &GPIO_InitStruct);

	// Select Debugg Tx and Rx Pin 
	GPIO_InitStruct.Pin = (DEBUGG_TX_Pin | DEBUGG_RX_Pin);
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF1_USART3;
	HAL_GPIO_Init(DEBUGG_TX_RX_GPIO_Port, &GPIO_InitStruct);

	memset(&GPIO_InitStruct, 0, sizeof(GPIO_InitTypeDef));
}

