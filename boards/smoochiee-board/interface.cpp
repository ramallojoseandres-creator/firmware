#include "core/bus_HAL.h"
#include "core/powerSave.h"
#include "core/utils.h"
#include <Arduino.h>
#include <LittleFS.h>
#include <Wire.h>
#include <interface.h>

// ILI9341 + resistive XPT2046 touchscreen on the TFT_eSPI SPI bus (no physical
// nav buttons), so input is handled through tft.getTouch() like the CYD boards.
#define XPT2046_CS TOUCH_CS

// A GPIO left undefined in this codebase resolves to -1, which becomes 255
// when stored in a uint8_t. Guard every pin op so an unused peripheral never
// spams "perimanGetPinBus(): Invalid pin: 255".
static inline bool validPin(int p) { return p >= 0 && p < 254; }

/***************************************************************************************
** Function name: _setup_gpio()
** Location: main.cpp
** Description:   initial setup for the device
***************************************************************************************/
void _setup_gpio() {
    // Keep SPI chip-selects deselected until we explicitly talk to each device
    if (validPin(XPT2046_CS)) {
        pinMode(XPT2046_CS, OUTPUT);
        digitalWrite(XPT2046_CS, HIGH);
    }
    if (validPin(CC1101_SS_PIN)) {
        pinMode(CC1101_SS_PIN, OUTPUT);
        digitalWrite(CC1101_SS_PIN, HIGH);
        bruceConfigPins.rfModule = CC1101_SPI_MODULE;
    }
    if (validPin(NRF24_SS_PIN)) {
        pinMode(NRF24_SS_PIN, OUTPUT);
        digitalWrite(NRF24_SS_PIN, HIGH);
    }
    if (validPin(SDCARD_CS)) {
        pinMode(SDCARD_CS, OUTPUT);
        digitalWrite(SDCARD_CS, HIGH);
    }

    bruceConfigPins.irRx = RXLED;

    // System I2C bus (PN532 and any other I2C peripheral)
    if (validPin(SYS_I2C_SDA) && validPin(SYS_I2C_SCL)) {
        setSysI2CBus(&Wire);
        Wire.begin(SYS_I2C_SDA, SYS_I2C_SCL);
    }
}

/***************************************************************************************
** Function name: _post_setup_gpio()
** Location: main.cpp
** Description:   second stage gpio setup, runs after the display is initialized
***************************************************************************************/
void _post_setup_gpio() {
    // Touch setup for the TFT_eSPI resistive driver.
    // We only APPLY a saved calibration here; we never run the blocking
    // calibrateTouch() at boot, so the device always reaches the menu even
    // when the touchscreen is not wired yet. Once the touch is wired, a
    // one-time calibration pass can be enabled to generate /calData.
    if (validPin(TOUCH_CS)) {
        pinMode(TOUCH_CS, OUTPUT);
        uint16_t calData[5];
        File caldata = LittleFS.open("/calData", "r");
        if (caldata) {
            Serial.print("\ntft Calibration data: ");
            for (int i = 0; i < 5; i++) {
                String line = caldata.readStringUntil('\n');
                calData[i] = line.toInt();
                Serial.printf("%d, ", calData[i]);
            }
            Serial.println();
            caldata.close();
            tft.setTouch(calData);
        } else {
            // No saved calibration yet. Apply a safe default first so getTouch()
            // never divides by zero (which crash-loops the device). Then offer a
            // one-time calibration: if the user HOLDS the screen during this short
            // window we run the real calibration and save it; otherwise we boot
            // straight to the menu with the default. This way a disconnected or
            // badly wired touch never hangs the boot.
            uint16_t defCal[5] = {300, 3600, 300, 3600, 7};
            tft.setTouch(defCal);
            Serial.println("No /calData - default cal; hold screen to calibrate.");

            tft.setRotation(ROTATION);
            tft.fillScreen(TFT_BLACK);
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
            tft.drawCentreString("Hold screen to calibrate", tft.width() / 2, tft.height() / 2, 2);

            uint32_t start = millis();
            int heldCount = 0;
            while (millis() - start < 2500) {
                uint16_t tx, ty;
                if (tft.getTouch(&tx, &ty)) heldCount++;
                else heldCount = 0;
                if (heldCount > 4) { // sustained press -> the touch is really there
                    tft.calibrateTouch(calData, TFT_WHITE, TFT_BLACK, 10);
                    File w = LittleFS.open("/calData", "w");
                    if (w) {
                        w.printf(
                            "%d\n%d\n%d\n%d\n%d\n",
                            calData[0], calData[1], calData[2], calData[3], calData[4]
                        );
                        w.close();
                    }
                    tft.setTouch(calData);
                    Serial.println("Touch calibrated and saved to /calData.");
                    break;
                }
                delay(50);
            }
        }
    }

    // Make sure the backlight is on after the display is up
    if (validPin(TFT_BL)) {
        pinMode(TFT_BL, OUTPUT);
        analogWrite(TFT_BL, 255);
    }

    // This ILI9341 panel renders normal colors WITHOUT inversion. Force it off
    // so a stale/inverted config value doesn't leave the whole UI white.
    bruceConfig.colorInverted = 0;
    tft.invertDisplay(0);
}

