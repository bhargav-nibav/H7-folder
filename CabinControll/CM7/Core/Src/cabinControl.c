/*
 * cabinControl.c
 *
 *  Created on: Oct 19, 2023
 *      Author: Bhargav-4836
 */


/**
 * @file cabinControl.cpp
 */
#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"
#include "cabinControl.h"
#include "debug.h"
#include "VoltageControl.h"

#include "stddef.h"
// #include "cabinUpdate.h"
#include "lightFanControl.h"
#include "displayError.h"
#include "childLockControl.h"
#include "contactCharger.h"
#include "messageQueue.h"


/*****************************************************************************************/
typedef void (*OUTPUTCONTROL)(bool);
/*****************************************************************************************/
static void callStatus();
bool sendMultipleRegisterMsg = false;
bool callRegisteredFlag = false;
bool callClearedFlag = false;
uint32_t desiredFloor = 0;
bool prevEmergencyState = false;

bool landingLeverStatus = false;
unsigned long int llMaxTimer = 0;
typedef struct cabinControlOutput
{
  uint8_t bit_pos;
  OUTPUTCONTROL outputControl;
} CAB_OUTPUT_CONTROL;

struct CABIN_CONTROL
{
  uint32_t outputState;
  uint32_t callRegistered;
};
uint32_t callReg = 0;

char buff[50];
const char emgon[] = "EMG on";
const char emgoff[] = "EMG off";

long int timerMulti = 0;
/*****************************************************************************************/
extern bool sliderEnabled;
bool callClearContent = false;
extern int curFloorcall;
extern bool enablePWMTimer;
/*****************************************************************************************/
void shaftReadyForUpdate();
void cancelShaftUpdateonJob();
/*****************************************************************************************/

struct CABIN_CONTROL cabinControl;
OTA_STATE cabin_ota = OTA_IDLE;
bool updateAvailable = false;
/*****************************************************************************************/
bool communicationFailure, normalSafetyFailure = false;
/*****************************************************************************************/
bool onJob, prevJobStatus;
extern bool updateShaft, updateCabin;
void cancelCabinUpdateonJob();
void cabinReadyForUpdate();
bool checkForWifiData;

extern bool ssidCabin, pswdCabin, cabinClient, urlObtained;

bool startota();
const char *ssid = "controller_device";
const char *password = "strongpassword";

/*****************************************************************************************/
void door_status_lightcontrol(bool state);
void initLandingLeverOutput();
void initCabinControl();
void doorAlarmOutput(bool state);
void OverloadAlarmActiveOutput(bool state);
void checkCallsRegstrationAndCalls();
void callRegsitered(uint8_t flr);
void callCleared(uint8_t flr);

void cabinCurrentFloorNum(uint32_t floorNum);

/*****************************************************************************************/
const CAB_OUTPUT_CONTROL cabinOutputs[] = {

    {OUTPUT_CABIN_LANDING_LEVER, LandingLeverActiveOutput},
    {OUTPUT_LIFT_OVERLOAD, OverloadAlarmActiveOutput},
    {OUTPUT_LIFT_DOOR_ALARM, doorAlarmOutput},
    {OUTPUT_LIFT_DOOR_STATUS, door_status_lightcontrol},
};

/*****************************************************************************************/

bool firstBooked, firstCleared = false;
/*****************************************************************************************/

void cabinRegisterCabCalls(uint32_t call)
{
  cabinControl.callRegistered = call;
  callReg = 0;
  int len = snprintf(buff, sizeof(buff), "Set call = %X" + cabinControl.callRegistered);
  HAL_UART_Transmit(&huart3, (uint8_t *)buff, len, 100);
}

void cabinServedOrClearCalls(uint32_t call)
{
  if (cabinControl.callRegistered != call)
  {
    cabinControl.callRegistered = call;
  }
  cabinControl.callRegistered &= ~(call);
//  Serial.printf("clear call = %d call = %d", cabinControl.callRegistered, call);
  int len = snprintf(buff, sizeof(buff), ("clear call = %ld" ), call);
  HAL_UART_Transmit(&huart3, (uint8_t *)buff, len, 100);
}

