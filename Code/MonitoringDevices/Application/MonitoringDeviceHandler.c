/*
---------------------------------------------------------------------------------
File Name : 					MonitoringDeviceHandler.c
---------------------------------------------------------------------------------

 Program Description    : Handles data coming from different device
 Author                 : Bhavesh Dhameliya
 Revision History       : 01/06/2021

---------------------------------------------------------------------------------
| Effeced Date |    Person        |        Description                          |
|--------------|------------------|---------------------------------------------|
---------------------------------------------------------------------------------
*/

//-----------------------------------------------------------------------------
// Including the modified definations and header files
//-----------------------------------------------------------------------------
#include "stm32f0xx_hal_conf.h"

#include "TimerHandler.h"
#include "debugger.h"
#include "GPIODriver.h"
#include "MonitoringDeviceHandler.h"

//---------------------------- Defines & Structures ----------------------------
#define MONITORING_DEVICE_ID		(000)
#define MAX_DEVICES					(11)
#define MACK_PACKET_SIZE			(100)
#define MAX_CIRCULAR_QUEUE_SIZE		(20)
#define DEVICE_FAILURE_COUNT		(28)		/* 28 *100msec == 2800msec */

#define SWAP_NUMBER(num1,num2) 	(num1 ^= num2 ^= num1 ^= num2)

//#define DEBUGG_PRINT_ENABLE

typedef struct		__attribute__((packed))
{
	/* device ID of each device from which we are receiving data */
	uint8_t			deviceID;
	
	/* total mesage of particular device*/
	uint32_t		totalMessages;
	
	/* ACK receive flag, indicates that recently we received ACK 
		if this flag remains zero for long duration, we will declare 
		that device is not functioning properly */
	uint8_t			ackReceivedFlg;
	
	/* when we check ack flag and it's not available, this counter will
		increament to detect failure */
	uint8_t			failureCnt;
	
}DEVICE_ENTRY_LIST_t;

typedef struct	__attribute__((packed))
{
	DEVICE_ENTRY_LIST_t		deviceEntryList[MAX_DEVICES];
	uint32_t				totalMessages;
	uint16_t				calculatedCRC;
	
}MONITORING_DEVICE_STATISTICS_t;

typedef struct __attribute__((packed))
{
	uint16_t		messageId:8;	
	uint16_t		ackBit:1;
	uint16_t		commandBit:1;
	uint16_t		heartBit:1;
	uint16_t		reserved:5;
	
}MESSAGE_ID_FOMAT_t;

typedef union __attribute__((packed))
{
	uint16_t			messageIdData;
	MESSAGE_ID_FOMAT_t	meessageIdFormat;
	
}MESSAGE_ID_u;

typedef struct __attribute__((packed))
{
	uint8_t			destinationAddr;
	uint8_t			sourceAddr;
	MESSAGE_ID_u	messageIdInfo;
	uint8_t			packetLen;
	
}PROTOCOL_FORMAT_t;


//---------------------------- Static Variables --------------------------------
/* All data received in UART ISR will be copy into this */
static uint8_t U1RX_Buffer[MACK_PACKET_SIZE];	
/* receive packet length, updated in UART ISR */
static uint8_t U1RX_DataLen = 0;

/* UART transmit buffer */
static uint8_t U1TX_Buffer[20];
/* UART transmit length */
static uint8_t U1TX_DataLen = 0;

/* incoming packet to process further, will be copied from U1RX_Buffer when dead time expires */
static uint8_t inComingDataBuff[MACK_PACKET_SIZE];
/* incoming packet length */
static uint8_t inComingDataLen = 0;

/* To indicate complete packet has been received */
static uint8_t packetAvailableFlg = 0;

/* write index for circular buffer */
static uint32_t readIndex = 0;
/* read index for circular buffer */
static uint32_t writeIndex = 0;

/* circular buffer queue with memory allocated */
static PROTOCOL_FORMAT_t circularQueueForPackets[MAX_CIRCULAR_QUEUE_SIZE];

/*Monitoring Device list with statistical information */
static MONITORING_DEVICE_STATISTICS_t	monitoringDeviceList;

//---------------------------- Global Variables --------------------------------


//------------------------- Extern Global Variables ----------------------------


//--------------------------- Private function prototypes ----------------------
static uint8_t GetPacketFromQueue(PROTOCOL_FORMAT_t *packetData);
static void AddPacketToQueue(PROTOCOL_FORMAT_t *packetData);
static void SendACKPacketToDevice(PROTOCOL_FORMAT_t *packetData);

