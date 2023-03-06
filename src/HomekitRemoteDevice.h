#pragma once
#ifndef __REMOTEHOMEKITDEVICE_H__
#define __REMOTEHOMEKITDEVICE_H__

#include "common.h"
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

class HomekitRemoteDevice {
  WebSocketsClient *webSocket;
  const char *deviceID;
  unsigned long lastMessage = 0;
  bool awaitingResponse = false;

public:
  HomekitRemoteDevice(WebSocketsClient *webSocket, const char *deviceID) {
    this->webSocket = webSocket;
    this->deviceID = deviceID;

    StaticJsonDocument<92> doc;
    doc["command"] = HKR_REGISTER_COMMAND;
    doc["device"] = deviceID;
    sendHKRMessage(doc);
  }

  virtual void handleHKRCommand(const JsonDocument &doc) = 0;

  void sendHKRMessage(const JsonDocument &doc, bool checkResponse = true) {
    String message;
    serializeJson(doc, message);
    if (!webSocket->sendTXT(message)) HK_ERROR_LINE("Error sending message: %s", message);
    lastMessage = millis();
    awaitingResponse = checkResponse;
  }

  void HKRMessageRecieved(const JsonDocument &doc) {
    const char *command = doc["command"].as<const char *>();
    if (strcmp(command, HKR_RESPONSE_COMMAND) == 0) {
      if (awaitingResponse) {
        awaitingResponse = false;
        lastMessage = 0;
      } else {
        HK_ERROR_LINE("Unexpected hub response.");
      }
    } else {
      handleHKRCommand(doc);
    }
  }

  void listenForHKRResponse() {
    if (!awaitingResponse) return;

    const unsigned long now = millis();
    const unsigned long diff = max(now, lastMessage) - min(now, lastMessage);
    if (diff >= HKR_RESPONSE_TIMEOUT) {
      HK_ERROR_LINE("Homekit hub not responding.");
      awaitingResponse = false;
    }
  }
};

#endif