/*
void cabinCalibrationStatus(int call)
{
  switch (call)
  {
  case AUTO_CALIB_IDLE:
    Serial.println("Auto calibration is in AUTO_CALIB_IDLE state");
    break;
  case AUTO_CALIB_WAIT_DOOR_CLOSE:
     Serial.println("Auto calibration is in AUTO_CALIB_WAIT_DOOR_CLOSE state");
    break;
  case AUTO_CALIB_MSG_DOOR_CLOSE:
    Serial.println("Auto calibration is in AUTO_CALIB_MSG_DOOR_CLOSE state");
    break;
  case AUTO_CALIB_CHECK_INPUTS:
    Serial.println("Auto calibration is in AUTO_CALIB_CHECK_INPUTS state");
    break;
  case AUTO_CALIB_MSG_CHECK_INPUTS:
    Serial.println("Auto calibration is in AUTO_CALIB_MSG_CHECK_INPUTS state");
    break;
  case AUTO_CALIB_PROCESSING:
    Serial.println("Auto calibration is in AUTO_CALIB_PROCESSING state");
    break;
  case AUTO_CALIB_MSG_PROCESSING:
    Serial.println("Auto calibration is in AUTO_CALIB_MSG_PROCESSING state");
    break;
  case AUTO_CALIB_ERROR:
    Serial.println("Auto calibration is in AUTO_CALIB_ERROR state");
    break;
  case AUTO_CALIB_MSG_ERROR:
    Serial.println("Auto calibration is in AUTO_CALIB_MSG_ERROR state");
    break;
  case AUTO_CALIB_COMPLETE:
    Serial.println("Auto calibration is in AUTO_CALIB_COMPLETE state");
    break;
  case AUTO_CALIB_MSG_COMPLETE:
    Serial.println("Auto calibration is in AUTO_CALIB_MSG_COMPLETE state");
    break;
  default:
    Serial.println("wrong auto calibration state arrived.");
    break;
  }
}

*/



void procesOutputs()
{
  static uint32_t outputState;

  uint32_t currentOutputState = cabinControl.outputState;
  if (outputState != currentOutputState)
  {
    // Serial.printf("currentOutputState =%x \r\n",currentOutputState);
    for (uint8_t outputIndex = 0; outputIndex < LENGTH_OF_ARRAY(cabinOutputs); outputIndex++)
    {
      cabinOutputs[outputIndex].outputControl((bool)CHECK_BIT(currentOutputState, cabinOutputs[outputIndex].bit_pos));
      //break;
    }
    outputState = currentOutputState;
  }
}

void cabinSetOutputsFromShaft(uint32_t outputState)
{
  cabinControl.outputState |= outputState;
}

void cabinClearOutputsFromShaft(uint32_t outputState)
{
  cabinControl.outputState &= ~outputState;
}

void cabinSaveOutputToBeprocessed(uint32_t outputState)
{
  cabinControl.outputState = outputState;
}

void initCabinControl()
{
  initLandingLeverOutput();
//  initContactSenseOutput();
  initEmergency();
//  initChildLock();
//  initPWM();
  initDisplayPins();
//  resetRelay();
//  initVoltage();
}

/// @brief set landing lever gpio as output. set the landing lever state to disengage.
void initLandingLeverOutput()
{
//  pinMode(LANDING_LEVER_OUTPUT_GPIO, OUTPUT);
  LandingLeverActiveOutput(false);
}

/// @brief if state = 1, landing lever engage, else landing lever disengage.
/// @param state bool varaible containing required state landing lever should be in.
void LandingLeverActiveOutput(bool state)
{
  static bool prevState = false;
  if (!callRegisteredFlag && state)
  {
    enablePWMTimer = true;
  }
//  uint8_t buf[] = {0x7E, 0x7F};
  if (prevState != state)
  {
    landingLeverStatus = state;
    llMaxTimer = HAL_GetTick();
    HAL_GPIO_WritePin(landing_lever_GPIO_Port, landing_lever_Pin, landingLeverStatus);

    //    digitalWrite(LANDING_LEVER_OUTPUT_GPIO, landingLeverStatus);

    char buff[50];

   int len = snprintf(buff, sizeof(buff), "landing lever = %d",state);

    HAL_UART_Transmit(&huart3, (uint8_t *)buff, len, 100);

//    Serial.println("landing lever = " + String(landingLeverStatus));
    androidDataframe(LANDING_LEVER_STATUS, (uint8_t)landingLeverStatus);
    prevState = state;
  }
}

