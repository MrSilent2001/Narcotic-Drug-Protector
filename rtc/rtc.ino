#include <DS3231.h>

DS3231 rtc(SDA,SCL);

Time t;

void setup() {
  // put your setup code here, to run once:
  rtc.begin();
  Serial.begin(9600);

  //Initialize Time
  // rtc.setDOW(SUNDAY);
  // rtc.setDate(21, 2, 2023);
  // rtc.setTime(14, 17, 50); 
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
  delay(10000);

  t =rtc.getTime();
  int hour = t.hour;
  int min = t.min;

  Serial.print(hour);
  Serial.print(" ");
  Serial.println(min);

  if((hour == 19) && min == 15){
     //Door Locked
     Serial.print("Time is 7.15");
  }
  else{
     
  }
}
