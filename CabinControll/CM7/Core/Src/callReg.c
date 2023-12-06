/*
 * callReg.c
 *
 *  Created on: Oct 19, 2023
 *      Author: Bhargav-4836
 */



/**
 * @file callReg.cpp
*/

#include "callReg.h"
#include "cabinControl.h"
#include "common.h"
#include "espCom.h"
#include "debug.h"
#include <math.h>
#include "lightFanControl.h"
#include <stdio.h>
#include "displayError.h"

uint8_t widgetNumber = 0xFF;
bool callBookEnabled = false;

bool callEngageEnabled = false; // for both 2nd and 3rd page
bool pwmEnabled = false;
bool errorEnabled = false;
bool factorySettingsEnabled = false;
bool enableSiren = false;
int autoReturn = 2000;


extern uint32_t callReg;
////siren related variables
#define SIREN_TIMEOUT 5000
#define HB_TIMEOUT 3200
bool sirenStatus = false;
bool hBStatus = false;
bool heartBeatTimerStart = false;
uint32_t sirenDisableTime, hBTimeOut;
// uint32_t sirenDisableTime;
bool sirenTimerStart = false;
// android floor related
int curFloorcall = 0;

#define CALL_CHECK_TIMEOUT 100

#define CALL_HEADER1 0x65
#define CALL_HEADER2 0x00

#define COMPONENT_HEADER1 0x01
#define COMPONENT_HEADER2 0x11

#define COMPONENT_HEADER3 0x04 // replace with slider
#define AUTO_RETURN 0x03

#define PAGE2_HEADER 0x02
#define PAGE2_ID 0x0B

#define PAGE3_HEADER 0x03
#define PAGE3_ID 0x0C
#define EMG_HEADER 0x04
#define EMG_ID 0x00

#define PAGE_ZER0 0x00
#define PAGE_ONE 0x01
#define PAGE_TWO 0x02
#define PAGE_THREE 0x03
#define PAGE_FOUR 0x04
#define PAGE_FIVE 0x05
#define PAGE_SIX 0x06
#define PAGE_SEVEN 0x07
#define CALIBRATION_REQUEST 0x70
#define buttonWidget 0x00
#define sliderWidget 0x04
#define sirenWidget 0x09
#define emergencyWidget 0x08
#define callWidget 0x11
#define dataRequestWidget 0x0A


/**
 * configuration related widget id, boolean and data points
*/

#define CONFIGURATION_WIDGET 0x90
bool configurationDataEnabled = false;
bool lidarConfig = false;
bool advanedSettingTabStatus = false;
bool advancedTabOpened = false;
bool sendSaveConfig = false;
bool ascendingSLStartConfig, ascendingSLEndConfig, descendingSLStartConfig, dlDurationConfig = false;
bool llDurationConfig, avgUpSpeedConfig, avgupSlowSpeedConfig, avgDownSpeedConfig = false;
bool cabinMcIDConfig, shaftMcIDConfig =false;

bool upDownSpeedConfig = false;
bool ascendingConfig = false;
bool servoMaxOpenConfig = false;
bool servoMinCloseConfig = false;

bool setMacConfig = false;
bool setShaftMac = false;
bool setCabinMac = false;
uint8_t mac[6];

#define MAX_VALUES 4
int maxLidar = 0;
int lidarPos[MAX_VALUES]= {0};

int ascendingSLStart, ascendingSLEnd, descendingSLStart, dlDuration= 0;
int llDuration, avgUpSpeed, avgupSlowSpeed, avgDownSpeed = 0;

uint32_t servoMinSet = 0;
uint32_t servoMaxSet = 0;
/*
Android data format
*/

#define SLIDER_ID1 0x01
#define SLIDER_ID2 0x02
#define SLIDER_ID3 0x03
#define CALL_TRAIL_HEAD 0xFF

#define CALL_ATTEND 0x00
#define CALL_DISCONNECT 0x01
#define OUT_GOING 0x11
#define CALL_TRAIL_HEAD 0xFF

#define FIRMWARE_VERSION 0x87
#define HEART_BEAT 0x76
bool getCabinVersion, getShaftVersion;

int cabinMinor, cabinMajor;
int shaftMajor, shaftMinor;

