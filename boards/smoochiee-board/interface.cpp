#include "core/bus_HAL.h"
#include "core/powerSave.h"
#include "core/utils.h"
#include <Arduino.h>
#include <LittleFS.h>
#include <interface.h>

// This board drives an ILI9341 + resistive XPT2046 touchscreen over the
// TFT_eSPI SPI bus (no physical nav buttons), so input is handled through
// tft.getTouch() exactly like the CYD-2432S028 variant.
#define XPT2046_CS TOUCH_CS

// A GPIO left undefined in this codebase resolves to -1, which becomes 255
// when stored in a uint8_t. Guard every pin op so an unused peripheral never
// spams "perimanGetPinBus(): Invalid pin: 255".
static inline bool validPin(int p) { return p >= 0 && p < 254; }

// Power handler for battery detection
#ifdef XPOWERS_CHIP_BQ25896
#include <Wire.h>
#include <XPowersLib.h>
XPowersPPM PPM;
#endif

/***************************************************************************************
** Function name: _setup_gpio()
** Location: main.cpp
** Description:   initial setup for the device
***************************************************************************************/
void _setup_gpio() {
    // Keep the touch controller deselected until we explicitly talk to it
    if (validPin(XPT2046_CS)) {
        pinMode(XPT2046_CS, OUTPUT);
        digitalWrite(XPT2046_CS, HIGH);
    }

    // Block CC1101 / NRF24 at boot only if the board actually wires them
    if (validPin(CC1101_SS_PIN)) {
        pinMode(CC1101_SS_PIN, OUTPUT);
        digitalWrite(CC1101_SS_PIN, HIGH);
        bruceConfigPins.rfModule = CC1101_SPI_MODULE;
    }
    if (validPin(NRF24_SS_PIN)) {
        pinMode(NRF24_SS_PIN, OUTPUT);
        digitalWrite(NRF24_SS_PIN, HIGH);
    }

    bruceConfigPins.irRx = RXLED;

    setSysI2CBus(&Wire); // PMU lives on the default Wire object
    Wire.setPins(SYS_I2C_SDA, SYS_I2C_SCL);
    bool pmu_ret = false;
    Wire.begin(SYS_I2C_SDA, SYS_I2C_SCL);
    pmu_ret = PPM.init(Wire, SYS_I2C_SDA, SYS_I2C_SCL, BQ25896_SLAVE_ADDRESS);
    if (pmu_ret) {
        PPM.setSysPowerDownVoltage(3300);
        PPM.setInputCurrentLimit(3250);
        Serial.printf("getInputCurrentLimit: %d mA\n", PPM.getInputCurrentLimit());
        PPM.disableCurrentLimitPin();
        PPM.setChargeTargetVoltage(4208);
        PPM.setPrechargeCurr(64);
        PPM.setChargerConstantCurr(832);
        PPM.getChargerConstantCurr();
        Serial.printf("getChargerConstantCurr: %d mA\n", PPM.getChargerConstantCurr());
        PPM.enableMeasure(PowersBQ25896::CONTINUOUS);
        PPM.disableOTG();
        PPM.enableCharge();
    }
}

/***************************************************************************************
** Function name: _post_setup_gpio()
** Location: main.cpp
** Description:   second stage gpio setup, runs after the display is initialized
***************************************************************************************/
void _post_setup_gpio() {
    // Touch calibration for the TFT_eSPI resistive driver
    if (validPin(TOUCH_CS)) {
        pinMode(TOUCH_CS, OUTPUT);
        uint16_t calData[5];
        File caldata = LittleFS.open("/calData", "r");

        if (!caldata) {
            tft.setRotation(ROTATION);
            tft.calibrateTouch(calData, TFT_WHITE, TFT_BLACK, 10);

            caldata = LittleFS.open("/calData", "w");
            if (caldata) {
                caldata.printf(
                    "%d\n%d\n%d\n%d\n%d\n", calData[0], calData[1], calData[2], calData[3], calData[4]
                );
                caldata.close();
            }
        } else {
            Serial.print("\ntft Calibration data: ");
            for (int i = 0; i < 5; i++) {
                String line = caldata.readStringUntil('\n');
                calData[i] = line.toInt();
                Serial.printf("%d, ", calData[i]);
            }
            Serial.println();
            caldata.close();
        }
        tft.setTouch(calData);
    }

    // Make sure the backlight is on after the display is up
    if (validPin(TFT_BL)) {
        pinMode(TFT_BL, OUTPUT);
        analogWrite(TFT_BL, 255);
    }
}

bool isCharging() { return PPM.isCharging(); }

int getBattery() {
    int voltage = PPM.getBattVoltage();
    int percent = (voltage - 3300) * 100 / (float)(4150 - 3350);

    if (percent < 0) return 1;
    if (percent > 100) percent = 100;

    if (PPM.isCharging() && percent >= 97) {
        PPM.disableBatLoad();
        percent = 95; // estimate still charging
    }

    if (PPM.isChargeDone()) { percent = 100; }

    return percent;
}

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
