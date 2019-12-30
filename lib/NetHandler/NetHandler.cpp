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

      Serial.printf("\n\nETH Hostname        : %s\n", ETH.getHostname());
      Serial.print( "ETH MAC-Address     : "); Serial.println(ETH.macAddress());
      Serial.print( "ETH IP-Address      : " ); 
      Serial.println(ETH.localIP());

      Serial.print( "ETH Phy             : " ); 
      Serial.print(( ETH.fullDuplex()) ? "FULL" : "HALF");
      Serial.print(" DUPLEX, ");
      Serial.print(ETH.linkSpeed());

      Serial.println("Mbps");
      Serial.print( "ETH Netmask         : " );
      Serial.println( ETH.subnetMask() );
      Serial.print( "ETH Gateway         : " );
      Serial.println( ETH.gatewayIP() );
      Serial.print( "ETH DNS Server      : " );
      Serial.println( ETH.dnsIP() );
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
