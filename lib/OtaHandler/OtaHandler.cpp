#include <App.hpp>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include "OtaHandler.hpp"

void InitializeOTA()
{
  MDNS.begin( OTA_HOSTNAME );
  ArduinoOTA.setHostname(OTA_HOSTNAME);
  ArduinoOTA.setPassword(OTA_PASSWORD);

  ArduinoOTA
  .onStart([]()
  {
    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.printf("\n\n[%d] OTA - Start updating %s\n", xPortGetCoreID(), 
      (ArduinoOTA.getCommand() == U_FLASH) ? "sketch" : "filesystem" );
  } )

  .onEnd([]()
  {
    digitalWrite( BUILTIN_LED, BOARD_LED_OFF );
    Serial.printf("\n[%d] OTA - Upload finished.\n", xPortGetCoreID() );
    Serial.println("\n\n*** restarting system ***\n\n");
  } )

  .onProgress([](unsigned int progress, unsigned int total)
  {
    digitalWrite( BUILTIN_LED, 1 ^ digitalRead(BUILTIN_LED));
    Serial.printf("[%d] OTA - Progress: %u%%\r", xPortGetCoreID(), (progress / (total / 100)));
  } )

  .onError([](ota_error_t error)
  {
    Serial.printf("Error[%u]: ", error);

    if (error == OTA_AUTH_ERROR)
      Serial.println("Auth Failed");
    else
    if (error == OTA_BEGIN_ERROR)
      Serial.println("Begin Failed");
    else
    if (error == OTA_CONNECT_ERROR)
      Serial.println("Connect Failed");
    else
    if (error == OTA_RECEIVE_ERROR)
      Serial.println("Receive Failed");
    else
    if (error == OTA_END_ERROR)
      Serial.println("End Failed");
  } );

  ArduinoOTA.begin();
  MDNS.addServiceTxt("_arduino", "_tcp", "fw_name", APP_NAME );
  MDNS.addServiceTxt("_arduino", "_tcp", "fw_version", APP_VERSION );

  Serial.println("Update Over-The-Air : initialized");
}
