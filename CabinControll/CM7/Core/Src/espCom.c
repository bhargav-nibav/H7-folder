/*
 * espCom.c
 *
 *  Created on: Oct 27, 2023
 *      Author: Bhargav-4836
 */



/**
 * @file espCom.cpp
 */

#include "espCom.h"
#include <cstdint>
#include <cstring>
// #include "Serial32.h"
#include "messageQueue.h"
/****************************************************************************/
#define ESP_TX_PKT_RESPTIME 50
#define ESP_TX_PKT_RETRY_COUNT 5
// #define CONFIG_ESP32_WIFI_IRAM_OPT              1
// #define CONFIG_ESP32_WIFI_RX_IRAM_OPT           1
wifi_interface_t wifi_inter = WIFI_IF_STA;

bool callBookMessage = false;

void modifyPeer();
uint32_t postCabinVersion();
/****************************************************************************/
#define MAC_ID_SIZE 6
#define MAX_FLOOR_CONST 8

bool modify_shaft_peer_mac = false;

bool writeParameter(PARAMETER_NAME_t param, const uint8_t *value, uint8_t len);
// CONFIG_FILE config_file;
typedef enum
{
  ESP_NO_ERROR,
  ESP_PACKET_SENT,
  ESP_LINK_CONNECTED,
  ESP_DATA_TX_FAIL,
  ESP_DATA_TX_SUCCESS,
  ESP_LOST_COM
} ESP_COM_PKT_STATUS;

typedef enum ESP_COM_STATE_M
{
  ESP_COMM_IDLE,
  ESP_COMM_PACKET_SEND,
  ESP_COMM_WAIT_PACKET_RESPONSE,
  ESP_COMM_PROCESS_RESPONSE,
  ESP_COMM_RETRY,
  ESP_COMM_ERROR,
  ESP_COMM_END,
} ESP_COMM_STATE;

typedef enum
{
  CMD_REGISTER_CALL_REQ,
  CMD_RESPONSE_REG_CALL,
  CMD_RESPONSE_CLEAR_CALL,
  CMD_REQUEST_OUTPUT_SET,
  CMD_REQUEST_OUTPUT_CLEAR,
  CMD_REQUEST_CABIN_APP,
  CMD_REQUEST_FLOOR_NUMBER,
  CMD_REQUEST_LIFT_IDLE_SIG,
  CMD_REQUEST_MASTER_WRITE_CONFIG,
  CMD_REQUEST_MASTER_READ_CONFIG,
  CMD_REQUEST_SLAVE_WRITE_CONFIG,
  CMD_REQUEST_SLAVE_READ_CONFIG,
  CMD_REQUEST_SAVE_CONFIG,
  CMD_REQUEST_MOTOR_STATUS,
  CMD_REQUEST_EVO_STATUS,
  CMD_REQUEST_SAFETY_FAILURE,
  CMD_REQUEST_MACHINE_STATUS,
  CMD_REQUEST_OTA_STATUS,
  CMD_REQUEST_LOCK_REQ,
  CMD_REQUEST_LOCK_STATUS,
  CMD_REQUEST_ANDROID_STATUS,
  CMD_REQUEST_WAKEUP_SHAFT,
  CMD_REQUEST_WAKEUP_CABIN,
  CMD_REQUEST_NOTUSED = 0xFFFF,
} CMD_ESP_REQ;

typedef struct
{
  CMD_ESP_REQ cmd;
  uint32_t info;
  uint32_t liftStatus;
} REMOTE_CALL_REG;

/****************************************************************************/

typedef struct
{
  ESP_COMM_STATE espComStatem;
  ESP_COMMAND_REQUEST espRequest;
  REQ_STATUS espReqStatus;
  volatile ESP_COM_PKT_STATUS espPktStatus;
  uint8_t packetBuffer[1024];
  uint32_t stTime;
  uint32_t prevTicks;
  uint8_t packeLen;
  uint8_t retries;
  uint8_t config_resp_received;
} ESP_COM;

/****************************************************************************/
int cabinFloorNum = 0;

