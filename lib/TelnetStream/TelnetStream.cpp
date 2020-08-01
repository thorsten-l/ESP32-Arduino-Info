#include "App.hpp"
#include "TelnetStream.h"

extern const char *getJsonStatus(WiFiClient *client);

void TelnetStreamClass::printHelp()
{
  TelnetStream.println( "\n\nHELP page\n"
    "h : this help\n"
    "b : print banner\n"
    "s : status\n"
    "j : json status\n"
    "q,e : quit / exit \n"
    ); 
}

void TelnetStreamClass::printBanner()
{
  appShowHeader(TelnetStream);
  TelnetStream.printf( "Uptime : %s\n", appUptime() );
  TelnetStream.printf( "Client IP-Address = %s\n\n", 
    client.remoteIP().toString().c_str());
}

TelnetStreamClass::TelnetStreamClass(uint16_t port) :server(port) {
}

void TelnetStreamClass::begin() {
  isConnected = false;
  server.begin();
  client = server.available();
}

void TelnetStreamClass::handle() 
{
  TelnetStream.available();
  
  if (client && client.connected()) 
  {  
    if ( isConnected == false )
    {
      isConnected = true;
      printBanner();
    }

    if ( TelnetStream.available() )
    {
      int c = TelnetStream.read();

      switch( c )
      {
        case 'h':
        case 'H':
          printHelp();
          break;

        case 'b':
        case 'B':
          printBanner();
          break;

        case 'j':
        case 'J':
          // TelnetStream.println(getJsonStatus(&client));
          break;

        case 's':
        case 'S':
          {
            TelnetStream.printf( "\nTime      : %s\n", appDateTime() );
            TelnetStream.printf( "Uptime    : %s\n", appUptime() );
            TelnetStream.printf( "Free Heap : %u\n", ESP.getFreeHeap() );
            TelnetStream.println();
          }
          break;

        case 'q':
        case 'Q':
        case 'e':
        case 'E':
          isConnected = false;
          stop();
          break;
      }
    }
  }
}

void TelnetStreamClass::stop() {
  client.stop();
}

boolean TelnetStreamClass::disconnected() {
#ifdef ESP32
  if (!server)
    return true;
#else
  if (server.status() == CLOSED)
    return true;
#endif
  if (!client) {
    client = server.available();
  }
  if (client) {
    if (client.connected())
      return false;
    client.stop();
    client = server.available();
  }
  isConnected = false;
  return true;
}

int TelnetStreamClass::read() {
  if (disconnected())
    return -1;
  return client.read();
}

int TelnetStreamClass::available() {
  if (disconnected())
    return 0;
  return client.available();
}

int TelnetStreamClass::peek() {
  if (disconnected())
    return -1;
  return client.peek();
}

size_t TelnetStreamClass::write(uint8_t val) {
  if (disconnected())
    return 1;
  return client.write(val);
}

void TelnetStreamClass::flush() {
  if (disconnected())
    return;
  client.flush();
}

TelnetStreamClass TelnetStream(23);