bool checkFirmwareEnabled;
#define GET_CREDENTIALS 0x88
#define FIRMWARE_AVAILABLE 0x89
bool updateCabin, updateShaft;
bool notifyUpdateEnabled;

extern String ssidcabin, ssidshaft, pswdcabin, pswdshaft, cabinclient, shaftclient, usrnmserver, pswdserver;
bool ssidCabin, ssidShaft, pswdCabin, pswdShaft, cabinClient, shaftClinet, usrnmServer, pswdServer;
bool urlObtained;
extern String updateUrlHost, updateUrlPath;

bool autoreturn = false;
uint16_t get_timevalue = 2;
//
bool gotPWM = false;
bool pwmID1 = false;
bool pwmID2 = false;
bool firstTime = true;

bool sliderEnabled = false;
int slidedTimes = -10000;


//
/*User settings fan and light duration configuration*/
#define pwmDurationConfig 0x04
bool pwmDurationFlag = false;

/*Factory settings - configuration*/
//
bool calibrationMessage = false;
typedef enum
{
  CHECK_HEADER1,
  CHECK_PAGE,
  CHECK_COMPONENT,
  COMPONENT_NUMBER,
  GET_DATA1,
  CHECK_FOOTER
} CABIN_PACKET_REQ;

const uint16_t callFloorValue[FLOOR_NUM::MAX_FLOOR] = {0x1201, 0x0101, 0x1101, 0x1301};

