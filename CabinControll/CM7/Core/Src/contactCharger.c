/*
 * contactCharger.c
 *
 *  Created on: Oct 19, 2023
 *      Author: Bhargav-4836
 */



/**
 * @file contactCharger.cpp
*/

#include "contactCharger.h"
#include "cabinControl.h"
#include "messageQueue.h"
#include "common.h"

#define CONTACT_SENSE_GPIO 37



void initContactSenseOutput()
{
  pinMode(CONTACT_SENSE_GPIO, INPUT);
}



void toggleChargeIcon(int _State)
{
  uint8_t buf[] = {0x0d, 0x0e, 0x0f};
  if(_State >= LENGTH_OF_ARRAY(buf))
  {
    Serial.println("Out of bound value");
  }
  else
  {
    Serial1.flush();    
    #ifdef ENABLE_PROTO
androidDataframe(CONTACT_CHARGER, (uint8_t)_State);
    #else
Serial1.write(buf[_State]);
    #endif
    Serial1.flush();
  }
}

bool getCCstate()
{
  return digitalRead(CONTACT_SENSE_GPIO);
}