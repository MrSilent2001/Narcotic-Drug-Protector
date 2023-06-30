#include <DS3231.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <dht.h>

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

int count = 0;
double temperature;
String name;
String date;
String time;



void setup() {

  Serial.begin(9600);

  //============================== GSM =================================================
  SIM900.begin(9600);

  //============================== LCD =================================================
  lcd.init();
  lcd.begin(20, 4);
  lcd.backlight();

  //=============================== RTC ================================================
  rtc.begin();
  //Initialize Time
  rtc.setDOW(FRIDAY);
  rtc.setDate(01, 7, 2023);
  rtc.setTime(13,50,35);

  //=================================== Relay ==========================================
  pinMode(4, OUTPUT);

  digitalWrite(4,HIGH);
  
  //ultrasonic
  pinMode(trig , OUTPUT);
  pinMode(echo , INPUT);

  //===================== pin:11 for fans & pin:12 for cooling module ===================
  pinMode(11,OUTPUT);  
  pinMode(12,OUTPUT);  

  //LED
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);  

  //=================================== Fingerprint ======================================
  while (!Serial); 
  delay(100);
  Serial.println("\n\nAdafruit finger detect test");

  // set the data rate for the sensor serial port
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) {
       delay(1); 
      }
  }

  finger.getTemplateCount();
  Serial.println("Waiting for valid finger...");
  Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");

  //GSM
  Connect2Server();
}

void loop() {

  //========================================= RTC ============================================
  date = rtc.getDateStr();
  time = rtc.getTimeStr();
  
  //================================= Temperature Sensor =====================================
  int val=temp.read22(6);
  temperature = temp.temperature;

  lcd.setCursor(0, 0);
  lcd.print(date);
  lcd.setCursor(6, 1);
  lcd.print(time);
  lcd.setCursor(14, 0);
  lcd.print(temperature);
  lcd.print("C");

  getFingerprintIDez();
  delay(50);

  
  if(temperature > 30){
    //LED
      digitalWrite(A0, HIGH);   
      digitalWrite(A1, LOW);

      digitalWrite(12,HIGH);  //cooling module on:
      digitalWrite(11,HIGH); //fans on:

  }
  else{
    //LED
      digitalWrite(A1,HIGH);
      digitalWrite(A0, LOW);  
         
      digitalWrite(12,LOW); //cooling module off:
      //fans on 5s for cool heatsinks:
      delay(5000);
      digitalWrite(11,LOW); //fans off:
            
  }
  
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
    Serial.println("Error!!! No Record Found");
    lcd.clear();
    lcd.setCursor(1, 1);
    lcd.print("Invalid Fingerprint");
    lcd.setCursor(5, 2);          
    delay(2000);
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

    if(count == 3){
      Serial.println("Maximum Attempts Exceeded!");
      lcd.clear();
      lcd.setCursor(1, 1);
      lcd.print("Try Again in 30s!"); 
    
      delay(30000);
      lcd.clear();    
      count=0;
    
      // message to security  
      SendMessage("+94771773680",("Unauthorized Access!"));
      if (SIM900.available() > 0)
        Serial.write(SIM900.read());
        delay(1000);
      // message to pharmacy incharge
      SendMessage("+94776044674",("Unauthorized Access!"));  
      if (SIM900.available() > 0)
        Serial.write(SIM900.read());   
    }

    while (!finger.getImage());
    return -1;
  }

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);

  int fingerPrintId = finger.fingerID;

  //GSM
  Field1(String(fingerPrintId));

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
      name = "Dinuka";
      break;
    case 5:
      name = "Jithmi";
      break;
  }

  Field2(name);

  Field3(date);
  Field4(time);
  Field5(String(temperature));

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

  delay(2000);

  //LED off
  digitalWrite(A2, LOW);

  lcd.clear();

  //Lock
  delay(2000);
  digitalWrite(4,HIGH);
  
  //Ultrasonic
  distance(name);
 

  //GSM
  EndConnection();
  

  while(!finger.getImage());

  return finger.fingerID;
  
}

//========================== Ultarsonic =====================================================
 void distance(String name){
  digitalWrite(trig , LOW);
  delayMicroseconds(2);
  digitalWrite(trig , HIGH);
  delayMicroseconds(10);
  digitalWrite(trig , LOW);

  long time = pulseIn(echo, HIGH);

  long disCm = time * 0.034 / 2;  
  
  if(disCm > 16){
    Serial.print("Door Opened ");
    Serial.println(disCm);
    SendMessage("+94776044674",("Door is Opened by Mr " + name).c_str());
    ShowSerialData();
    
  }
  else{
    Serial.print("Door Closed ");
    Serial.println(disCm);
  }
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

void Connect2Server(){

  SIM900.println("AT");
  delay(200);

  SIM900.println("AT+CPIN");
  delay(200);

  SIM900.println("AT+CREG?");
  delay(200);

  SIM900.println("AT+CGATT?");
  delay(200);

  SIM900.println("AT+CIPSHUT");
  delay(200);

  SIM900.println("AT+CIPSTATUS");
  delay(200);

  SIM900.println("AT+CIPMUX=0");
  delay(200);
  ShowSerialData();

  SIM900.println("AT+CSTT=\"internet\"");
  delay(200);
  ShowSerialData();

  SIM900.println("AT+CIICR");
  delay(200);
  ShowSerialData();

  SIM900.println("AT+CIFSR");
  delay(200);
  ShowSerialData();

  SIM900.println("AT+CIPSPRT=0");
  delay(100);
  ShowSerialData();

  SIM900.println("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"");
  delay(200);
  ShowSerialData();

  SIM900.println("AT+CIPSEND");
  delay(200);
  ShowSerialData();
}

void Field1(String data){
  String str = "GET https://api.thingspeak.com/update?api_key=0ZIM50LG3KMK87O0&field1=" + String(data);
  SIM900.println(str);

  delay(1000);
  ShowSerialData();
  SIM900.println((char)26);
  delay(2000);
  SIM900.println();

  ShowSerialData();
}

void Field2(String data){
  String str = "GET https://api.thingspeak.com/update?api_key=0ZIM50LG3KMK87O0&field2=" + String(data);
  SIM900.println(str);

  delay(1000);
  ShowSerialData();
  SIM900.println((char)26);
  delay(2000);
  SIM900.println();

  ShowSerialData();
}

void Field3(String data){
  String str = "GET https://api.thingspeak.com/update?api_key=0ZIM50LG3KMK87O0&field3=" + String(data);
  SIM900.println(str);

  delay(1000);
  ShowSerialData();
  SIM900.println((char)26);
  delay(2000);
  SIM900.println();

  ShowSerialData();
}

void Field4(String data){
  String str = "GET https://api.thingspeak.com/update?api_key=0ZIM50LG3KMK87O0&field4=" + String(data);
  SIM900.println(str);

  delay(1000);
  ShowSerialData();
  SIM900.println((char)26);
  delay(2000);
  SIM900.println();

  ShowSerialData();
}

void Field5(String data){
  String str = "GET https://api.thingspeak.com/update?api_key=0ZIM50LG3KMK87O0&field5=" + String(data);
  SIM900.println(str);

  delay(1000);
  ShowSerialData();
  SIM900.println((char)26);
  delay(2000);
  SIM900.println();

  ShowSerialData();
}

void EndConnection(){
  SIM900.println("AT+CIPSHUT");
  delay(1000);
  ShowSerialData();
}

void ShowSerialData(){
  while(SIM900.available()!=0)
    Serial.write(SIM900.read());
}

