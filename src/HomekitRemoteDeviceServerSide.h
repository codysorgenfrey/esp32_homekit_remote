#pragma once
#ifndef __REMOTEHOMEKITDEVICESERVERSIDE_H__
#define __REMOTEHOMEKITDEVICESERVERSIDE_H__

#include "common.h"
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include "HomekitRemoteBase.h"

class HomekitRemoteDeviceServerSide : protected HomekitRemoteBase {
protected:
  WebSocketsServer *webSocket;
  const char *deviceID;
  int clientID = -1;

  void handleHKRCommandRegister(const char *dID, int cID) {
    deviceID = dID;
    clientID = cID;
    HK_LOG_LINE("Registered %s as HKR device id: %i.", deviceID, clientID);
  }

public:
  HomekitRemoteDeviceServerSide(WebSocketsServer *ws) {
    webSocket = ws;
  }

  void sendHKRMessage(const JsonDocument &doc, bool checkResponse = true) {
    if (clientID == -1) {
      HK_ERROR_LINE("HKR device %s not registered.", doc["device"].as<const char *>());
      return;
    }

    String message;
    serializeJson(doc, message);
    if (!webSocket->sendTXT(clientID, message))
      HK_ERROR_LINE("Error sending message: %s", message);
    lastMessage = millis();
    awaitingResponse = checkResponse;
  }

  void HKRMessageRecieved(int id, const JsonDocument &doc) {
    const char *command = doc["command"].as<const char *>();
    if (strcmp(command, HKR_RESPONSE_COMMAND) == 0) {
      handleHKRCommandResponse();
    } else if (strcmp(command, HKR_REGISTER_COMMAND) == 0) {
      handleHKRCommandRegister(doc["device"].as<const char *>(), id);
    } else {
      handleHKRCommand(doc);
    }
  }
};

#endif