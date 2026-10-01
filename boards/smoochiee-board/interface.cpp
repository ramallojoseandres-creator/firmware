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
            // No saved calibration: apply a safe default so getTouch() never
            // divides by zero (which crashes/boot-loops the device). Rough
            // mapping for a 240x320 ILI9341 + XPT2046; refine with a real
            // calibration pass once the touchscreen is wired.
            uint16_t defCal[5] = {300, 3600, 300, 3600, 7};
            tft.setTouch(defCal);
            Serial.println("No /calData - using default touch calibration.");
        }
    }

    // Make sure the backlight is on after the display is up
    if (validPin(TFT_BL)) {
        pinMode(TFT_BL, OUTPUT);
        analogWrite(TFT_BL, 255);
    }
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
