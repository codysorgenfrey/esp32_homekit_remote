#pragma once
#ifndef __HKRCOMMON_H__
#define __HKRCOMMON_H__

#include <Arduino.h>

// General
#define HKR_COMMAND "command"
#define HKR_DEVICE "device"
#define HKR_PAYLOAD "payload"
#define HKR_COMMAND_REGISTER "register"
#define HKR_COMMAND_RESPONSE "response"
#define HKR_RESPONSE_TIMEOUT 5000 // 5 seconds
#define HKR_MAX_JSON_DOC_SIZE 256

// Errors
enum HKR_ERROR {
    HKR_ERROR_TIMEOUT,
    HKR_ERROR_UNEXPECTED_RESPONSE,
    HKR_ERROR_WEBSOCKET_ERROR,
    HKR_ERROR_CONNECTION_REFUSED,
    HKR_ERROR_DEVICE_NOT_REGISTERED,
    HKR_ERROR_JSON_DESERIALIZE,
};

// Logging
#define HK_DEBUG_LEVEL_NONE 0
#define HK_DEBUG_LEVEL_INFO 1

#define HK_DEBUG HK_DEBUG_LEVEL_INFO

#if HK_DEBUG >= HK_DEBUG_LEVEL_INFO
    #define HK_LOG_LINE(message, ...) printf(">>> [%7lu][%.2fkb] HomeKit Hub: " message "\n", millis(), (esp_get_free_heap_size() * 0.001f), ##__VA_ARGS__)
#else
    #define HK_LOG_LINE(message, ...)
#endif

#endif