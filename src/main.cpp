#include <Arduino.h>
#include <App.hpp>
#include <ESP.h>
#include <WiFi.h>
#include "soc/efuse_reg.h"
#include <OtaHandler.hpp>

byte mac[6];
uint64_t chipid;

volatile SemaphoreHandle_t mutex;

static int counter;
static time_t lastTimestamp = 0;
static const char* rollingChars = "|/-\\";

struct tm timeinfo;

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
  Serial.printf("2nd Task core       : %d\n", xPortGetCoreID() );
  xSemaphoreGive( mutex );
  vTaskDelete(NULL);
}

void connectWiFi()
{
  Serial.print("\nConnecting to WiFi network ");

  WiFi.mode(WIFI_STA);
  WiFi.persistent(true);
  WiFi.setHostname( OTA_HOSTNAME );
  WiFi.begin( WIFI_SSID, WIFI_PASS );

  while (WiFi.status() != WL_CONNECTED)
  {
    digitalWrite( BOARD_LED, 1 ^ digitalRead(BOARD_LED));
    delay(500);
    Serial.print(".");
  }

  Serial.println(" connected.\n");
  digitalWrite( BOARD_LED, false );

  WiFi.macAddress(mac);
  Serial.printf("WiFi MAC Address    : %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5] );
  Serial.printf("WiFi Hostname       : %s\n", WiFi.getHostname());
  Serial.print( "WiFi IP-Address     : " );
  Serial.println( WiFi.localIP() );
  Serial.print( "WiFi Gateway-IP     : " );
  Serial.println( WiFi.gatewayIP() );
  Serial.print( "WiFi Subnetmask     : " );
  Serial.println( WiFi.subnetMask() );
  Serial.print( "WiFi DNS Server     : " );
  Serial.println( WiFi.dnsIP() );
  Serial.println();
}

void setup()
{
  pinMode( BOARD_LED, OUTPUT );
  digitalWrite( BOARD_LED, true );
  Serial.begin(115200);
  mutex = xSemaphoreCreateMutex();
  delay( 3000 ); // wait for serial monitor
  Serial.println( "\n\n\n" APP_NAME " - Version " APP_VERSION " by Dr. Thorsten Ludewig" );
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

  WiFi.disconnect();
  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);
  delay(1000);

  connectWiFi();
  InitializeOTA();

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
  time_t currentTimestamp = millis();

  if (( currentTimestamp - lastTimestamp >= 1000 ))
  {
    bool wifiIsConnected = WiFi.isConnected();
    Serial.printf("\r[%d] Running: ", xPortGetCoreID() );
    Serial.print( rollingChars[counter]);
    Serial.printf( " and WiFi is%s connected", wifiIsConnected ? "" : " NOT");
    counter++;
    counter %=4 ;
    lastTimestamp = currentTimestamp;

    if( wifiIsConnected == false )
    {
      connectWiFi();
    }
  }

  ArduinoOTA.handle();
}