/****************************************************************************/
static ESP_COM espCom;

REMOTE_CALL_REG fromCabin, fromShaft[3];
volatile uint8_t shaft_command_incomming_index = 0;
volatile uint8_t shaft_command_processing_index = 0;

esp_now_peer_info_t peerInfo;

uint8_t broadcastAddress[] = {0xff, 0xff, 0xff, 0x00, 0x00, 0x00}; //{0xf4,0x12,0xfa,0xec,0xc2,0x24}; //{0xf4,0x12,0xfa,0xec,0xc2,0x44};// {0xF4,0x12,0xFA,0xEC,0xC1,0xDC};//{0xF4,0x12,0xFA,0xEC,0xB7,0xD4};//{0xf4, 0x12, 0xFA, 0xC1, 0xC3, 0x8C};

// f4:12:fa:c1:c3:8c
//  f4:12:fa:ec:b7:d4 g+1  f4:12:fa:ec:c1:dc
//  f4:12:fa:ec:b7:d4

/****************************************************************************/

void espCommInit();
void setEspPacketSts(ESP_COM_PKT_STATUS sts);
ESP_COM_PKT_STATUS checkLastTxPacketSuccess();
static bool setlockLandingGear(bool state);
bool senMsgToShaft(const unsigned char *info, uint32_t len);
void processEspCommunication();
void sendRequestTocabin(ESP_COMMAND_REQUEST cmd);
bool sendEspReqCommand(ESP_COMMAND_REQUEST espRequest);
void resetEspCom();

/****************************************************************************/
/****************************************************************************/
void sendRequestTocabin(ESP_COMMAND_REQUEST cmd)
{
  espCom.espRequest = OPEN_LANDING_LEVER;
}

static bool setlockLandingGear(bool state)
{
  REMOTE_CALL_REG cabinMsg;

  memset(&cabinMsg, false, sizeof(cabinMsg));
  if (state)
  {
    cabinMsg.cmd = CMD_REQUEST_OUTPUT_SET;
  }
  else
  {
    cabinMsg.cmd = CMD_REQUEST_OUTPUT_CLEAR;
  }
  SET_BIT(cabinMsg.info, CABIN_LANDING_LEVER);
  return senMsgToShaft((const unsigned char *)&cabinMsg, sizeof(cabinMsg));
}

bool senMsgToShaft(const unsigned char *info, uint32_t len)
{
  bool status = false;
  if (esp_now_send(peerInfo.peer_addr, info, len) == ESP_OK)
  {
    Serial.println("Sucess");
    status = true;
  }
  return status;
}

ESP_COM_PKT_STATUS checkLastTxPacketSuccess()
{
  return espCom.espPktStatus;
}

void setEspPacketSts(ESP_COM_PKT_STATUS sts)
{
  espCom.espPktStatus = sts;
}

// Defining OnDataSent() function. this is a call back function executed when a message is sent.
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  // todo check mac id if tx node.
  DEBUG_PRINT("on packet sent = %d \r\n", status);
  if (ESP_NOW_SEND_SUCCESS == status)
  {
    setEspPacketSts(ESP_DATA_TX_SUCCESS);
  }
  else
  {
    setEspPacketSts(ESP_DATA_TX_FAIL);
  }
}

// Create callback function "onDataRecv()" that is called when ESP32 received the data via ESP-NOW.
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  static int MsgCount = 0;
  MsgCount++;
  for (uint8_t i = 0; i < len; i++)
  {
    CAB_PRINT_1(String(incomingData[i]) + " ");
  }
  Serial.print("new message received : ");
  Serial.println(MsgCount);
  MsgCount %= 100;

  memcpy(&fromShaft[shaft_command_incomming_index], incomingData, sizeof(REMOTE_CALL_REG));
  shaft_command_incomming_index = (shaft_command_incomming_index + 1) % LENGTH_OF_ARRAY(fromShaft);

  // overrun handking ...
}

