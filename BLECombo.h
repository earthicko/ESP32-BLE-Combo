#ifndef ESP32_BLE_COMBO_H
#define ESP32_BLE_COMBO_H

#include "hidReportDescriptor.h"
#include "keyVals.h"
#include "mediaKeyReport.h"
#include <NimBLECharacteristic.h>
#include <NimBLEHIDDevice.h>
#include <Print.h>
#include <sdkconfig.h>

#define BLE_KEYBOARD_VERSION "0.0.4"
#define BLE_KEYBOARD_VERSION_MAJOR 0
#define BLE_KEYBOARD_VERSION_MINOR 0
#define BLE_KEYBOARD_VERSION_REVISION 4

// Mouse
#define MOUSE_LEFT 1
#define MOUSE_RIGHT 2
#define MOUSE_MIDDLE 4
#define MOUSE_BACK 8
#define MOUSE_FORWARD 16
#define MOUSE_ALL (MOUSE_LEFT | MOUSE_RIGHT | MOUSE_MIDDLE)

//  Low level key report: up to 6 keys and shift, ctrl etc at once
typedef struct keyreport_s {
    uint8_t modifiers;
    uint8_t reserved;
    uint8_t keys[6];
} keyreport_t;

class BLECombo : public Print, public NimBLEServerCallbacks, public NimBLECharacteristicCallbacks {
private:
    NimBLEHIDDevice* _hid;
    NimBLEHIDDevice* _hidMouse;
    NimBLECharacteristic* _inputKeyboard;
    NimBLECharacteristic* _inputMouse;
    NimBLECharacteristic* _inputMediaKeys;
    NimBLECharacteristic* _outputKeyboard;
    // NimBLEAdvertising*    _advertising;
    uint8_t _buttons; // mouse buttons
    keyreport_t _keyReport;
    MediaKeyReport _mediaKeyReport;
    std::string _deviceName;
    std::string _deviceManufacturer;
    uint8_t _keyboardLedsStatus;
    uint8_t _batteryLevel;
    bool _connected = false;
    uint32_t _delay_ms = 7;
    // internals
    void delayMillis(uint64_t ms);
    void setButtons(uint8_t b);
    void sendReport(keyreport_t* keys);
    void sendReport(MediaKeyReport* keys);

    uint16_t _vid = 0x05ac;
    uint16_t _pid = 0x820a;
    uint16_t _version = 0x0210;

    uint16_t _vid_mouse = 0xe502;
    uint16_t _pid_mouse = 0xa111;
    uint16_t _version_mouse = 0x0210;

    static const uint8_t _asciimap[128];

public:
    BLECombo(std::string deviceName = "ESP32 Combo", std::string deviceManufacturer = "Espressif", uint8_t batteryLevel = 90);
    // initializers
    void begin(void);
    void end(bool clearAll = false);
    // status getters/setters
    bool isConnected(void);
    void setBatteryLevel(uint8_t level);
    void setName(std::string deviceName);
    void setDelay(uint32_t ms);
    void setVendorId(uint16_t vid);
    void setProductId(uint16_t pid);
    void setVersion(uint16_t version);
    // keyboard
    size_t keyPress(uint8_t k);
    size_t keyPress(const MediaKeyReport k);
    size_t keyRelease(uint8_t k);
    size_t keyRelease(const MediaKeyReport k);
    size_t write(uint8_t c);
    size_t write(const MediaKeyReport c);
    size_t write(const uint8_t* buffer, size_t size);
    void keyReleaseAll(void);
    bool getKeyLedStatus(uint8_t led);
    // mouse
    void mouseClick(uint8_t b = MOUSE_LEFT);
    void mouseMove(signed char x, signed char y, signed char wheel = 0, signed char hWheel = 0);
    void mousePress(uint8_t b = MOUSE_LEFT); // press LEFT by default
    void mouseRelease(uint8_t b = MOUSE_LEFT); // release LEFT by default
    bool isMousePressed(uint8_t b = MOUSE_LEFT); // check LEFT by default

protected:
    virtual void onStarted(NimBLEServer* pServer);
    virtual void onConnect(NimBLEServer* pServer) override;
    virtual void onDisconnect(NimBLEServer* pServer) override;
    virtual void onWrite(NimBLECharacteristic* me) override;
};

#endif // ESP32_BLE_KEYBOARD_H
