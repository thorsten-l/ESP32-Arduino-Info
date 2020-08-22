#ifndef __APP_HPP__
#define __APP_HPP__

#include <AppConfig.h>
#include <WiFi.h>

#define LOG0( format ) Serial.printf( "(%ld) " format, millis())
#define LOG1( format, x) Serial.printf( "(%ld) " format, millis(), x )
#define TLOG0( format ) TelnetStream.printf( "(%ld) " format, millis())
#define TLOG1( format, x) TelnetStream.printf( "(%ld) " format, millis(), x )

#define APP_NAME "ESP32 Chip Info - Arduino"
#define APP_VERSION "1.8.2"
#define APP_AUTHOR "Dr. Thorsten Ludewig <t.ludewig@gmail.com>"
#define APP_CONFIG_FILE "/config.bin"

#define BOARD_LED_OFF (1-BOARD_LED_ON)

extern void appSetup();
extern void appLoop();
extern void appShowHeader(Stream& out);
extern const char *appUptime();
extern const char *appDateTime();
extern int wifiConnectCounter;

#endif
