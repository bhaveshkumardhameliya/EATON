/*
---------------------------------------------------------------------------------
File Name : 									debugger.c
---------------------------------------------------------------------------------

 Program Description    : Main MCU UART0 used as a debugger port for monitoring
 Author                 :    
 Revision History       :    

---------------------------------------------------------------------------------
| Effeced Date |    Person        |        Description                          |
|--------------|------------------|---------------------------------------------|
---------------------------------------------------------------------------------
*/

//-----------------------------------------------------------------------------
// Including the modified definations and header files
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "stm32f0xx_hal.h"
#include "debugger.h"
#include "MonitoringDeviceHandler.h"

//-----------------------------------------------------------------------------
// Internal Function PROTOTYPES Declerations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// UART receive state header variables
//-----------------------------------------------------------------------------
#define MAX_BUFFER_SIZE		500

#define WELCOME_TXT_LENGTH	45//61//49//52//60

#define MONITORING_DEV_INFO	'M'
#define DEVICE_INFO			'D'

extern enum ERROR_MESSAGE_ID Supv_Mcu_Error_Code;

//-----------------------------------------------------------------------------
// Internal Variables
//-----------------------------------------------------------------------------
static uint8_t U3RX_State = CLR;
static uint8_t U3RX_DataLen = CLR;
static uint8_t U3RX_DataReadyFlg = CLR;
static int32_t U3RX_CommandType;
static uint8_t U3TX_DataLen;
static uint8_t U3TX_Buffer[250];
static uint8_t U3RX_Buffer[20];

const uint8_t WelcomeText[WELCOME_TXT_LENGTH] = 
{
	"Monitoring Device Application Ver - 1.0.0\r\n"
};



//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------



void UART3_RX_Handler(uint16_t receivedData)
{
	switch(U3RX_State)
	{
		case 0:
		{
			if( ((receivedData >= '0') && (receivedData <= '9')) || 
				((receivedData >= 'A') && (receivedData <= 'Z')) )
			{
				U3RX_DataLen = CLR;
				U3RX_Buffer[U3RX_DataLen++] = receivedData;
				U3RX_State++;
			}
			else
			{
				U3RX_DataLen = CLR;
				U3RX_State = CLR;
			}
		}
		break;

		case 1:
		{
			if(receivedData == 0x0D)
			{
				U3RX_Buffer[U3RX_DataLen++] = receivedData;
				U3RX_DataLen = CLR;
				U3RX_State = CLR;
				U3RX_DataReadyFlg = SET_BYTE;        
			}
			else
			{
				U3RX_Buffer[U3RX_DataLen++] = receivedData;
			}
		}
		break;
		default:
		break;
	}
}

void ProcessDebuggCommand(void)
{
	uint8_t devID;
	
	if(U3RX_DataReadyFlg)
	{
		U3RX_DataReadyFlg = 0;
		
		U3RX_CommandType = U3RX_Buffer[0];//CLR;
		
		switch(U3RX_CommandType)
		{
			case MONITORING_DEV_INFO:
				PrintBuffer("MD Total Message [%d]\r\n", GetMonitoringDeviceMessages());
				break;
			
			case DEVICE_INFO:
				devID = (U3RX_Buffer[1] - '0');
				PrintBuffer("Dev#%d Total Message [%d]\r\n", 
								devID, GetIndividualDeviceMessages(devID));
				break;
			
			default:
				break;
		}
	}
}

void UART3_TX_Handler(void)
{
	static uint8_t localPacketLen;
	
	if(localPacketLen == U3TX_DataLen)
	{   
		UART_StartTxInterrupt(UART3_INSTANCE, DISABLE);
		localPacketLen = CLR;
		U3TX_DataLen = CLR;
		U3TX_Buffer[localPacketLen] = '\0';
	}
	else
	{
		SEND_DATA(UART3_INSTANCE, U3TX_Buffer[localPacketLen++]);
	}
}


void UART0_SendWelcomeMsg(void)
{	
	PrintBuffer((const char *)WelcomeText);
}


void PrintBuffer(const char *buff,...)
{
	uint32_t Buff_Length;
	va_list args;

	int8_t buffer[255]={0};

	va_start(args,buff);
	vsprintf((char *)buffer, buff, args );
	
	strcpy((char *)U3TX_Buffer, (char *)buffer);
	Buff_Length = strlen((char *)U3TX_Buffer);
	U3TX_DataLen = Buff_Length;
	
	//UART_StartTxInterrupt(UART3_INSTANCE, ENABLE);
	UART_TransmitData(UART3_INSTANCE, (uint8_t *)U3TX_Buffer, U3TX_DataLen, 1000);
	va_end(args);
}
