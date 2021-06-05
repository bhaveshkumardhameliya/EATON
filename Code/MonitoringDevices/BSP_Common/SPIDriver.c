/**
  ******************************************************************************
  * File Name          : SPIDriver.c
  * Description        : The SPI initialization routines.
  ******************************************************************************
  ******************************************************************************
  *
  *          This file provides firmware functions to manage the following
  *          functionalities of the Serial Peripheral Interface (SPI) peripheral:
  *           + Initialization and de-initialization functions
  *           + IO operation functions
  *           + Peripheral Control functions
  *           + Peripheral State functions
  *
**/

//-------------------------------- Includes ------------------------------------
#include "stm32f0xx_hal.h"
#include "GPIODriver.h"

#ifdef HAL_SPI_MODULE_ENABLED	// enable this macro from stm32f0xx_hal_conf.h file 
								// to include SPI Module module in project.

//---------------------------- Defines & Structures ----------------------------

/** @defgroup SPI_Interrupt_definition SPI Interrupt Definition
  * @{
  */
#define SPI_IT_TXE                      SPI_CR2_TXEIE
#define SPI_IT_RXNE                     SPI_CR2_RXNEIE
#define SPI_IT_ERR                      SPI_CR2_ERRIE
/**
  * @}
  */

/** @defgroup SPI_Flags_definition SPI Flags Definition
  * @{
  */
#define SPI_FLAG_RXNE                   SPI_SR_RXNE   /* SPI status flag: Rx buffer not empty flag       */
#define SPI_FLAG_TXE                    SPI_SR_TXE    /* SPI status flag: Tx buffer empty flag           */
#define SPI_FLAG_BSY                    SPI_SR_BSY    /* SPI status flag: Busy flag                      */
#define SPI_FLAG_CRCERR                 SPI_SR_CRCERR /* SPI Error flag: CRC error flag                  */
#define SPI_FLAG_MODF                   SPI_SR_MODF   /* SPI Error flag: Mode fault flag                 */
#define SPI_FLAG_OVR                    SPI_SR_OVR    /* SPI Error flag: Overrun flag                    */
#define SPI_FLAG_FRE                    SPI_SR_FRE    /* SPI Error flag: TI mode frame format error flag */
#define SPI_FLAG_FTLVL                  SPI_SR_FTLVL  /* SPI fifo transmission level                     */
#define SPI_FLAG_FRLVL                  SPI_SR_FRLVL  /* SPI fifo reception level                        */
/**
  * @}
  */

/** @brief  Enable the SPI peripheral.
  * @param  __HANDLE__ specifies the SPI Handle.
  *         This parameter can be SPI where x: 1, 2, or 3 to select the SPI peripheral.
  * @retval None
  */
#define __HAL_SPI_ENABLE(__HANDLE__)  SET_BIT((__HANDLE__)->Instance->CR1, SPI_CR1_SPE)

/** @brief  Disable the SPI peripheral.
  * @param  __HANDLE__ specifies the SPI Handle.
  *         This parameter can be SPI where x: 1, 2, or 3 to select the SPI peripheral.
  * @retval None
  */
#define __HAL_SPI_DISABLE(__HANDLE__) CLEAR_BIT((__HANDLE__)->Instance->CR1, SPI_CR1_SPE)

/** @brief  Enable the specified SPI interrupts.
  * @param  __HANDLE__ specifies the SPI Handle.
  *         This parameter can be SPI where x: 1, 2, or 3 to select the SPI peripheral.
  * @param  __INTERRUPT__ specifies the interrupt source to enable.
  *         This parameter can be one of the following values:
  *            @arg SPI_IT_TXE: Tx buffer empty interrupt enable
  *            @arg SPI_IT_RXNE: RX buffer not empty interrupt enable
  *            @arg SPI_IT_ERR: Error interrupt enable
  * @retval None
  */
#define __HAL_SPI_ENABLE_IT(__HANDLE__, __INTERRUPT__)   SET_BIT((__HANDLE__)->Instance->CR2, (__INTERRUPT__))