FLOOR_NUM getCabincall()
{
  uint8_t rbyte;
  uint32_t startTime;
  CABIN_PACKET_REQ cabinCallButtonState;
  uint16_t callVaue;
  bool loopCon;
  uint8_t pageNum = 0xFF;
  // pageCheck pageValues;
  startTime = millis();
  cabinCallButtonState = CHECK_HEADER1;
  callVaue = 0;

  loopCon = true;
  while (loopCon && Serial1.available())
  {
    rbyte = Serial1.read();

     //Serial.println("Received Serial Data"+String(rbyte));
    switch (cabinCallButtonState)
    {
    case CHECK_HEADER1:
      if (rbyte == CALL_HEADER1)
      {
        cabinCallButtonState = CHECK_PAGE;
      }
      break;
    case CHECK_PAGE:
      pageNum = rbyte;
      cabinCallButtonState = CHECK_COMPONENT;
      break;
    case CHECK_COMPONENT:
      switch (pageNum)
      {
      case PAGE_ZER0:
        if (rbyte == buttonWidget)
        {
          callBookEnabled = true;
          cabinCallButtonState = COMPONENT_NUMBER;
        }
        else if (rbyte == sirenWidget)
        {
          enableSiren = true;
          cabinCallButtonState = GET_DATA1;
        }
        else if (rbyte == emergencyWidget)
        {
          callReg = 0;
          ESP.restart();
        }
        else if (rbyte == dataRequestWidget)
        {
          initData();
          // ESP.restart();
        }
        break;
      case PAGE_THREE:
        if (rbyte == buttonWidget)
        {
          callEngageEnabled = true;
          cabinCallButtonState = COMPONENT_NUMBER;
        }
        break;
      case PAGE_FOUR:
        if (rbyte == buttonWidget)
        {
          pwmEnabled = true;
          cabinCallButtonState = COMPONENT_NUMBER;
        }
        else if (rbyte == pwmDurationConfig)
        {
          pwmDurationFlag = true;
          cabinCallButtonState = COMPONENT_NUMBER;
        }
        break;
      case PAGE_FIVE:
        if (rbyte == buttonWidget)
        {
          errorEnabled = true;
          cabinCallButtonState = COMPONENT_NUMBER;
        }
        break;
      case PAGE_SIX:
        if (rbyte == buttonWidget)
        {
          factorySettingsEnabled = true;
          cabinCallButtonState = COMPONENT_NUMBER;
        }
        break;
      case PAGE_SEVEN:
        if (rbyte == buttonWidget)
        {
          factorySettingsEnabled = true;
          cabinCallButtonState = COMPONENT_NUMBER;
        }
        break;
      case FIRMWARE_VERSION:
        if (rbyte == 0x00)
        {
          // get major and minor of cabin
          getCabinVersion = true;
        }
        else if (rbyte == 0x01)
        {
          // get major and minor of shaft
          getShaftVersion = true;
        }
        checkFirmwareEnabled = true;
        cabinCallButtonState = COMPONENT_NUMBER;
        break;
      case CALIBRATION_REQUEST:
        calibrationMessage = true;
        cabinCallButtonState = CHECK_FOOTER;
        break;
      case HEART_BEAT:
        hBStatus = true;
        Serial.println("Heart beat received");
        cabinCallButtonState = CHECK_FOOTER;
        break;
      case GET_CREDENTIALS:
        if (rbyte == 0x01)
        {
          int wifiSSIDSize = Serial1.read();
          ssidcabin = "";
          for (int i = 0; i < wifiSSIDSize; i++)
          {
            ssidcabin += char(Serial1.read());
          }
          Serial.println("received ssid " + ssidcabin);
          ssidCabin = true;
        }
        else if (rbyte == 0x02)
        {
          int wifiPasswordSize = Serial1.read();
          Serial.println("Cabin password size " + String(wifiPasswordSize));
          pswdcabin = "";
          for (int i = 0; i < wifiPasswordSize; i++)
          {
            pswdcabin += char(Serial1.read());
          }
          Serial.println("received password " + pswdcabin);
          pswdCabin = true;
        }
        if (Serial1.read() == 0x00)
        {
          Serial.println("last byte received from dynamic wifi setting");
        }
        cabinCallButtonState = CHECK_FOOTER;
        break;
      case FIRMWARE_AVAILABLE:
        Serial1.flush();
        if (rbyte == 0x01)
        {
          updateShaft = true;
          
          #ifdef ENABLE_PROTO

          #else
            Serial1.write(0xD4);
          #endif
          Serial.println("Shaft update available");
        }
        else if (rbyte == 0x00)
        {
          updateCabin = true;
          Serial.println("Cabin update available");
        }
        else if (rbyte == 0x02)
        {
          int hotspotStatus = Serial1.read();
          Serial.printf("check wifi status %d", hotspotStatus);
          switch (hotspotStatus)
          {
          case 0x00:
            Serial.println("unknown devices");
            break;
          case 1:
            Serial.println("cabin connected");
            break;
          case 0x02:
            Serial.println("Shaft connected");
            break;
          default:
            Serial.println("no other");
            break;
          }
        }
        else if (rbyte == 0x04)
        {
          // Serial.print("received url with maxsize");
          int maxSize = Serial1.read();
          char arr[maxSize];
          String s;
          for (int i = 0; i < maxSize; i++)
          {
            arr[i] = Serial1.read();
            s += char(arr[i]);
          }
          updateUrlHost = s;
          Serial.println(s);
        }
        else if (rbyte == 0x05)
        {
          // Serial.print("received url with maxsize");
          int maxSize = Serial1.read();
          char arr[maxSize];
          String s;
          for (int i = 0; i < maxSize; i++)
          {
            arr[i] = Serial1.read();
            s += char(arr[i]);
          }
          updateUrlPath = s;
          Serial.println(s);
          if (strlen(s.c_str()) > 0)
          {
            urlObtained = true;
          }
        }
        Serial1.flush();
        cabinCallButtonState = CHECK_FOOTER;
        break;
      case CONFIGURATION_WIDGET:
        switch(rbyte)
        {
          case 00:
          {
            advanedSettingTabStatus = true;
            Serial.println("received advanced tab handler");
          }
            break;
          case 01:
            Serial.println("Lidar config received");
            lidarConfig = true;
            break;
          case 2:
            ascendingConfig = true;
            break;
          case 3:
            descendingSLStartConfig = true;
            break;
          case 4:
            dlDurationConfig = true;
            break;
          case 5:
            llDurationConfig = true;
            break;
          case 6:
            upDownSpeedConfig = true;
            break;
          case 7:
            setMacConfig = true;
            break;
          case 8:
            Serial.println("reseved for future use");
            break;
          default:
            break;
        }
        cabinCallButtonState = COMPONENT_NUMBER;
        break;
      default:
        cabinCallButtonState = CHECK_FOOTER;
        break;
      }
      break;
    case COMPONENT_NUMBER:
      if(lidarConfig)
      {
        lidarConfig=false;
        //erase any existing values
        for(int res=0;res<MAX_VALUES;res++)
        {
            lidarPos[res]=0;
        }
        maxLidar = rbyte/2;
        for(int i = 0; i<maxLidar*2; i++)
        {
          int tmpStore = i;
          int tmpVal = Serial1.read();
          if(tmpStore%2==0)
          {
            lidarPos[tmpStore/2]+=tmpVal*256;
          }
          else
          {
            tmpStore=(i-1)/2;
            lidarPos[tmpStore]+=tmpVal;
            Serial.println("value of floor "+ String (i-1)+ " is "+ String(lidarPos[tmpStore]));
          }
        }
      }

      if(descendingSLStartConfig)
      {
        descendingSLStartConfig=false;

        if(rbyte == 0x01)
        {
          cabinCallButtonState = GET_DATA1;  
        }
      }
      if(ascendingConfig)
      {
          ascendingConfig = false;
          switch (rbyte)
          {
          case 1:
            ascendingSLStartConfig = true;
            cabinCallButtonState = GET_DATA1;  
            break;
          case 2:
            ascendingSLEndConfig = true;
            cabinCallButtonState = GET_DATA1;  
            break;
          default:
            cabinCallButtonState = CHECK_FOOTER; 
            break;
          }
      }
      if(dlDurationConfig)
      {
        if(rbyte == 0x01)
        {
          cabinCallButtonState = GET_DATA1;  
        }
        else
        {
          cabinCallButtonState = CHECK_FOOTER;  
          dlDurationConfig = false;
          break;
        }
      }
      if(llDurationConfig)
      {
        if(rbyte == 0x01)
        {
          cabinCallButtonState = GET_DATA1;  
        }
        else
        {
          cabinCallButtonState = CHECK_FOOTER; 
          llDurationConfig= false;
          break;
        }
      }
      if(setMacConfig)
      {
          setMacConfig=false;
          if(rbyte == 0x01)
          {
            for(int i=0;i<6;i++)
            {
              mac[i]=Serial1.read();
              Serial.println("mac = "+String(mac[i]));
            }
            setShaftPeerMac(mac);
            cabinCallButtonState = CHECK_FOOTER; 
            break;
          }
          else if(rbyte == 0x02)
          {
            setCabinMac = true;
          }
      }
      if(upDownSpeedConfig)
      {
        upDownSpeedConfig=false;
        cabinCallButtonState = GET_DATA1;
        switch(rbyte)
        {
          case 1:
          avgUpSpeedConfig = true;
          break;
          case 2:
          avgupSlowSpeedConfig = true;
          break;
          case 3:
          avgDownSpeedConfig = true;
          break;
          case 4:
          {
            servoMaxOpenConfig=true;
          }
          break;
          case 5:
          {
            servoMinCloseConfig = true;
          }
          break;
          default:
          Serial.println("Unknown value obtained");
          break;
        }
      }

      if (checkFirmwareEnabled)
      {
        if (getCabinVersion)
        {
          cabinMajor = rbyte;
          Serial.println("Cabin major = " + String(rbyte));
        }
      }
      else
      {
        widgetNumber = rbyte;
      }
      cabinCallButtonState = GET_DATA1;
      break;
    case GET_DATA1:
      if(dlDurationConfig)
      {
        dlDurationConfig=false;
        dlDuration = rbyte;
        Serial.println("Door solenoid duration obtained = "+String(dlDuration));
      }

      if(servoMaxOpenConfig)
      {
        servoMaxOpenConfig=false;
        servoMaxSet=rbyte;
        Serial.println("Servo max openign obtained = "+String(servoMaxSet));
      }
      if(servoMinCloseConfig)
      {
        servoMinCloseConfig=false;
        servoMinSet=rbyte;
        Serial.println("Servo min opening obtained = "+String(servoMinSet));
      }
      if(advanedSettingTabStatus)
      {
        if(rbyte == 0x01)
        {
          Serial.println("advanced tab message received opened");
          advancedTabOpened = true;
        }
        else if (rbyte == 0x00)
        {
          Serial.println("advanced tab message received closed");
          advancedTabOpened = false;
        }
        else if(rbyte == 0x02)
        {
          Serial.println("advanced configuration save config");
          sendSaveConfig = true;
        }
        advanedSettingTabStatus = false;
      }
      if(descendingSLStartConfig)
      {
        descendingSLStartConfig=false;
        descendingSLStart = rbyte;
        Serial.println("Descending slow start value obtained = "+String(descendingSLStart));
      }
      if(ascendingSLStartConfig)
      {
        ascendingSLStartConfig=false;
        ascendingSLStart = rbyte;
        Serial.println("Ascending slow start value obtained = "+String(ascendingSLStart));
      }
      if(ascendingSLEndConfig)
      {
        ascendingSLEndConfig=false;
        ascendingSLEnd = rbyte;
        Serial.println("Ascending slow end value obtained = "+String(ascendingSLEnd));
      }
      if(avgDownSpeedConfig)
      {
        avgDownSpeedConfig=false;
        avgDownSpeed = rbyte;
        Serial.println("Average down speed obtained = "+String(avgDownSpeed));
      }
      if(avgUpSpeedConfig)
      {
        avgUpSpeedConfig=false;
        avgUpSpeed = rbyte;
        Serial.println("Average up speed obtained = "+String(avgUpSpeed));
      }
      if(avgupSlowSpeedConfig)
      {
        avgupSlowSpeedConfig=false;
        avgupSlowSpeed = rbyte;
        Serial.println("Average up slow speed obtained = "+String(avgupSlowSpeed));
      }
      if(llDurationConfig)
      {
        llDuration = rbyte;
        llDurationConfig = false;
        Serial.println("landing lever duration obtained = "+ String(llDuration));
      }
      if (callBookEnabled)
      {
        callVaue = rbyte;
        callVaue <<= 8;
        callVaue |= widgetNumber;
        callBookEnabled = false;
      }
      if (enableSiren)
      {
        Serial.println("siren acting");
        enableSiren = false;
        sirenStatus = true;
        Serial.println("siren call");
      }
      if (checkFirmwareEnabled)
      {
        if (getCabinVersion)
        {
          cabinMinor = rbyte;
          Serial.println("Cabin minor = " + String(rbyte));
        }
        else
        {
          shaftMinor = rbyte;
        }
      }
      if (pwmEnabled)
      {
        if(rbyte<=0x64)
        {
          if (widgetNumber == SLIDER_ID1)
          {
            
            storeDeviceIntensity(FAN_OUTPUT_GPIO, rbyte);
            setDeviceIntensity(FAN_OUTPUT_GPIO, rbyte);
            pwmEnabled = false;
            Serial.println("fan slider acting" + String(rbyte));
          }
          else if (widgetNumber == SLIDER_ID2)
          {
            storeDeviceIntensity(LIGHT_OUTPUT_GPIO, rbyte);
            setDeviceIntensity(LIGHT_OUTPUT_GPIO, rbyte);
            pwmEnabled = false;
            Serial.println("light slider acting"+String(rbyte));
          }
          else if (widgetNumber == SLIDER_ID3)
          {
            storeDeviceIntensity(NIBAV_OUTPUT_GPIO, rbyte);
            setDeviceIntensity(NIBAV_OUTPUT_GPIO, rbyte);
            pwmEnabled = false;
            Serial.println("Nibav Logo slider acting"+String(rbyte));
          }
        }
        else
        {
          Serial.println("slider out of range");
        }
      }
      if (factorySettingsEnabled)
      {
        processFactorySettings(widgetNumber, rbyte);
        factorySettingsEnabled = false;
      }
      cabinCallButtonState = CHECK_FOOTER;
      break;
    case CHECK_FOOTER:
      if (callVaue == CALL_TRAIL_HEAD)
      {
        loopCon = false;
      }
      break;
    default:
      cabinCallButtonState = CHECK_HEADER1;
      break;
    }
    if (tickDiff(startTime, millis()) > CALL_CHECK_TIMEOUT)
    {
      return MAX_FLOOR;
    }
  }
  if (checkFirmwareEnabled)
  {
    Serial1.flush();
    if (getCabinVersion)
    {
      if (CABIN_MINOR == cabinMinor && CABIN_MAJOR == cabinMajor)
      {
        Serial.println("cabin version match");
        #ifdef ENABLE_PROTO

        #else
          Serial1.write(0x9B);
        #endif
      }
      else
      {
        Serial.println("cabin version mismatch");
        
        #ifdef ENABLE_PROTO

        #else
          Serial1.write(0x9C);
        #endif
      }
    }
    Serial1.flush();
    checkFirmwareEnabled = false;
    getCabinVersion = false;
    getShaftVersion = false;
  }
  for (uint8_t f = (uint8_t)FLOOR_NUM::GND_FLOOR; f < FLOOR_NUM::MAX_FLOOR; f++)
  {
    if (callVaue == callFloorValue[f])
    {
      curFloorcall = (FLOOR_NUM)f;
      return (FLOOR_NUM)f;
    }
  }
  return MAX_FLOOR;
}

