#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <stdint.h>

#define EXTERNAL_NUM_INTERRUPTS 16
#define NUM_DIGITAL_PINS        40
#define NUM_ANALOG_INPUTS       16

#define analogInputToDigitalPin(p)  (((p)<20)?(esp32_adc2gpio[(p)]):-1)
#define digitalPinToInterrupt(p)    (((p)<40)?(p):-1)
#define digitalPinHasPWM(p)         (p < 34)

#define ETH_CLK_MODE ETH_CLOCK_GPIO17_OUT
#define ETH_PHY_POWER 12

#define NRST 5

static const uint8_t KEY_BUILTIN = 0;

static const uint8_t TX = 1;
static const uint8_t RX = 3;

#define TX1 4
#define RX1 36

#define TX2 33
#define RX2 35

static const uint8_t SDA = 32;
static const uint8_t SCL = 16;

static const uint8_t SS    = 13;
static const uint8_t MOSI  = 15;
static const uint8_t MISO  = 2;
static const uint8_t SCK   = 14;

#define BOARD_HAS_1BIT_SDMMC

#endif /* Pins_Arduino_h */
