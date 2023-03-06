#pragma once
#ifndef __HKRCOMMON_H__
#define __HKRCOMMON_H__

#include <Arduino.h>

#define HKR_REGISTER_COMMAND "register"
#define HKR_RESPONSE_COMMAND "response"
#define HKR_RESPONSE_TIMEOUT 5000 // 5 seconds

// Logging
#define HK_DEBUG_LEVEL_NONE -1
#define HK_DEBUG_LEVEL_ERROR 0
#define HK_DEBUG_LEVEL_INFO 1

#define HK_DEBUG HK_DEBUG_LEVEL_INFO

#if HK_DEBUG >= HK_DEBUG_LEVEL_ERROR
    #define HK_ERROR_LINE(message, ...) printf("ERR [%7lu][%.2fkb] HomeKit Hub: " message "\n", millis(), (esp_get_free_heap_size() * 0.001f), ##__VA_ARGS__)
#else
    #define HK_ERROR_LINE(message, ...)
#endif

#if HK_DEBUG >= HK_DEBUG_LEVEL_INFO
    #define HK_LOG_LINE(message, ...) printf(">>> [%7lu][%.2fkb] HomeKit Hub: " message "\n", millis(), (esp_get_free_heap_size() * 0.001f), ##__VA_ARGS__)
#else
    #define HK_LOG_LINE(message, ...)
#endif

#if HK_DEBUG >= HK_DEBUG_LEVEL_VERBOSE
    #define HK_VERB_LINE(message, ...) printf(">>> [%7lu][%.2fkb] HomeKit Hub: " message "\n", millis(), (esp_get_free_heap_size() * 0.001f), ##__VA_ARGS__)
#else
    #define HK_VERB_LINE(message, ...)
#endif

#endif