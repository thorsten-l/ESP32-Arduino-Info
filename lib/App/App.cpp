#include <Arduino.h>
#include <App.hpp>

#include <Esp.h>
#include <SPIFFS.h>
#include <WiFi.h>

#ifdef HAVE_ETH_IF
#include <ETH.h>
#include <NetHandler.hpp>
#endif

#include "soc/soc.h"
#include "soc/efuse_reg.h"
#include "soc/rtc_cntl_reg.h"

#include <OtaHandler.hpp>
#include <SdCardHandler.hpp>
#include <WebHandler.hpp>
#include <TelnetStream.h>

byte mac[6];
uint64_t chipid;

volatile SemaphoreHandle_t mutex;

static int counter;
static time_t lastTimestamp = 0;
static const char *rollingChars = "|/-\\";

int wifiConnectCounter = 0;
time_t appStartTime = 0l;

struct tm appTimeinfo;
char appUptimeBuffer[64];
char appLocalIpBuffer[32];
char appDateTimeBuffer[32];

const char *appDateTime()
{
  time_t now = time(nullptr);
  localtime_r( &now, &appTimeinfo );
  sprintf( appDateTimeBuffer, "%4d-%02d-%02d %02d:%02d:%02d", 
  appTimeinfo.tm_year+1900, appTimeinfo.tm_mon+1, appTimeinfo.tm_mday,
  appTimeinfo.tm_hour, appTimeinfo.tm_min, appTimeinfo.tm_sec );
  return appDateTimeBuffer;
}

const char *appUptime()
{
  time_t uptime = millis() / 1000;
  int uptimeSeconds = uptime % 60;
  int uptimeMinutes = (uptime / 60) % 60;
  int uptimeHours = (uptime / 3600) % 24;
  time_t uptimeDays = (uptime / 86400);
  sprintf( appUptimeBuffer, 
    "%ld days, %d hours, %d minutes, %d seconds",
    uptimeDays, uptimeHours, uptimeMinutes, uptimeSeconds );
  return appUptimeBuffer;
}

void appShowHeader(Stream& out)
{
  out.println( "\n\n" APP_NAME " - " APP_VERSION " - " APP_AUTHOR);
  out.println("BUILD: " __DATE__ " " __TIME__);
  out.println("PIOENV: " PIOENV );
  out.println("PIOPLATFORM: " PIOPLATFORM );
  out.println("PIOFRAMEWORK: " PIOFRAMEWORK );
  out.printf( "ESP SDK Version: %s\n", ESP.getSdkVersion() );
}

int getChipRevision()
{
  return ((REG_READ(EFUSE_BLK0_RDATA3_REG) >> EFUSE_RD_CHIP_VER_REV1_S) &&
          EFUSE_RD_CHIP_VER_REV1_V);
}

void printAsDouble(const char *label, uint32_t value, double divisor,
                   const char *unit)
{
  Serial.print(label);
  double dvalue = (double)value;
  dvalue /= divisor;
  Serial.print(dvalue);
  Serial.println(unit);
}

void secondTask(void *parameter)
{
  xSemaphoreTake(mutex, portMAX_DELAY);
  Serial.printf("2nd Task core       : %d\n", xPortGetCoreID());
  xSemaphoreGive(mutex);
  vTaskDelete(NULL);
}

void connectWiFi()
{
  Serial.print("\nConnecting to WiFi network ");

  WiFi.begin();
  // WiFi.mode(WIFI_STA);
  WiFi.setHostname(OTA_HOSTNAME);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED)
  {
#ifdef BUILTIN_LED
    digitalWrite(BUILTIN_LED, 1 ^ digitalRead(BUILTIN_LED));
#endif
    delay(500);
    Serial.print(".");
  }

  Serial.println(" connected.\n");
#ifdef BUILTIN_LED
  digitalWrite(BUILTIN_LED, BOARD_LED_ON);
