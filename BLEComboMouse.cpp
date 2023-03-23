#include "BLECombo.h"
#include <HIDTypes.h>
#include <NimBLEDevice.h>
#include <NimBLEHIDDevice.h>
#include <NimBLEServer.h>
#include <NimBLEUtils.h>
#include <driver/adc.h>
#include <sdkconfig.h>

#if defined(CONFIG_ARDUHAL_ESP_LOG)
#include <esp32-hal-log.h>
#define LOG_TAG ""
#else
#include <esp_log.h>
static const char* LOG_TAG = "BLEDevice";
#endif

void BLECombo::mouseNotify(uint8_t len, uint8_t* buf)
{
    _inputMouse->setValue(buf, len);
    _inputMouse->notify();
}

void BLECombo::mouseClick(uint8_t b)
{
    _buttons = b;
    this->mouseMove(0, 0, 0, 0);
    this->delayMillis(_delay_ms);
    _buttons = 0;
    this->mouseMove(0, 0, 0, 0);
}

void BLECombo::mouseMove(signed char x, signed char y, signed char wheel, signed char hWheel)
{
    if (!this->isConnected())
        return;
    uint8_t m[5];
    m[0] = _buttons;
    m[1] = x;
    m[2] = y;
    m[3] = wheel;
    m[4] = hWheel;
    _inputMouse->setValue((uint8_t*)m, sizeof(m));
    _inputMouse->notify();
    this->delayMillis(_delay_ms);
}

void BLECombo::mousePress(uint8_t b)
{
    this->setButtons(_buttons | b);
}

void BLECombo::mouseRelease(uint8_t b)
{
    this->setButtons(_buttons & ~b);
}

bool BLECombo::isMousePressed(uint8_t b)
{
    if ((b & _buttons) > 0)
        return true;
    return false;
}
