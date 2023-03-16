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

BLECombo::BLECombo(std::string deviceName, std::string deviceManufacturer, uint8_t batteryLevel)
    : _hid(0)
    , _deviceName(deviceName.substr(0, 15))
    , _deviceManufacturer(deviceManufacturer.substr(0, 15))
    , _batteryLevel(batteryLevel)
{
}

void BLECombo::begin(void)
{
    NimBLEDevice::init(_deviceName);
    NimBLEServer* pServer = NimBLEDevice::createServer(); // return the same server :(
    pServer->setCallbacks(this);

    _hidMouse = new NimBLEHIDDevice(pServer);
    _inputMouse = _hidMouse->inputReport(MOUSE_ID); // <-- input REPORTID from report map

    _hid = new NimBLEHIDDevice(pServer);
    _inputKeyboard = _hid->inputReport(KEYBOARD_ID); // <-- input REPORTID from report map
    _outputKeyboard = _hid->outputReport(KEYBOARD_ID);
    _inputMediaKeys = _hid->inputReport(MEDIA_KEYS_ID);

    _outputKeyboard->setCallbacks(this);

    _hid->manufacturer()->setValue(_deviceManufacturer);
    _hid->pnp(0x02, _vid, _pid, _version);
    _hid->hidInfo(0x00, 0x01);

    _hidMouse->manufacturer()->setValue(_deviceManufacturer);
    _hidMouse->pnp(0x02, _vid_mouse, _pid_mouse, _version_mouse);
    _hidMouse->hidInfo(0x00, 0x02);

    NimBLEDevice::setSecurityAuth(true, true, true);

    _hidMouse->reportMap((uint8_t*)hidReportDescriptorMouse, sizeof(hidReportDescriptorMouse));
    _hid->reportMap((uint8_t*)hidReportDescriptorKeyboard, sizeof(hidReportDescriptorKeyboard));

    // oN ANDROID depend on what start first that will act first
    _hidMouse->startServices();
    _hid->startServices();

    this->onStarted(pServer);

    NimBLEAdvertising* advertising = pServer->getAdvertising();
    advertising->setAppearance(GENERIC_HID);
    advertising->addServiceUUID(_hidMouse->hidService()->getUUID());
    advertising->addServiceUUID(_hid->hidService()->getUUID());
    //   advertising->setScanResponse(true); // this wont work if you use multiple devices
    advertising->setScanResponse(false);
    //   advertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
    //   advertising->setMaxPreferred(0x12);
    advertising->start();

    _hid->setBatteryLevel(_batteryLevel);
    // hid_mouse->setBatteryLevel(batteryLevel);

    ESP_LOGD(LOG_TAG, "Advertising started!");
}

void BLECombo::end(bool clearAll)
{
    NimBLEDevice::stopAdvertising();
    NimBLEServer* pServer = NimBLEDevice::getServer();
    if (pServer->getConnectedCount() > 0) {
        pServer->disconnect(pServer->getPeerIDInfo(0).getConnHandle());
    }
    // delete hid;
    // delete hid_mouse;
    NimBLEDevice::deinit(clearAll);
}
