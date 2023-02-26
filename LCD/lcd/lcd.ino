#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(63, 20,4);

void setup() {
  lcd.init();
  lcd.begin(20,4);
  lcd.backlight();
  lcd.setCursor(3,0);
  lcd.print("Ardiuno");
  delay(1000);
  lcd.setCursor(5,1);
  lcd.print("Programming");

  

}

void loop() {
  // put your main code here, to run repeatedly:

}
