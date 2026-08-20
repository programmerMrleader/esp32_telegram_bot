#ifndef SETTINGS_H
#define SETTINGS_H
#pragma once

#include <LiquidCrystal.h>
#include <string>// define the wifi
#define WIFI_SSID "WIFI_SSID"//""
#define WIFI_PASSWORD "WIFI_PASSWORD"//""
// Telegram BOT Token (Get from Botfather)
#define BOT_TOKEN "YOUR_BOT_TOKEN"
#define UNI_SPLASH_TOKEN "YOUR_UNI_SPLASH_TOKEN"

// declearing constant
inline const unsigned long BOT_MTBS = 1000; // mean time between scan messages
inline const char* api_endpoint = "api.unsplash.com"; // api end point url 
inline unsigned long bot_lasttime; // last time messages' scan has been done
inline const char* music_api_endpoint = "musicbrainz.org";

struct MusicQuery {
    String title,artist,year;
};
enum ScrollCode {
    LEFT,
    RIGHT,
    UP,
    DOWN
};
void lcd_display_setup(); 
void lcd_show_message(const char* text);
void lcd_loop_message(const char* text);
void lcd_scroll(uint8_t code);
#endif // ending the def 