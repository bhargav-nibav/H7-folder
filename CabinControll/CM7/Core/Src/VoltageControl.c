/*
 * VoltageControl.c
 *
 *  Created on: Oct 19, 2023
 *      Author: Bhargav-4836
 */



/**
 * @file VoltageControl.cpp
 */
#include "messageQueue.h"
#include "usart.h"
//#include "driver/adc.h"
//#include "esp_adc_cal.h"
#include "main.h"
#include <iomanip>
adc1_channel_t channel = ADC1_CHANNEL_6;
esp_adc_cal_characteristics_t adc1_chars;
long int batTimer = 0;
float vout = 0;
#include "VoltageControl.h"
#include "espcom.h"
#include <math.h>
using namespace std;
#define VOLTAGE_PIN 7
#define max_voltage 25
float COMPENSATION[]={0.27f,0.3f};
//#define COMPENSATION 0.28f;
int bat_percentage = 0;
int prev_percentage = 1; //
float input_volt = 0.0;
float temp = 0.0;
float r1 = 10000.0; // r1 value
float r2 = 1100.0;  // r2 value

/************************************/
bool compare_float(float x, float y);

#define BATTERY_TIMER 15
#define maxSample (10 * BATTERY_TIMER)
bool currentChargingState;
uint32_t batteryDisableTime;
float bat_percentage_2 = 0;
int sampleData = 0;
float samplePercentage[maxSample] = {};

#define FIFTY_PERCENTAGE_REF 22.7f
#define LESSER_HALF_FACTOR 0.044f
#define GREATER_HALF_FACTOR 0.044f

#define LESSER_PERCENTAGE(x) (50 + ((x - FIFTY_PERCENTAGE_REF) / LESSER_HALF_FACTOR))
#define GREATER_PERCENTAGE(x) (50 + ((x - FIFTY_PERCENTAGE_REF) / GREATER_HALF_FACTOR))

/************************************/
#define battery_TIMEOUT BATTERY_TIMER * 1000
bool batteryStatus = true;
bool batteryTimerStart = false;
/*********************************/
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int movingAverage(int _input_percentage)
{
  if (sampleData >= maxSample)
  {
    sampleData = 0;
    int averagePercentage = 0;
    averagePercentage = accumulate(samplePercentage, samplePercentage + maxSample, 0.0);
    Serial.printf("average percentage = %d max sample = %d sampledata =%d\n", averagePercentage, maxSample, sampleData);
    prev_percentage = ceil((averagePercentage / maxSample) * 100.0) / 100.0;
    //prev_percentage = (averagePercentage / maxSample);
    if(prev_percentage>98)
    {
      prev_percentage=100;
    }
    Serial.printf("prev percentage = %d\n", prev_percentage);
  }
  else
  {
    samplePercentage[sampleData] = _input_percentage;
    sampleData++;
  }

  return prev_percentage;
}

#if 0
void vol_measurement()
{
  float Value = analogRead(VOLTAGE_PIN);
  temp = ((Value * 3.15) / 4096.0);
  input_volt = temp / (r2 / (r1 + r2)) + COMPENSATION ;
  input_volt = movingAverage(input_volt);
  bat_percentage = mapfloat(input_volt, 20, max_voltage, 0, 100);
  bat_percentage = processPercentage(bat_percentage);
}
#endif

