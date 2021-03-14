#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <stdint.h>

#define ETH_CLK_MODE    ETH_CLOCK_GPIO0_IN
#define ETH_POWER_PIN   16
#define ETH_TYPE        ETH_PHY_LAN8720
#define ETH_ADDR        1
#define ETH_MDC_PIN     23
#define ETH_MDIO_PIN    18

static const uint8_t TX = 1;
static const uint8_t RX = 3;

#define TX2 17
#define RX2 5

#endif /* Pins_Arduino_h */