void process_esp_request()
{
  static bool modifyPeerFlag = false;
  static long int modifyPeerTimer = millis();

  if(returnAvailableState())
  {
    setAvailableState(false);
    //modify peer

    writeConfigFile();
    modifyPeerFlag = true;
    modifyPeerTimer = millis();
  }
  if(modifyPeerFlag && ( millis()- modifyPeerTimer > 500))
  {
    modifyPeerFlag = false;
    modifyPeer();
  }

  if (shaft_command_incomming_index != shaft_command_processing_index)
  {
    switch (fromShaft[shaft_command_processing_index].cmd)
    {
    case CMD_REGISTER_CALL_REQ:
    {
      Serial.println("cmd reg call");
      // registerCabinCall(fromCabin.info);
    }
    break;
    case CMD_RESPONSE_REG_CALL:
    {
      cabinRegisterCabCalls(fromShaft[shaft_command_processing_index].info);
      Serial.println("res reg");
    }
    break;
    case CMD_REQUEST_OUTPUT_SET:
    {
      cabinSetOutputsFromShaft(fromShaft[shaft_command_processing_index].info);
      Serial.println("req op set");
    }
    break;

    case CMD_REQUEST_OUTPUT_CLEAR:
    {
      cabinClearOutputsFromShaft(fromShaft[shaft_command_processing_index].info);
      Serial.println("req op clear");
    }
    break;
    case CMD_RESPONSE_CLEAR_CALL:
    {
      cabinServedOrClearCalls(fromShaft[shaft_command_processing_index].info);
      Serial.println("res clr");
    }
    break;
    case CMD_REQUEST_CABIN_APP:
    {
      cabinCalibrationStatus(fromShaft[shaft_command_processing_index].info);
      // Serial.println("auto calibration data received" + String(fromCabin.info));
    }
    break;
    case CMD_REQUEST_FLOOR_NUMBER:
    {
      cabinFloorNum = fromShaft[shaft_command_processing_index].info;
      // time consuming procesing ...
      cabinCurrentFloorNum(cabinFloorNum);
    }
    break;
    case CMD_REQUEST_LIFT_IDLE_SIG:
    {
      cabinShaftIdleStatus(fromShaft[shaft_command_processing_index].info);
    }
    break;
    case CMD_REQUEST_MASTER_WRITE_CONFIG:
    {
      //
    }
    break;
    case CMD_REQUEST_OTA_STATUS:
    {
      shaftUpdateProcess(fromShaft[shaft_command_processing_index].info);
    }
    break;
    case CMD_REQUEST_MOTOR_STATUS:
    {
      displayMotorStatus(fromShaft[shaft_command_processing_index].info);
    }
    break;
    case CMD_REQUEST_EVO_STATUS:
    {
      displayEVStatus(fromShaft[shaft_command_processing_index].info);
    }
    break;
    case CMD_REQUEST_SAFETY_FAILURE:
    {
      displaySafetyFailure(fromShaft[shaft_command_processing_index].info, fromShaft[shaft_command_processing_index].liftStatus);
    }
    break;
    case CMD_REQUEST_MACHINE_STATUS:
    {
      processMachineState(fromShaft[shaft_command_processing_index].info);
    }
    break;

    case CMD_REQUEST_SLAVE_WRITE_CONFIG:
    {
      // espCom.config_resp_received = true;
      // espCom.packeLen = len;
      // memcpy(espCom.packetBuffer,incomingData,len);
    }
    break;
    case CMD_REQUEST_SLAVE_READ_CONFIG:
    {
      // espCom.config_resp_received = true;
      // espCom.packeLen = len;
      // memcpy(espCom.packetBuffer,incomingData,len);
    }
    break;
    case CMD_REQUEST_LOCK_STATUS:
    {
      sendLockStatDisp(fromShaft[shaft_command_processing_index].info, fromShaft[shaft_command_processing_index].liftStatus);
    }
    break;
    default:
      Serial.println("Other data received cmd = " + String(fromShaft[shaft_command_processing_index].cmd) + "Info = " + String(fromShaft[shaft_command_processing_index].info));
      break;
    }
    shaft_command_processing_index = (shaft_command_processing_index + 1) % LENGTH_OF_ARRAY(fromShaft);
  }
}

