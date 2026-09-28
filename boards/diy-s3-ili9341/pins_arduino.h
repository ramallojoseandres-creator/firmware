#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include "soc/soc_caps.h"
#include <stdint.h>

// DIY: ESP32-S3 N16R8 + 2.8" ILI9341 SPI + XPT2046 + SD
//
// CABLEADO
//  Pantalla: CS->10 RESET->14 DC->2 SDI(MOSI)->11 SCK->12 LED->21 SDO->sin conectar
//  Tactil:   T_CLK->6 T_CS->9 T_DIN->5 T_DO->4 T_IRQ->sin conectar
//  SD:       SD_CS->8 SD_MOSI->11 SD_MISO->13 SD_SCK->12
//  Modulos externos (NRF24 / CC1101): SCK->40 MOSI->41 MISO->42 CS->39 CE/GDO0->38
//  IR/RF: 47 (TX) y 48 (RX) por defecto; tambien 1 y 18

#ifndef DEVICE_NAME
#define DEVICE_NAME "DIY S3 ILI9341"
#endif

#define USB_VID 0x303a
#define USB_PID 0x1001

static const uint8_t TX = 43;
static const uint8_t RX = 44;

// I2C externo (Grove)
#define GROVE_SDA 16
#define GROVE_SCL 17
static const uint8_t SDA = GROVE_SDA;
static const uint8_t SCL = GROVE_SCL;

// Bus SPI para modulos externos (separado de la pantalla)
#define SPI_SCK_PIN 40
#define SPI_MOSI_PIN 41
#define SPI_MISO_PIN 42
#define SPI_SS_PIN 39
static const uint8_t SS = SPI_SS_PIN;
static const uint8_t MOSI = SPI_MOSI_PIN;
static const uint8_t SCK = SPI_SCK_PIN;
static const uint8_t MISO = SPI_MISO_PIN;

// SD: mismo bus que la pantalla, CS propio
#define SDCARD_CS 8
#define SDCARD_SCK 12
#define SDCARD_MISO 13
#define SDCARD_MOSI 11

#define USE_CC1101_VIA_SPI
#define CC1101_GDO0_PIN 38
#define CC1101_SS_PIN 39
#define CC1101_MOSI_PIN SPI_MOSI_PIN
#define CC1101_SCK_PIN SPI_SCK_PIN
#define CC1101_MISO_PIN SPI_MISO_PIN

#define USE_NRF24_VIA_SPI
#define NRF24_CE_PIN 38
#define NRF24_SS_PIN 39
#define NRF24_MOSI_PIN SPI_MOSI_PIN
#define NRF24_SCK_PIN SPI_SCK_PIN
#define NRF24_MISO_PIN SPI_MISO_PIN

#define USE_W5500_VIA_SPI
#define W5500_SS_PIN -1
#define W5500_MOSI_PIN SPI_MOSI_PIN
#define W5500_SCK_PIN SPI_SCK_PIN
#define W5500_MISO_PIN SPI_MISO_PIN
#define W5500_INT_PIN -1

// Pantalla ILI9341 (misma configuracion que la CYD-2432S028)
#define USER_SETUP_LOADED
#define ILI9341_2_DRIVER 1
#define TFT_WIDTH 240
#define TFT_HEIGHT 320
#define TFT_MISO 13
#define TFT_MOSI 11
#define TFT_SCLK 12
#define TFT_CS 10
#define TFT_DC 2
#define TFT_RST 14
#define TFT_BL 21
#define TFT_BACKLIGHT_ON HIGH
#define SMOOTH_FONT 1
#define TOUCH_CS -1
#define SPI_FREQUENCY 40000000
#define SPI_READ_FREQUENCY 20000000

#define HAS_SCREEN 1
#define ROTATION 3
#define MINBRIGHT 160
#define BACKLIGHT 21

#define FP 1
#define FM 2
#define FG 3

// Boton BOOT
#define HAS_BTN 0
#define BTN_ALIAS "\"Ok\""
#define BTN_PIN 0
#define BTN_ACT LOW

// Sin RGB, audio ni bateria
#define RGB_LED -1
#define PIN_CLK -1
#define I2S_SCLK_PIN -1
#define I2S_DATA_PIN -1
#define PIN_DATA -1
#define BCLK -1
#define WCLK -1
#define DOUT -1
#define FM_RSTPIN -1

// IR
#define TXLED 47
#define RXLED 48
#define LED_ON HIGH
#define LED_OFF LOW
#define IR_TX_PINS '{{"GPIO47", 47}, {"GPIO48", 48}, {"GPIO1", 1}, {"GPIO18", 18}}'
#define IR_RX_PINS '{{"GPIO48", 48}, {"GPIO47", 47}, {"GPIO1", 1}, {"GPIO18", 18}}'

// RF (modulos de un pin)
#define RF_TX_PINS '{{"GPIO47", 47}, {"GPIO48", 48}, {"GPIO1", 1}, {"GPIO18", 18}}'
#define RF_RX_PINS '{{"GPIO48", 48}, {"GPIO47", 47}, {"GPIO1", 1}, {"GPIO18", 18}}'

// GPS
#define SERIAL_TX 43
#define SERIAL_RX 44
#define GPS_SERIAL_TX SERIAL_TX
#define GPS_SERIAL_RX SERIAL_RX

// BadUSB
#define BAD_TX GROVE_SDA
#define BAD_RX GROVE_SCL

#define DEEPSLEEP_WAKEUP_PIN 0
#define DEEPSLEEP_PIN_ACT LOW

#endif
