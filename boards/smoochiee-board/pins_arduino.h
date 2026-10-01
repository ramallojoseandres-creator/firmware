#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include "soc/soc_caps.h"
#include <stdint.h>

/* =====================================================================
 *  RamaSoft / Smoochiee - ESP32-S3-WROOM-1 (N16R8, OPI PSRAM)
 *  2.8" ILI9341 240x320 TFT + XPT2046 resistive touch
 *
 *  Shared SPI bus (display + touch + SD + NRF24 + CC1101):
 *      MOSI 11   SCK 12   MISO 13
 *  Each SPI device keeps its own CS, so every module can stay wired at
 *  the same time; the firmware selects one at a time.
 *
 *  Reserved on this module (never assign): 19/20 (native USB),
 *  26-32 (SPI flash), 33-37 (OPI PSRAM), 43/44 (UART0 debug serial).
 * ===================================================================== */

// ---- Arduino core serial (UART0 / USB-CDC debug) ----
static const uint8_t TX = 43;
static const uint8_t RX = 44;

// ---- Shared SPI bus ----
static const uint8_t SS = 10;
static const uint8_t MOSI = 11;
static const uint8_t MISO = 13;
static const uint8_t SCK = 12;

// ---- System I2C bus (PN532, and any other I2C device) ----
static const uint8_t SDA = 8;
static const uint8_t SCL = 9;
#define GROVE_SDA 8
#define GROVE_SCL 9
#define SYS_I2C_SDA 8
#define SYS_I2C_SCL 9

#define USB_as_HID 1

// ---- No physical navigation buttons: input is via touchscreen ----
#define BTN_ALIAS "\"OK\""
#define SEL_BTN -1
#define UP_BTN -1
#define DW_BTN -1
#define R_BTN -1
#define L_BTN -1
#define BTN_ACT LOW

// ---- Infrared (per wiring guide) ----
#define RXLED 1  // IR receiver  (IR_SIG, GPIO1)
#define TXLED 2  // IR emitter   (IR_OUT, GPIO2)
#define LED_ON HIGH
#define LED_OFF LOW

// ---- Display: 2.8" ILI9341 240x320 ----
#define HAS_SCREEN 1
#define ROTATION 1
#define MINBRIGHT (uint8_t)1

#define USER_SETUP_LOADED 1
#define ILI9341_DRIVER 1
#define TFT_RGB_ORDER 0
#define TFT_WIDTH 240
#define TFT_HEIGHT 320
#define TFT_BACKLIGHT_ON 1
#define TFT_BL 38
#define TFT_RST 5
#define TFT_DC 7
#define TFT_MISO 13
#define TFT_MOSI 11
#define TFT_SCLK 12
#define TFT_CS 10
#define TOUCH_CS 3
#define SMOOTH_FONT 1
#define SPI_FREQUENCY 40000000
#define SPI_READ_FREQUENCY 20000000
#define SPI_TOUCH_FREQUENCY 2500000

// ---- MicroSD (shared SPI bus, CS on GPIO4) ----
#define SDCARD_CS 4
#define SDCARD_SCK 12
#define SDCARD_MISO 13
#define SDCARD_MOSI 11

// ---- Secondary SPI bus used by Bruce for RF modules (shares the TFT bus) ----
#define SPI_SCK_PIN 12
#define SPI_MOSI_PIN 11
#define SPI_MISO_PIN 13
#define SPI_SS_PIN 14

// ---- NRF24L01 (shared SPI bus) ----
#define USE_NRF24_VIA_SPI
#define NRF24_CE_PIN 16
#define NRF24_SS_PIN 14
#define NRF24_MOSI_PIN SPI_MOSI_PIN
#define NRF24_SCK_PIN SPI_SCK_PIN
#define NRF24_MISO_PIN SPI_MISO_PIN

// ---- CC1101 (shared SPI bus) ----
#define USE_CC1101_VIA_SPI
#define CC1101_SS_PIN 15
#define CC1101_GDO0_PIN 17
#define CC1101_GDO2_PIN 18
#define CC1101_MOSI_PIN SPI_MOSI_PIN
#define CC1101_SCK_PIN SPI_SCK_PIN
#define CC1101_MISO_PIN SPI_MISO_PIN

// ---- GPS (NEO-6M, UART) ----
#define GPS_SERIAL_TX 21  // ESP TX -> GPS RX
#define GPS_SERIAL_RX 47  // ESP RX <- GPS TX

// ---- Onboard RGB LED (ESP32-S3-DevKitC-1, WS2812 on GPIO48) ----
#define HAS_RGB_LED 1
#define RGB_LED 48
#define LED_TYPE WS2812B
#define LED_ORDER GRB
#define LED_TYPE_IS_RGBW 0
#define LED_COUNT 1
#define LED_COLOR_STEP 15

#endif /* Pins_Arduino_h */
