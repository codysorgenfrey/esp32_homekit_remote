#pragma once
#ifndef __REMOTEHOMEKITDEVICE_H__
#define __REMOTEHOMEKITDEVICE_H__

#include "common.h"
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include "HomekitRemoteBase.h"

class HomekitRemoteDevice : protected HomekitRemoteBase {
protected:
  WebSocketsClient *webSocket;

  void registerHKRDevice() {
    StaticJsonDocument<92> doc;
    doc[HKR_DEVICE] = deviceID;
    doc[HKR_COMMAND] = HKR_COMMAND_REGISTER;
    sendHKRMessage(doc, true, [this](bool success) {
      if (success) HK_LOG_LINE("Registered with HomeKit Hub");
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
    const JsonDocument &doc,
    bool requireResponse = true,
    std::function<void(bool)> onResponse = NULL
  ) {
    String message;
    serializeJson(doc, message);
    if (!webSocket->sendTXT(message)) handleHKRError(HKR_ERROR_WEBSOCKET_ERROR);
    lastMessage = millis();
    awaitingResponse = requireResponse;
    HKRResponseCallback = onResponse;
  }

  void HKRMessageRecieved(const JsonDocument &doc) {
    const char *command = doc["command"].as<const char *>();
    if (strcmp(command, HKR_COMMAND_RESPONSE) == 0) {
      handleHKRResponse(doc[HKR_PAYLOAD].as<bool>());
    } else {
      handleHKRCommand(doc);
    }
  }
};

#endif