/*
+------------------------------------------------------------------------------
| Function : MonitoringDeviceInit(...)
+------------------------------------------------------------------------------
| Purpose: Initialize all process related variable and conitions.
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
void MonitoringDeviceInit(void)
{
	int cnt = 0;
	
	memset(circularQueueForPackets, 0, sizeof(PROTOCOL_FORMAT_t));
	
	/* updating each device ID into list */
	/* This can be done via some config file, i have just use static list of entry */
	/* Device ID starts with 1, always 0 will be left blank, 
			can be use for monitoring device  */
	for(cnt = 1; cnt < MAX_DEVICES; cnt++)
	{
		monitoringDeviceList.deviceEntryList[cnt].deviceID = cnt;
	}
}

/*
+------------------------------------------------------------------------------
| Function : UART1_RX_Handler(...)
+------------------------------------------------------------------------------
| Purpose: This Function receives data from UART1 from ISR.
+------------------------------------------------------------------------------
| Algorithms: 
|   - Appends data received on UART Port 1 into local buffer.
|	
+------------------------------------------------------------------------------
| Parameters:  
|		uint16_t receivedData
|
+------------------------------------------------------------------------------
| Return Value: 
|		None
|  
+------------------------------------------------------------------------------
*/	
void UART1_RX_Handler(uint16_t receivedData)
{
	/* Need to discard all the data which is beyind packet size */
	/* Someone is trying to send so much data which is not define in protocol */
	if(U1RX_DataLen >= MACK_PACKET_SIZE)
	{
		return;
	}
	
	/* Fill the local buffer with receive data and increment the index of buffer */
	/* we are receiveing all data as 8-bit format only so copying only 8 bit from receivedData */
	U1RX_Buffer[U1RX_DataLen++] = (uint8_t)receivedData;
	
	/* logic to say my complete packet is received is as follow 
		We are using 115200 baudrate, so tranfer one byte we need total 10 bits,
		1 - Start bit
		8 - Data bit 
		1 - End bit
	
		(1/115200) * 10 = 86 micro seconds
		
		Each packlet should be continous, so if we are not receiving any data within 
		2msec time, we can declare that complete packet has been recived
	*/
	if(U1RX_DataLen == 1)	/* This is first byte */
	{
		/* Start Timer 3 of 2 msec */
		Timer_StartStop(TIMER_3_INSTANCE, 1);
	}
	else
	{
		Timer_Reload(TIMER_3_INSTANCE, (SystemCoreClock / (500) ) - 1);
	}
}

/*
+------------------------------------------------------------------------------
| Function : TIMER_3_IRQ_Handler(...)
+------------------------------------------------------------------------------
| Purpose: This is IRQ Handler function for Timer 3
+------------------------------------------------------------------------------
| Algorithms: 
|   	- This timer was started when we started data receiving from devices,
|		- we are reloading this timer each time we receive byte, 
|		- Now, timer got expire meaning that dead time is expire and need to 
|		  start processing packet
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
void TIMER_3_IRQ_Handler(void)
{
	/* Take copy of recived buffer, Data will not be corrupted if data recieves 
		immediately before we process the already receive packet */
	memcpy(inComingDataBuff, U1RX_Buffer, U1RX_DataLen);
	inComingDataLen = U1RX_DataLen;
	
	/* Set packet available flag */
	packetAvailableFlg = 1;
	
//	Timer_StartStop(TIMER_3_INSTANCE, 0);
}

