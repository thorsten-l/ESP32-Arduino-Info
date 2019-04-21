#!/bin/bash

git clone https://github.com/thorsten-l/ESP32-Arduino-Info.git
cd /ESP32-Arduino-Info
cp lib/App/App.hpp.sample lib/App/App.hpp
/usr/local/bin/platformio run -e lolin32 -e lolin32lite -e esp32cam