//void door_status_lightcontrol(bool state)
//{
////  Serial.println("DoorStatus = 0x" + String(state));
//	int len = snprintf(buff, sizeof(buff), "DoorStatus = %X", state);
//	HAL_UART_Transmit(&huart3, (uint8_t*) buff, len, 100);
//  if (!callRegisteredFlag)
//  {
//    enablePWMTimer = true;
//  }
//}

//
//void OverloadAlarmActiveOutput(bool state)
//{
//  DEBUG_PRINT_1("overload alaram = 0x" + String(state));
//  if (state)
//  {
//    CLR_BIT(cabinControl.outputState, OUTPUT_LIFT_OVERLOAD);
//    androidDataframe(SAFETY_FAIL, OVERLOAD_DEVICE_FAILURE);
//    androidDataframe(SAFETY_FAIL, OVERLOAD_DEVICE_FAILURE);
//  }
//}

/// @brief read the input from emergency button. On button press, send emergency on and release send emergency release.
void Emergency_Read()
{
//  bool state = !digitalRead(LIFT_EMERGENCY_INPUT_GPIO);
	bool state = !(HAL_GPIO_ReadPin(emergency_GPIO_Port, emergency_Pin));
 if (prevEmergencyState != state)
 {
   if (!state)
   {
//      ESPCOM_PRINT("EMG on");
     HAL_UART_Transmit(&huart3,(uint8_t *)emgon, sizeof(emgon), 100);
    //  sendLiftEmergency(true);
   }
   else
   {
//      ESPCOM_PRINT("EMG off");
     HAL_UART_Transmit(&huart3,(uint8_t *)emgoff, sizeof(emgoff), 100);
    //  sendLiftEmergency(false);
   }
 }
 prevEmergencyState = state;
}

void callRegsitered(uint8_t flr)
{
  onJob = true;
  // fan light always on
  callRegisteredFlag = true;
  callClearedFlag = false;
  resetTimer();
//  controlPWMState(true);
  desiredFloor = flr;

androidDataframe(CALL_BOOKING, (uint8_t)flr);



  switch (flr)
  {
  case GND_FLOOR:
    /* code */
//    CAB_PRINT("ground floor booked");
    // callReg=0;
    break;
  case FIRST_FLOOR:
//    CAB_PRINT("first floor booked");
    // callReg=1;
    break;
  case SECOND_FLOOR:
//    CAB_PRINT("second floor booked");
     break;
  case THIRD_FLOOR:
//    CAB_PRINT("Third floor booked");
     break;
  default:
//    Serial.println("Something else booked");
     break;
  }
//  Serial.println("callreg : " + String(callReg)); // callReg=0;
  	  int len = snprintf(buff, sizeof(buff),"callreg : %ld", callReg) ;
		HAL_UART_Transmit(&huart3, (uint8_t*)buff , len, 100);
}

void callCleared(uint8_t flr)
{
//  
  callRegisteredFlag = false;
  callClearedFlag = true;
  onJob = false;
  enablePWMTimer = true;
  desiredFloor = flr;
 // Serial.printf("callreg : %d \r\n", callReg);
  int len = snprintf(buff, sizeof(buff), ("callreg : %ld") , callReg);
  HAL_UART_Transmit(&huart3, (uint8_t*)buff , len, 100);

androidDataframe(CALL_CLEAR, (uint8_t)flr);

  // callReg=0; if call reg is not 0, then enable it
  switch (flr)
  {
  case GND_FLOOR:
//    CAB_PRINT("ground floor cleared");
    // callReg = 0;
    break;
  case FIRST_FLOOR:
//    CAB_PRINT("first floor cleared");
    // firstCleared = true;
    break;
  case SECOND_FLOOR:
//    CAB_PRINT("second floor cleared");
    break;
  case THIRD_FLOOR:
//    CAB_PRINT("third floor cleared");
    break;
  default:
//    Serial.println("Something else captured");
    break;
  }
}