/** @brief  Disable the specified SPI interrupts.
  * @param  __HANDLE__ specifies the SPI handle.
  *         This parameter can be SPIx where x: 1, 2, or 3 to select the SPI peripheral.
  * @param  __INTERRUPT__ specifies the interrupt source to disable.
  *         This parameter can be one of the following values:
  *            @arg SPI_IT_TXE: Tx buffer empty interrupt enable
  *            @arg SPI_IT_RXNE: RX buffer not empty interrupt enable
  *            @arg SPI_IT_ERR: Error interrupt enable
  * @retval None
  */
#define __HAL_SPI_DISABLE_IT(__HANDLE__, __INTERRUPT__)  CLEAR_BIT((__HANDLE__)->Instance->CR2, (__INTERRUPT__))

/** @brief  Check whether the specified SPI interrupt source is enabled or not.
  * @param  __HANDLE__ specifies the SPI Handle.
  *         This parameter can be SPI where x: 1, 2, or 3 to select the SPI peripheral.
  * @param  __INTERRUPT__ specifies the SPI interrupt source to check.
  *          This parameter can be one of the following values:
  *            @arg SPI_IT_TXE: Tx buffer empty interrupt enable
  *            @arg SPI_IT_RXNE: RX buffer not empty interrupt enable
  *            @arg SPI_IT_ERR: Error interrupt enable
  * @retval The new state of __IT__ (TRUE or FALSE).
  */
#define __HAL_SPI_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__) ((((__HANDLE__)->Instance->CR2 & (__INTERRUPT__)) == (__INTERRUPT__)) ? SET : RESET)

/** @brief  Check whether the specified SPI flag is set or not.
  * @param  __HANDLE__ specifies the SPI Handle.
  *         This parameter can be SPI where x: 1, 2, or 3 to select the SPI peripheral.
  * @param  __FLAG__ specifies the flag to check.
  *         This parameter can be one of the following values:
  *            @arg SPI_FLAG_RXNE: Receive buffer not empty flag
  *            @arg SPI_FLAG_TXE: Transmit buffer empty flag
  *            @arg SPI_FLAG_CRCERR: CRC error flag
  *            @arg SPI_FLAG_MODF: Mode fault flag
  *            @arg SPI_FLAG_OVR: Overrun flag
  *            @arg SPI_FLAG_BSY: Busy flag
  *            @arg SPI_FLAG_FRE: Frame format error flag
  *            @arg SPI_FLAG_FTLVL: SPI fifo transmission level
  *            @arg SPI_FLAG_FRLVL: SPI fifo reception level
  * @retval The new state of __FLAG__ (TRUE or FALSE).
  */
#define __HAL_SPI_GET_FLAG(__HANDLE__, __FLAG__) ((((__HANDLE__)->Instance->SR) & (__FLAG__)) == (__FLAG__))

typedef enum
{
	SPI_IN_MASTER_MODE,
	SPI_IN_SLAVE_MODE,
	MAX_SPI_MODE
	
}SPI_MODE_e;

/**
  * @brief  SPI handle Structure definition
  */
typedef struct
{
	SPI_TypeDef			*Instance;      		/*!< SPI registers base address */
	SPI_MODE_e			spiMode;				/*!< Master or slave Mode */
	uint8_t				*pTxBuffPtr;			/*!< Pointer to SPI Tx transfer Buffer */
	uint8_t				*pRxBuffPtr;			/*!< Pointer to SPI Rx transfer Buffer */
	uint8_t				currPacketTxBufPos;		/*!< Current Packet Tx buffer transfer position*/ 
	uint8_t				currPacketRxBufPos;		/*!< Current Packet Rx buffer transfer position*/ 
	uint8_t				txBufSize;				/*!< SPI Tx Transfer size */ 
	uint8_t				rxBufSize;				/*!< SPI Rx Transfer size */ 
	uint8_t				spiErrorFlg;

} SPI_HandleTypeDef;

