#include <Arduino.h>
#include <LiquidCrystal.h>
#include <string.h>
#include "settings.h"
LiquidCrystal lcd(14,27,26,25,33,32);

void lcd_display_setup(int* lcd_pings){
    lcd.begin(16,2);
    lcd.clear();
} 
void lcd_show_message(char *text){
    size_t len = strlen(text);
    while (len <= 16) {
        lcd.print(text);
        if (len > 16){
            lcd.autoscroll();
        }
    }
}
void lcd_scroll(uint8_t code){
    if (code == ScrollCode::LEFT) {
        lcd.scrollDisplayLeft();
        lcd.clear();
    }
    else if (code == ScrollCode::RIGHT) {
        lcd.scrollDisplayRight();
        lcd.clear();
    }
    else if (code == ScrollCode::UP) {
        lcd.clear();
        lcd.setCursor(0,0);
    }
    else if (code == ScrollCode::DOWN) {
        lcd.clear();
        lcd.setCursor(0,1);
    }
    else {
        lcd.clear();
        lcd.print("Wrong Code you put there nigga\n");
        delay(1000);
        lcd.clear();
    }
}