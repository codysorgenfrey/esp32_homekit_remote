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

  void registerHKRDevice() {
    HKR_LOG_LINE("Registering with HomeKit Hub");
    StaticJsonDocument<8> payload;
    payload.set(deviceID);
    sendHKRMessage(
      HKR_COMMAND_REGISTER,
      payload.as<JsonVariant>(),
      true,
      [this](bool success) {
        if (success) HKR_LOG_LINE("Registered successfully");
        else handleHKRError(HKR_ERROR_CONNECTION_REFUSED);
    });
  }

public:
  HomekitRemoteDevice(WebSocketsClient *ws, const char *dID) {
    webSocket = ws;
    deviceID = dID;
    registerHKRDevice();
  }

  void sendHKRMessage(
    const char *command,
    const JsonVariant &payload,
    bool requireResponse = true,
    std::function<void(bool)> onResponse = NULL
  ) {
    StaticJsonDocument<HKR_MAX_JSON_DOC_SIZE> doc;
    doc[HKR_DEVICE] = deviceID;
    doc[HKR_COMMAND] = command;
    doc[HKR_PAYLOAD] = payload;

    String message;
    serializeJson(doc, message);
    if (!webSocket->sendTXT(message)) handleHKRError(HKR_ERROR_WEBSOCKET_ERROR);
    lastMessage = millis();
    awaitingResponse = requireResponse;
    HKRResponseCallback = onResponse;
  }

  void receiveHKRMessage(int id, JsonDocument &doc) {
    const char *command = doc[HKR_COMMAND];
    if (strcmp(command, HKR_COMMAND_RESPONSE) == 0) {
      handleHKRResponse(doc[HKR_PAYLOAD].as<bool>());
    } else {
      handleHKRCommand(doc[HKR_COMMAND], doc[HKR_PAYLOAD]);
    }
  }
};

#endif