//---------------------------- Static Variables --------------------------------
SPI_HandleTypeDef spiHandle;


//---------------------------- Global Variables --------------------------------


//------------------------- Extern Global Variables ----------------------------



//--------------------------- Private function prototypes ----------------------

/*
+------------------------------------------------------------------------------
| Function : SPI_HAL_Init(...)
+------------------------------------------------------------------------------
| Purpose: This function initialiazes SPI parameters in its SFR 
+------------------------------------------------------------------------------
| Algorithms: 
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
void SPI_HAL_Init(SPI_InitTypeDef *spiConfigParams)
{
	uint32_t frxth = 0;
	
	/* SPI Peripheral clock enable */
	__HAL_RCC_SPI1_CLK_ENABLE();
	
	/* SPI1 interrupt Init */
    HAL_NVIC_SetPriority(SPI1_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(SPI1_IRQn);
	
	/* Initialize SPI register array */
	spiHandle.Instance = SPI1;
	
	if(spiConfigParams->Mode == SPI_MODE_SLAVE)
	{
		spiHandle.spiMode = SPI_IN_SLAVE_MODE;
	}
	else
	{
		spiHandle.spiMode = SPI_IN_MASTER_MODE;
	}
	
	/* Disable the selected SPI peripheral */
	__HAL_SPI_DISABLE(&spiHandle);
	
	/* Align by default the rs fifo threshold on the data size */
	if (spiConfigParams->DataSize > SPI_DATASIZE_8BIT)
	{
		frxth = SPI_RXFIFO_THRESHOLD_HF;
	}
	else
	{
		frxth = SPI_RXFIFO_THRESHOLD_QF;
	}

	/* CRC calculation is valid only for 16Bit and 8 Bit */
	if ((spiConfigParams->DataSize != SPI_DATASIZE_16BIT) && 
		(spiConfigParams->DataSize != SPI_DATASIZE_8BIT))
	{
		/* CRC must be disabled */
		spiConfigParams->CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	}

	/* Align the CRC Length on the data size */
	if (spiConfigParams->CRCLength == SPI_CRC_LENGTH_DATASIZE)
	{
		/* CRC Length aligned on the data size : value set by default */
		if (spiConfigParams->DataSize > SPI_DATASIZE_8BIT)
		{
			spiConfigParams->CRCLength = SPI_CRC_LENGTH_16BIT;
		}
		else
		{
			spiConfigParams->CRCLength = SPI_CRC_LENGTH_8BIT;
		}
	}

	/*----------------------- SPIx CR1 & CR2 Configuration ---------------------*/
	/* Configure : SPI Mode, Communication Mode, Clock polarity and phase, NSS management,
	Communication speed, First bit and CRC calculation state */
	WRITE_REG(spiHandle.Instance->CR1, (spiConfigParams->Mode | 
									spiConfigParams->Direction |
									spiConfigParams->CLKPolarity | 
									spiConfigParams->CLKPhase | 
									(spiConfigParams->NSS & SPI_CR1_SSM) |
									spiConfigParams->BaudRatePrescaler | 
									spiConfigParams->FirstBit | 
									spiConfigParams->CRCCalculation));	
	
	/* Configure : NSS management, TI Mode, NSS Pulse, Data size and Rx Fifo Threshold */
	WRITE_REG(spiHandle.Instance->CR2, (((spiConfigParams->NSS >> 16U) & SPI_CR2_SSOE) | 
											spiConfigParams->TIMode |
											spiConfigParams->NSSPMode | 
											spiConfigParams->DataSize) | 
										frxth);	
	
	/*--------------------- SPIx Interrupt Configuration ---------------------*/
	/* Enable SPI_IT_TXE, SPI_IT_RXNE and ERR interrupt */
	__HAL_SPI_ENABLE_IT(&spiHandle, (SPI_IT_RXNE | SPI_IT_ERR));
	//__HAL_SPI_ENABLE_IT(&spiHandle, SPI_IT_TXE);
	
	/* Enable SPI peripheral */
    __HAL_SPI_ENABLE(&spiHandle);
}

