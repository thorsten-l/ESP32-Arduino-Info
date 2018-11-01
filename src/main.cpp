#include <Arduino.h>
#include <ESP.h>
#include <WiFi.h>
#include "soc/efuse_reg.h"

byte mac[6];
uint64_t chipid;

volatile SemaphoreHandle_t mutex;

static int counter;
static const char* rollingChars = "|/-\\";

struct tm timeinfo;

#define TIMEZONE "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00"
#define NTP_SERVER "192.168.5.1"

int getChipRevision()
{
  return (( REG_READ(EFUSE_BLK0_RDATA3_REG) >>
            EFUSE_RD_CHIP_VER_REV1_S ) && EFUSE_RD_CHIP_VER_REV1_V );
}

void printAsDouble( const char* label, uint32_t value, double divisor, const char* unit )
{
  Serial.print( label );
  double dvalue = (double)value;
  dvalue /= divisor;
  Serial.print( dvalue );
  Serial.println( unit );
}

void secondTask( void * parameter )
{
  xSemaphoreTake( mutex, portMAX_DELAY );
  Serial.printf("Running core        : %d\n", xPortGetCoreID() );
  xSemaphoreGive( mutex );
  vTaskDelete(NULL);
}

void setup()
{
  Serial.begin(115200);
  mutex = xSemaphoreCreateMutex();
  delay( 3000 ); // wait for serial monitor
  Serial.println( "\n\n\nESP32 Chip Info - Arduino - Version 1.0.15 by Dr. Thorsten Ludewig" );
  Serial.println( "Build date: " __DATE__ " " __TIME__ "\n");

  Serial.printf("Chip Revision (ESP) : %d\n", ESP.getChipRevision());
  Serial.printf("Chip Revision (REG) : %d\n", getChipRevision() );
  Serial.println();
  Serial.printf("CPU Frequency       : %dMHz\n", ESP.getCpuFreqMHz() );
  Serial.println();
  printAsDouble("Flash Chip IDE Size : ", ESP.getFlashChipSize(), 1048576, "MB" );
  printAsDouble("Flash Chip Speed    : ", ESP.getFlashChipSpeed(), 1000000, "MHz" );
  Serial.println();
  printAsDouble("Free Heap           : ", ESP.getFreeHeap(), 1024, "KB" );
  Serial.printf("Cycle Count         : %d\n", ESP.getCycleCount());

  Serial.println();
  xTaskCreatePinnedToCore( &secondTask, "secondTask", 10000, NULL, 1, NULL, 0 );
  delay(50);
  xSemaphoreTake( mutex, portMAX_DELAY );
  Serial.printf("Running core        : %d\n", xPortGetCoreID() );
  xSemaphoreGive( mutex );

  Serial.println();

  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);

  Serial.print("Connecting to WiFi network ");
  delay(500);

  WiFi.begin(MY_WIFI_SSID, MY_WIFI_PASSWORD);
  WiFi.setAutoConnect(true);
  WiFi.setAutoReconnect(true);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println(" connected.\n");

  WiFi.macAddress(mac);
  Serial.printf("WiFi MAC Address    : %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5] );
  Serial.printf("WiFi Hostname       : %s\n", WiFi.getHostname());
  Serial.print( "WiFi IP-Address     : " );
  Serial.println( WiFi.localIP() );

  Serial.println();
  Serial.printf("SDK Version         : %s\n", ESP.getSdkVersion() );
  Serial.println();
  counter = 0;

  configTzTime( TIMEZONE, NTP_SERVER );

  if (getLocalTime(&timeinfo, 10000))    // wait up to 10sec to sync
  {
    Serial.println(&timeinfo, "Time set            : %A %d %B %Y %H:%M:%S");
    Serial.print( "Timezone            : ");
    Serial.println( getenv("TZ"));
  }
  else
  {
    Serial.println("Time not set");
  }

  getLocalTime(&timeinfo);
  Serial.println(&timeinfo, "Time                : %Y-%m-%d %H:%M:%S");
  Serial.println();
}

void loop()
{
  Serial.print("\rRunning: ");
  Serial.print( rollingChars[counter]);
  counter++;
  counter %=4 ;
  delay(1000);
}
