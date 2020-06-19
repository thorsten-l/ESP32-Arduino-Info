FROM ubuntu:20.04

RUN apt-get update && apt-get install -y --no-install-recommends wget unzip git make \
 srecord bc xz-utils gcc python3 curl python3-pip python3-dev build-essential 

RUN pip3 install -U platformio
RUN pio platform install espressif32@1.12.4 --with-package framework-arduinoespressif32
RUN git clone https://github.com/thorsten-l/ESP32-Arduino-Info.git

WORKDIR /ESP32-Arduino-Info
RUN /usr/local/bin/platformio run -e lolin32lite
COPY ./cmd.sh /

WORKDIR /
RUN rm -fr /ESP32-Arduino-Info

CMD ["/cmd.sh"]