/*
+------------------------------------------------------------------------------
| Function : SPI_SetBufferAndSize(...)
+------------------------------------------------------------------------------
| Purpose: This is function used to set the buffer pointer and its size
+------------------------------------------------------------------------------
| Algorithms: 
|   	- In master mode, application does not know how much size will be 
|			receive at startup, this API will initialize buffer size and ptr
|		- This call must be made from application after SPI_HAL_Init function,
|			with proper parameters to avoid buffer overflow condition.
|	
+------------------------------------------------------------------------------
| Parameters:  
|		uint8_t * - Transmitt buffer ptr
|		uint8_t * - Received buffer ptr
|		uint16_t - Size of transmitt and receive buffer, should be same in app
|
+------------------------------------------------------------------------------
| Return Value: 
|		None
|  
+------------------------------------------------------------------------------
*/
void SPI_SetBufferAndSize(uint8_t *pTxData, uint8_t *pRxData, uint16_t size)
{
	/* Set Buffer ptrs of application and its size */
	spiHandle.pTxBuffPtr = (uint8_t *)pTxData;
	spiHandle.txBufSize = size;
	spiHandle.pRxBuffPtr = (uint8_t *)pRxData;
	spiHandle.rxBufSize = size;
	
	/* Set the first byte of packet as slave don't know when data is going to transfer */
	if(SPI_IN_SLAVE_MODE == spiHandle.spiMode)
	{
		/* Trannsfer data to transmit, Set initial data into data register */
		*(__IO uint8_t *)&spiHandle.Instance->DR = spiHandle.pTxBuffPtr[0];
		*(__IO uint8_t *)&spiHandle.Instance->DR = spiHandle.pTxBuffPtr[1];
		
		spiHandle.currPacketTxBufPos = 2;
	}
}

/*
+------------------------------------------------------------------------------
| Function : SPI_SendReceiveDataStream(...)
+------------------------------------------------------------------------------
| Purpose: This is Starts the data transfer over SPI
+------------------------------------------------------------------------------
| Algorithms: 
|   	- Transmit and Receive size should be same for SPI
|		- Transmit and Receive data will be performed in ISR
|		- This routine capture the 
|	
+------------------------------------------------------------------------------
| Parameters:  
|		uint8_t * - Transmitt buffer ptr
|		uint8_t * - Received buffer ptr
|		uint16_t - Size of transmitt and receive buffer, should be same in app
|
+------------------------------------------------------------------------------
| Return Value: 
|		None
|  
+------------------------------------------------------------------------------
*/
void SPI_SendReceiveDataStream(uint8_t *pTxData, uint8_t *pRxData, uint16_t size)
{
	/* Set the transaction information */
	spiHandle.pTxBuffPtr = (uint8_t *)pTxData;
	spiHandle.txBufSize = size;
	spiHandle.pRxBuffPtr = (uint8_t *)pRxData;
	spiHandle.rxBufSize = size;
	
	/* set the initial size of trasfer packet to zero for RX and TX */
	spiHandle.currPacketRxBufPos = 0;
	spiHandle.currPacketTxBufPos = 0;
	
	if(SPI_IN_MASTER_MODE == spiHandle.spiMode)
	{
		/* Enable TXE, RXNE and ERR interrupt */
		/*TODO: Need to verify interrupt enable during intialization
				if SPI has same error like UART then we should not initialize at INIT */
		__HAL_SPI_ENABLE_IT(&spiHandle, (SPI_IT_TXE | SPI_IT_RXNE | SPI_IT_ERR));
	}
	else
	{
		/* Trannsfer data to transmit, Set initial data into data register */
		*(__IO uint8_t *)&spiHandle.Instance->DR = spiHandle.pTxBuffPtr[0];
		*(__IO uint8_t *)&spiHandle.Instance->DR = spiHandle.pTxBuffPtr[1];
		
		spiHandle.currPacketTxBufPos = 2;
	}
}

