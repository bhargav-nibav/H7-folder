/*
 * configuration.c
 *
 *  Created on: Oct 19, 2023
 *      Author: Bhargav-4836
 */

#include "SPIFFS.h"
#include "configuration.h"
#include "messageQueue.h"
#define CRC_INIT_VAL  0x0000

CONFIG_FILE config_file;
const char *configFileName = "/config.bin";

bool newMacAvailable = false;
bool readConfigFile();
static void configurationInitHardCode();
uint8_t defaultmacID[]={0,0,0,0,0,0};

char buff[50];

bool failedToRead = false;
/**************************************************************************************/
bool configFileInit()
{
  uint16_t crc;

  if (SPIFFS.begin(true))
  {
    DEBUG_PRINT_1("mount passed \r\n");
    if (readConfigFile())
    {
      crc = crc16_le(CRC_INIT_VAL, (uint8_t *)&config_file, sizeof(config_file) - sizeof(config_file.crc));
      if (crc == config_file.crc)
      {
        DEBUG_PRINT_1("SPIFFS file CRC passed \r\n");
        return true;
      }
      else
      {
        configurationInitHardCode();
        DEBUG_PRINT_1("SPIFFS file CRC failed \r\n");
        return true;
      }
    }
    else
    {
      DEBUG_PRINT_1("SPIFFS file read failed update new file\r\n");
      configurationInitHardCode();
      return true;
    }
  }
  else
  {
    DEBUG_PRINT_1("SPIFFS failed \r\n");
  }
  return false;
}

bool writeConfigFile()
{
  bool status;
  File file;
  status = false;
  if (SPIFFS.exists((const char *)configFileName))
  {
    if (SPIFFS.remove((const char *)configFileName))
    {
      status = true;
    }
  }
  else
  {
    status = true;
  }
  if (status)
  {
    file = SPIFFS.open(configFileName, FILE_WRITE, true);
    if (file)
    {
      // check checksum
      config_file.crc = crc16_le(CRC_INIT_VAL, (uint8_t *)&config_file, sizeof(config_file) - sizeof(config_file.crc));
      if (sizeof(config_file) == file.write((uint8_t *)&config_file, sizeof(config_file)))
      {
        status = true;
        DEBUG_PRINT_1("SPIFFS write passed \r\n");
      }
      file.close();
    }
    else
    {
      DEBUG_PRINT_1("SPIFFS file write open failed \r\n");
      status = false;
    }
  }
  return status;
}

bool readConfigFile()
{
  File file = SPIFFS.open(configFileName, FILE_READ);
  if (file)
  {
    if (sizeof(config_file) == file.read((uint8_t *)&config_file, sizeof(config_file)))
    {
        DEBUG_PRINT_1("SPIFFS file read passed \r\n");
        return true;
    }
    else
    {
      DEBUG_PRINT_1("SPIFFS file read failed \r\n");
    }
    file.close();
  }
  else
  {
    DEBUG_PRINT_1("SPIFFS file open failed \r\n");
  }
  return false;
}

CONFIG_FILE * getConfig()
{

  return &config_file;
}



static void configurationInitHardCode()
{
  CONFIG_FILE* configFile = getConfig();
  configFile->doorOpenTimeSec = 30;
  configFile->doorCloseTimeSec = 20;
  configFile->doorOpenTimeDuringCall = 5;
  esp_read_mac(configFile-> macIdCabin , ESP_MAC_WIFI_STA);
  memcpy(configFile->macIdShaft,&defaultmacID,sizeof(defaultmacID));


  writeConfigFile();


  if (!SPIFFS.exists((const char *)configFileName))
  {
    DEBUG_PRINT_LN("write fresh file \r\n");
  }
  else
  {
    if(SPIFFS.remove((const char *)configFileName))
    {
      writeConfigFile();
    }
  }
}


bool writeParameter(PARAMETER_NAME_t param , const uint8_t*value , uint8_t len)
{
  switch (param)
  {
    case NO_OF_FLOORS:
    if(len == sizeof(config_file.numberOfFloors))
    {

      int len = snprintf(buff, sizeof(buff), "Number of floor numbers = ",*value);
      HAL_UART_Transmitte(&huart, (uint8_t*)(buff),len, 100);

      config_file.numberOfFloors = *value;
    }
    break;
    case DOOR_OPEN_TIME:
    if(len == sizeof(config_file.doorOpenTimeSec))
    {
       int len = snprintf(buff, sizeof(buff), "Number of Door open duration = ",*value);
       HAL_UART_Transmitte(&huart, (uint8_t*)(buff),len, 100);

      config_file.doorOpenTimeSec = *value;
    }
      break;
    case DOOR_CLOSE_TIME:
    if(len == sizeof(config_file.doorCloseTimeSec))
    {
          int len = snprintf(buff, sizeof(buff), "Number of Door close = ",*value);
          HAL_UART_Transmitte(&huart, (uint8_t*)(buff),len, 100);

      config_file.doorCloseTimeSec = *value;
    }
    break;
    case DOOR_OPEN_CALL_TIME:

    int len = snprintf(buff, sizeof(buff), "Number of Door open time during call = ",*value);
    HAL_UART_Transmitte(&huart, (uint8_t*)(buff),len, 100);

    if(len == sizeof(config_file.doorOpenTimeDuringCall))
    {
      config_file.doorOpenTimeDuringCall = *value;
    }
      break;
    case SHAFT_MAC_ID:
    if(len == sizeof(config_file.macIdShaft))
    {
      memcpy(config_file.macIdShaft, value,sizeof(config_file.macIdShaft));
    }
      break;
    case CABIN_MAC_ID:
    if(len == sizeof(config_file.macIdCabin))
    {
      memcpy(config_file.macIdCabin, value,sizeof(config_file.macIdCabin));
    }
      break;
    case LIDAR_VALUE_AT_FLOOR:
    if(len == sizeof(config_file.LIDAR))
    {
      memcpy(config_file.LIDAR, value,sizeof(config_file.LIDAR));
    }
      break;
    default:
      break;
  }
  return 0;
}

