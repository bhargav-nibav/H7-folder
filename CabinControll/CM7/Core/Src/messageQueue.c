/*
* messageQueue.c
*
*  Created on: Oct 19, 2023
*      Author: Bhargav-4836
*/

#include "messageQueue.h"
#include <stdio.h>
//#include <queue>
#include <vector>


using namespace std;

#define QUEUE_TIMER 50
queue<vector<int>> androidQueue;

queue<uint8_t> msgQue;
queue<String> msgQue2;
long int queueTimer = millis();
void updateQueue(uint8_t msg)
{
   msgQue.push(msg);
   Serial.printf("queue size is %d", msgQue.size());
}

void sendDataToAndroid()
{
   if(!androidQueue.empty())
   {
       if((millis()- queueTimer > QUEUE_TIMER))
       {
           Serial.printf("queue size is %d difference is %d", androidQueue.size(), (millis()- queueTimer));
           vector<int> msgBuf = androidQueue.front();
           for (auto& it : msgBuf)
           {
               // Print the values
               Serial1.write((uint8_t)it);
           }
           Serial.println();

           androidQueue.pop();
           queueTimer = millis();
       }

   }
}


void androidDataframe(MESSAGE_TYPE msg_type, uint8_t data)
{
   if(msg_type == CALL_CLEAR)
   {
       Serial.printf("QUEUE: call clear message %d\n", data);
   }
   if(msg_type == CABIN_FLOOR)
   {
       Serial.printf("QUEUE: cabin floor message %d\n", data);
   }
   std::vector<int> vectorQueue{0xD5, (uint8_t)msg_type,data,0xFF};
   androidQueue.push(vectorQueue);
}

bool emptyQueue()
{
   return androidQueue.empty();
}