/// @brief initiate esp now communication.
// if communication fails, return to setup else add peers based on the preconfigured mac ID.
// register OnDataRecv as callback function for receiving data from shaft board.
// register OnDataSent as callback function for transmiting data to shaft board.
void espCommInit()
{

  delay(4 * 1000);
  WiFi.mode(WIFI_STA);
  setDefaultMac(broadcastAddress);
  CONFIG_FILE *memoryconfig = getConfig();
  // esp_err_t result = WiFi.setTxPower(WIFI_POWER_19_5dBm);
  // Serial.printf("power result =%d \r\n",(int)result);
  // // Init ESP-NOW
  // if (esp_now_init() != ESP_OK) {
  //   Serial.println("Error initializing ESP-NOW");
  //   return;
  // }
  // result =  esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_LR);
  // Serial.printf("intrf result =%d \r\n",(int)result);

  // result = esp_wifi_config_espnow_rate(WIFI_IF_STA, WIFI_PHY_RATE_LORA_500K);
  // Serial.printf("rate result =%d \r\n",(int)result);
  // WiFi.mode(WIFI_STA);
  esp_wifi_set_promiscuous(true);
  bool res = esp_wifi_set_channel(11, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);
  if (res != ESP_OK)
  {
    Serial.println("could not set channel");
  }
  wifi_config_t config;
  WiFi.setSleep(false);
  esp_wifi_set_ps(WIFI_PS_NONE);
  int8_t power = 0;
  esp_err_t err = esp_wifi_get_max_tx_power(&power);
  uint8_t protocol_bitmap_temp;
  wifi_mode_t mode = WIFI_MODE_NULL;
  err = esp_wifi_set_max_tx_power(power);
  if (err == ESP_OK)
  {
    Serial.println("max tx power = " + String(power));
  }

  if (esp_now_init() != 0)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  memcpy(peerInfo.peer_addr, &memoryconfig->macIdShaft, 6);
  for(int i=0;i<6;i++)
  {
    Serial.print(String(memoryconfig->macIdShaft[i])+",");
  }
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);

  Serial.println("ESP Board MAC Address:  " + WiFi.macAddress());
  printShaftMacID();
  espCom.espComStatem = ESP_COMM_IDLE;
  resetEspCom();
}

void resetEspCom()
{
  memset(&espCom, false, sizeof(espCom));
}

void processEspCommunication()
{
  switch (espCom.espComStatem)
  {
  case ESP_COMM_IDLE:
  {
    // ESPCOM_PRINT("ESP_COMM_IDLE");
  }
  break;
  case ESP_COMM_PACKET_SEND:
  {
    ESPCOM_PRINT("ESP_COMM_PACKET_SEND");
    if (senMsgToShaft((const unsigned char *)espCom.packetBuffer, (uint32_t)espCom.packeLen))
    {
      espCom.espReqStatus = REQ_STATUS::ESP_CMD_REQ_STATUS_BUSY;
      espCom.prevTicks = millis();
      espCom.espComStatem = ESP_COMM_WAIT_PACKET_RESPONSE;
      setEspPacketSts(ESP_PACKET_SENT);
      ESPCOM_PRINT("esp msg sent");
    }
    else
    {
      espCom.espComStatem = ESP_COMM_ERROR;
      ESPCOM_PRINT("esp error");
    }
  }
  break;
  case ESP_COMM_WAIT_PACKET_RESPONSE:
  {
    if (tickDiff(espCom.prevTicks, millis()) > ESP_TX_PKT_RESPTIME)
    {
      if (checkLastTxPacketSuccess() == ESP_DATA_TX_SUCCESS)
      {
        espCom.espComStatem = ESP_COMM_PROCESS_RESPONSE;
      }
      else if (espCom.retries < ESP_TX_PKT_RETRY_COUNT)
      {
        espCom.retries++;
        espCom.espComStatem = ESP_COMM_PACKET_SEND;
        ESPCOM_PRINT("esp retry \r\n");
      }
      else
      {
        espCom.espComStatem = ESP_COMM_ERROR;
      }
      espCom.prevTicks = millis();
    }
  }
  break;
  case ESP_COMM_PROCESS_RESPONSE:
  {
    ESPCOM_PRINT("ESP_COMM_PROCESS_RESPONSE");
    espCom.espReqStatus = REQ_STATUS::ESP_CMD_REQ_STATUS_COMPLETE;
    espCom.espComStatem = ESP_COMM_END;
  }
  break;
  case ESP_COMM_RETRY:
  {
    ESPCOM_PRINT("ESP_COMM_RETRY");
    espCom.espComStatem = ESP_COMM_END;
  }
  break;
  case ESP_COMM_ERROR:
  {
    ESPCOM_PRINT("1.ESP_COMM_ERROR");
    espCom.espReqStatus = REQ_STATUS::ESP_CMD_REQ_STATUS_ERROR;
    espCom.espComStatem = ESP_COMM_END;
    if (callBookMessage)
    {

#ifdef ENABLE_PROTO
      androidDataframe(SAFETY_FAIL, COM_FAILURE_ON_CALL);
#else
      Serial1.write(0x9A);
#endif
    }
  }
  break;
  case ESP_COMM_END:
  {
    if (callBookMessage)
    {
      callBookMessage = false;
    }
    ESPCOM_PRINT("ESP_COMM_END");
    espCom.prevTicks = millis();
    espCom.espComStatem = ESP_COMM_IDLE;
  }
  break;
  default:
  {
    break;
  }
  }
}

