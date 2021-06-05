/**
  ******************************************************************************
  * File Name          : UARTDriver.h
  * Description        : UART Module related defines, data structure and functions.
  ******************************************************************************
 
  ******************************************************************************
  */

#ifndef __UART_DRIVER__H
#define __UART_DRIVER__H

#ifdef __cplusplus
 extern "C" {
#endif
	 
	 
//-------------------------------- Includes ------------------------------------
#include "stm32f0xx_hal.h"

//---------------------------- Defines & Structures ----------------------------
typedef enum
{
	UART1_INSTANCE,
	UART3_INSTANCE,
	MAX_UART_INSTANCE
	
}UART_INSTANCE_NUM_e;

/**
  * @brief UART Init Structure definition
  */
typedef struct
{
	uint32_t BaudRate;		/*!< This member configures the UART communication baud rate.
							   The baud rate register is computed using the following formula:
							   - If oversampling is 16 or in LIN mode (LIN mode not available on F030xx devices),
								  Baud Rate Register = ((PCLKx) / ((huart->Init.BaudRate)))
							   - If oversampling is 8,
								  Baud Rate Register[15:4] = ((2 * PCLKx) / ((huart->Init.BaudRate)))[15:4]
								  Baud Rate Register[3] =  0
								  Baud Rate Register[2:0] =  (((2 * PCLKx) / ((huart->Init.BaudRate)))[3:0]) >> 1U      */

	uint32_t WordLength;	/*!< Specifies the number of data bits transmitted or received in a frame.
								This parameter can be a value of @ref UARTEx_Word_Length. */

	uint32_t StopBits;		/*!< Specifies the number of stop bits transmitted.
								This parameter can be a value of @ref UART_Stop_Bits. */

	uint32_t Parity;		/*!< Specifies the parity mode.
							   This parameter can be a value of @ref UART_Parity
							   @note When parity is enabled, the computed parity is inserted
									 at the MSB position of the transmitted data (9th bit when
									 the word length is set to 9 data bits; 8th bit when the
									 word length is set to 8 data bits). */

	uint32_t Mode;			/*!< Specifies whether the Receive or Transmit mode is enabled or disabled.
							   This parameter can be a value of @ref UART_Mode. */

	uint32_t HwFlowCtl;		/*!< Specifies whether the hardware flow control mode is enabled
							   or disabled.
							   This parameter can be a value of @ref UART_Hardware_Flow_Control. */

	uint32_t OverSampling;	/*!< Specifies whether the Over sampling 8 is enabled or disabled, to achieve higher speed (up to f_PCLK/8).
							   This parameter can be a value of @ref UART_Over_Sampling. */

	uint32_t OneBitSampling;	/*!< Specifies whether a single sample or three samples' majority vote is selected.
								   Selecting the single sample method increases the receiver tolerance to clock
								   deviations. This parameter can be a value of @ref UART_OneBit_Sampling. */
}UART_InitTypeDef;



typedef struct
{
	USART_TypeDef		*pRegInstance;	// UART Register array as per datasheet
	UART_InitTypeDef	Init;           // UART communication parameters      
	uint8_t				*pTxBuffPtr;	// Pointer to UART Tx transfer Buffer 
	uint8_t				*pRxBuffPtr;	// Pointer to UART Rx transfer Buffer 
	uint8_t				txBufSize;		// Data recieved from Rx 
	uint8_t				rxBufSize;		// Data recieved from Rx 
	
}UART_INSTANT_t;

//---------------------------- Global Variables --------------------------------
extern UART_INSTANT_t gUart1Instant;
extern UART_INSTANT_t gUart3Instant;


#define SEND_DATA(__UART_INSTANCE__, __DATA__) 	\
					(((__UART_INSTANCE__) == UART1_INSTANCE) ? \
						WRITE_REG(gUart1Instant.pRegInstance->TDR, ((__DATA__) &0xFF)) : \
						WRITE_REG(gUart3Instant.pRegInstance->TDR, ((__DATA__) &0xFF)) )

//--------------------------- Private function prototypes ----------------------
void Init_UARTs(void);

/*
+------------------------------------------------------------------------------
| Function : UART_Init(...)
+------------------------------------------------------------------------------
| Purpose: This function initializes the UART 2 for Modbus communication.
+------------------------------------------------------------------------------
| Algorithms: 
|           
|			
|	@note: The global instance for UART1/UART3 is used to send and recieve data over 
|			Modbus.
|		   The UART instance number as follows
|			- UART2_INSTANCE for Modbus Uart
|			- UART3_INSTANCE for Debugg port
|
+------------------------------------------------------------------------------
| Parameters:
|		UART_INSTANCE_NUM_e - Uart Instance number
|  		UART_InitTypeDef * - Uart Communication parameters
+------------------------------------------------------------------------------
| Return Value: 
|		0 = Success
|		1 = fail
|  
+------------------------------------------------------------------------------
*/
int UART_Init(UART_INSTANCE_NUM_e uartInstanceNo, UART_InitTypeDef *uartParams);
	
/*
+------------------------------------------------------------------------------
| Function : UART_Enable(...)
+------------------------------------------------------------------------------
| Purpose: This function enables UART module.
+------------------------------------------------------------------------------
| Algorithms: 
|       
|		
|
+------------------------------------------------------------------------------
| Parameters:  
|		UART_INSTANT_t * - Uart Instance
|
+------------------------------------------------------------------------------
| Return Value: 
|		None
|  
+------------------------------------------------------------------------------
*/
void UART_Enable(UART_INSTANT_t *uartInstance);

/*
+------------------------------------------------------------------------------
| Function : UART_Disable(...)
+------------------------------------------------------------------------------
| Purpose: This function disables UART module.
+------------------------------------------------------------------------------
| Algorithms: 
|       
|		
|
+------------------------------------------------------------------------------
| Parameters:  
|		UART_INSTANT_t * - Uart Instance
|
+------------------------------------------------------------------------------
| Return Value: 
|		None
|  
+------------------------------------------------------------------------------
*/
void UART_Disable(UART_INSTANT_t *uartInstance);

/*
+------------------------------------------------------------------------------
| Function : UART_StartTxInterrupt(...)
+------------------------------------------------------------------------------
| Purpose: This function enables transmitt interrupt bit in USART status register.
+------------------------------------------------------------------------------
| Algorithms: 
|       - Initialize CR1, CR2, CR3 registers
|		- Initialize the baudrate			
|
+------------------------------------------------------------------------------
| Parameters:  
|		UART_INSTANCE_NUM_e - Uart Instance number
|		uint8_t	- ENABLE = 1, DISABLE = 0
|
+------------------------------------------------------------------------------
| Return Value: 
|		None
|  
+------------------------------------------------------------------------------
*/
void UART_StartTxInterrupt(UART_INSTANCE_NUM_e uartInstanceNo, uint8_t enableDisableStatus);

uint8_t UART_TransmitData(UART_INSTANCE_NUM_e uartInstanceNo, 
							uint8_t *pData, 
							uint16_t dataSize, 
							uint32_t dataXferTimeout);

/*
+------------------------------------------------------------------------------
| Function : UART1_RX_Handler(...)
+------------------------------------------------------------------------------
| Purpose: This is weak receiving function of UART1.
+------------------------------------------------------------------------------
| Algorithms: 
|       This function must be implemented in User file to handle recieved data 
|		of UART1		
|
+------------------------------------------------------------------------------
| Parameters:  
|		uint16_t - received data
|
+------------------------------------------------------------------------------
| Return Value: 
|		None
|  
+------------------------------------------------------------------------------
*/
__weak void UART1_RX_Handler(uint16_t receivedData);


/*
+------------------------------------------------------------------------------
| Function : UART1_TX_Handler(...)
+------------------------------------------------------------------------------
| Purpose: This is weak transmitting function of UART1.
+------------------------------------------------------------------------------
| Algorithms: 
|       This function must be implemented in User file to handle transmission
|		of UART3		
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
__weak void UART1_TX_Handler(void);


/*
+------------------------------------------------------------------------------
| Function : UART3_RX_Handler(...)
+------------------------------------------------------------------------------
| Purpose: This is weak receiving function of UART3.
+------------------------------------------------------------------------------
| Algorithms: 
|       This function must be implemented in User file to handle recieved data 
|		of UART3		
|
+------------------------------------------------------------------------------
| Parameters:  
|		uint16_t - received data
|
+------------------------------------------------------------------------------
| Return Value: 
|		None
|  
+------------------------------------------------------------------------------
*/
__weak void UART3_RX_Handler(uint16_t receivedData);

/*
+------------------------------------------------------------------------------
| Function : UART3_TX_Handler(...)
+------------------------------------------------------------------------------
| Purpose: This is weak transmitting function of UART3.
+------------------------------------------------------------------------------
| Algorithms: 
|       This function must be implemented in User file to handle transmission
|		of UART3		
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
__weak void UART3_TX_Handler(void);


#ifdef __cplusplus
}
#endif

#endif // __UART_DRIVER__H
