#ifndef MEDIAKEYREPORT_H
#define MEDIAKEYREPORT_H

#include <stdint.h>

typedef uint8_t MediaKeyReport[2];

extern const MediaKeyReport KEY_MEDIA_NEXT_TRACK;
extern const MediaKeyReport KEY_MEDIA_PREVIOUS_TRACK;
extern const MediaKeyReport KEY_MEDIA_STOP;
extern const MediaKeyReport KEY_MEDIA_PLAY_PAUSE;
extern const MediaKeyReport KEY_MEDIA_MUTE;
extern const MediaKeyReport KEY_MEDIA_VOLUME_UP;
extern const MediaKeyReport KEY_MEDIA_VOLUME_DOWN;
extern const MediaKeyReport KEY_MEDIA_WWW_HOME;
extern const MediaKeyReport KEY_MEDIA_LOCAL_MACHINE_BROWSER;
extern const MediaKeyReport KEY_MEDIA_CALCULATOR;
extern const MediaKeyReport KEY_MEDIA_WWW_BOOKMARKS;
extern const MediaKeyReport KEY_MEDIA_WWW_SEARCH;
extern const MediaKeyReport KEY_MEDIA_WWW_STOP;
extern const MediaKeyReport KEY_MEDIA_WWW_BACK;
extern const MediaKeyReport KEY_MEDIA_CONSUMER_CONTROL_CONFIGURATION;
extern const MediaKeyReport KEY_MEDIA_EMAIL_READER;

#endif
