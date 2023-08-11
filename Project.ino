#include <DS3231.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <dht.h>
#include <Arduino.h>

//ultrasonic
#define trig 9
#define echo 10

//LCD
LiquidCrystal_I2C lcd(0x27, 20, 4);

//rtc
DS3231 rtc(SDA, SCL);

//fingerprint
SoftwareSerial mySerial(2, 3);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

//gsm
SoftwareSerial SIM900 (7,8); //RX,TX

//temperatue Sensor
dht temp;

//===================== Global Variables ==============================
int count = 0;
double temperature;
String name;
String date;
String time;
long disCm;
bool isValid = false;
int i = 0;

void setup() {

  //GSM
  SIM900.begin(9600);

  //LCD
  lcd.init();
  lcd.begin(20, 4);
  lcd.backlight();

  //RTC
  rtc.begin();

  //Initialize Time
  // rtc.setDate(20, 7, 2023);
  // rtc.setTime(22,40,20);

  //Relay 
  pinMode(4, OUTPUT);
  digitalWrite(4,HIGH);
    
  //ultrasonic
  pinMode(trig , OUTPUT);
  pinMode(echo , INPUT);

  //pin:11 for fans & pin:12 for cooling module
  pinMode(11,OUTPUT);  
  pinMode(12,OUTPUT);  

  //LED
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);  

  //Fingerprint detect test
  while (!Serial); 
  delay(100);

  // set the data rate for the sensor serial port
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
  } else {
    while (1) {
       delay(1); 
      }
  }
  
}

void loop() {

  //RTC 
  date = rtc.getDateStr();
  time = rtc.getTimeStr();
  
  //Temperature Sensor
  temp.read22(6);
  temperature = temp.temperature - 2;

  //LCD 
  LCD(date, time, temperature);

  //fingerprint
  getFingerprintIDez();
  delay(50);

  //Ultrasonic 


  //Peltier Module
  coolingSystem(temperature);

}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  lcd.setCursor(1, 3);
  lcd.print("Place Fingerprint:");

  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK){

    lcd.clear();
    lcd.setCursor(1, 1);
    lcd.print("Invalid Fingerprint");     
    delay(1200);
    lcd.clear();    

    //If fingerprint doesn't match; sounds a buzzer and allows to re-insert upto 3 times
    for(int i=0; i<3; i++){
      digitalWrite(5, HIGH);
      delay(300);
      digitalWrite(5, LOW);
      delay(100);
    }
    delay(2000);
    
    count++;

    if (isValid == true){
      count = 0;
    }
    
    if(count == 3){
      lcd.clear();
      lcd.setCursor(1, 1);
      lcd.print("Try Again in 30s!"); 
    
      delay(25000);
          
      count=0;
    
      //Message to Security
      SendMessage("+94776044674",("Unauthorized Access!"));  
      if (SIM900.available() > 0)
        Serial.write(SIM900.read());

      delay(5000);

      //Message to Pharmacy-in-Charge 
      SendMessage("+94771773680",("Unauthorized Access!")); //94771773680 94771084473
      if (SIM900.available() > 0)
        Serial.write(SIM900.read());

      lcd.clear();
    }

    while (!finger.getImage());
    return -1;
  }

  //================================== found a match!===================================================

  int fingerPrintId = finger.fingerID;
  isValid = true;

  switch(fingerPrintId){
    case 1:
      name = "Achala";
      break;
    case 2:
      name = "Devindi";
      break;
    case 3:
      name = "Wethma";
      break;
    case 4:
      name = "Jithmi";
      break;
    case 5:
      name = "Dinuka";
      break;
  }


  //LED on
  digitalWrite(A2, HIGH);
  delay(2000);

  //LCD
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Fingerprint Verified");
  lcd.setCursor(5, 2);
  lcd.print("Thank You!");

  //Unlock 
  digitalWrite(4, LOW);
  SendMessage("+94776044674",("Door is Opened by " + name).c_str());
  if (SIM900.available() > 0)
    SIM900.read(); 

  delay(2000);

  //LED off
  digitalWrite(A2, LOW);

  lcd.clear();

  //Lock
  delay(2000);
  digitalWrite(4,HIGH);
  
  // //Ultrasonic
  while(true){
    distance();
      if(isValid==true && disCm <16){
      SendMessage("+94776044674",("Door is Closed"));
       if (SIM900.available() > 0)
        Serial.write(SIM900.read()); 
    
     
      }
  } 
   
 
  while(!finger.getImage());
  return fingerPrintId;
  
}

//========================== Ultarsonic =====================================
 int distance(){
  digitalWrite(trig , LOW);
  delayMicroseconds(2);
  digitalWrite(trig , HIGH);
  delayMicroseconds(10);
  digitalWrite(trig , LOW);

  long time = pulseIn(echo, HIGH);

  disCm = time * 0.034 / 2;  
  
  if(isValid==true && disCm <16){
    SendMessage("+94776044674",("Door is Closed"));
    if (SIM900.available() > 0)
      Serial.write(SIM900.read()); 
    
  } 
  return disCm;
}

//==========================GSM Function=====================================================
void SendMessage(const char* recipient, const char* message){
  SIM900.println("AT+CMGF=1");
  delay(1000);
  SIM900.print("AT+CMGS=\"");
  SIM900.print(recipient);  // Set the recipient phone number
  SIM900.println("\"");
  delay(1000);
  SIM900.println(message);
  delay(1000);

  SIM900.write(26);  // Send the Ctrl+Z character to indicate the end of the message
  delay(100);
}

//================================== Peltier Module ====================================
void coolingSystem(double temperature){
  if(temperature > 28){
    //LED
    digitalWrite(A0, HIGH);   
    digitalWrite(A1, LOW);

    //Cooling Module On
    digitalWrite(12,HIGH); 
    //Fans On 
    digitalWrite(11,HIGH); 

  }
  else{
    //LED 
    digitalWrite(A1,HIGH);
    digitalWrite(A0, LOW);  

    //Cooling Module Off   
    digitalWrite(12,LOW); 
    //Fans On 5s for Cool Heatsinks
    //delay(10000);
    
    //Fans Off
    digitalWrite(11,LOW); 
            
  }

}

//================================= LCD ==========================================
void LCD(String date, String time, double temperature){

  lcd.setCursor(0, 0);
  lcd.print(date);
  lcd.setCursor(6, 1);
  lcd.print(time);
  lcd.setCursor(14, 0);
  lcd.print(temperature);
  lcd.print("C");
}
