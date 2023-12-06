/*
 * configuration.h
 *
 *  Created on: Oct 19, 2023
 *      Author: Bhargav-4836
 */

#ifndef INC_CONFIGURATION_H_
#define INC_CONFIGURATION_H_


#include "common.h"
#include "SPIFFS.h"
#include "rom/crc.h"
#include "debug.h"


/* You only need to format LittleFS the first time you run a
   test or else use the LittleFS plugin to create a partition
   https://github.com/me-no-dev/arduino-esp32fs-plugin */
#define FORMAT_SPIFFS_IF_FAILED true




#define MAC_ID_SIZE 6
#define MAX_FLOOR_CONST    8

typedef struct
{
  uint8_t numberOfFloors;
  uint8_t doorOpenTimeSec;
  uint8_t doorCloseTimeSec;
  uint8_t doorOpenTimeDuringCall;
  uint8_t macIdShaft[MAC_ID_SIZE];
  uint8_t macIdCabin[MAC_ID_SIZE];
  uint16_t LIDAR[MAX_FLOOR_CONST];
  uint16_t crc;
}CONFIG_FILE;

typedef enum
{
  NO_OF_FLOORS,
  DOOR_OPEN_TIME,
  DOOR_CLOSE_TIME,
  DOOR_OPEN_CALL_TIME,
  SHAFT_MAC_ID,
  CABIN_MAC_ID,
  LIDAR_VALUE_AT_FLOOR,
}PARAMETER_NAME_t;



bool configFileInit();

bool writeParameter(PARAMETER_NAME_t param , const uint8_t*value , uint8_t len);
uint8_t readParameter(PARAMETER_NAME_t param, uint8_t *value);
CONFIG_FILE * getConfig();
bool writeConfigFile();
void setDefaultMac(uint8_t *mac);
void setNewMacID(uint8_t *mac);
void setShaftPeerMac(uint8_t *mac);
bool returnAvailableState();
void setAvailableState(bool state);
//#define MAX_FLOOR (getConfig()->numberOfFloors)
#define TOP_FLOOR (getConfig()->numberOfFloors -1)


#endif /* INC_CONFIGURATION_H_ */
