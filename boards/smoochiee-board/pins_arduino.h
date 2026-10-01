#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include "soc/soc_caps.h"
#include <stdint.h>

static const uint8_t TX = 1;
static const uint8_t RX = 2;

static const uint8_t SDA = 47;
static const uint8_t SCL = 48;

static const uint8_t SS = 3;
static const uint8_t MOSI = 17;
static const uint8_t MISO = 8;
static const uint8_t SCK = 18;

#define SERIAL_RX 2
#define SERIAL_TX 1
#define GPS_SERIAL_TX SERIAL_TX
#define GPS_SERIAL_RX SERIAL_RX
#define USB_as_HID 1

// Touch-only board: no physical navigation buttons (handled via XPT2046 touch)
#define BTN_ALIAS "\"OK\""
#define SEL_BTN -1
#define UP_BTN -1
#define DW_BTN -1
#define R_BTN -1
#define L_BTN -1
#define BTN_ACT LOW

#define RXLED 4
#define TXLED 5
#define LED_ON HIGH
#define LED_OFF LOW

#define HAS_SCREEN 1
#define ROTATION 1
#define MINBRIGHT (uint8_t)1

#define USER_SETUP_LOADED 1
#define ILI9341_DRIVER 1
#define TFT_RGB_ORDER 0
#define TFT_WIDTH 240
#define TFT_HEIGHT 320
#define TFT_BACKLIGHT_ON 1
#define TFT_BL 16
#define TFT_RST -1
#define TFT_DC 15
#define TFT_MISO 8
#define TFT_MOSI 17
#define TFT_SCLK 18
#define TFT_CS 7
// NOTE: GPIO5 is also TXLED (IR TX). Confirm the real XPT2046 T_CS pin on your
// schematic; if touch does not respond, this is the first value to change.
#define TOUCH_CS 5
#define SMOOTH_FONT 1
#define SPI_FREQUENCY 20000000
#define SPI_READ_FREQUENCY 20000000
#define SPI_TOUCH_FREQUENCY 2500000

#define SDCARD_CS -1
#define SDCARD_SCK -1
#define SDCARD_MISO -1
#define SDCARD_MOSI -1

#define GROVE_SDA 47
#define GROVE_SCL 48
#define SYS_I2C_SDA 47
#define SYS_I2C_SCL 48

#define SPI_SCK_PIN 13
#define SPI_MOSI_PIN 12
#define SPI_MISO_PIN 11
#define SPI_SS_PIN 43

#define HAS_RGB_LED 1
#define RGB_LED 45
#define LED_TYPE WS2812B
#define LED_ORDER GRB
#define LED_TYPE_IS_RGBW 0
#define LED_COUNT 16
#define LED_COLOR_STEP 15

#define XPOWERS_CHIP_BQ25896
#define USE_BOOST

#define PIN_CLK 1
#define PIN_DATA 10
#define PIN_WS 2

#endif /* Pins_Arduino_h */