/***************************************************************************************
** Battery: this board runs the LiPo straight to 3V3 (no fuel gauge / PMU),
** so there is nothing to measure.
***************************************************************************************/
bool isCharging() { return false; }

int getBattery() { return 0; }

/*********************************************************************
** Function: setBrightness
** location: settings.cpp
** set brightness value
**********************************************************************/
void _setBrightness(uint8_t brightval) {
    if (!validPin(TFT_BL)) return;
    if (brightval == 0) {
        analogWrite(TFT_BL, brightval);
    } else {
        int bl = MINBRIGHT + round(((255 - MINBRIGHT) * brightval / 100));
        analogWrite(TFT_BL, bl);
    }
}

/*********************************************************************
** Function: InputHandler
** Handles the variables PrevPress, NextPress, SelPress, AnyKeyPress and EscPress
**********************************************************************/
void InputHandler(void) {
    return; // TEMP diagnostic: disable touch reads to test if they corrupt the display
    static long d_tmp = 0;
    if (millis() - d_tmp > 200 || LongPress) {
        TouchPoint t;
        checkPowerSaveTime();
        bool _IH_touched = tft.getTouch(&t.x, &t.y);
        if (_IH_touched) {
            NextPress = false;
            PrevPress = false;
            UpPress = false;
            DownPress = false;
            SelPress = false;
            EscPress = false;
            AnyKeyPress = false;
            NextPagePress = false;
            PrevPagePress = false;
            touchPoint.pressed = false;
            _IH_touched = false;

            if (bruceConfigPins.rotation == 3) {
                t.y = (tftHeight + 20) - t.y;
                t.x = tftWidth - t.x;
            }
            if (bruceConfigPins.rotation == 0) {
                int tmp = t.x;
                t.x = tftWidth - t.y;
                t.y = tmp;
            }
            if (bruceConfigPins.rotation == 2) {
                int tmp = t.x;
                t.x = t.y;
                t.y = (tftHeight + 20) - tmp;
            }

            if (!wakeUpScreen()) AnyKeyPress = true;
            else goto END;

            // Touch point global variable
            touchPoint.x = t.x;
            touchPoint.y = t.y;
            touchPoint.pressed = true;
            touchHeatMap(touchPoint);
        END:
            d_tmp = millis();
        }
    }
}

/*********************************************************************
** Function: powerOff
** location: mykeyboard.cpp
** Turns off the device (or try to)
**********************************************************************/
void powerOff() {}

/*********************************************************************
** Function: checkReboot
** location: mykeyboard.cpp
** Btn logic to turn off the device (name is odd btw)
**********************************************************************/
void checkReboot() {}