bool sendEspReqCommand(ESP_COMMAND_REQUEST espRequest)
{
  bool status;
  switch (espRequest)
  {
  case OPEN_LANDING_LEVER:
  {
    status = setlockLandingGear(false);
  }
  break;
  case CLOSE_LANDING_LEVER:
  {
    status = setlockLandingGear(true);
  }
  break;
  default:
    break;
  }
  return status;
}

REQ_STATUS requestStatus()
{
  return espCom.espReqStatus;
}


/// @brief set the output bit based on emergency input. After setting, transmit the data frame via esp now.
/// @param state output bit. state = 1, emergency on. state = 0, emergency off.
void sendLiftEmergency(bool state)
{
  REMOTE_CALL_REG cabinMsg;
  memset(&cabinMsg, false, sizeof(cabinMsg));
  if (state)
  {
    cabinMsg.cmd = CMD_REQUEST_OUTPUT_SET;
  }
  else
  {
    cabinMsg.cmd = CMD_REQUEST_OUTPUT_CLEAR;
  }

  SET_BIT(cabinMsg.info, OUTPUT_LIFT_EMERGENCY);
  memcpy(&espCom.packetBuffer, &cabinMsg, sizeof(cabinMsg));
  espCom.packeLen = sizeof(cabinMsg);
  espCom.espComStatem = ESP_COMM_PACKET_SEND;
  espCom.retries = false;
  // ESPCOM_PRINT("call door open \r\n");
}

void sendLiftChildlock(bool state)
{
  REMOTE_CALL_REG cabinMsg;
  memset(&cabinMsg, false, sizeof(cabinMsg));
  if (state)
  {
    cabinMsg.cmd = CMD_REQUEST_OUTPUT_SET;
  }
  else
  {
    cabinMsg.cmd = CMD_REQUEST_OUTPUT_CLEAR;
  }

  SET_BIT(cabinMsg.info, OUTPUT_LIFT_CHILDLOCK);
  memcpy(&espCom.packetBuffer, &cabinMsg, sizeof(cabinMsg));
  espCom.packeLen = sizeof(cabinMsg);
  espCom.espComStatem = ESP_COMM_PACKET_SEND;
  espCom.retries = false;
}

