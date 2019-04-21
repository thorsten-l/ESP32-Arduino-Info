#!/bin/sh
rm -f firmware.*
docker build -t pio-esp32-adruino-info .
docker run --name pio-esp32-adruino-info pio-esp32-adruino-info
docker cp pio-esp32-adruino-info:/ESP32-Arduino-Info/.pioenvs/lolin32/firmware.bin firmware.lolin32
docker cp pio-esp32-adruino-info:/ESP32-Arduino-Info/.pioenvs/lolin32lite/firmware.bin firmware.lolin32lite
docker cp pio-esp32-adruino-info:/ESP32-Arduino-Info/.pioenvs/esp32cam/firmware.bin firmware.esp32cam
docker rm pio-esp32-adruino-info
