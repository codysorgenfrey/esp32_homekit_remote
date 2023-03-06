#pragma once
#ifndef __REMOTEHOMEKITBASE_H__
#define __REMOTEHOMEKITBASE_H__

#include "common.h"
#include <ArduinoJson.h>

class HomekitRemoteBase {
protected:
  unsigned long lastMessage = 0;
  bool awaitingResponse = false;

  void handleHKRCommandResponse() {
    if (awaitingResponse) {
      awaitingResponse = false;
      lastMessage = 0;
    } else {
      HK_ERROR_LINE("Unexpected HKR response.");
    }
  }
  
public:
  void listenForHKRResponse() {
    if (!awaitingResponse) return;

    const unsigned long now = millis();
    const unsigned long diff = max(now, lastMessage) - min(now, lastMessage);
    if (diff >= HKR_RESPONSE_TIMEOUT) {
      HK_ERROR_LINE("Homekit hub not responding.");
      awaitingResponse = false;
    }
  }

  virtual void handleHKRCommand(const JsonDocument &doc) = 0;
};

#endif