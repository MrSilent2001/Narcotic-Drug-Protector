#define trig 2
#define echo 4

const int blueLED = 7;
const int redLED = 8;

void setup() {
  // put your setup code here, to run once:
  pinMode(trig , OUTPUT);
  pinMode(echo , INPUT);

  pinMode(blueLED, OUTPUT);
  pinMode(redLED , OUTPUT);

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(trig , LOW);
  delayMicroseconds(2);
  digitalWrite(trig , HIGH);
  delayMicroseconds(5);
  digitalWrite(trig , LOW);

  long time = pulseIn(echo, HIGH);

  long disInch = time / 74 / 2;
  long disCm = time / 29 / 2;

  Serial.print(disInch);
  Serial.print("inch  ");
  Serial.print(disCm);
  Serial.println("cm");

  delay(2000);

  if (disCm > 15){
    digitalWrite(redLED , HIGH);
    digitalWrite(blueLED , LOW);
  }
  else{
    digitalWrite(blueLED , HIGH);
    digitalWrite(redLED , LOW);
  }

}
