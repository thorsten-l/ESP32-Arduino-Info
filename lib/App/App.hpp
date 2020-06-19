#ifndef __APP_HPP__
#define __APP_HPP__

#include <AppConfig.h>

#define LOG0( format ) Serial.printf( "(%ld) " format, millis())
#define LOG1( format, x) Serial.printf( "(%ld) " format, millis(), x )

#define APP_NAME "ESP32 Chip Info - Arduino"
#define APP_VERSION "1.7.1"

#define BOARD_LED_OFF (1-BOARD_LED_ON)

#endif