#endif

  wifiConnectCounter++;

  WiFi.macAddress(mac);
  Serial.printf("WiFi MAC Address    : %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0],
                mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial.printf("WiFi Hostname       : %s\n", WiFi.getHostname());
  Serial.print("WiFi IP-Address     : ");
  Serial.println(WiFi.localIP());
  Serial.print("WiFi Gateway-IP     : ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("WiFi Subnetmask     : ");
  Serial.println(WiFi.subnetMask());
  Serial.print("WiFi DNS Server     : ");
  Serial.println(WiFi.dnsIP());
  Serial.print("WiFi connect counter: " );
  Serial.println(wifiConnectCounter);
  Serial.println();
}

void appSetup()
{
  struct timeval tv;
  struct timezone tz;
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  tz.tz_dsttime = 0;
  tz.tz_minuteswest = 0;
  settimeofday(&tv, &tz); // reset clock for correct uptime

  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector

#ifdef BUILTIN_LED
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, BOARD_LED_OFF);
#endif
  Serial.begin(115200);
  mutex = xSemaphoreCreateMutex();
  delay(3000); // wait for serial monitor
  Serial.println("\n\n\n" APP_NAME " - Version " APP_VERSION
                 " by " APP_AUTHOR );
  Serial.println("Build date: " __DATE__ " " __TIME__ "\n");

  if (!SPIFFS.begin(true))
  {
    Serial.println("SPIFFS Mount Failed... reboot system");
    delay(5000);
    ESP.restart();
    delay(10000);
  }

  Serial.printf("Chip Revision (ESP) : %d\n", ESP.getChipRevision());
  Serial.printf("Chip Revision (REG) : %d\n", getChipRevision());
  Serial.println();
  Serial.printf("CPU Frequency       : %dMHz\n", ESP.getCpuFreqMHz());
  Serial.println();
  printAsDouble("Flash Chip          : ", ESP.getFlashChipSize(), 1048576,
                "MB");
  printAsDouble("Flash Chip Speed    : ", ESP.getFlashChipSpeed(), 1000000,
                "MHz");
  Serial.println();
  printAsDouble("Heap Size           : ", ESP.getHeapSize(), 1024, "KB");
  printAsDouble("Free Heap           : ", ESP.getFreeHeap(), 1024, "KB");
  printAsDouble("Sketch Size         : ", ESP.getSketchSize(), 1024, "KB");
  printAsDouble("Free Sketch Space   : ", ESP.getFreeSketchSpace(), 1024, "KB");
  printAsDouble("SPIFFS total bytes  : ", SPIFFS.totalBytes(), 1024, "KB");
  printAsDouble("SPIFFS used bytes   : ", SPIFFS.usedBytes(), 1024, "KB");
  SPIFFS.end();

  printAsDouble("PSRAM Size          : ", ESP.getPsramSize(), 1024, "KB");
  printAsDouble("Free PSRAM          : ", ESP.getFreePsram(), 1024, "KB");

#ifdef BOARD_HAS_SDCARD_SLOT
  Serial.println();
  initializeSdCard();
#endif

  Serial.printf("\nCycle Count         : %u\n", ESP.getCycleCount());
  Serial.println();
  xTaskCreatePinnedToCore(&secondTask, "secondTask", 10000, NULL, 1, NULL, 0);
  delay(50);
  xSemaphoreTake(mutex, portMAX_DELAY);
  Serial.printf("Running core        : %d\n", xPortGetCoreID());
  xSemaphoreGive(mutex);

  WiFi.mode(WIFI_OFF);
  WiFi.disconnect(true, true);
  delay(2000);

#ifdef HAVE_ETH_IF
  WiFi.onEvent(NetEventHandler);

#ifdef NRST
    pinMode(NRST, OUTPUT);
    digitalWrite(NRST, 0);
    delay(200);
    digitalWrite(NRST, 1);
    delay(200);
    digitalWrite(NRST, 0);
    delay(200);
    digitalWrite(NRST, 1);
#endif

  ETH.begin();
  ETH.setHostname(OTA_HOSTNAME);
#else
  connectWiFi();
#endif

#ifndef OTA_DISABLED
  InitializeOTA();
#else
  Serial.println("Update Over-The-Air : disabled");
#endif

  TelnetStream.begin();

  Serial.println();
  Serial.printf("SDK Version         : %s\n", ESP.getSdkVersion());
  Serial.printf("PIO Environment     : %s\n", PIOENV);
  Serial.printf("PIO Platform        : %s\n", PIOPLATFORM);
  Serial.printf("PIO Framework       : %s\n", PIOFRAMEWORK);
  Serial.printf("Arduino Board       : %s\n", ARDUINO_BOARD);
  Serial.println();
  counter = 0;

  configTzTime(TIMEZONE, NTP_SERVER);

  if (getLocalTime(&appTimeinfo, 20000)) // wait up to 20sec to sync
  {
    time_t now = time(nullptr);
    appStartTime = now - (millis()/1000);
    localtime_r(&appStartTime, &appTimeinfo);
    Serial.println( &appTimeinfo, "App start time      : %Y-%m-%d %H:%M:%S");
    localtime_r(&now, &appTimeinfo);
    Serial.println(&appTimeinfo, "Time                : %Y-%m-%d %H:%M:%S");
    Serial.print(  "Timezone            : ");
    Serial.println(getenv("TZ"));
    Serial.println();
  }
  else
  {
    Serial.println("Time not set");
  }

#ifdef TEST_BUTTONS
  pinMode( KEY_BUILTIN, INPUT_PULLUP );
#endif

}

void appLoop()
{
  time_t currentTimestamp = millis();

  if (
#ifdef HAVE_ETH_IF
      eth_connected
#else
      WiFi.isConnected()
#endif
  )
  {
    TelnetStream.handle();
    ArduinoOTA.handle();
    webHandler.handle();
  }

  if ((currentTimestamp - lastTimestamp >= 1000))
  {
    Serial.printf("\r[%d] Running: ", xPortGetCoreID());
    Serial.print(rollingChars[counter]);

#ifdef TEST_BUTTONS
    Serial.printf(" %d", digitalRead(KEY_BUILTIN));
#endif

#ifdef HAVE_ETH_IF
    if (eth_connected)
    {
      Serial.print(" connected to Ethernet");
    }
#else
    bool wifiIsConnected = WiFi.isConnected();
    Serial.printf(" and WiFi is%s connected", wifiIsConnected ? "" : " NOT");

    if (wifiIsConnected == false)
    {
      connectWiFi();
    }
#endif

    lastTimestamp = currentTimestamp;
    counter++;
    counter %= 4;
  }
}