// p = 0000 // n = 0001 & == 0
// p = 0000 // n = 0001 | == 1
void checkCallsRegstrationAndCalls()
{
  if (cabinControl.callRegistered != callReg)
  {
//    Serial.printf("check call ress = 0x%x  reg val = 0x%x \r\n", callReg, cabinControl.callRegistered);
    for (uint8_t flr = false; flr < MAX_FLOOR; flr++)
    {
      if ((!CHECK_BIT(callReg, flr)) && CHECK_BIT(cabinControl.callRegistered, flr))
      {
        // call registered
        //CAB_PRINT_1("call reg = 0x" + String(flr));
        desiredFloor = flr;
        sendMultipleRegisterMsg = true;
        callRegisteredFlag = true;
        callClearedFlag = false;
        desiredFloor = flr;
        LandingLeverActiveOutput(false);
      }
      else if ((CHECK_BIT(callReg, flr)) && (!CHECK_BIT(cabinControl.callRegistered, flr)))
      {
        // call cleared
        sendMultipleRegisterMsg = false;
        //CAB_PRINT_1("call cleared = 0x" + String(flr));
        callCleared(flr);
      }
    }
    callReg = cabinControl.callRegistered;
  }
}

void cabinDebugOutputs()
{
  checkCallsRegstrationAndCalls();
}

void initData()
{
  HAL_Delay(50);
  initCC();                              // contact charger
  initCL();                              // child lock
  cabinCurrentFloorNum(cabinCurFloor()); // current floor number
  callStatus();                          // cleared or registered
}


void cabinCurrentFloorNum(uint32_t floorNum)
{
//  uint8_t buf[] = {0x70, 0x71, 0x72, 0x73};
  androidDataframe(CABIN_FLOOR, (uint8_t) floorNum);

  switch (floorNum)
  {
  case GND_FLOOR:
//    Serial.println("Cabin at ground floor");
    break;
  case FIRST_FLOOR:
//    Serial.println("Cabin at first floor");
    break;
 case SECOND_FLOOR:
//   Serial.println("Cabin at second floor");
   break;
 case THIRD_FLOOR:
//   Serial.println("Cabin at third floor");
   break;
  default:
//    Serial.println("Something else captured");
    break;
  }
  // update floor number on display automatically.
  // will cause fan light to be enabled automatically for 30s.
  if (!callRegisteredFlag)
  {
    //Serial.printf("automatic LOP floor update %d %d", floorNum, desiredFloor);
    callCleared(floorNum);
  }
}

void cabinShaftIdleStatus(uint32_t idleStatus)
{

  if (communicationFailure)
  {
    cabinControl.callRegistered = 0;
    checkCallsRegstrationAndCalls();
    cabinCurrentFloorNum(0x00);
    communicationFailure = false;
  }

  if (idleStatus == 0x0001)
  {
    //Serial.println("Shaft is in idle state");

  }
  else if (idleStatus == 0x0000)
  {
    //Serial.println("Shaft is in busy state");
  }
  else
  {
    //Serial.println("Shaft status Something else captured");
  }
  if (!callRegisteredFlag)
  {
    enablePWMTimer = true;
  }
}

/// @brief Door related function
/// @param state receives either true or false
/// @todo Decide whether it is needed or not
void doorAlarmOutput(bool state)
{
  if (state)
  {
    // update floor number on slider automatically.
    if (!callRegisteredFlag)
    {
      if (cabinCurFloor() != desiredFloor)
      {
//        Serial.println("2. automatic LOP floor update");
        desiredFloor = cabinCurFloor();
        // callCleared(desiredFloor);
      }
    }
  }
  int len = snprintf(buff, sizeof(buff), ("door alarm msg  = %ob " ), state);

  HAL_UART_Transmit(&huart3, (uint8_t *)buff, len, 100);
}

//void checkErr()
//{
//  esp_reset_reason_t err_code = esp_reset_reason();
//  switch (err_code)
//  {
//  case ESP_RST_UNKNOWN:
//    Serial.println("ESP_RST_UNKNOWN");
//    break;
//  case ESP_RST_POWERON:
//    Serial.println("ESP_RST_POWERON");
//    break;
//  case ESP_RST_EXT:
//    Serial.println("ESP_RST_EXT");
//    break;
//  case ESP_RST_SW:
//    Serial.println("ESP_RST_SW");
//    break;
//  case ESP_RST_PANIC:
//    Serial.println("ESP_RST_PANIC");
//    break;
//  case ESP_RST_INT_WDT:
//    Serial.println("ESP_RST_INT_WDT");
//    break;
//  case ESP_RST_TASK_WDT:
//    Serial.println("ESP_RST_TASK_WDT");
//    break;
//  case ESP_RST_WDT:
//    Serial.println("ESP_RST_WDT");
//    break;
//  case ESP_RST_BROWNOUT:
//    Serial.println("ESP_RST_BROWNOUT");
//    break;
//  case ESP_RST_SDIO:
//    Serial.println("ESP_RST_SDIO");
//    break;
//  default:
//    break;
//  }
//}

