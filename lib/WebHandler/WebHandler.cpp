#include <App.hpp>
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Esp.h>
#include <FS.h>
#include <SPIFFS.h>
#include <OtaHandler.hpp>
#include "WebHandler.hpp"

#ifdef HAVE_ETH_IF
#include <ETH.h>
#endif

WebHandler webHandler;
static WebServer server(80);
size_t fsTotalBytes;
size_t fsUsedBytes;
char buffer[3000];

WebHandler::WebHandler() 
{ 
  initialized = false; 
}

void WebHandler::setup()
{
  LOG0("HTTP server setup...\n");

  fsTotalBytes = 0;
  fsUsedBytes = 0;

  if(SPIFFS.begin())
  {
    fsTotalBytes = SPIFFS.totalBytes();
    fsUsedBytes = SPIFFS.usedBytes();
    SPIFFS.end();
  }

  server.on("/", []() {

    server.sendHeader("Access-Control-Allow-Origin", "*");

    time_t now = time(nullptr);

    sprintf(buffer,
          "{"
          "\"millis\":%lu,"
          "\"utc\":%lu,"
          "\"utc_ctime\":\"%s\","
          "\"host_name\":\"%s.local\","
          "\"esp_sdk_version\":\"%s\","
          "\"platformio_env\":\"%s\","
          "\"platformio_platform\":\"%s\","
          "\"platformio_framework\":\"%s\","
          "\"arduino_board\":\"%s\","
          "\"chip_id\":\"%08llX\","
          "\"cpu_freq\":\"%dMhz\","
          "\"flash_size\":%u,"
          "\"flash_speed\":%u,"
          "\"fw_name\":\"%s\","
          "\"fw_version\":\"%s\","
          "\"build_date\":\"%s\","
          "\"build_time\":\"%s\","

          "\"net_ip_address\":\"%s\","
          "\"net_gateway_ip\":\"%s\","
          "\"net_subnet_mask\":\"%s\","
          "\"net_dns_ip\":\"%s\","
          "\"net_mac_address\":\"%s\","

#ifdef HAVE_ETH_IF
          "\"eth_duplex\":\"%s\","
          "\"eth_link_speed\":%u,"
#else
/*
          "\"wifi_ssid\":\"%s\","
          "\"wifi_channel\":%d,"
          "\"wifi_phy_mode\":\"%s\","
*/
#endif

          "\"spiffs_total\":%u,"
          "\"spiffs_used\":%u,"
          "\"free_heap\":%u,"
          "\"sketch_size\":%u,"
          "\"free_sketch_space\":%u,"
          "\"remote_client_ip\":\"%s\","
          "\"remote_client_port\":%u"
          "}",
          millis(),
          now,
          strtok( ctime(&now), "\n" ),
#ifdef HAVE_ETH_IF
          ETH.getHostname(),
#else
          WiFi.getHostname(),
#endif
          ESP.getSdkVersion(),
          PIOENV,
          PIOPLATFORM,
          PIOFRAMEWORK,
          ARDUINO_BOARD,
          ESP.getEfuseMac(),
          ESP.getCpuFreqMHz(), 
          ESP.getFlashChipSize(), 
          ESP.getFlashChipSpeed(), 
          APP_NAME,
          APP_VERSION, 
          __DATE__, 
          __TIME__,

#ifdef HAVE_ETH_IF
          ETH.localIP().toString().c_str(),
          ETH.gatewayIP().toString().c_str(),
          ETH.subnetMask().toString().c_str(),
          ETH.dnsIP().toString().c_str(),
          ETH.macAddress().c_str(),
          ( ETH.fullDuplex()) ? "full" : "half",
          ETH.linkSpeed(),
#else
          WiFi.localIP().toString().c_str(),
          WiFi.gatewayIP().toString().c_str(),
          WiFi.subnetMask().toString().c_str(),
          WiFi.dnsIP().toString().c_str(),
          WiFi.macAddress().c_str(),
#endif


/*
          WIFI_SSID,
          0l, // wifiHandler.getConnectCounter(),
          WiFi.channel(),
          "xx", // wifiHandler.getPhyMode(),
          "xx", // wifiHandler.getMacAddress(),
          "xx", // WiFi.hostname().c_str(),
          WiFi.localIP().toString().c_str(),
          WiFi.gatewayIP().toString().c_str(),
          WiFi.subnetMask().toString().c_str(),
          WiFi.dnsIP().toString().c_str(),
*/
          fsTotalBytes, 
          fsUsedBytes,
          ESP.getFreeHeap(), 
          ESP.getSketchSize(), 
          ESP.getFreeSketchSpace(),
          server.client().remoteIP().toString().c_str(),
          server.client().remotePort()
        );

    String message(buffer);

    server.send(200, "application/json", message );
  } );

  MDNS.addService("http", "tcp", 80);
  MDNS.addServiceTxt("http", "tcp", "path", "/");
  MDNS.addServiceTxt("http", "tcp", "fw_name", APP_NAME);
  MDNS.addServiceTxt("http", "tcp", "fw_version", APP_VERSION);
  server.begin();

  LOG0("HTTP server started\n");
  initialized = true;
}

void WebHandler::handle()
{
  if ( !initialized )
  {
    setup();
  }
  server.handleClient();
}
