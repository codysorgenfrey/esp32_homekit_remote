#pragma once
#ifndef __REMOTEHOMEKITDEVICE_H__
#define __REMOTEHOMEKITDEVICE_H__

#include "common.h"
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

class HomekitRemoteDevice {
  WebSocketsClient *webSocket;
  const char *deviceID;
  int clientID = -1;
  bool isServerSide = false;
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

  void handleHKRCommandRegister(const char *dID, int cID) {
    deviceID = dID;
    clientID = cID;
    HK_LOG_LINE("Registered %s as HKR device id: %i.", deviceID, clientID);
  }

public:
  // Client side
  HomekitRemoteDevice(WebSocketsClient *ws, const char *dID) {
    webSocket = ws;
    deviceID = dID;

    StaticJsonDocument<92> doc;
    doc["command"] = HKR_REGISTER_COMMAND;
    doc["device"] = deviceID;
    sendHKRMessage(doc);
  }

  // Server side
  HomekitRemoteDevice(WebSocketsClient *ws) {
    webSocket = webSocket;
    isServerSide = true;
  }

  void sendHKRMessage(const JsonDocument &doc, bool checkResponse = true) {
    if (isServerSide && clientID == -1) {
      HK_ERROR_LINE("HKR device %s not registered.", doc["device"].as<const char *>());
      return;
    }

    String message;
    serializeJson(doc, message);
    if (!webSocket->sendTXT(message)) HK_ERROR_LINE("Error sending message: %s", message);
    lastMessage = millis();
    awaitingResponse = checkResponse;
  }

  // Client side
  void HKRMessageRecieved(const JsonDocument &doc) {
    const char *command = doc["command"].as<const char *>();
    if (strcmp(command, HKR_RESPONSE_COMMAND) == 0) {
      handleHKRCommandResponse();
    } else {
      handleHKRCommand(doc);
    }
  }

  // Server side
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