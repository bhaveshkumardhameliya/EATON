/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
 
  ******************************************************************************
  */

//-------------------------------- Includes ------------------------------------
#include "stm32f0xx_hal.h"

#include "debugger.h"
#include "GPIODriver.h"
#include "TimerHandler.h"
#include "MonitoringDeviceHandler.h"
#include "IWDGDriver.h"

//---------------------------- Defines & Structures ----------------------------
#define ROM_CHUNK_SIZE			32
#define ROM_SIZE_FOR_CODE		0x7000
#define ROM_TOTAL_BLOCK			(ROM_SIZE_FOR_CODE/ROM_CHUNK_SIZE)		

//#define SW_FMEA_CORRUPT_FLASH_DATA

//---------------------------- Static Variables --------------------------------
const uint32_t FLASH_START_ADDRESS = 0x08000000; // Flash start address
const uint32_t FLASH_LENGTH = 0x00020000; // Flash size minus the 4 bytes for CRC


//---------------------------- Global Variables --------------------------------
__IO uint32_t  TimingDelay = 0;
__IO uint8_t   Counter = 0;

// place variable1 in a section called .ARM.__AT_0x0801FFFC
uint32_t romCRC __attribute__((at(0x8008000))) = 0x9401;		//0x801FAEC

//--------------------------- Private function prototypes ----------------------
void Delay(__IO uint32_t nTime);
void ValidateFlashCRC(void);
void ValidateCompleteFlashCRC(void);

int main(void)
{
	CRC_InitTypeDef crcConfig;
	
	//------------------------- MCU Configuration---------------------------------
	// Check reset due to IWDG timer 
	if(__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) == SET) 
	{                      
		__HAL_RCC_CLEAR_RESET_FLAGS();   // Clear Reset Flags
		
		// Disable watchdog @Power ON till MCU intialize, 
		// watchdog is active after a watchdog reset
		IWDG_Disable();
	}
	
	// Reset of all peripherals, Initializes the Flash interface and the Systick.
	HAL_Init();
	
	GPIO_Init();
	
	// Update System Clock 
	SystemCoreClockUpdate();
	
	// Enable Clock Security System(CSS): this will generate an NMI exception
	// when HSE clock fails
	HAL_RCC_EnableCSS();

	Init_UARTs();
	
	Timers_Initialization();
	
	MonitoringDeviceInit();
	
	UART0_SendWelcomeMsg();
	
	crcConfig.InitValue = 0xFFFF;
	crcConfig.PolynomialCoefficient = 0x8005;
	crcConfig.CRCLength = CRC_POLYLENGTH_16B;
	crcConfig.InputDataInversionMode = CRC_INPUTDATA_INVERSION_BYTE;
	crcConfig.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_ENABLE;
	CRC_HAL_Init(&crcConfig);
	
	ValidateCompleteFlashCRC();
	
	IWDG_Init();

	
	while(1)
	{
		IWDG_Refresh();
		
		ProcessInComingDataFromDevice();
		
		ProcessMonitoringDeviceData();
		
		if(gPerformOneMsecJobs)
		{
			gPerformOneMsecJobs = 0;
		}
		
		if(gPerformHundreadMsecJobs)
		{
			gPerformHundreadMsecJobs = 0;
			HundreadMiliSecJobs();
			CheckDeviceAvailability();
		}
		
		if(gPerformOneSecJobs)
		{
			gPerformOneSecJobs = 0;
			OneSecJobs();
		}
		
		ProcessDebuggCommand();
	}
}

/*
+------------------------------------------------------------------------------
| Function : Delay(...)
+------------------------------------------------------------------------------
| Purpose: This function provides delay functionality
+------------------------------------------------------------------------------
| Algorithms: 
|           
|			
|	@note: This is blocking function call.
|
+------------------------------------------------------------------------------
| Parameters:  
|  		uint32_t - Delay time number in msec
+------------------------------------------------------------------------------
| Return Value: 
|		None
|  
+------------------------------------------------------------------------------
*/
void Delay(__IO uint32_t nTime)
{
	TimingDelay = nTime;

	while(TimingDelay != 0);
}

