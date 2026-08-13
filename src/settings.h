#ifndef SETTINGS_H
#define SETTINGS_H
//code here 

// define the wifi
#define WIFI_SSID "Kaung Myat Thu"//"Chessy Guy"
#define WIFI_PASSWORD "441796487"//"waiyeanhein0098765@#!$"
// Telegram BOT Token (Get from Botfather)
#define BOT_TOKEN "8890339447:AAGxJhTSK7QdhyPGpT-ehnpjZazrKFSBruc"
#define UNI_SPLASH_TOKEN "RpHOfOma9J0uXIi0v2zDq1kYRFkdfbXVKGxf68ymyeQ"

// declearing constant
const unsigned long BOT_MTBS = 1000; // mean time between scan messages
const char* api_endpoint = "api.unsplash.com"; // api end point url 
unsigned long bot_lasttime; // last time messages' scan has been done
const char* music_api_endpoint = "musicbrainz.org";
int lcd_pings[] = {};

#endif // ending the def 