/*
+------------------------------------------------------------------------------
| Function : ProcessInComingDataFromDevice(...)
+------------------------------------------------------------------------------
| Purpose: Process incoming data over network
+------------------------------------------------------------------------------
| Algorithms: 
|   	- Process data if complete packet has received
|		- Checks that packet is for monitoring device or not
|		- Calculate CRC and validate against recieved CRC
|		- Process packet and message information
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
void ProcessInComingDataFromDevice(void)
{
	char test[200];
	uint32_t calculatedCRC = 0;
	uint32_t receivedCRC = 0;
	
	PROTOCOL_FORMAT_t packetInfo;
	
	/* Check complete packet has been received or not */
	if(packetAvailableFlg)
	{
		/* reset the flag like what we do in ISR */
		packetAvailableFlg = 0;
		
		/* validate that packet is intended for us or not, by checking fist byte */
		if(inComingDataBuff[0] == MONITORING_DEVICE_ID)
		{
			/* calculate the CRC to validate packet integrity */
			/* while calculating CRC we always need to remove the CRC byte */
			calculatedCRC = CRC_8BitsCompute(inComingDataBuff, (inComingDataLen - 2), 1);
			
			receivedCRC = (((uint32_t)inComingDataBuff[inComingDataLen - 2] << 8) | (uint32_t)inComingDataBuff[inComingDataLen -1]);
			
			/* Check CRC validation */
			if(calculatedCRC == receivedCRC)
			{
				memcpy(&packetInfo, inComingDataBuff, sizeof(PROTOCOL_FORMAT_t));

#ifdef DEBUGG_PRINT_ENABLE
				PrintBuffer("Valid Message From [%d]\r\n", inComingDataBuff[1]);
				
				PrintBuffer("Message HDR [%d]\r\n", sizeof(PROTOCOL_FORMAT_t));
				
				PrintBuffer("Destination Addr [%d]\r\n", packetInfo.destinationAddr);
				PrintBuffer("Source Addr [%d]\r\n", packetInfo.sourceAddr);
				PrintBuffer("MessageID[%d],[%d],[%d],[%d],[%d]\r\n", 
									packetInfo.messageIdInfo.meessageIdFormat.messageId,
									packetInfo.messageIdInfo.meessageIdFormat.reserved,
									packetInfo.messageIdInfo.meessageIdFormat.heartBit,
									packetInfo.messageIdInfo.meessageIdFormat.commandBit,
									packetInfo.messageIdInfo.meessageIdFormat.ackBit);
				PrintBuffer("Packet Len [%d]\r\n", packetInfo.packetLen);
				PrintBuffer("-------------------------\r\n");

#endif			
				/* Add packet to process statistical data */
				AddPacketToQueue(&packetInfo);
				
				/* Send ACK packet to Device */
				SendACKPacketToDevice(&packetInfo);				
			}
			else
			{
				PrintBuffer("InValid CRC\r\n");
			}
		}
		else
		{
			PrintBuffer("Message not intended for MonitoringDevice\r\n");
		}
		U1RX_DataLen = 0;
	}
}

/*
+------------------------------------------------------------------------------
| Function : ProcessMonitoringDeviceData(...)
+------------------------------------------------------------------------------
| Purpose: This is business logic for monitoring device
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
void ProcessMonitoringDeviceData(void)
{
	uint8_t cnt = 0;
	uint8_t updateCRCFlg = 0;
	
	PROTOCOL_FORMAT_t receivedPacket;
	
	do
	{
		/* Verify that data is available to process */
		if(GetPacketFromQueue(&receivedPacket) == SUCCESS)
		{
			/* i have assing device ID from 1 to 10, easy to index it*/
			/* if we want to have different indexing mechanism based on numbering,
				we can use the lookup table to search the index of that device */
			
			/* we will check whether this data is for ACK or for actual packet */
			if(receivedPacket.messageIdInfo.meessageIdFormat.heartBit)
			{
				/* update that ACK has been received */
				monitoringDeviceList.deviceEntryList[receivedPacket.sourceAddr].ackReceivedFlg = 1;
				
//				/* update CRC flag to check data integrity periodically */
//				updateCRCFlg = 1;
			}
			/* checking command bit to get actual message */
			else if(receivedPacket.messageIdInfo.meessageIdFormat.commandBit)
			{
				/* update message counter for individual device*/
				monitoringDeviceList.deviceEntryList[receivedPacket.sourceAddr].totalMessages ++;
				
				/* update overall message counter for monitoring device*/
				monitoringDeviceList.totalMessages ++;
				
//				/* update CRC flag to check data integrity periodically */
//				updateCRCFlg = 1;
			}	
		}
		else
		{
			/* exit if no more messages available */
			break;
		}
		
		/* looping counter */
		cnt ++;
		
		/* Don't want to block main loop if too many packets are available */
	}while(cnt < 5);
}

