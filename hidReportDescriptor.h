#ifndef HIDREPORTDESCRIPTOR_H
#define HIDREPORTDESCRIPTOR_H

#include <stdint.h>

// Report IDs:
#define MOUSE_ID 0x00
#define KEYBOARD_ID 0x01
#define MEDIA_KEYS_ID 0x02

#define N_ELEM_KEYBOARD 127
#define N_ELEM_MOUSE 67

extern const uint8_t hidReportDescriptorKeyboard[N_ELEM_KEYBOARD];
extern const uint8_t hidReportDescriptorMouse[N_ELEM_MOUSE];

#endif;
