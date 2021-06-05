/**
  ******************************************************************************
  * File Name          : CRCDriver.c
  * Description        : The CRC initialization routines.
  ******************************************************************************
  ******************************************************************************
  *
  *          This file provides firmware functions to manage the following 
  *          functionalities of the Cyclic Redundancy Check (CRC) peripheral:
  *           + Initialization and de-initialization functions
  *           + Peripheral Control functions 
  *           + Peripheral State functions
  *
**/

//-------------------------------- Includes ------------------------------------
#include "stm32f0xx_hal.h"

#ifdef HAL_CRC_MODULE_ENABLED	// enable this macro from stm32f0xx_hal_conf.h file 
								// to include SPI Module module in project.

//---------------------------- Defines & Structures ----------------------------


//---------------------------- Static Variables --------------------------------
CRC_HandleTypeDef CRCHandle;

//---------------------------- Global Variables --------------------------------


//------------------------- Extern Global Variables ----------------------------



//--------------------------- Private function prototypes ----------------------

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
void CRC_HAL_Init(CRC_InitTypeDef *CRCParams)
{
	/* Enable CRC AHB clock interface */
	__HAL_RCC_CRC_CLK_ENABLE();

	/* Set CRC Handle */
	CRCHandle.Instance = CRC;
	
	CRC_DeInit();
	
	/* Init the INIT register, as per selected polinomial method */
	CRC_SetInitRegister(CRCParams->InitValue);
	
	/* Select 8-bit polynomial size */
	CRC_PolynomialSizeSelect(CRCParams->CRCLength);
	
	/* Select input data revesal method */
	CRC_ReverseInputDataSelect(CRCParams->InputDataInversionMode);
	
	/* Select output data format */
	CRC_ReverseOutputDataCmd(CRCParams->OutputDataInversionMode);
	
	/* Select polynomial coeffiecinet value */
	CRC_SetPolynomial(CRCParams->PolynomialCoefficient);
}

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
void CRC_DeInit(void)
{
	/* Set DR register to reset value */
	CRCHandle.Instance->DR = 0xFFFFFFFF;

	/* Set the POL register to the reset value: 0x04C11DB7 */
	CRCHandle.Instance->POL = 0x04C11DB7;

	/* Reset IDR register */
	CRCHandle.Instance->IDR = 0x00;

	/* Set INIT register to reset value */
	CRCHandle.Instance->INIT = 0xFFFFFFFF;

	/* Reset the CRC calculation unit */
	CRCHandle.Instance->CR = CRC_CR_RESET;
}

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
void CRC_SetInitRegister(uint32_t CRC_InitValue)
{
	CRCHandle.Instance->INIT = CRC_InitValue;
}

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
void CRC_PolynomialSizeSelect(uint32_t CRC_PolSize)
{
	/* initialize CRC bit length*/
	MODIFY_REG(CRCHandle.Instance->CR, CRC_CR_POLYSIZE, CRC_PolSize);
}

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
|		uint32_t - data format method as per selection CRC method
|
+------------------------------------------------------------------------------
| Return Value: 
|		None
|  
+------------------------------------------------------------------------------
*/
void CRC_ReverseInputDataSelect(uint32_t CRC_ReverseInputData)
{
	/* set input data inversion mode */
	MODIFY_REG(CRCHandle.Instance->CR, CRC_CR_REV_IN, CRC_ReverseInputData);  
}

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
void CRC_ReverseOutputDataCmd(uint32_t OutputReverseMode)
{
	/* Enable reverse operation on output data */
	MODIFY_REG(CRCHandle.Instance->CR, CRC_CR_REV_OUT, OutputReverseMode); 
}

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
void CRC_SetPolynomial(uint32_t CRC_Pol)
{
	CRCHandle.Instance->POL = CRC_Pol;
}

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
void CRC_ResetDR(void)
{
	/* Reset CRC generator */
	CRCHandle.Instance->CR |= CRC_CR_RESET;
}

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
uint16_t CRC_8BitsCompute(uint8_t* data, uint32_t size, uint8_t resetCRC)
{
	uint8_t* dataEnd = data+size;

	if(resetCRC)
	{
		/* Reset CRC data register to avoid overlap when computing new data stream */
		CRC_ResetDR();
	}
	
	while(data < dataEnd)
	{
		/* Write the input data in the CRC data register */
		*(uint8_t*)(CRC_BASE) = (uint8_t) *data++;
	}

	/* Return the CRC value */
	return (CRCHandle.Instance->DR);
}

#endif //HAL_CRC_MODULE_ENABLED
