#pragma once
#ifndef __REMOTEHOMEKITBASE_H__
#define __REMOTEHOMEKITBASE_H__

#include "common.h"
#include <ArduinoJson.h>

class HomekitRemoteBase {
protected:
  const char *deviceID;
  unsigned long lastMessage = 0;
  bool awaitingResponse = false;
  std::function<void(bool)> HKRResponseCallback = NULL;

  void handleHKRResponse(bool success) {
    if (awaitingResponse) {
      awaitingResponse = false;
      lastMessage = 0;
      if (HKRResponseCallback != NULL) {
        HKRResponseCallback(success);
        HKRResponseCallback = NULL;
      }
    } else {
      handleHKRError(HKR_ERROR_UNEXPECTED_RESPONSE);
    }
  }
  
public:
  void listenForHKRResponse() {
    if (!awaitingResponse) return;

    const unsigned long now = millis();
    const unsigned long diff = max(now, lastMessage) - min(now, lastMessage);
    if (diff >= HKR_RESPONSE_TIMEOUT) {
      handleHKRError(HKR_ERROR_TIMEOUT);
      awaitingResponse = false;
    }
  }

  void sendHKRResponse(bool success) {
    StaticJsonDocument<8> payload;
    payload.set(success);
    sendHKRMessage(HKR_COMMAND_RESPONSE, payload.as<JsonVariant>(), false);
  }

  void HKRWebsocketEvent(int id, uint8_t *message) {
    if (strncmp((const char *)message, "{", 1) == 0) {
      StaticJsonDocument<HKR_MAX_JSON_DOC_SIZE> doc;
      DeserializationError err = deserializeJson(doc, message);
      if (err) {
        handleHKRError(HKR_ERROR_JSON_DESERIALIZE);
        HKR_LOG_LINE("Error: %s", err.c_str());
        return;
      }

      if(strcmp(doc[HKR_DEVICE], deviceID) == 0) receiveHKRMessage(id, doc);
    }
  }

  virtual void sendHKRMessage(
    const char *command,
    const JsonVariant &payload,
    bool requireResponse = true,
    std::function<void(bool)> onResponse = NULL
  ) = 0;

  virtual void receiveHKRMessage(int id, JsonDocument &doc) = 0;

  virtual void handleHKRCommand(
    const char *command,
    const JsonVariant &payload
  ) = 0;
  
  virtual void handleHKRError(HKR_ERROR err) = 0;
};

#endif