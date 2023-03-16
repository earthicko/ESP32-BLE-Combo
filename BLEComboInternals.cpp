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

void BLECombo::sendReport(keyreport_t* keys)
{
    if (!this->isConnected())
        return;
    _inputKeyboard->setValue((uint8_t*)keys, sizeof(keyreport_t));
    _inputKeyboard->notify();
    this->delayMillis(_delay_ms);
}

void BLECombo::sendReport(MediaKeyReport* keys)
{
    if (!this->isConnected())
        return;
    _inputMediaKeys->setValue((uint8_t*)keys, sizeof(MediaKeyReport));
    _inputMediaKeys->notify();
    this->delayMillis(_delay_ms);
}

void BLECombo::setButtons(uint8_t b)
{
    if (b == _buttons)
        return;
    _buttons = b;
    this->mouseMove(0, 0, 0, 0);
}

void BLECombo::delayMillis(uint64_t ms)
{
    uint64_t m = esp_timer_get_time();
    if (ms == 0)
        return;
    uint64_t e = (m + (ms * 1000));
    if (m > e) { // overflow
        while (esp_timer_get_time() > e)
            taskYIELD();
    }
    while (esp_timer_get_time() < e)
        taskYIELD();
}

void BLECombo::onStarted(NimBLEServer* pServer)
{
}

void BLECombo::onConnect(NimBLEServer* pServer)
{
    // connected = pServer->getConnectedCount();
    _connected = true;
    // pServer->getAdvertising()->start(); //do it :D
    // NimBLEDevice::getAddress();
    // NimBLEDevice::getDisconnectedClient();
    // pServer->disconnect(); // need test this
    // auto peer = pServer->getPeerInfo((connected)-1);
    // getConnHandle
    // }

    // ESP_LOGD(LOG_TAG, "Connected to: %d",peer.getAddress());
    // ESP_LOGD(LOG_TAG, "latency %d ms",peer.getConnLatency());
    // if(connected == 0){
    // NimBLEDevice::addIgnored(peer.getAddress());
    // ESP_LOGD(LOG_TAG, "Subscribed to Mouse: %d",inputMouse->getSubscribedCount());
    // inputMouse->setSubscribe(); //add or remove machine that connected
    // }

    // delayMillis(10000);
    // pServer->disconnect(peer.getConnHandle());//CHECK IF NOT WHAT WE NEED ?
}

void BLECombo::onDisconnect(NimBLEServer* pServer)
{
    // connected = pServer->getConnectedCount();
    _connected = false;
}

void BLECombo::onWrite(NimBLECharacteristic* me)
{
    uint8_t* value = (uint8_t*)(me->getValue().c_str());
    _keyboardLedsStatus = *value;
    ESP_LOGI(LOG_TAG, "special keys: %d", *value);
}