/*
+------------------------------------------------------------------------------
| Function : ValidateCompleteFlashCRC(...)
+------------------------------------------------------------------------------
| Purpose: Validates CRC for flash
+------------------------------------------------------------------------------
| Algorithms: 
|		- Complete flash crc during power up
+------------------------------------------------------------------------------
| Parameters:  
|  		None
+------------------------------------------------------------------------------
| Return Value: 
|		None
|  
+------------------------------------------------------------------------------
*/
void ValidateCompleteFlashCRC(void)
{
	uint8_t cnt = 0;
	/* uint8_t pointer to flash address (FA) start */
	static uint8_t *pFA = (uint8_t*) FLASH_START_ADDRESS;
	uint8_t flashData[ROM_CHUNK_SIZE] = {0};/* Flash data */
	uint32_t calculatedCRC = 0;
	
	uint32_t sizeToCalculateInInteration = 0;
	
	uint8_t crcResetFlg = 0;
	
	static uint32_t blockCnt = 0;
	static uint32_t romLocationCnt = 0;
	do
	{
		/* Check block size is exceeding */
		if(blockCnt < ROM_TOTAL_BLOCK)
		{
			/* fill the size of CRC to be calculated for - length of chunk data */
			sizeToCalculateInInteration = ROM_CHUNK_SIZE;
		}
		else
		{
			/* All blocks are completed and now remaining bytes are less than chunk size*/
			/* example - 16050/32 =  501, the remaining bytes are not in chunk of 32 bytes
						remaining bytes are 18 bytes, so we are calculating CRC for
						remaining bytes now*/
			sizeToCalculateInInteration = (ROM_SIZE_FOR_CODE % ROM_CHUNK_SIZE);
		}
		
		/* Get the data from flash for selected size only */
		for(cnt = 0; cnt < sizeToCalculateInInteration; cnt ++)
		{
			flashData[cnt] = (uint8_t)(*pFA);
			
			pFA ++;
		}

		/* update the rom location counter, 
			this will be use to verify that we scan complete flash */
		romLocationCnt += sizeToCalculateInInteration;
		
		/* we are starting CRC first time then, CRC reset flag should be 1
			To clear previous crc data from DR register of CRC handler */
		if(blockCnt == 0)
		{
			crcResetFlg = 1;
		}
		
		/* compute CRC for chunk of data */
		calculatedCRC = CRC_8BitsCompute(flashData, sizeToCalculateInInteration, crcResetFlg);
		
		/* update block counter */
		blockCnt ++;
	}
	while(romLocationCnt < ROM_SIZE_FOR_CODE);
	
	/* verify that we reached to rom size or not */
	if(romLocationCnt >= ROM_SIZE_FOR_CODE)
	{
		/* reset all static variables here, next time we should start from Starting point of flash */
		blockCnt = 0;
		romLocationCnt = 0;
		pFA = (uint8_t*) FLASH_START_ADDRESS;
		
		/* validate CRC is matching with actual value */
		if(calculatedCRC == romCRC)
		{
			/* we need to have this line what will happened if above line itself corrupted */
			if(calculatedCRC != romCRC)
			{
				PrintBuffer("CRC MisMatch [%d] = [%d]\r\n", calculatedCRC, romCRC);
			}
			PrintBuffer("Flash Checksum CRC [%d] = [%d]\r\n", calculatedCRC, romCRC);
		}
		else
		{
			PrintBuffer("CRC MisMatch [%d] = [%d]\r\n", calculatedCRC, romCRC);
		}
	}
}

/*
+------------------------------------------------------------------------------
| Function : ValidateFlashCRC(...)
+------------------------------------------------------------------------------
| Purpose: Validates CRC for flash
+------------------------------------------------------------------------------
| Algorithms: 
|		- We will calculate the flash crc at chunk of 32 bytes at a time only
|		- reason to same is, don't want to block the super loop for long period
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
void ValidateFlashCRC(void)
{
	uint8_t cnt = 0;
	/* uint8_t pointer to flash address (FA) start */
	static uint8_t *pFA = (uint8_t*) FLASH_START_ADDRESS;
	uint8_t flashData[ROM_CHUNK_SIZE] = {0};/* Flash data */
	uint32_t calculatedCRC = 0;
	
	uint32_t sizeToCalculateInInteration = 0;
	
	uint8_t crcResetFlg = 0;
	
	static uint32_t blockCnt = 0;
	static uint32_t romLocationCnt = 0;
	
	/* Check block size is exceeding */
	if(blockCnt < ROM_TOTAL_BLOCK)
	{
		/* fill the size of CRC to be calculated for - length of chunk data */
		sizeToCalculateInInteration = ROM_CHUNK_SIZE;
	}
	else
	{
		/* All blocks are completed and now remaining bytes are less than chunk size*/
		/* example - 16050/32 =  501, the remaining bytes are not in chunk of 32 bytes
					remaining bytes are 18 bytes, so we are calculating CRC for
					remaining bytes now*/
		sizeToCalculateInInteration = (ROM_SIZE_FOR_CODE % ROM_CHUNK_SIZE);
	}
	
	/* Get the data from flash for selected size only */
	for(cnt = 0; cnt < sizeToCalculateInInteration; cnt ++)
	{
		flashData[cnt] = (uint8_t)(*pFA);
		
		pFA ++;
	}

#ifdef SW_FMEA_CORRUPT_FLASH_DATA
	flashData[8] &= 0x00; 
#endif	
	
	/* update the rom location counter, 
		this will be use to verify that we scan complete flash */
	romLocationCnt += sizeToCalculateInInteration;
	
	/* we are starting CRC first time then, CRC reset flag should be 1
		To clear previous crc data from DR register of CRC handler */
	if(blockCnt == 0)
	{
		crcResetFlg = 1;
	}
	
	/* compute CRC for chunk of data */
	calculatedCRC = CRC_8BitsCompute(flashData, sizeToCalculateInInteration, crcResetFlg);
	
	/* update block counter */
	blockCnt ++;
	
	/* verify that we reached to rom size or not */
	if(romLocationCnt >= ROM_SIZE_FOR_CODE)
	{
		/* reset all static variables here, next time we should start from Starting point of flash */
		blockCnt = 0;
		romLocationCnt = 0;
		pFA = (uint8_t*) FLASH_START_ADDRESS;
		
		/* validate CRC is matching with actual value */
		if(calculatedCRC == romCRC)
		{
			/* we need to have this line what will happened if above line itself corrupted */
			if(calculatedCRC != romCRC)
			{
				PrintBuffer("CRC MisMatch [%d] = [%d]\r\n", calculatedCRC, romCRC);
			}
		}
		else
		{
			PrintBuffer("CRC MisMatch [%d] = [%d]\r\n", calculatedCRC, romCRC);
		}
	}
}
