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

  void registerHKRDevice(int cID, const char *dID) {
    if (dID != deviceID) {
      handleHKRError(HKR_ERROR_DEVICE_NOT_REGISTERED);
      return;
    }
    
    clientID = cID;
    HK_LOG_LINE("Registered %s as HKR device id: %i.", deviceID, clientID);
    
    sendHKRResponse(true);
  }

public:
  HomekitRemoteDeviceServerSide(WebSocketsServer *ws, const char *dID) {
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

  void receiveHKRMessage(int id, uint8_t *message) {
    StaticJsonDocument<HKR_MAX_JSON_DOC_SIZE> doc;
    DeserializationError err = deserializeJson(doc, message);
    if (err) handleHKRError(HKR_ERROR_JSON_DESERIALIZE);

    const char *command = doc[HKR_COMMAND];
    if (strcmp(command, HKR_COMMAND_RESPONSE) == 0) {
      handleHKRResponse(doc[HKR_PAYLOAD]);
    } else if (strcmp(command, HKR_COMMAND_REGISTER) == 0) {
      registerHKRDevice(id, doc[HKR_DEVICE]);
    } else {
      handleHKRCommand(doc[HKR_COMMAND], doc[HKR_PAYLOAD]);
    }
  }
};

#endif