// void sendLiftSiren();
void sendLiftSiren(bool _state)
{
  REMOTE_CALL_REG cabinMsg;
  memset(&cabinMsg, false, sizeof(cabinMsg));
  if (_state)
  {
    cabinMsg.cmd = CMD_REQUEST_OUTPUT_SET;
  }
  else
  {
    cabinMsg.cmd = CMD_REQUEST_OUTPUT_CLEAR;
  }

  SET_BIT(cabinMsg.info, OUTPUT_LIFT_SIREN);
  cabinMsg.liftStatus = postCabinVersion();
  memcpy(&espCom.packetBuffer, &cabinMsg, sizeof(cabinMsg));
  espCom.packeLen = sizeof(cabinMsg);
  espCom.espComStatem = ESP_COMM_PACKET_SEND;
  espCom.retries = false;
}

void sendCmdRegCabCallReq(uint8_t floorNum)
{
  REMOTE_CALL_REG cabinMsg;

  memset(&cabinMsg, false, sizeof(cabinMsg));
  SET_BIT(cabinMsg.info, (uint8_t)floorNum);
  cabinMsg.cmd = CMD_REGISTER_CALL_REQ;
  cabinMsg.liftStatus = postCabinVersion();
  memcpy(&espCom.packetBuffer, &cabinMsg, sizeof(cabinMsg));
  espCom.packeLen = sizeof(cabinMsg);
  espCom.espComStatem = ESP_COMM_PACKET_SEND;
  espCom.retries = false;
  callBookMessage = true;
}

void sendCalibrationRequest()
{
  REMOTE_CALL_REG cabinMsg;
  memset(&cabinMsg, false, sizeof(cabinMsg));
  cabinMsg.cmd = CMD_REQUEST_CABIN_APP;
  cabinMsg.info = CABIN_APP_AUTO_FLOOR_CALIB;
  memcpy(&espCom.packetBuffer, &cabinMsg, sizeof(cabinMsg));
  espCom.packeLen = sizeof(cabinMsg);
  espCom.espComStatem = ESP_COMM_PACKET_SEND;
  espCom.retries = false;
  ESPCOM_PRINT("send calibration request");
}

void sendLockStatusReq(uint32_t info)
{
  REMOTE_CALL_REG cabinMsg;
  memset(&cabinMsg, false, sizeof(cabinMsg));
  cabinMsg.cmd = CMD_REQUEST_LOCK_REQ;
  cabinMsg.info = info;
  memcpy(&espCom.packetBuffer, &cabinMsg, sizeof(cabinMsg));
  espCom.packeLen = sizeof(cabinMsg);
  espCom.espComStatem = ESP_COMM_PACKET_SEND;
  espCom.retries = false;
  ESPCOM_PRINT("send lock status request");
}

int cabinCurFloor()
{
  return cabinFloorNum;
}

/****************************************************************************
 * E N D                 O F             F I L E
 ****************************************************************************/

void processConfiguration(uint32_t param)
{
  uint32_t indx;
  indx = 0;
  espCom.packetBuffer[indx++] = CMD_REQUEST_MASTER_WRITE_CONFIG;
  espCom.packetBuffer[indx++] = 0;
  espCom.packetBuffer[indx++] = 0;
  espCom.packetBuffer[indx++] = 0;
  espCom.packetBuffer[indx++] = (PARAMETER_NAME_t)param;
  indx += readParameter((PARAMETER_NAME_t)param, &espCom.packetBuffer[indx]);
  Serial.println("Length = " + String(indx));
  senMsgToShaft((const unsigned char *)espCom.packetBuffer, (uint32_t)indx);
}

typedef bool (*ptr2fn)(void);

// ptr2fn

