#include <Arduino.h>
#include <LiquidCrystal.h>
#include <string.h>
#include "settings.h"
                //RS E  D4 D5 D6 D7

LiquidCrystal lcd( 27, 26, 25, 33, 32,4); // RS, E, D4, D5, D6, D7

void lcd_display_setup(){
    lcd.begin(16,2);
    lcd.clear();
    lcd.setCursor(0, 0);
}

void lcd_show_message(const char *text){
    lcd.clear();
    lcd.setCursor(0, 0);

    String message = String(text);
    if (message.length() > 32) {
        message = message.substring(0, 32);
    }

    if (message.length() <= 16) {
        lcd.print(message);
        return;
    }

    lcd.print(message.substring(0, 16));
    lcd.setCursor(0, 1);
    lcd.print(message.substring(16));
}
void lcd_loop_message(const char *text) {
    lcd.print(text);
}
void lcd_scroll(uint8_t code){
    if (code == ScrollCode::LEFT) {
        lcd.scrollDisplayLeft();
    }
    else if (code == ScrollCode::RIGHT) {
        lcd.scrollDisplayRight();
    }
    else if (code == ScrollCode::UP) {
        lcd.clear();
        lcd.setCursor(0, 0);
    }
    else if (code == ScrollCode::DOWN) {
        lcd.clear();
        lcd.setCursor(0, 1);
    }
    else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Wrong code");
        delay(1000);
        lcd.clear();
    }
}