/// @brief send the current status of the call book / clear
static void callStatus()
{
  if (callRegisteredFlag && !callClearedFlag)
  {
    // call is registered send the floor number with booked message to android
//    Serial.println("Implement timer to clear registered call if no clear message is available");
    callRegsitered(desiredFloor);
  }
  else if (!callRegisteredFlag && callClearedFlag)
  {
    // call is cleared and no new calls are registered
    callCleared(desiredFloor);
  }
  else if (!callClearedFlag && !callRegisteredFlag)
  {
    // no call registeration and no call clear. cabin is initialized after restart.
    //  send current floor number as clear message, by default will be zero
    callCleared(cabinCurFloor());
  }
}

/// @brief funtion to check whether the lift call is cleared or not
bool returnCallStatus()
{
  return callClearedFlag;
}

void shaftUpdateProcess(uint32_t info)
{
  if (info == 1)
  {
//    Serial.println("shaft update in progress");
//    
androidDataframe(UPDATE_STATUS, UPDATE_AVAILABLE);

    
  }
  else if (info == 0)
  {
//    Serial.println("Shaft update done or cancelled");
//    

androidDataframe(UPDATE_STATUS, UPDATE_DONE);
    
  }
  else if (info == 2)
  {
//    Serial.println("cabin update available");
    updateAvailable = true;
  }
  else
  {
//    Serial.println("Error message received");
  }
}

void displayMotorStatus(uint32_t info)
{
//  uint8_t buf[] = {0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B};
  switch (info)
  {
  case 0:

    HAL_UART_Transmit(&huart3, (uint8_t *)"Motor off", sizeof("Motor off"), 100);

    break;
  case 1:

    HAL_UART_Transmit(&huart3, (uint8_t *)"One motor", sizeof("One motor"), 100);
    break;
  case 2:

    HAL_UART_Transmit(&huart3, (uint8_t *)"Two motor", sizeof("Two motor"), 100);
    break;
  case 3:

    HAL_UART_Transmit(&huart3, (uint8_t *)"Three motor", sizeof("Three motor"), 100);
    break;
  case 4:

    HAL_UART_Transmit(&huart3, (uint8_t *)"Four motor", sizeof("Four motor"), 100);
    break;
  case 5:

    HAL_UART_Transmit(&huart3, (uint8_t *)"Five motor", sizeof("Five motor"), 100);
    break;
  case 6:

    HAL_UART_Transmit(&huart3, (uint8_t *)"Six motor", sizeof("Six motor"), 100);
    break;
  case 7:

    HAL_UART_Transmit(&huart3, (uint8_t *)"Seven motor", sizeof("Seven motor"), 100);
    break;
  default:

    HAL_UART_Transmit(&huart3, (uint8_t *)"Error", sizeof("Error"), 100);
    break;
  }
  // have not set value for 0 motor

androidDataframe(MOTOR_COUNT, (uint8_t)info);
}

void displayEVStatus(uint32_t info)
{
// uint8_t buf[] = {0x7E, 0x7F};

 androidDataframe(EVO_STATUS, (uint8_t)info);
 switch (info)
 {
 case 0:

   HAL_UART_Transmit(&huart3, (uint8_t *)"EVO off", sizeof("EVO off"), 100);
   break;
 case 1:

   HAL_UART_Transmit(&huart3, (uint8_t *)"EVO On", sizeof("EVO On"), 100);
   break;
 default:

   HAL_UART_Transmit(&huart3, (uint8_t *)"Error value", sizeof("Error value"), 100);
   break;
 }
}