uint8_t readParameter(PARAMETER_NAME_t param, uint8_t *value)
{
  uint8_t length;
  switch (param)
  {
    case NO_OF_FLOORS:
    {
      memcpy(value ,&config_file.numberOfFloors ,sizeof(config_file.numberOfFloors));
      length = sizeof(config_file.numberOfFloors);
    }
      break;
    case DOOR_OPEN_TIME:
    {
      length = sizeof(config_file.doorOpenTimeSec);
      memcpy(value ,&config_file.doorOpenTimeSec ,sizeof(config_file.doorOpenTimeSec));
    }
      break;
    case DOOR_CLOSE_TIME:
    {
      length = sizeof(config_file.doorCloseTimeSec);
      memcpy(value ,&config_file.doorCloseTimeSec ,sizeof(config_file.doorCloseTimeSec));
    }
      break;
    case DOOR_OPEN_CALL_TIME:
    {
      length = sizeof(config_file.doorOpenTimeDuringCall);
      memcpy(value ,&config_file.doorOpenTimeDuringCall ,sizeof(config_file.doorOpenTimeDuringCall));
    }
      break;
    case SHAFT_MAC_ID:
    {
      length = sizeof(config_file.macIdShaft);
      memcpy(value ,&config_file.macIdShaft ,sizeof(config_file.macIdShaft));
    }
      break;
    case CABIN_MAC_ID:
    {
      length = sizeof(config_file.macIdCabin);
      memcpy(value ,&config_file.macIdCabin ,sizeof(config_file.macIdCabin));
    }
      break;
    case LIDAR_VALUE_AT_FLOOR:
    {
      length = sizeof(config_file.LIDAR);
      memcpy(value ,&config_file.LIDAR ,sizeof(config_file.LIDAR));
    }
      break;
    default:
    length = 0;
      break;
  }
  return length;
}


void setDefaultMac(uint8_t *mac)
{
  uint8_t i =0;
  memcpy(defaultmacID, &mac, sizeof(config_file.macIdShaft));
}

void setNewMacID(uint8_t *mac)
{
  memcpy(config_file.macIdShaft, mac, sizeof(config_file.macIdShaft));
  for(int i=0;i<6;i++)
  {
	  int len = snprintf(buff, sizeof(buff), "%x_",config_file.macIdShaft[i]);
	  HAL_UART_Transmitte(&huart, (uint8_t*)(buff),len, 100);
  }
  if(writeConfigFile())
  {
    HAL_UART_Transmitte(&huart, (uint8_t*)("config passed"), sizeof("config passed"), 100);
  }
  else
  {
    HAL_UART_Transmitte(&huart, (uint8_t*)("write config failed"), sizeof("write config failed"), 100);
  }
  
  HAL_UART_Transmitte(&huart, (uint8_t*)("Peer mac set ---"), sizeof("Peer mac set ---"), 100);

}


void setShaftPeerMac(uint8_t *mac)
{
  Serial.println();
  bool compareResult = false;
  for (int i = 0; i < 6; i++)
  {
    if (mac[i] != config_file.macIdShaft[i])
    {
      config_file.macIdShaft[i]=mac[i];
      int len = snprintf(buff, sizeof(buff), "%x_",config_file.macIdShaft[i]);
      HAL_UART_Transmitte(&huart, (uint8_t*)(buff),len, 100);
      compareResult = true;
    }
  }
  if (!compareResult)
  {
    HAL_UART_Transmitte(&huart, (uint8_t*)("Existed MaC ID received"), sizeof("Existed MaC ID received"), 100);
    for(int i=0;i<6;i++)
    {
      int len = snprintf(buff, sizeof(buff), "%x_",config_file.macIdShaft[i]);
      HAL_UART_Transmitte(&huart, (uint8_t*)(buff),len, 100);
    }
  }
  else
  {
    HAL_UART_Transmitte(&huart, (uint8_t*)("new MaC ID received"), sizeof("new MaC ID received"), 100);
    newMacAvailable = true;
  }
}


bool returnAvailableState()
{
  return newMacAvailable;
}

void setAvailableState(bool state)
{
  newMacAvailable = state;
}
