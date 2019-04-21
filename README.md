# ESP32-Arduino-Info

Prints out chip, sdk and wifi information using the *Arduino* framework.

## Compile requirements

- rename or copy lib/App/App.hpp.sample to lib/App/App.hpp
  and change &lt;your wifi ssid>, &lt;your wifi password>
  and &lt;your ntp server>

## Sample Output of an AI-Thinker ESP32-CAM module

```t
ets Jun  8 2016 00:22:57

rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:2
load:0x3fff0018,len:4
load:0x3fff001c,len:1100
load:0x40078000,len:9232
load:0x40080400,len:6412
entry 0x400806a8



ESP32 Chip Info - Arduino - Version 1.3.1 by Dr. Thorsten Ludewig
Build date: Apr 21 2019 15:07:39

Chip Revision (ESP) : 1
Chip Revision (REG) : 1

CPU Frequency       : 240MHz

Flash Chip IDE Size : 4.00MB
Flash Chip Speed    : 40.00MHz

Heap Size           : 316.03KB
Free Heap           : 290.53KB
Sketch Size         : 871.28KB
Free Sketch Space   : 3072.00KB
PSRAM Size          : 4095.95KB
Free PSRAM          : 4095.95KB

SD Card Type        : SDHC
SD Card Size        : 15223MB
SD Card Total space : 15215MB
SD Card Used space  : 2MB

Cycle Count         : 1199861274

2nd Task core       : 0
Running core        : 1

Connecting to WiFi network .. connected.

WiFi MAC Address    : CC:50:E3:XX:XX:XX
WiFi Hostname       : esp32-arduino-info
WiFi IP-Address     : 192.168.XXX.XXX
WiFi Gateway-IP     : 192.168.XXX.1
WiFi Subnetmask     : 255.255.255.0
WiFi DNS Server     : 192.168.XXX.XXX

Update Over-The-Air : initialized

SDK Version         : v3.2-18-g977854975

Time set            : Sunday 21 April 2019 15:08:23
Timezone            : CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00
Time                : 2019-04-21 15:08:23

[1] Running: / and WiFi is connected
```