void displaySafetyFailure(uint32_t info, uint32_t liftStatus)
{
  // 
  switch (info)
  {
 case CALL_SERVE_STS_DOORLOCK_FAIL:
 {
      char doorfail[] = "DOOR_SWITCH_Error";
	 HAL_UART_Transmit(&huart3, (uint8_t *)doorfail, sizeof(doorfail), 100);
   //sendLockStatDisp(liftStatus, 0x00);

androidDataframe(SAFETY_FAIL, DOOR_FAILURE);
 }
 break;
 case CALL_SERVE_STS_SAFETY_FAILURE:
 {
   char mlfail[] = "MECH_LOCK_Error";

	 HAL_UART_Transmit(&huart3, (uint8_t *)mlfail, sizeof(mlfail), 100);
androidDataframe(SAFETY_FAIL, MECH_FAILURE);
 }

 break;
 case CALL_SERVE_CAN_STS_SAFETY_FAILURE:
 {
   communicationFailure = true;

   char canfail[] = "CAN_ERROR";
   HAL_UART_Transmit(&huart3, (uint8_t *)canfail, sizeof(canfail), 100);
androidDataframe(SAFETY_FAIL, CAN_FAILURE);
 }
 break;
  case CALL_SERVE_STS_LL_FAIL:
  {
    // communicationFailure = true;
    char llfail[] = "landing lever fail";
    HAL_UART_Transmit(&huart3, (uint8_t *)llfail, sizeof(llfail), 100);
    // LandingLeverActiveOutput(false)

  androidDataframe(SAFETY_FAIL, LANDING_LEVER_FAILURE);
  }
  break;
  case CALL_SERVE_STS_LIDAR_FAIL:
  {
    communicationFailure = true;
     char ldrfail[] = "LIDAR_1";
    HAL_UART_Transmit(&huart3, (uint8_t *)ldrfail, sizeof(ldrfail), 100);
    androidDataframe(SAFETY_FAIL, LIDAR_FAILURE);
  }
  break;
  case CALL_SERVE_STS_CAB_DERAIL:
  {
    char cabdir[] = "CALL_SERVE_STS_CAB_DERAIL";
    HAL_UART_Transmit(&huart3, (uint8_t *)cabdir, sizeof(cabdir), 100);
    androidDataframe(SAFETY_FAIL, CABIN_DERAIL_FAILURE);
  }
  break;
  case CALL_SERVE_STS_OVER_LOAD:
  {
    char ovtriger[] = "OV_TRIGGERED";
    HAL_UART_Transmit(&huart3, (uint8_t *)ovtriger, sizeof(ovtriger), 100);
    androidDataframe(SAFETY_FAIL, OVERLOAD_DEVICE_FAILURE);
  }
  break;
  case CALL_SERVE_STS_POWER_FAILURE:
  {
    communicationFailure = true;

    char pwrfail[] = "CALL_SERVE_STS_POWER_FAILURE";
    HAL_UART_Transmit(&huart3, (uint8_t *)pwrfail, sizeof(pwrfail), 100);

  androidDataframe(SAFETY_FAIL, POWER_FAILURE);
  }
  break;
  case CALL_SERVE_STS_ML_FAIL_BEFORE_CALL:
  {
	 char mlfail[] = "MECH_LOCK_Error";
    HAL_UART_Transmit(&huart3, (uint8_t *)mlfail, sizeof(mlfail), 100);

androidDataframe(SAFETY_FAIL, MECH_FAILURE);
  }
  break;
  case CALL_SERVE_STS_ESP_COM_FAILURE:
  {

    char espfail[] = "ESP_COM_FAILURE";
    HAL_UART_Transmit(&huart3, (uint8_t *)espfail, sizeof(espfail), 100);


androidDataframe(SAFETY_FAIL, ESP_COM_FAILURE);
  }
  break;
  case CALL_SERVE_STS_ANDROID_FAILURE:
  {
    {
      communicationFailure = true;
      char androiderroer[] = "Android failure error";
      HAL_UART_Transmit(&huart3, (uint8_t *)androiderroer, sizeof(androiderroer), 100);

androidDataframe(SAFETY_FAIL, ANDROID_FAILURE);
    }
  }
  default:
  {
    int len = snprintf(buff, sizeof(buff), ("received status %ld"), info);
    HAL_UART_Transmit(&huart3, (uint8_t *)buff, len, 100);
  }
  break;
  }
  // 
}
void checkCallRegistration()
{
  static bool curRegis = false;
  static bool callTimerflag = false;
  static uint32_t callTimerMillis;

  if (curRegis != callRegisteredFlag)
  {
    if (callRegisteredFlag)
    {
      // trigger timer
      callTimerflag = true;
      callTimerMillis = HAL_GetTick();
    //  Serial.println("call will be cleared after 75s if no clear message");
    }
    else
    {
      // reset timer
      callTimerflag = false;
    }
  }
  curRegis = callRegisteredFlag;
  if (landingLeverStatus)
  {
    if (HAL_GetTick() - llMaxTimer > 60 * 1000)
    {
      LandingLeverActiveOutput(false);
      llMaxTimer = HAL_GetTick();
    }
  }
  if (callTimerflag)
  {
    if ((HAL_GetTick() - callTimerMillis) >= 100 * 1000)
    {
      // clear call or send a request to shaft for recent status
//      Serial.println("call booked for more than 75s");
      cabinControl.callRegistered = 0;
      callTimerflag = false;
    }
  }
}

