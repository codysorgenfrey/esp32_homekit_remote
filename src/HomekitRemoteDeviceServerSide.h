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
  int clientID = -1;

  void registerHKRDevice(const char *dID, int cID) {
    deviceID = dID;
    clientID = cID;
    HK_LOG_LINE("Registered %s as HKR device id: %i.", deviceID, clientID);
    
    sendHKRResponse(true);
  }

public:
  HomekitRemoteDeviceServerSide(WebSocketsServer *ws) {
    webSocket = ws;
  }

  void sendHKRMessage(
    const JsonDocument &doc,
    bool requireResponse = true,
    std::function<void(bool)> onResponse = NULL
  ) {
    if (clientID == -1) {
      handleHKRError(HKR_ERROR_DEVICE_NOT_REGISTERED);
      return;
    }

    String message;
    serializeJson(doc, message);
    if (!webSocket->sendTXT(clientID, message)) handleHKRError(HKR_ERROR_WEBSOCKET_ERROR);
    lastMessage = millis();
    awaitingResponse = requireResponse;
    HKRResponseCallback = onResponse;
  }

  void HKRMessageRecieved(int id, const JsonDocument &doc) {
    const char *command = doc[HKR_COMMAND].as<const char *>();
    if (strcmp(command, HKR_COMMAND_RESPONSE) == 0) {
      handleHKRResponse(doc[HKR_PAYLOAD].as<bool>());
    } else if (strcmp(command, HKR_COMMAND_REGISTER) == 0) {
      registerHKRDevice(doc[HKR_DEVICE].as<const char *>(), id);
    } else {
      handleHKRCommand(doc);
    }
  }
};

#endif