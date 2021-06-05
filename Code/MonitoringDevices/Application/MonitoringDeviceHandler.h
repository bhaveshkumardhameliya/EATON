/*
---------------------------------------------------------------------------------
File Name : 					MonitoringDeviceHandler.h
---------------------------------------------------------------------------------

 Program Description    : Handles data coming from different device
 Author                 : Bhavesh Dhameliya
 Revision History       : 01/06/2021

---------------------------------------------------------------------------------
| Effeced Date |    Person        |        Description                          |
|--------------|------------------|---------------------------------------------|
---------------------------------------------------------------------------------
*/

#ifndef __MONITORING_DEVICES_H_
#define __MONITORING_DEVICES_H_

#include <stdint.h>
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
void MonitoringDeviceInit(void);
	
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
|		- Process packet and message information7
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
void ProcessInComingDataFromDevice(void);

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
void ProcessMonitoringDeviceData(void);

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
void CheckDeviceAvailability(void);

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
uint32_t GetMonitoringDeviceMessages(void);

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
uint32_t GetIndividualDeviceMessages(uint8_t deviceID);

#endif /*#ifndef __MONITORING_DEVICES_H_*/