void vol_measurement()
{
  if (HAL_GetTick() - batTimer > 100)
  {
    batTimer = HAL_GetTick();

    // float Value = analogRead(VOLTAGE_PIN);
    // temp = ((Value * 3.15) / 4096.0);
    // input_volt = temp / (r2 / (r1 + r2)) + COMPENSATION ;
    // input_volt = movingAverage(input_volt);
    // bat_percentage = mapfloat(input_volt, 20, max_voltage, 0, 100);
    // bat_percentage = processPercentage(bat_percentage);
    int val = adc1_get_raw(channel);
    uint32_t mV = esp_adc_cal_raw_to_voltage(val, &adc1_chars);
    vout = (r1 + r2) * mV / r2;
    float Vout = vout / 1000.0;
    Vout = ceil(Vout * 100.0) / 100.0;
    if(Vout <= FIFTY_PERCENTAGE_REF)
    {
      Vout = Vout + COMPENSATION[0];
    }
    else
    {
      Vout = Vout + COMPENSATION[1];
    }

    if (Vout == FIFTY_PERCENTAGE_REF)
    {
      bat_percentage = 50;
    }
    else if (Vout > FIFTY_PERCENTAGE_REF)
    {
      bat_percentage =  (int)GREATER_PERCENTAGE(Vout); //more than 50%
    }
    else
    {
      bat_percentage = (int)LESSER_PERCENTAGE(Vout); //less than 50%
    }
    if (bat_percentage < 0)
    {
      bat_percentage = 0;
    }
    else if (bat_percentage > 100)
    {
      bat_percentage = 100;
    }

    int len = snprintf(buff, sizeof(buff),("value = %d mv = %d vout = %.2f Vout = %.2f percentage is %d\n"),val, mV,vout, Vout, bat_percentage);
      bat_percentage = movingAverage(bat_percentage);
      len = snprintf(buff, sizeof(buff), ("moving average percentage = %d\n"), bat_percentage);
      HAL_UART_Transmit(huart, (uint8_t *)buff, len, Timeout)

  }
}

void displayIcon()
{
  uint8_t _value = 0;
  if (bat_percentage >20)
  {
    char normal[] = "Normal power";

    HAL_UART_Transmit(&huart3, (uint8_t *)normal, sizeof(normal), 100);

      _value = 0;
  }
  else if (bat_percentage <= 20)
  {

    char low[] = "low power";

    HAL_UART_Transmit(&huart3, (uint8_t *)low, sizeof(low), 100);

      _value = 1;
  }
  Serial1.flush();
  lowPowerModeDisplay(_value);
  Serial1.flush();
}

void displayPercentage()
{
  androidDataframe(BATTERY_PERCENTAGE, bat_percentage);
}

int processPercentage(int _bat_percentage)
{
  if (_bat_percentage >= 100)
  {
    _bat_percentage = 100;
  }
  else if (_bat_percentage >= 80 && _bat_percentage < 100)
  {
    _bat_percentage = 80;
  }
  else if (_bat_percentage >= 60 && _bat_percentage < 80)
  {
    _bat_percentage = 60;
  }
  else if (_bat_percentage >= 40 && _bat_percentage < 60)
  {
    _bat_percentage = 40;
  }
  else if (_bat_percentage >= 20 && _bat_percentage < 40)
  {
    _bat_percentage = 20;
  }
  else if (_bat_percentage >= 10 && _bat_percentage < 20)
  {
    _bat_percentage = 10;
  }
  else if (_bat_percentage < 10)
  {
    _bat_percentage = 0;
  }
  return _bat_percentage;
}

void lowPowerModeDisplay(uint8_t _State)
{
  uint8_t buf[] = {0x12, 0x13};
  if (_State >= LENGTH_OF_ARRAY(buf))
  {
    HAL_UART_Transmit(&huart3, (uint8_t *)"Out of bound value", sizeof("Out of bound value"), 100);

  }
  else
  {
	  androidDataframe(LOW_POWER_MODE, (uint8_t)_State);
  }
}

void checkbatteryStatus()
{
  vol_measurement();
  if (tickDiff(batteryDisableTime, HAL_GetTick()) > battery_TIMEOUT)
  {
    HAL_UART_Transmit(&huart3, (uint8_t *)"battery timer ended", sizeof("battery timer ended"), 100);
    batteryDisableTime = HAL_GetTick();
    displayPercentage();
    HAL_Delay(1);
    displayIcon();
  }
}

void setBatteryTimerInit()
{
  batteryDisableTime = HAL_GetTick();
}

void initVoltage()
{
//  adc_atten_t atten = ADC_ATTEN_DB_11;
//  esp_err_t err = adc1_config_channel_atten(channel, atten);
  if (err != ESP_OK)
  {
    HAL_UART_Transmit(&huart3, (uint8_t *)"Setting attn failed", sizeof("Setting attn failed"), 100);
  }
//  // analogReadResolution(12);
//  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_MAX, 0, &adc1_chars);
  batTimer = HAL_GetTick();
}

bool compare_float(float x, float y)
{
  float epsilon = 0.028f;
  if (fabs(x - y) < epsilon)
    return true; // they are same
  return false;  // they are not same
}
