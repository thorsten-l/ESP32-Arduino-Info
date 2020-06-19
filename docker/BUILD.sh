#!/bin/sh
rm -f firmware.*
docker build -t pio-esp32-adruino-info .
# docker build --no-cache -t pio-esp32-adruino-info .
docker run --name pio-esp32-adruino-info pio-esp32-adruino-info
docker cp pio-esp32-adruino-info:/ESP32-Arduino-Info/.pio/build/lolin32/firmware.bin firmware.lolin32
docker cp pio-esp32-adruino-info:/ESP32-Arduino-Info/.pio/build/lolin32lite/firmware.bin firmware.lolin32lite
docker cp pio-esp32-adruino-info:/ESP32-Arduino-Info/.pio/build/esp32cam/firmware.bin firmware.esp32cam
docker cp pio-esp32-adruino-info:/ESP32-Arduino-Info/.pio/build/esp32-poe/firmware.bin firmware.esp32-poe
docker cp pio-esp32-adruino-info:/ESP32-Arduino-Info/.pio/build/heltec_wifi_lora_32/firmware.bin firmware.heltec_wifi_lora_32
docker cp pio-esp32-adruino-info:/ESP32-Arduino-Info/.pio/build/esp32dev/firmware.bin firmware.esp32dev
docker rm pio-esp32-adruino-info
