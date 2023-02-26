#include <DS3231.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(63, 20,4);

DS3231 rtc(SDA,SCL);

Time t;

void setup() {
  // put your setup code here, to run once:
  lcd.init();
  lcd.begin(20,4);
  lcd.backlight();
  
  
  rtc.begin();
  Serial.begin(9600);

  //Initialize Date & Time
  // rtc.setDOW(SUNDAY);
  // rtc.setDate(26, 2, 2023);
  // rtc.setTime(10, 38, 05); 
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print(rtc.getTimeStr());
  Serial.print(" ");
  Serial.print(rtc.getDOWStr());
  Serial.print(" ");
  Serial.println(rtc.getDateStr());
  Serial.print(" ");

  Serial.print(rtc.getTemp());
  Serial.print("C ");
  delay(500);

  t =rtc.getTime();
  int hour = t.hour;
  int min = t.min;

  Serial.print(hour);
  Serial.print(" ");
  Serial.println(min);
  
  lcd.setCursor(0,0);
  lcd.print(rtc.getDateStr());
  // lcd.setCursor(7,2);
  // lcd.print(rtc.getDOWStr());
  lcd.setCursor(6,3);
  lcd.print(rtc.getTimeStr());
  //delay(1000);
  lcd.setCursor(14,0);
  lcd.print(rtc.getTemp());
  lcd.print("C");


  if((hour == 19) && min == 15){
     //Door Locked
     Serial.print("Time is 7.15");
  }
  else{
     
  }
}
