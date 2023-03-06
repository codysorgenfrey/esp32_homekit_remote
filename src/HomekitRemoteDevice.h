#pragma once
#ifndef __REMOTEHOMEKITDEVICE_H__
#define __REMOTEHOMEKITDEVICE_H__

#include "common.h"
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include "HomekitRemoteBase.h"

class HomekitRemoteDevice : public HomekitRemoteBase {
protected:
  WebSocketsClient *webSocket;
  const char *deviceID;
  int clientID = -1;

public:
  HomekitRemoteDevice(WebSocketsClient *ws, const char *dID) {
    webSocket = ws;
    deviceID = dID;

    StaticJsonDocument<92> doc;
    doc["command"] = HKR_REGISTER_COMMAND;
    doc["device"] = deviceID;
    sendHKRMessage(doc);
  }

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
      handleHKRCommandResponse();
    } else {
      handleHKRCommand(doc);
    }
  }
};

#endif