#include <App.hpp>
#include "NetHandler.hpp"

bool eth_connected = false;

void NetEventHandler(WiFiEvent_t event)
{
  switch (event) {

    case SYSTEM_EVENT_ETH_START:
      Serial.println("\nETH Started");
      //set eth hostname here
      ETH.setHostname(OTA_HOSTNAME);
      break;

    case SYSTEM_EVENT_ETH_CONNECTED:
      Serial.println("\nETH Connected - Link up");
      break;

    case SYSTEM_EVENT_ETH_GOT_IP:
      Serial.print("\nETH MAC: ");
      Serial.print(ETH.macAddress());
      Serial.print(", IPv4: ");
      Serial.print(ETH.localIP());
      if (ETH.fullDuplex()) {
        Serial.print(", FULL_DUPLEX");
      }
      Serial.print(", ");
      Serial.print(ETH.linkSpeed());
      Serial.println("Mbps");

      Serial.print( "Netmask: " );
      Serial.print( ETH.subnetMask() );

      Serial.print( ", Gateway: " );
      Serial.print( ETH.gatewayIP() );

      Serial.println();
      eth_connected = true;
      break;

    case SYSTEM_EVENT_ETH_DISCONNECTED:
      Serial.println("\nETH Disconnected");
      eth_connected = false;
      break;

    case SYSTEM_EVENT_ETH_STOP:
      Serial.println("\nETH Stopped");
      eth_connected = false;
      break;

    case SYSTEM_EVENT_AP_START:
      Serial.println( "\nWiFi AP Started" );
      break;

    default:
      Serial.print("\nNet event : ");
      Serial.println( event );
      break;
  }
}