//void processMachineState(uint32_t info)
//{
//}
//
void sendLockStatDisp(uint32_t info, uint32_t liftStat)
{
 uint8_t doorLock = info;
 uint8_t mechLock = (info >> 16);

// uint8_t mechLockArray[] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87};
// uint8_t doorLockArray[] = {0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F};

 for (int i = 0; i < 4; i++)
 {
   if (CHECK_BIT(doorLock, i))
   {

//      Serial.printf("Floor  %d Door is closed ", i);
   	int len = snprintf(buff, sizeof(buff), "Floor  %d  Door is open ", i);
   	HAL_UART_Transmit(&huart3, (uint8_t *)buff, len, 100);

androidDataframe(DOOR_CLOSE_STATUS, i);
   }
   else
   {
//      Serial.printf("Floor  %d Door is open ", i);//
   	int len = snprintf(buff, sizeof(buff), ("Floor  %d Door is open "), i);
   	HAL_UART_Transmit(&huart3, (uint8_t *)buff, len, 100);
    androidDataframe(DOOR_OPEN_STATUS, i);
   }

   if (CHECK_BIT(mechLock, i))
   {
//      Serial.printf("Mechanical lock is open\n");
   	char mlOpen[] = "Mechanical lock is open\n";
   	      HAL_UART_Transmit(&huart3, (uint8_t *)mlOpen, sizeof(mlOpen), 100);
	    androidDataframe(MECH_LOCK_OUTSIDE, i);
   }
   else
   {
//      Serial.printf("Mechanical lock is closed\n");
     char mlClose[] = "Mechanical lock is closed\n";
     HAL_UART_Transmit(&huart3, (uint8_t *)mlClose, sizeof(mlClose), 100);
     androidDataframe(MECH_LOCK_INSIDE, i);
   }
 }
 if (liftStat)
 {
   uint32_t tmpVar = 0;
   SET_BIT(tmpVar, (liftStat - 1));
   cabinRegisterCabCalls(tmpVar);
 }
}

void sendMultipleTimes()
{
  static int multiCounter = 0;

  if (sendMultipleRegisterMsg && (HAL_GetTick() - timerMulti > 100))
  {
    multiCounter++;
    callRegsitered(desiredFloor);
    timerMulti = HAL_GetTick();
  }
  if (multiCounter >= 1)
  {
    multiCounter = 0;
    sendMultipleRegisterMsg = false;
  }
}

void initCC()
{
    int chargingState = getCCstate();
    toggleChargeIcon(chargingState);
}

/// @brief funtion to display charging icon
// if charging, charging icon will be displayed
// if discharge during call clear state, it will be shown as error
// if discharge during call booked state, it will be empty

void displayCCicon()
{

  long int interV = 0;
  static bool prevBatteryState = false;
  bool chargingState =  getCCstate();
  if(prevBatteryState!=getCCstate())
  {
    prevBatteryState=getCCstate();
    interV=HAL_GetTick();
  }
  if(HAL_GetTick()- interV > 1*2000)
  {

    interV = HAL_GetTick();
    toggleChargeIcon(chargingState);
  }
}