/*
+------------------------------------------------------------------------------
| Function : SPI1_IRQHandler(...)
+------------------------------------------------------------------------------
| Purpose: This is IRQ function for SPI 1 Module
+------------------------------------------------------------------------------
| Algorithms: 
|   	
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
void SPI1_IRQHandler(void) 
{
	if((__HAL_SPI_GET_FLAG(&spiHandle, SPI_FLAG_RXNE) != RESET) && 
		__HAL_SPI_GET_IT_SOURCE(&spiHandle, SPI_IT_RXNE) != RESET)
	{
		/* Received all data */
		if(spiHandle.currPacketRxBufPos >= spiHandle.rxBufSize)
		{
			/* Call the callback function to indicate reception is completed */
			/* Intention to have callback, 
				no need to set the flag of spi data available from here and 
				it should be module independent */
			
			/* Generate Overrun error of SPI */
		}
		else
		{
			/* Collect the received data */
			spiHandle.pRxBuffPtr[spiHandle.currPacketRxBufPos] = *((__IO uint8_t *)&spiHandle.Instance->DR);
			/* In case of Master, the amount of data transfer is only amount of data receive 
				So in Master also we know the data size */
			spiHandle.currPacketRxBufPos++;
		}
	}
	
	if(SPI_IN_MASTER_MODE == spiHandle.spiMode)
	{
		if((__HAL_SPI_GET_FLAG(&spiHandle, SPI_FLAG_TXE) != RESET) && 
			__HAL_SPI_GET_IT_SOURCE(&spiHandle, SPI_IT_TXE) != RESET)
		{
			/* No Further data to transmit */
			if(spiHandle.currPacketTxBufPos >= spiHandle.txBufSize)
			{
				/* Need to disable interrrupt for Transmit*/
				/* Disable TXE interrupt */
				__HAL_SPI_DISABLE_IT(&spiHandle, SPI_IT_TXE);
			}
			else
			{
				/* Trannsfer data to transmit */
				*(__IO uint8_t *)&spiHandle.Instance->DR = spiHandle.pTxBuffPtr[spiHandle.currPacketTxBufPos];
				spiHandle.currPacketTxBufPos++;
			}
		}
	}
	else
	{
		if(__HAL_SPI_GET_FLAG(&spiHandle, SPI_FLAG_TXE) != RESET)
		{
			/* No Further data to transmit */
			if(spiHandle.currPacketTxBufPos < spiHandle.txBufSize)			
			{
				/* Trannsfer data to transmit */
				*(__IO uint8_t *)&spiHandle.Instance->DR = spiHandle.pTxBuffPtr[spiHandle.currPacketTxBufPos];
				spiHandle.currPacketTxBufPos++;
			}
		}
	}
	
	if((__HAL_SPI_GET_FLAG(&spiHandle, (SPI_FLAG_MODF | SPI_FLAG_OVR | SPI_FLAG_FRE)) != RESET) && 
		__HAL_SPI_GET_IT_SOURCE(&spiHandle, SPI_IT_ERR) != RESET)
	{
		spiHandle.spiErrorFlg = 1;
	}

	
	/* call the user function if any processing is required for that */
	SPI_IRQ_Handler();
}

/*
+------------------------------------------------------------------------------
| Function : SPI_IRQ_Handler(...)
+------------------------------------------------------------------------------
| Purpose: This is IRQ Handler function for SPI
+------------------------------------------------------------------------------
| Algorithms: 
|   	- This function must be implemented in User file to handle SPI data transfer
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
__weak void SPI_IRQ_Handler(void)
{
	/* NOTE : This function Should not be modified, when the callback is needed,
	the SPI_IRQ_Handler could be implemented in the user file
	*/
}

uint8_t SPI_GetReceivedPacketSize(void)
{
	return spiHandle.currPacketRxBufPos;
}

#endif //HAL_SPI_MODULE_ENABLED
