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
    StaticJsonDocument<92> doc;
    doc[HKR_DEVICE] = deviceID;
    doc[HKR_COMMAND] = HKR_COMMAND_RESPONSE;
    doc[HKR_PAYLOAD] = success;
    sendHKRMessage(doc, false);
  }

  virtual void sendHKRMessage(
    const JsonDocument &doc,
    bool requireResponse = true,
    std::function<void(bool)> onResponse = NULL
  ) = 0;
  virtual void handleHKRCommand(const JsonDocument &doc) = 0;
  virtual void handleHKRError(HKR_ERROR err) = 0;
};

#endif