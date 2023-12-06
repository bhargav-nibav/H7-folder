/*
 * espCom.h
 *
 *  Created on: Oct 27, 2023
 *      Author: Bhargav-4836
 */

#ifndef INC_ESPCOM_H_
#define INC_ESPCOM_H_




#include "common.h"
#include "debug.h"
#include "callReg.h"
#include "configuration.h"
#include "cabinControl.h"
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

/**
 * Shaft to Cabin request
 *c
 */
#define CABIN_LANDING_LEVER 1

#define OUTPUT_LIFT_EMERGENCY 1
#define OUTPUT_LIFT_CHILDLOCK 2
#define OUTPUT_LIFT_SIREN 3
#define OUTPUT_SHAFT_CONFIGURATION 4

#define CABIN_APP_AUTO_FLOOR_CALIB 1

typedef enum
{
  ESP_CMD_REQ_STATUS_COMPLETE,
  ESP_CMD_REQ_STATUS_BUSY,
  ESP_CMD_REQ_STATUS_ERROR,
} REQ_STATUS;


typedef enum
{
  ADVANCED_TAB_CLOSED,
  ADVANCED_TAB_OPENED,
  ADVANCED_TAB_SAVE_MESSAGE,
} ADVANCED_SETTING;

typedef enum
{
  OPEN_LANDING_LEVER,
  CLOSE_LANDING_LEVER,
  SEND_REGED_CALLS,
  SEND_SERVED_FLOOR_NUM,

} ESP_COMMAND_REQUEST;

// #ifdef __cplusplus
//  extern "C" {
// #endif /* __cplusplus */

void espCommInit();
void processEspCommunication();
/**
 * @brief Register call to shaft over esp communication.
 * @param[IN] floorNum floor number to register in shaft board.
 */
void sendCmdRegCabCallReq(uint8_t floorNum);

REQ_STATUS requestStatus();
void sendLiftEmergency(bool state);
void sendLiftChildlock(bool state);
void sendLiftSiren(bool _state);
void sendCalibrationRequest();
void sendLockStatusReq(uint32_t info);
void cabinCalibrationStatus(int call);
int cabinCurFloor();
// #ifdef __cplusplus
// }
// #endif /* __cplusplus */


#endif /* INC_ESPCOM_H_ */

void processConfiguration(uint32_t param);
void setConfigFile();
bool read_shaft_parameter(PARAMETER_NAME_t param);
void process_esp_request();
void printInactive();
void sendAndroidStatus(bool status);
void printShaftMacID();
void setShaftPeerMac(uint8_t *mac);