/*
+------------------------------------------------------------------------------
| Function : CheckDeviceAvailability(...)
+------------------------------------------------------------------------------
| Purpose: This is function verifies that any device is not functioning properly
+------------------------------------------------------------------------------
| Algorithms: 
|   	This finction will be called from every 100msec.
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
void CheckDeviceAvailability(void)
{
	uint8_t cnt = 0;
	
	/* loop for each device */
	for(cnt = 1; cnt <= MAX_DEVICES; cnt ++)
	{
		/* Check device availability by ack received or not */
		if(monitoringDeviceList.deviceEntryList[cnt].ackReceivedFlg)
		{
			/* clear this flag from here, if next time ACK packet received from device
				this flag will be set */
			monitoringDeviceList.deviceEntryList[cnt].ackReceivedFlg = 0;
			
			/* reset previous failure count if any, we received an ACK from device */
			monitoringDeviceList.deviceEntryList[cnt].failureCnt = 0;
		}
		else if(monitoringDeviceList.deviceEntryList[cnt].failureCnt < DEVICE_FAILURE_COUNT)
		{
			/* increament failure count */
			monitoringDeviceList.deviceEntryList[cnt].failureCnt ++;
			/* This function always called @100msec, 
				so counting for 800msec that we haven't received any data from device,
				meaning that Device is having some trouble to send data */
			if(monitoringDeviceList.deviceEntryList[cnt].failureCnt >= DEVICE_FAILURE_COUNT)
			{
				//monitoringDeviceList.deviceEntryList[cnt].failureCnt = 0;
				PrintBuffer("ERR_DEV#%d\r\n", cnt);
			}
		}
	}
}

/*
+------------------------------------------------------------------------------
| Function : GetMonitoringDeviceMessages(...)
+------------------------------------------------------------------------------
| Purpose: Provides total messages of monitoring device
+------------------------------------------------------------------------------
| Algorithms: 
|   	- returns total messages of monitoring device
|	
+------------------------------------------------------------------------------
| Parameters:  
|		None
|
+------------------------------------------------------------------------------
| Return Value: 
|		uint32_t - Total Message of monitoring device
|  
+------------------------------------------------------------------------------
*/
uint32_t GetMonitoringDeviceMessages(void)
{
	return monitoringDeviceList.totalMessages;
}

/*
+------------------------------------------------------------------------------
| Function : GetIndividualDeviceMessages(...)
+------------------------------------------------------------------------------
| Purpose: Provides total messages of Individual device
+------------------------------------------------------------------------------
| Algorithms: 
|   	- returns total messages of Individual device
|	
+------------------------------------------------------------------------------
| Parameters:  
|		uint8_t - Device id 
|
+------------------------------------------------------------------------------
| Return Value: 
|		uint32_t - Total Message of Individual device
|  
+------------------------------------------------------------------------------
*/
uint32_t GetIndividualDeviceMessages(uint8_t deviceID)
{
	if(deviceID < MAX_DEVICES)
	{
		return monitoringDeviceList.deviceEntryList[deviceID].totalMessages;
	}
	else
	{
		return 0;
	}	
}

/*
+------------------------------------------------------------------------------
| Function : SendACKPacketToDevice(...)
+------------------------------------------------------------------------------
| Purpose: Fetches packet from queue
+------------------------------------------------------------------------------
| Algorithms: 
|   	- Reads packet information from Queue w.r.t current index
|	
+------------------------------------------------------------------------------
| Parameters:  
|		PROTOCOL_FORMAT_t * - Where data needs to copy
|
+------------------------------------------------------------------------------
| Return Value: 
|		None
|  
+------------------------------------------------------------------------------
*/
static void SendACKPacketToDevice(PROTOCOL_FORMAT_t *packetData)
{
	uint8_t cnt = 0;
	uint16_t calculatedCRC = 0;
	
	/* Verify input data first */
	if(packetData != NULL)
	{
		/* making sure that previous value doesn't represent */
		U1TX_DataLen = 0;
		
		/* swap the sorce and destination as we are sending data to source address */
		SWAP_NUMBER(packetData->destinationAddr, packetData->sourceAddr);

		/* Clear the status information */
		packetData->messageIdInfo.messageIdData &= (0x00FF);

		/* Set the ACK bit */
		packetData->messageIdInfo.meessageIdFormat.ackBit = 1;
		
		packetData->packetLen = 0;
		
		calculatedCRC = CRC_8BitsCompute((uint8_t *)packetData, sizeof(PROTOCOL_FORMAT_t), 1);
		
		memcpy(U1TX_Buffer, packetData, sizeof(PROTOCOL_FORMAT_t));
		U1TX_DataLen = sizeof(PROTOCOL_FORMAT_t);
		U1TX_Buffer[U1TX_DataLen ++] = (uint8_t)(calculatedCRC >> 8); 
		U1TX_Buffer[U1TX_DataLen ++] = (uint8_t)(calculatedCRC); 
		
		UART_TransmitData(UART1_INSTANCE, 
							U1TX_Buffer, 
							U1TX_DataLen, 
							100);

#ifdef DEBUGG_PRINT_ENABLE
		PrintBuffer("ACK : ");
		for(cnt = 0; cnt < U1TX_DataLen; cnt ++)
		{
			PrintBuffer("%x ", U1TX_Buffer[cnt]);
		}
		PrintBuffer("\r\n");
#endif
	}
}

