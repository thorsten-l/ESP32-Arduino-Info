#ifndef __NET_HANDLER_HPP__
#define __NET_HANDLER_HPP__

#include <Arduino.h>
#include <ETH.h>

extern void NetEventHandler(WiFiEvent_t event);
extern bool eth_connected;

#endif
