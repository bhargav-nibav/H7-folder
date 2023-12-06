/*
 * displayError.c
 *
 *  Created on: Oct 19, 2023
 *      Author: Bhargav-4836
 */



/**
 * @file displayError.cpp
*/

#include "displayError.h"
#include "espCom.h"
#define D_POSITIVE 8
#define D_NEGATIVE 19
#define DISPLAY_STATUS_GPIO 11

#define MAX_RETRY 5

static void processValues(int num_high, int num_low, bool lastState);

long int resetTimer = millis();
bool resetFlag = false;
void initDisplayStatus()
{
  pinMode(DISPLAY_STATUS_GPIO, INPUT);
}

typedef enum
{
  DEVICE_CONNECTED,
  DEVICE_DISCONNECTED
}ANDROID_ST;

void checkLEDStatus()
{
  // initialize timeout here
  static uint32_t oneSecPeriod = millis();
  static bool prevDispStatus = false;
  static int nLow = 0;
  static int nHigh = 0;
  bool curDispStatus = digitalRead(DISPLAY_STATUS_GPIO);
  if (prevDispStatus != curDispStatus)
  {
    if (curDispStatus)
    {
      nHigh++;
    }
    else
    {
      nLow++;
    }
  }
  prevDispStatus = curDispStatus;
  if (tickDiff(oneSecPeriod, millis()) > 1000)
  {
    processValues(nHigh, nLow, digitalRead(DISPLAY_STATUS_GPIO));
    oneSecPeriod = millis();
    nLow = 0;
    nHigh = 0;
  }
  if(resetFlag && (millis() - resetTimer > 100))
  {
    resetFlag = false;
    Serial.println("100ms delay reset end");
    digitalWrite(D_NEGATIVE, HIGH);
  }
}

static void processValues(int num_high, int num_low, bool lastState)
{
  static bool deviceConnected = false;
  static int retryReset = 0;
  if (lastState)
  {
    if (num_low == 0)
    {
      if (num_high == 0)
      {
        Serial.println("Device disconnected");
        retryReset++;
      }
      else
      {
        Serial.println("Device low to high " + String(num_high));
      }
    }
    else // if(num_low=1)
    {
      Serial.println("1. may be error low = " + String(num_low) + " High =" + String(num_high));
      retryReset++;
    }
  }
  else
  {
    if (num_low == 0)
    {
      if (num_high == 0)
      {
        sendAndroidStatus(DEVICE_CONNECTED);
        Serial.println("Device connected");
        retryReset = 0;
        resetFlag=false;
      }
      else
      {
        Serial.println("2. may be error low = " + String(num_low) + " High =" + String(num_high));
        retryReset++;
      }
    }
    else
    {
      Serial.println("3. may be error low = " + String(num_low) + " High =" + String(num_high));
      retryReset++;
    }
  }
  //Serial.println("Reset count = " + String(retryReset));
  if (retryReset == MAX_RETRY)
  {
    sendAndroidStatus(DEVICE_DISCONNECTED);
    Serial.println("resetting connection");
    retryReset = 0;
    //resetRelay();
     digitalWrite(D_POSITIVE, LOW);
     digitalWrite(D_NEGATIVE, LOW);
     resetFlag=true;
     resetTimer = millis();
     Serial.println("100ms delay reset begin");
  }
}

void resetRelay()
{
  //TODO remove delay
  digitalWrite(D_POSITIVE, LOW);
  digitalWrite(D_NEGATIVE, LOW);
  delay(100);
  digitalWrite(D_NEGATIVE, HIGH);
}

void initDisplayPins()
{
  pinMode(D_POSITIVE, OUTPUT);
  pinMode(D_NEGATIVE, OUTPUT);
  digitalWrite(D_POSITIVE, LOW);
  digitalWrite(D_NEGATIVE, HIGH);
}
