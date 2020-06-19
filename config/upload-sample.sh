#!/bin/bash

# sample upload script 

# To protect your OTA authentication password from accidentally 
# pushing it to github, copy this script into the 'private' folder.

# You have to create the 'private' folder in the project root by your own.

# platform.ini
# upload_command = ./private/upload-sample.sh

$HOME/.platformio/penv/bin/python \
  $HOME/.platformio/packages/framework-arduinoespressif32/tools/espota.py \
  --debug --progress -i "<your esp's ip address>" \
  --auth="<your ota password>" -f ".pio/build/<your environment>/firmware.bin"
