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

bool BLECombo::isConnected(void)
{
    return _connected;
}

void BLECombo::setBatteryLevel(uint8_t level)
{
    _batteryLevel = level;
    if (_hid)
        _hid->setBatteryLevel(_batteryLevel);
    if (_hidMouse)
        _hidMouse->setBatteryLevel(_batteryLevel);
}

// must be called before begin in order to set the name
void BLECombo::setName(std::string deviceName)
{
    _deviceName = deviceName;
}

/**
 * @brief Sets the waiting time (in milliseconds) between multiple keystrokes in NimBLE mode.
 *
 * @param ms Time in milliseconds
 */
void BLECombo::setDelay(uint32_t ms)
{
    _delay_ms = ms;
}

void BLECombo::setVendorId(uint16_t vid)
{
    _vid = vid;
}

void BLECombo::setProductId(uint16_t pid)
{
    _pid = pid;
}

void BLECombo::setVersion(uint16_t version)
{
    _version = version;
}
