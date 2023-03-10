#pragma once
#ifndef __REMOTEHOMEKITDEVICESERVERSIDE_H__
#define __REMOTEHOMEKITDEVICESERVERSIDE_H__

#include "common.h"
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include "HomekitRemoteBase.h"

class HomekitRemoteDeviceServerSide : public HomekitRemoteBase {
protected:
  WebSocketsServer *webSocket;
  int clientID = -1;

  void registerHKRClient(int cID, const char *dID) {
    if (strcmp(dID, deviceID) != 0) {
      handleHKRError(HKR_ERROR_DEVICE_NOT_REGISTERED);
      return;
    }
    
    clientID = cID;
    HKR_LOG_LINE("Registered %s as HKR device id: %i.", deviceID, clientID);
    
    sendHKRResponse(true);
    handleHKRClientRegistered(cID, dID);
  }

public:
  void registerHKRDevice(WebSocketsServer *ws, const char *dID) {
    webSocket = ws;
    deviceID = dID;
  }

  void sendHKRMessage(
    const char *command,
    const JsonVariant &payload,
    bool requireResponse = true,
    std::function<void(bool)> onResponse = NULL
  ) {
    if (clientID == -1) {
      handleHKRError(HKR_ERROR_DEVICE_NOT_REGISTERED);
      return;
    }

    StaticJsonDocument<HKR_MAX_JSON_DOC_SIZE> doc;
    doc[HKR_DEVICE] = deviceID;
    doc[HKR_COMMAND] = command;
    doc[HKR_PAYLOAD] = payload;

    String message;
    serializeJson(doc, message);
    if (!webSocket->sendTXT(clientID, message)) handleHKRError(HKR_ERROR_WEBSOCKET_ERROR);
    lastMessage = millis();
    awaitingResponse = requireResponse;
    HKRResponseCallback = onResponse;
  }

  void receiveHKRMessage(int id, JsonDocument &doc) {
    const char *command = doc[HKR_COMMAND];
    if (strcmp(command, HKR_COMMAND_RESPONSE) == 0) {
      handleHKRResponse(doc[HKR_PAYLOAD]);
    } else if (strcmp(command, HKR_COMMAND_REGISTER) == 0) {
      registerHKRClient(id, doc[HKR_DEVICE]);
    } else {
      handleHKRCommand(doc[HKR_COMMAND], doc[HKR_PAYLOAD]);
    }
  }

  void handleHKRClientRegistered(int cID, const char *dID) {}
};

#endif