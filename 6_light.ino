const int pinPhoto = 35;
int raw = 0;

void setup() {
  Serial.begin(115200);
  pinMode( pinPhoto, INPUT );
}

void loop() {
  raw = analogRead( pinPhoto );
  Serial.println( raw );
  delay(200);
}
