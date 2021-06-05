/**
  ******************************************************************************
  * File Name          : SPIDriver.h
  * Description        : CRC Module related defines, data structure and functions.
  ******************************************************************************
 
  ******************************************************************************
  */

#ifndef __SPI_DRIVER__H
#define __SPI_DRIVER__H

#ifdef __cplusplus
 extern "C" {
#endif
	 
	 
//-------------------------------- Includes ------------------------------------
#include "stm32f0xx_hal.h"

//---------------------------- Defines & Structures ----------------------------

/** @defgroup SPI_Exported_Constants SPI Exported Constants
  * @{
  */

/** @defgroup SPI_Error_Code SPI Error Code
  * @{
  */
#define HAL_SPI_ERROR_NONE              (0x00000000U)   /*!< No error                               */
#define HAL_SPI_ERROR_MODF              (0x00000001U)   /*!< MODF error                             */
#define HAL_SPI_ERROR_CRC               (0x00000002U)   /*!< CRC error                              */
#define HAL_SPI_ERROR_OVR               (0x00000004U)   /*!< OVR error                              */
#define HAL_SPI_ERROR_FRE               (0x00000008U)   /*!< FRE error                              */
#define HAL_SPI_ERROR_DMA               (0x00000010U)   /*!< DMA transfer error                     */
#define HAL_SPI_ERROR_FLAG              (0x00000020U)   /*!< Error on RXNE/TXE/BSY/FTLVL/FRLVL Flag */
#define HAL_SPI_ERROR_ABORT             (0x00000040U)   /*!< Error during SPI Abort procedure       */
/**
  * @}
  */

/** @defgroup SPI_Mode SPI Mode
  * @{
  */
#define SPI_MODE_SLAVE                  (0x00000000U)
#define SPI_MODE_MASTER                 (SPI_CR1_MSTR | SPI_CR1_SSI)
/**
  * @}
  */

/** @defgroup SPI_Direction SPI Direction Mode
  * @{
  */
#define SPI_DIRECTION_2LINES            (0x00000000U)
#define SPI_DIRECTION_2LINES_RXONLY     SPI_CR1_RXONLY
#define SPI_DIRECTION_1LINE             SPI_CR1_BIDIMODE
/**
  * @}
  */

/** @defgroup SPI_Data_Size SPI Data Size
  * @{
  */
#define SPI_DATASIZE_4BIT               (0x00000300U)
#define SPI_DATASIZE_5BIT               (0x00000400U)
#define SPI_DATASIZE_6BIT               (0x00000500U)
#define SPI_DATASIZE_7BIT               (0x00000600U)
#define SPI_DATASIZE_8BIT               (0x00000700U)
#define SPI_DATASIZE_9BIT               (0x00000800U)
#define SPI_DATASIZE_10BIT              (0x00000900U)
#define SPI_DATASIZE_11BIT              (0x00000A00U)
#define SPI_DATASIZE_12BIT              (0x00000B00U)
#define SPI_DATASIZE_13BIT              (0x00000C00U)
#define SPI_DATASIZE_14BIT              (0x00000D00U)
#define SPI_DATASIZE_15BIT              (0x00000E00U)
#define SPI_DATASIZE_16BIT              (0x00000F00U)
/**
  * @}
  */

/** @defgroup SPI_Clock_Polarity SPI Clock Polarity
  * @{
  */
#define SPI_POLARITY_LOW                (0x00000000U)
#define SPI_POLARITY_HIGH               SPI_CR1_CPOL
/**
  * @}
  */

/** @defgroup SPI_Clock_Phase SPI Clock Phase
  * @{
  */
#define SPI_PHASE_1EDGE                 (0x00000000U)
#define SPI_PHASE_2EDGE                 SPI_CR1_CPHA
/**
  * @}
  */

/** @defgroup SPI_Slave_Select_management SPI Slave Select Management
  * @{
  */
#define SPI_NSS_SOFT                    SPI_CR1_SSM
#define SPI_NSS_HARD_INPUT              (0x00000000U)
#define SPI_NSS_HARD_OUTPUT             (SPI_CR2_SSOE << 16U)
/**
  * @}
  */

/** @defgroup SPI_NSSP_Mode SPI NSS Pulse Mode
  * @{
  */
#define SPI_NSS_PULSE_ENABLE            SPI_CR2_NSSP
#define SPI_NSS_PULSE_DISABLE           (0x00000000U)
/**
  * @}
  */

/** @defgroup SPI_BaudRate_Prescaler SPI BaudRate Prescaler
  * @{
  */
#define SPI_BAUDRATEPRESCALER_2         (0x00000000U)
#define SPI_BAUDRATEPRESCALER_4         (SPI_CR1_BR_0)
#define SPI_BAUDRATEPRESCALER_8         (SPI_CR1_BR_1)
#define SPI_BAUDRATEPRESCALER_16        (SPI_CR1_BR_1 | SPI_CR1_BR_0)
#define SPI_BAUDRATEPRESCALER_32        (SPI_CR1_BR_2)
#define SPI_BAUDRATEPRESCALER_64        (SPI_CR1_BR_2 | SPI_CR1_BR_0)
#define SPI_BAUDRATEPRESCALER_128       (SPI_CR1_BR_2 | SPI_CR1_BR_1)
#define SPI_BAUDRATEPRESCALER_256       (SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0)
/**
  * @}
  */

/** @defgroup SPI_MSB_LSB_transmission SPI MSB LSB Transmission
  * @{
  */
#define SPI_FIRSTBIT_MSB                (0x00000000U)
#define SPI_FIRSTBIT_LSB                SPI_CR1_LSBFIRST
/**
  * @}
  */

/** @defgroup SPI_TI_mode SPI TI Mode
  * @{
  */
#define SPI_TIMODE_DISABLE              (0x00000000U)
#define SPI_TIMODE_ENABLE               SPI_CR2_FRF
/**
  * @}
  */

/** @defgroup SPI_CRC_Calculation SPI CRC Calculation
  * @{
  */
#define SPI_CRCCALCULATION_DISABLE      (0x00000000U)
#define SPI_CRCCALCULATION_ENABLE       SPI_CR1_CRCEN
/**
  * @}
  */

/** @defgroup SPI_CRC_length SPI CRC Length
  * @{
  * This parameter can be one of the following values:
  *     SPI_CRC_LENGTH_DATASIZE: aligned with the data size
  *     SPI_CRC_LENGTH_8BIT    : CRC 8bit
  *     SPI_CRC_LENGTH_16BIT   : CRC 16bit
  */
#define SPI_CRC_LENGTH_DATASIZE         (0x00000000U)
#define SPI_CRC_LENGTH_8BIT             (0x00000001U)
#define SPI_CRC_LENGTH_16BIT            (0x00000002U)
/**
  * @}
  */

/** @defgroup SPI_FIFO_reception_threshold SPI FIFO Reception Threshold
  * @{
  * This parameter can be one of the following values:
  *     SPI_RXFIFO_THRESHOLD or SPI_RXFIFO_THRESHOLD_QF :
  *          RXNE event is generated if the FIFO
  *          level is greater or equal to 1/2(16-bits).
  *     SPI_RXFIFO_THRESHOLD_HF: RXNE event is generated if the FIFO
  *          level is greater or equal to 1/4(8 bits). */
#define SPI_RXFIFO_THRESHOLD            SPI_CR2_FRXTH
#define SPI_RXFIFO_THRESHOLD_QF         SPI_CR2_FRXTH
#define SPI_RXFIFO_THRESHOLD_HF         (0x00000000U)

/**
  * @}
  */


/** @defgroup SPI_transmission_fifo_status_level SPI Transmission FIFO Status Level
  * @{
  */
#define SPI_FTLVL_EMPTY                 (0x00000000U)
#define SPI_FTLVL_QUARTER_FULL          (0x00000800U)
#define SPI_FTLVL_HALF_FULL             (0x00001000U)
#define SPI_FTLVL_FULL                  (0x00001800U)

/**
  * @}
  */

/** @defgroup SPI_reception_fifo_status_level SPI Reception FIFO Status Level
  * @{
  */
#define SPI_FRLVL_EMPTY                 (0x00000000U)
#define SPI_FRLVL_QUARTER_FULL          (0x00000200U)
#define SPI_FRLVL_HALF_FULL             (0x00000400U)
#define SPI_FRLVL_FULL                  (0x00000600U)
/**
  * @}
  */
  
/**
  * @}
  */

/**
  * @brief  SPI Configuration Structure definition
  */
typedef struct
{
	uint32_t Mode;                /*!< Specifies the SPI operating mode.
									 This parameter can be a value of @ref SPI_Mode */

	uint32_t Direction;           /*!< Specifies the SPI bidirectional mode state.
									 This parameter can be a value of @ref SPI_Direction */

	uint32_t DataSize;            /*!< Specifies the SPI data size.
									 This parameter can be a value of @ref SPI_Data_Size */

	uint32_t CLKPolarity;         /*!< Specifies the serial clock steady state.
									 This parameter can be a value of @ref SPI_Clock_Polarity */

	uint32_t CLKPhase;            /*!< Specifies the clock active edge for the bit capture.
									 This parameter can be a value of @ref SPI_Clock_Phase */

	uint32_t NSS;                 /*!< Specifies whether the NSS signal is managed by
									 hardware (NSS pin) or by software using the SSI bit.
									 This parameter can be a value of @ref SPI_Slave_Select_management */

	uint32_t BaudRatePrescaler;   /*!< Specifies the Baud Rate prescaler value which will be
									 used to configure the transmit and receive SCK clock.
									 This parameter can be a value of @ref SPI_BaudRate_Prescaler
									 @note The communication clock is derived from the master
									 clock. The slave clock does not need to be set. */

	uint32_t FirstBit;            /*!< Specifies whether data transfers start from MSB or LSB bit.
									 This parameter can be a value of @ref SPI_MSB_LSB_transmission */

	uint32_t TIMode;              /*!< Specifies if the TI mode is enabled or not.
									 This parameter can be a value of @ref SPI_TI_mode */

	uint32_t CRCCalculation;      /*!< Specifies if the CRC calculation is enabled or not.
									 This parameter can be a value of @ref SPI_CRC_Calculation */

	uint32_t CRCPolynomial;       /*!< Specifies the polynomial used for the CRC calculation.
									 This parameter must be an odd number between Min_Data = 1 and Max_Data = 65535 */

	uint32_t CRCLength;           /*!< Specifies the CRC Length used for the CRC calculation.
									 CRC Length is only used with Data8 and Data16, not other data size
									 This parameter can be a value of @ref SPI_CRC_length */

	uint32_t NSSPMode;            /*!< Specifies whether the NSSP signal is enabled or not .
									 This parameter can be a value of @ref SPI_NSSP_Mode
									 This mode is activated by the NSSP bit in the SPIx_CR2 register and
									 it takes effect only if the SPI interface is configured as Motorola SPI
									 master (FRF=0) with capture on the first edge (SPIx_CR1 CPHA = 0,
									 CPOL setting is ignored).. */
}SPI_InitTypeDef;




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
void SPI_HAL_Init(SPI_InitTypeDef *spiConfigParams);

/*
+------------------------------------------------------------------------------
| Function : SPI_SetBufferAndSize(...)
+------------------------------------------------------------------------------
| Purpose: This is function used to set the buffer pointer and its size
+------------------------------------------------------------------------------
| Algorithms: 
|   	- In master mode, application does not know how much size will be 
|			receive at startup, So make initial buffer size for
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
void SPI_SetBufferAndSize(uint8_t *pTxData, uint8_t *pRxData, uint16_t size);
	
/*
+------------------------------------------------------------------------------
| Function : SPI_SendReceiveDataStream(...)
+------------------------------------------------------------------------------
| Purpose: This is Starts the data transfer over SPI
+------------------------------------------------------------------------------
| Algorithms: 
|   	- Transmitt and Received size should be same for SPI
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
void SPI_SendReceiveDataStream(uint8_t *pTxData, uint8_t *pRxData, uint16_t size);

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
__weak void SPI_IRQ_Handler(void);

uint8_t SPI_GetReceivedPacketSize(void);

#ifdef __cplusplus
}
#endif

#endif // __SPI_DRIVER__H
