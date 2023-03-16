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

/*
 * keyPress() adds the specified key (printing, non-printing, or modifier)
 * to the persistent key report and sends the report.  Because of the way
 * USB HID works, the host acts like the key remains pressed until we
 * call keyRelease(), keyReleaseAll(), or otherwise clear the report and resend.
 */
size_t BLECombo::keyPress(uint8_t k)
{
    if (k >= 136) { // it's a non-printing key (not a modifier)
        k = k - 136;
    } else if (k >= 128) { // it's a modifier key
        _keyReport.modifiers |= (1 << (k - 128));
        k = 0;
    } else { // it's a printing key
        k = pgm_read_byte(_asciimap + k);
        if (!k) {
            setWriteError();
            return 0;
        }
        if (k & 0x80) { // it's a capital letter or other character reached with shift
            _keyReport.modifiers |= 0x02; // the left shift modifier
            k &= 0x7F;
        }
    }

    // Add k to the key report only if it's not already present
    // and if there is an empty slot.
    uint8_t i;
    int has_k = 0;
    for (i = 0; i < 6; i++) {
        if (_keyReport.keys[i] == k) {
            has_k = 1;
            break;
        }
    }
    if (!has_k) {
        for (i = 0; i < 6; i++) {
            if (_keyReport.keys[i] == 0x00) {
                _keyReport.keys[i] = k;
                break;
            }
        }
        if (i == 6) {
            setWriteError();
            return 0;
        }
    }
    this->sendReport(&_keyReport);
    return 1;
}

size_t BLECombo::keyPress(const MediaKeyReport k)
{
    uint16_t k_16 = k[1] | (k[0] << 8);
    uint16_t mediaKeyReport_16 = _mediaKeyReport[1] | (_mediaKeyReport[0] << 8);

    mediaKeyReport_16 |= k_16;
    _mediaKeyReport[0] = (uint8_t)((mediaKeyReport_16 & 0xFF00) >> 8);
    _mediaKeyReport[1] = (uint8_t)(mediaKeyReport_16 & 0x00FF);

    this->sendReport(&_mediaKeyReport);
    return 1;
}

// keyRelease() takes the specified key out of the persistent key report and
// sends the report.  This tells the OS the key is no longer pressed and that
// it shouldn't be repeated any more.
size_t BLECombo::keyRelease(uint8_t k)
{
    uint8_t i;
    if (k >= 136) { // it's a non-printing key (not a modifier)
        k = k - 136;
    } else if (k >= 128) { // it's a modifier key
        _keyReport.modifiers &= ~(1 << (k - 128));
        k = 0;
    } else { // it's a printing key
        k = pgm_read_byte(_asciimap + k);
        if (!k) {
            return 0;
        }
        if (k & 0x80) { // it's a capital letter or other character reached with shift
            _keyReport.modifiers &= ~(0x02); // the left shift modifier
            k &= 0x7F;
        }
    }

    // Test the key report to see if k is present.  Clear it if it exists.
    // Check all positions in case the key is present more than once (which it shouldn't be)
    for (i = 0; i < 6; i++) {
        if (0 != k && _keyReport.keys[i] == k) {
            _keyReport.keys[i] = 0x00;
        }
    }

    this->sendReport(&_keyReport);
    return 1;
}

size_t BLECombo::keyRelease(const MediaKeyReport k)
{
    uint16_t k_16 = k[1] | (k[0] << 8);
    uint16_t mediaKeyReport_16 = _mediaKeyReport[1] | (_mediaKeyReport[0] << 8);
    mediaKeyReport_16 &= ~k_16;
    _mediaKeyReport[0] = (uint8_t)((mediaKeyReport_16 & 0xFF00) >> 8);
    _mediaKeyReport[1] = (uint8_t)(mediaKeyReport_16 & 0x00FF);

    this->sendReport(&_mediaKeyReport);
    return 1;
}

void BLECombo::keyReleaseAll(void)
{
    _keyReport.keys[0] = 0;
    _keyReport.keys[1] = 0;
    _keyReport.keys[2] = 0;
    _keyReport.keys[3] = 0;
    _keyReport.keys[4] = 0;
    _keyReport.keys[5] = 0;
    _keyReport.modifiers = 0;
    _mediaKeyReport[0] = 0;
    _mediaKeyReport[1] = 0;
    this->sendReport(&_keyReport);
}

bool BLECombo::getKeyLedStatus(uint8_t led)
{
    return (_keyboardLedsStatus & led) != 0;
}

size_t BLECombo::write(uint8_t c)
{
    uint8_t p = keyPress(c); // Keydown
    keyRelease(c); // Keyup
    return p; // just return the result of keyPress() since keyRelease() almost always returns 1
}

size_t BLECombo::write(const MediaKeyReport c)
{
    uint16_t p = keyPress(c); // Keydown
    keyRelease(c); // Keyup
    return p; // just return the result of keyPress() since keyRelease() almost always returns 1
}

size_t BLECombo::write(const uint8_t* buffer, size_t size)
{
    size_t n = 0;
    while (size--) {
        if (*buffer != '\r') {
            if (write(*buffer)) {
                n++;
            } else {
                break;
            }
        }
        buffer++;
    }
    return n;
}
