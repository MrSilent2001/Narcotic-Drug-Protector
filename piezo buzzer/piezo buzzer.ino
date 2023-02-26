void setup() {
  // put your setup code here, to run once:
  pinMode(10,OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  tone (10, 2000);
  delay(5000);
  tone (10,500);
  delay(5000);
}