typedef enum
{
  CALLREG_STATE_IDLE,
  CALLREG_STATE_NEW_CALL,
  CALLREG_STATE_SEND,
  CALLREG_STATE_VALID,
  CALLREG_STATE_END,
} CALLREG_STATEM;

void registerCabinCall()
{
  static CALLREG_STATEM callstate;
  static FLOOR_NUM f;
  static uint32_t callStartTime;

  switch (callstate)
  {
  case CALLREG_STATE_IDLE:
  {
    f = getCabincall();
    if (MAX_FLOOR != f)
    {
      CAB_PRINT_1("call reg req = " + String(f));
      Serial.println();
      callstate = CALLREG_STATE_NEW_CALL;
      callStartTime = millis();
    }
  }
  break;
  case CALLREG_STATE_NEW_CALL:
  {
    if (ESP_CMD_REQ_STATUS_BUSY != requestStatus())
    {
      sendCmdRegCabCallReq(f);
      callstate = CALLREG_STATE_VALID;
    }
    else if (tickDiff(callStartTime, millis()) > ONE_SECOND)
    {
      callstate = CALLREG_STATE_END;
    }
  }
  break;
  case CALLREG_STATE_VALID:
  {
    if (ESP_CMD_REQ_STATUS_COMPLETE == requestStatus())
    {
      callstate = CALLREG_STATE_END;
    }
    else if (ESP_CMD_REQ_STATUS_ERROR == requestStatus())
    {
      callstate = CALLREG_STATE_END;
    }
  }
  break;
  case CALLREG_STATE_END:
  {
    callstate = CALLREG_STATE_IDLE;
  }
  break;

  default:
    break;
  }
}

