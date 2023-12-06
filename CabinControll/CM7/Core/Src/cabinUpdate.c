/*
 * cabinUpdate.c
 *
 *  Created on: Oct 22, 2023
 *      Author: Bhargav-4836
 */



/**
 * @file cabinUpdate.cpp
*/

#include "cabinUpdate.h"
#define port 80
#include "messageQueue.h"
WiFiClient otaClient;

long contentLength = 0;
bool isValidContentType = false;

String updateUrlPath = "/swUpdate.bin";
String updateMsg = "message";
String updateSucess = "/get?message=sucess";
String updateFail = "/get?message=error";

extern String ssidcabin, pswdcabin, updateUrlHost, host, bin;

bool postComplete(String postMsg);

String getHeaderValue(String header, String headerName)
{
  return header.substring(strlen(headerName.c_str()));
}

bool execOTA() {
  bool state = false;
  Serial.println("Connecting to: " + String(host));
  if (otaClient.connect(host.c_str(), port)) {
    Serial.println("Fetching Bin: " + String(bin));
    otaClient.print(String("GET ") + updateUrlPath + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Cache-Control: no-cache\r\n" +
                 "Connection: close\r\n\r\n");

    unsigned long timeout = millis();
    while (otaClient.available() == 0) {
      if (millis() - timeout > 10000) {
        Serial.println("Client Timeout !");
        otaClient.stop();
        return state;
      }
    }
    while (otaClient.available()) {
      String line = otaClient.readStringUntil('\n');
      line.trim();
      if (!line.length()) {
        break;
      }
      if (line.startsWith("HTTP/1.1")) {
        if (line.indexOf("200") < 0) {
          Serial.println("Got a non 200 status code from server. Exiting OTA Update.");
          break;
        }
      }
      if (line.startsWith("Content-Length: ")) {
        contentLength = atol((getHeaderValue(line, "Content-Length: ")).c_str());
        Serial.println("Got " + String(contentLength) + " bytes from server");
      }

      if (line.startsWith("Content-Type: ")) {
        String contentType = getHeaderValue(line, "Content-Type: ");
        Serial.println("Got " + contentType + " payload.");
        if (contentType == "application/octet-stream") {
          isValidContentType = true;
        }
      }
    }
  } else {

    Serial.println("Connection to " + String(host) + " failed. Please check your setup");
    return state;
  }

  Serial.println("contentLength : " + String(contentLength) + ", isValidContentType : " + String(isValidContentType));

  if (contentLength && isValidContentType) {
    bool canBegin = Update.begin(contentLength);

    if (canBegin) {
      Serial.println("Begin OTA. This may take 2 - 5 mins to complete. Things might be quite for a while.. Patience!");
      size_t written = Update.writeStream(otaClient);

      if (written == contentLength) {
        Serial.println("Written : " + String(written) + " successfully");
      } else {
        Serial.println("Written only : " + String(written) + "/" + String(contentLength) + ". Retry?" );
        return state;
      }

      if (Update.end()) {
        Serial.println("OTA done!");
        if (Update.isFinished()) {
          Serial.println("Update successfully completed. Rebooting.");
          return true;
          //ESP.restart();
        } else {
          Serial.println("Update not finished? Something went wrong!");
          return false;
        }
      } else {
        Serial.println("Error Occurred. Error #: " + String(Update.getError()));
        return false;
      }
    } else {
      Serial.println("Not enough space to begin OTA");
      otaClient.flush();
      return false;
    }
  } else {
    Serial.println("There was no content in the response");
    otaClient.flush();
    return false;
  }
  return true;
}

bool postComplete(String postMsg)
{
  bool state = false;
  Serial.println("Connecting to: " + String(host));
  if (otaClient.connect(host.c_str(), port)) {
    Serial.println("sending post message: " + String(postMsg));
    otaClient.print(String("GET ") + postMsg + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Cache-Control: no-cache\r\n" +
                 "Connection: close\r\n\r\n");

    unsigned long timeout = millis();
    while (otaClient.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println("Client Timeout !");
        otaClient.stop();
        return state;
      }
    }
    while (otaClient.available()) {
      String line = otaClient.readStringUntil('\n');
      line.trim();
      if (!line.length()) {
        break;
      }
      if (line.startsWith("HTTP/1.1")) {
        if (line.indexOf("200") < 0) {
          Serial.println("Got a non 200 status code from server. Exiting OTA Update.");
          break;
        }
      }
    }
  } else {
    Serial.println("Connection to " + String(host) + " failed. Please check your setup");
  }
     return state;
}
