/*
 * cabinControl.h
 *
 *  Created on: Oct 19, 2023
 *      Author: Bhargav-4836
 */

#ifndef INC_CABINCONTROL_H_
#define INC_CABINCONTROL_H_





#endif /* INC_CABINCONTROL_H_ */

#include "main.h"
#include "common.h"
#include "usart.h"
#include "gpio.h"
#include "stdbool.h"


typedef enum
{
  GND_FLOOR,
  FIRST_FLOOR,
  SECOND_FLOOR,
  THIRD_FLOOR,
  MAX_FLOOR,
} FLOOR_NUM;

FLOOR_NUM flr;

//#define LANDING_LEVER_OUTPUT_GPIO 40
//#define LIFT_EMERGENCY_INPUT_GPIO 38
//#define LIGHT_OUTPUT_GPIO 36
//#define FAN_OUTPUT_GPIO 35



/***** OUTPUT BIT status ********************************************************************/
#define OUTPUT_CABIN_LANDING_LEVER 1
#define OUTPUT_LIFT_OVERLOAD 2
#define OUTPUT_LIFT_DOOR_ALARM 3
#define OUTPUT_LIFT_DOOR_STATUS 4

#define OUTPUT_OTA_PROCESS 5
#define OUTPUT_BATTERY_STATE 6
/***** OUTPUT BIT END ***********************************************************************/
/**********************************autocalibration********************************************/
typedef enum
{
  AUTO_CALIB_IDLE,
  AUTO_CALIB_WAIT_DOOR_CLOSE,
  AUTO_CALIB_MSG_DOOR_CLOSE,
  AUTO_CALIB_CHECK_INPUTS,
  AUTO_CALIB_MSG_CHECK_INPUTS,
  AUTO_CALIB_PROCESSING,
  AUTO_CALIB_MSG_PROCESSING,
  AUTO_CALIB_ERROR,
  AUTO_CALIB_MSG_ERROR,
  AUTO_CALIB_COMPLETE,
  AUTO_CALIB_MSG_COMPLETE,
} AUTO_CALIB_APP_em;

//static AUTO_CALIB_APP_em autoCalibState;
void initCabinControl();
void cabinCurFloor();
/**
 * @brief Should be called in super loop to process the outputs
 *
 */
void procesOutputs();
void cabinRegisterCabCalls(uint32_t call);
void cabinServedOrClearCalls(uint32_t call);

/**
 * @brief from shaft it can request output to be processed in cabin.
 *
 * @param outputState
 */
void cabinSaveOutputToBeprocessed(uint32_t outputState);

void cabinClearOutputsFromShaft(uint32_t outputState);
void cabinSetOutputsFromShaft(uint32_t outputState);

void cabinDebugOutputs();

void initEmergency();
void Emergency_Read();
void LandingLeverActiveOutput(bool state);
void cabinCurrentFloorNum(uint32_t floorNum);
void cabinShaftIdleStatus(uint32_t idleStatus);

void initData();

bool returnCallStatus();

void shaftUpdateProcess(uint32_t info);
void displayEVStatus(uint32_t info);
void displayMotorStatus(uint32_t info);
void checkCallRegistration();

typedef enum
{
  CALL_SERVE_STS_NOFAIL,
  CALL_SERVE_STS_DOORLOCK_FAIL,
  CALL_SERVE_STS_SAFETY_FAILURE,
  CALL_SERVE_CAN_STS_SAFETY_FAILURE,
  CALL_SERVE_STS_LL_FAIL,
  CALL_SERVE_STS_LIDAR_FAIL,
  CALL_SERVE_STS_CAB_DERAIL,
  CALL_SERVE_STS_OVER_LOAD,
  CALL_SERVE_STS_OVER_SPEED,
  CALL_SERVE_STS_POWER_FAILURE,
  CALL_SERVE_STS_ML_FAIL_BEFORE_CALL,
  CALL_SERVE_STS_ESP_COM_FAILURE,
  CALL_SERVE_STS_ANDROID_FAILURE,
  CALL_SERVE_STS_COMPLETE
} SAFETY_CODE;

typedef enum{
  MECH_LOCK_0,
  MECH_LOCK_1,
  MECH_LOCK_2,
  MECH_LOCK_3,
  DOOR_SWITCH_0,
  DOOR_SWITCH_1,
  DOOR_SWITCH_2,
  DOOR_SWITCH_3,
  CAN_STATUS_0,
  CAN_STATUS_1,
  CAN_STATUS_2,
  CAN_STATUS_3,
}DEVICE_STATUS;

typedef enum{
  OTA_IDLE=0x00,
  OTA_WAIT_FOR_UPDATE,
  OTA_CONNECT_WIFI,
  OTA_POST_VERSION,
  OTA_PARSE_RESPONSE,
  OTA_GET_UPDATE,
  OTA_POST_RESULT,
  OTA_DISABLE_MESSAGE,
  OTA_RESTART
}OTA_STATE;

void displaySafetyFailure(uint32_t info, uint32_t liftStatus);

void processMachineState(uint32_t info);
void sendLockStatDisp(uint32_t  info, uint32_t liftStat);



void sendMultipleTimes();

void initCC();
void displayCCicon();