bool read_shaft_parameter(PARAMETER_NAME_t param)
{
  uint32_t indx;
  uint32_t start_time;
  bool status = false;
  indx = 0;
  // copy command
  *((CMD_ESP_REQ *)&espCom.packetBuffer[indx]) = CMD_REQUEST_MASTER_READ_CONFIG;
  indx += sizeof(CMD_ESP_REQ);
  espCom.packetBuffer[indx++] = (PARAMETER_NAME_t)param;
  espCom.config_resp_received = false;
  senMsgToShaft((const unsigned char *)espCom.packetBuffer, (uint32_t)indx);
  start_time = millis();

  while (!espCom.config_resp_received or tickDiff(start_time, millis() < ONE_SECOND))
  {
    /* code */
  }
  if (espCom.config_resp_received)
  {
    indx = 0;
    if (*((CMD_ESP_REQ *)&espCom.packetBuffer[indx]) == CMD_REQUEST_SLAVE_READ_CONFIG)
    {
      indx += sizeof(CMD_ESP_REQ);
      if (writeParameter((PARAMETER_NAME_t)espCom.packetBuffer[indx], &espCom.packetBuffer[indx + 1],
                         (espCom.packeLen - sizeof(CMD_ESP_REQ) - 1)))
      {
        status = true;
      }
    }
  }
  return status;
}

bool write_shaft_parameter(PARAMETER_NAME_t param, const uint8_t *value, uint8_t len)
{
  return false;
}

void printInactive()
{
  static long int inter = millis();
  if (millis() - inter > 2000)
  {
    inter = millis();
    uint16_t sec;
    esp_err_t err = esp_wifi_get_inactive_time(wifi_inter, &sec);
    if (err == ESP_OK)
    {
      Serial.printf("Protocol inactive = %d\n", sec);
    }
  }
}

void sendAndroidStatus(bool status)
{
  static bool prevStatus = true;
  if (status || (prevStatus != status))
  {
    prevStatus = status;
    REMOTE_CALL_REG cabinMsg;
    memset(&cabinMsg, false, sizeof(cabinMsg));
    cabinMsg.cmd = CMD_REQUEST_ANDROID_STATUS;
    cabinMsg.info = (uint32_t)status;
    cabinMsg.liftStatus = postCabinVersion();
    memcpy(&espCom.packetBuffer, &cabinMsg, sizeof(cabinMsg));
    espCom.packeLen = sizeof(cabinMsg);
    espCom.espComStatem = ESP_COMM_PACKET_SEND;
    espCom.retries = false;
    ESPCOM_PRINT("send android status request");
  }
}

void modifyPeer()
{
    Serial.println("Peer mac set");
    esp_err_t err = esp_now_del_peer(peerInfo.peer_addr);
    switch (err)
    {
    case ESP_OK:
      Serial.println("Peer deleted");
      break;
    case ESP_ERR_ESPNOW_NOT_INIT:
      Serial.println("ESPNow not initialized");
      break;
    case ESP_ERR_ESPNOW_ARG:
      Serial.println("Invalid argument");
      break;
    case ESP_ERR_ESPNOW_FULL:
      Serial.println("Peer list full");
      break;
    default:
      Serial.println("error not found");
      break;
    }
    CONFIG_FILE *file =  getConfig();
    memcpy(peerInfo.peer_addr, file->macIdShaft, 6 * file->macIdShaft[0]);
    // memcpy(peerInfo.peer_addr, broadcastAddress, 6*mac[0]);
    err = esp_now_add_peer(&peerInfo);
    switch (err)
    {
      case ESP_OK:
        Serial.println("Peer added");
        ESP.restart();
        break;
      case ESP_ERR_ESPNOW_NOT_INIT:
        Serial.println("ESPNow not initialized");
        break;
      case ESP_ERR_ESPNOW_ARG:
        Serial.println("Invalid argument");
        break;
      case ESP_ERR_ESPNOW_NOT_FOUND:
        Serial.println("esp now not found");
        break;
      default:
        Serial.println("[esp err]:" + String(err));
        break;
    }
}

void printShaftMacID()
{
  CONFIG_FILE *conf = getConfig();
  char buf[20];
  snprintf(buf, sizeof(buf), "[%x:%x:%x:%x:%x:%x]", conf->macIdShaft[0], conf->macIdShaft[1], conf->macIdShaft[2], conf->macIdShaft[3], conf->macIdShaft[4], conf->macIdShaft[5]);
  Serial.println(buf);
}


uint32_t postCabinVersion()
{
  uint32_t version = CABIN_MINOR;
  version |= (CABIN_MAJOR<<16);
  return version;
}