void processFactorySettings(uint8_t _widgetNumber, uint8_t _rbyte)
{
  if (_widgetNumber == 0x26)
  {
    get_timevalue = _rbyte;
  }
  if (_widgetNumber == 0x27)
  {
    // yet to be defineds
  }
}

void checkSirenStatus()
{
  if (sirenStatus)
  {
    sendLiftSiren(true);
    sirenTimerStart = true;
    sirenStatus = false;
    sirenDisableTime = millis();
    // Serial.println("siren init");
  }
  else
  {
    if (sirenTimerStart)
    {
      if (tickDiff(sirenDisableTime, millis()) > SIREN_TIMEOUT)
      {
        sendLiftSiren(false);
        sirenTimerStart = false;
        // Serial.println("siren timer ended");
      }
    }
  }
}


void getVersionData()
{
  Serial.printf("current version major : %d minor : %d", CABIN_MAJOR, CABIN_MINOR);
}

void checkShaftDataReq()
{
  if (calibrationMessage == true)
  {
    // send message
    sendCalibrationRequest();
    calibrationMessage = false;
  }
  if(advancedTabOpened)
  {
    sendLockStatusReq(ADVANCED_TAB_OPENED);
    advancedTabOpened=false;
  }
}

void heartBeatStatus()
{
  if (hBStatus)
  {
    heartBeatTimerStart = true;
    hBStatus = false;
    hBTimeOut = millis();
    // Serial.println("siren init");
  }
  else
  {
    if (heartBeatTimerStart)
    {
      if (tickDiff(hBTimeOut, millis()) > HB_TIMEOUT)
      {
        resetRelay();
        heartBeatTimerStart = false;   
        // Serial.println("siren timer ended");
      }
    }
  }
}
