#!/bin/bash

git clone https://github.com/thorsten-l/ESP32-Arduino-Info.git
cd /ESP32-Arduino-Info
/usr/local/bin/platformio run \
  -e esp32-poe \
  -e esp32cam \
  -e heltec_wifi_lora_32 \
  -e esp32dev \
  -e lolin32 \
  -e lolin32lite
