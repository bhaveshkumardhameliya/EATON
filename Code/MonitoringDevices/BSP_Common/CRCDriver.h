/**
  ******************************************************************************
  * File Name          : CRCDriver.h
  * Description        : CRC Module related defines, data structure and functions.
  ******************************************************************************
 
  ******************************************************************************
  */

#ifndef __CRC_DRIVER__H
#define __CRC_DRIVER__H

#ifdef __cplusplus
 extern "C" {
#endif
	 
	 
//-------------------------------- Includes ------------------------------------
#include "stm32f0xx_hal.h"

//---------------------------- Defines & Structures ----------------------------


/** @defgroup CRCEx_Polynomial_Sizes Polynomial sizes to configure the IP
  * @{
  */
#define CRC_POLYLENGTH_32B                  (0x00000000U)
#define CRC_POLYLENGTH_16B                  ((uint32_t)CRC_CR_POLYSIZE_0)
#define CRC_POLYLENGTH_8B                   ((uint32_t)CRC_CR_POLYSIZE_1)
#define CRC_POLYLENGTH_7B                   ((uint32_t)CRC_CR_POLYSIZE)

/** @defgroup CRCEx_Input_Data_Inversion Input Data Inversion Modes
  * @{
  */
#define CRC_INPUTDATA_INVERSION_NONE		(0x00000000U)
#define CRC_INPUTDATA_INVERSION_BYTE		((uint32_t)CRC_CR_REV_IN_0)
#define CRC_INPUTDATA_INVERSION_HALFWORD	((uint32_t)CRC_CR_REV_IN_1)
#define CRC_INPUTDATA_INVERSION_WORD		((uint32_t)CRC_CR_REV_IN)

/** @defgroup CRCEx_Output_Data_Inversion Output Data Inversion Modes
  * @{
  */
#define CRC_OUTPUTDATA_INVERSION_DISABLE         (0x00000000U)
#define CRC_OUTPUTDATA_INVERSION_ENABLE          ((uint32_t)CRC_CR_REV_OUT)

/** 
  * @brief CRC Init Structure definition  
  */ 
typedef struct
{
	/*!< Set CRC generating polynomial. 7, 8, 16 or 32-bit long value for a polynomial degree
	   respectively equal to 7, 8, 16 or 32. This field is written in normal representation, 
	   e.g., for a polynomial of degree 7, X^7 + X^6 + X^5 + X^2 + 1 is written 0x65.
	   No need to specify it if DefaultPolynomialUse is set to DEFAULT_POLYNOMIAL_ENABLE   */                                                
	uint32_t PolynomialCoefficient;      

	/*!< This parameter is a value of @ref CRCEx_Polynomial_Sizes and indicates CRC length.
	   Value can be either one of
	   CRC_POLYLENGTH_32B                  (32-bit CRC)
	   CRC_POLYLENGTH_16B                  (16-bit CRC)
	   CRC_POLYLENGTH_8B                   (8-bit CRC)
	   CRC_POLYLENGTH_7B                   (7-bit CRC) */
	uint32_t CRCLength;                 
	
	/*!< Init value to initiate CRC computation. No need to specify it if DefaultInitValueUse 
	   is set to DEFAULT_INIT_VALUE_ENABLE   */ 
	uint32_t InitValue;                                                                

	/*!< This parameter is a value of @ref CRCEx_Input_Data_Inversion and specifies input data inversion mode. 
	   Can be either one of the following values 
	   CRC_INPUTDATA_INVERSION_NONE      no input data inversion
	   CRC_INPUTDATA_INVERSION_BYTE      byte-wise inversion, 0x1A2B3C4D becomes 0x58D43CB2
	   CRC_INPUTDATA_INVERSION_HALFWORD  halfword-wise inversion, 0x1A2B3C4D becomes 0xD458B23C
	   CRC_INPUTDATA_INVERSION_WORD      word-wise inversion, 0x1A2B3C4D becomes 0xB23CD458 */  
	uint32_t InputDataInversionMode;    
	
	/*!< This parameter is a value of @ref CRCEx_Output_Data_Inversion and specifies output data (i.e. CRC) inversion mode.
		Can be either 
		CRC_OUTPUTDATA_INVERSION_DISABLE   no CRC inversion, or 
		CRC_OUTPUTDATA_INVERSION_ENABLE    CRC 0x11223344 is converted into 0x22CC4488 */                                           	
	uint32_t OutputDataInversionMode;   
	
}CRC_InitTypeDef;

typedef struct
{
  CRC_TypeDef                 *Instance;   /*!< Register base address        */ 
   
}CRC_HandleTypeDef;


/*
+------------------------------------------------------------------------------
| Function : CRC_HAL_Init(...)
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
void CRC_HAL_Init(CRC_InitTypeDef *CRCParams);

/*
+------------------------------------------------------------------------------
| Function : CRC_DeInit(...)
+------------------------------------------------------------------------------
| Purpose: This function resets all parameters of CRC module.
+------------------------------------------------------------------------------
| Algorithms: 
|		- Should be called from Init function before setting any parameters.
+------------------------------------------------------------------------------
| Parameters:  
|		None.s
|
+------------------------------------------------------------------------------
| Return Value: 
|		None
|  
+------------------------------------------------------------------------------
*/
void CRC_DeInit(void);

/*
+------------------------------------------------------------------------------
| Function : CRC_SetInitRegister(...)
+------------------------------------------------------------------------------
| Purpose: This function sets the initial value to calculate CRC.
+------------------------------------------------------------------------------
| Algorithms: 
|		Please select appropriate intial value as it can change complete CRC as 
|		result.
+------------------------------------------------------------------------------
| Parameters:  
|		uint32_t - initial value
|
+------------------------------------------------------------------------------
| Return Value: 
|		None
|  
+------------------------------------------------------------------------------
*/
void CRC_SetInitRegister(uint32_t CRCParams);

/*
+------------------------------------------------------------------------------
| Function : CRC_PolynomialSizeSelect(...)
+------------------------------------------------------------------------------
| Purpose: Selects the polynomial size
+------------------------------------------------------------------------------
| Algorithms: 
|		CRC_POLYLENGTH_7B: 7-bit polynomial for CRC calculation
|		CRC_POLYLENGTH_8B: 8-bit polynomial for CRC calculation
|		CRC_POLYLENGTH_16B: 16-bit polynomial for CRC calculation
|		CRC_POLYLENGTH_32B: 32-bit polynomial for CRC calculation
+------------------------------------------------------------------------------
| Parameters:  
|		uint32_t - Select CRC size as result
|
+------------------------------------------------------------------------------
| Return Value: 
|		None
|  
+------------------------------------------------------------------------------
*/
void CRC_PolynomialSizeSelect(uint32_t CRC_PolSize);

/*
+------------------------------------------------------------------------------
| Function : CRC_ReverseInputDataSelect(...)
+------------------------------------------------------------------------------
| Purpose: Selects the reverse operation to be performed on input data.
+------------------------------------------------------------------------------
| Algorithms: 
|	Specifies the reverse operation on input data.
|		This parameter can be:
|		CRC_INPUTDATA_INVERSION_NONE      no input data inversion
|		CRC_INPUTDATA_INVERSION_BYTE      byte-wise inversion, 0x1A2B3C4D becomes 0x58D43CB2
|		CRC_INPUTDATA_INVERSION_HALFWORD  halfword-wise inversion, 0x1A2B3C4D becomes 0xD458B23C
|		CRC_INPUTDATA_INVERSION_WORD      word-wise inversion, 0x1A2B3C4D becomes 0xB23CD458
|
+------------------------------------------------------------------------------
| Parameters:  
|		uint32_t - data reversal method as per selection CRC method
|
+------------------------------------------------------------------------------
| Return Value: 
|		None
|  
+------------------------------------------------------------------------------
*/
void CRC_ReverseInputDataSelect(uint32_t CRC_ReverseInputData);

/*
+------------------------------------------------------------------------------
| Function : CRC_ReverseOutputDataCmd(...)
+------------------------------------------------------------------------------
| Purpose: Enables or disable the reverse operation on output data.
+------------------------------------------------------------------------------
| Algorithms: 
|		The reverse operation on output data is performed on 32-bit.
|		CRC_OUTPUTDATA_INVERSION_DISABLE   no CRC inversion, or 
|		CRC_OUTPUTDATA_INVERSION_ENABLE    CRC 0x11223344 is converted into 0x22CC4488
|
+------------------------------------------------------------------------------
| Parameters:  
|		uint32_t - data format method as per selection CRC method
|
+------------------------------------------------------------------------------
| Return Value: 
|		None
|  
+------------------------------------------------------------------------------
*/
void CRC_ReverseOutputDataCmd(uint32_t OutputReverseMode);

/*
+------------------------------------------------------------------------------
| Function : CRC_SetPolynomial(...)
+------------------------------------------------------------------------------
| Purpose: Initializes the polynomail coefficients.
+------------------------------------------------------------------------------
| Algorithms: 
|		Select the appropriate polynomial coefficient based on CRC convertion 
|		menthod.
|
|		This can be found on below link
|			http://www.sunshine2k.de/coding/javascript/crc/crc_js.html
|
+------------------------------------------------------------------------------
| Parameters:  
|		uint32_t - Polynomial copefficient value
|
+------------------------------------------------------------------------------
| Return Value: 
|		None
|  
+------------------------------------------------------------------------------
*/
void CRC_SetPolynomial(uint32_t CRC_Pol);

/*
+------------------------------------------------------------------------------
| Function : CRC_ResetDR(...)
+------------------------------------------------------------------------------
| Purpose: Resets the CRC calculation unit and sets INIT register content in DR register.
+------------------------------------------------------------------------------
| Algorithms: 
|		This function must be called before starting new CRC for different data
|		stream.
|		This function will clear previously calculated CRC from its register.
|		Also it will set the initial value from INIT register for new CRC calculation.
|
+------------------------------------------------------------------------------
| Parameters:  
|		uint32_t - Polynomial copefficient value
|
+------------------------------------------------------------------------------
| Return Value: 
|		None
|  
+------------------------------------------------------------------------------
*/
void CRC_ResetDR(void);


/*
+------------------------------------------------------------------------------
| Function : CRC_8BitsCompute(...)
+------------------------------------------------------------------------------
| Purpose:  Enter 8-bit input data to the CRC calculator.
+------------------------------------------------------------------------------
| Algorithms: 
|
|
+------------------------------------------------------------------------------
| Parameters:  
|		uint8_t * - pBuffer pointer to the input data buffer
|		uint32_t -  input data buffer length
|		uint8_t - used in continous CRC checking in chunks
+------------------------------------------------------------------------------
| Return Value: 
|		uint16_t - result CRC of 16 bit. Actual results based on CRC bit method
|  
+------------------------------------------------------------------------------
*/
uint16_t CRC_8BitsCompute(uint8_t* data, uint32_t size, uint8_t resetCRC);

#ifdef __cplusplus
}
#endif

#endif // __CRC_DRIVER__H