/*
+------------------------------------------------------------------------------
| Function : GetPacketFromQueue(...)
+------------------------------------------------------------------------------
| Purpose: Fetches packet from queue
+------------------------------------------------------------------------------
| Algorithms: 
|   	- Reads packet information from Queue w.r.t current index
|	
+------------------------------------------------------------------------------
| Parameters:  
|		PROTOCOL_FORMAT_t * - Where data needs to copy
|
+------------------------------------------------------------------------------
| Return Value: 
|		uint8_t - SUCESS/FAIL
|  
+------------------------------------------------------------------------------
*/
static uint8_t GetPacketFromQueue(PROTOCOL_FORMAT_t *packetData)
{
	uint8_t retVal = ERROR;
	
	/* Verify input data first */
	if(packetData != NULL)
	{
		/* Check Queue has any data left, if read and write index are not same, Queue has some data */
		if(writeIndex != readIndex)
		{
			/* copy content to user */
			memcpy(packetData, &circularQueueForPackets[readIndex], sizeof(PROTOCOL_FORMAT_t));
			
			/* we fetch the data from Queue so increament the readIndex */
			readIndex ++;
			
			/* Check read index overflow */
			if(readIndex >= MAX_CIRCULAR_QUEUE_SIZE)
			{
				/* set back read index to 0, incase of overflow from max queue size */
				readIndex = 0;
			}
#ifdef DEBUGG_PRINT_ENABLE
			
			PrintBuffer("R [%d] W [%d]\r\n", readIndex, writeIndex);

#endif			
			retVal = SUCCESS;
		}
	}
	
	return retVal;
}

/*
+------------------------------------------------------------------------------
| Function : AddPacketToQueue(...)
+------------------------------------------------------------------------------
| Purpose: Adds packet into queue
+------------------------------------------------------------------------------
| Algorithms: 
|   	- Add packet information into Queue w.r.t current index
|		- Idea is to rollover when reader is not getting time, will overwrite
|			oldest packet with new packet
|
+------------------------------------------------------------------------------
| Parameters:  
|		PROTOCOL_FORMAT_t * - Where data needs to copy
|
+------------------------------------------------------------------------------
| Return Value: 
|		None
|  
+------------------------------------------------------------------------------
*/
static void AddPacketToQueue(PROTOCOL_FORMAT_t *packetData)
{
	/* Verify input data first */
	if(packetData != NULL)
	{
		/* copy packet into queue */
		memcpy(&circularQueueForPackets[writeIndex], packetData, sizeof(PROTOCOL_FORMAT_t));
		
		/* increament write index, as we already stored packet into queue */
		writeIndex ++;
		
		/* check whether write index is overflowing or not */
		if(writeIndex >= MAX_CIRCULAR_QUEUE_SIZE)
		{
			/* write index is full, so make it zero to fill next packet 
				As this queue will follow FIFO concept */
			writeIndex = 0;
		}
		
		/* Now consider that write index is rollover and still we haven't read any
		   data from Queue. It means that no consumer right now due to some reason.
		   In this condition we are changing write index and read index is 
			matching same value, then consumer will assume that Queue 
		   doesn't have any data, so shifting read index by one to get oldest packet
		   from queue if read index is same as Write index*/
		if(readIndex == writeIndex)
		{
			/* shifting read index by one as per above explaination */
			readIndex ++;
			/* check whether read index is overflowing or not */
			if(readIndex >= MAX_CIRCULAR_QUEUE_SIZE)
			{
				readIndex = 0;
			}
		}
#ifdef DEBUGG_PRINT_ENABLE
		
		PrintBuffer("W [%d] R [%d]\r\n", writeIndex, readIndex);
		
#endif
	}
}