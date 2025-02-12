#hallo und willkommen zur Testung das ist unser Projekt der Testung

void setup() {
  Serial.begin(115200);

  ADMUX = ADMUX | B00001000;
  ADMUX = ADMUX | (1<<MUX3);
  // Temperatur Sensor auswählen
  ADMUX |= 1 << MUX3;
  ADMUX = ADMUX & B11111000;
  ADMUX = ADMUX & (~(1<<MUX2));
  // interne Referenz Spannung auswählen
  ADMUX |= 1 << REFS0;
}

void loop() {
  Serial.println(ADMUX,BIN);

  delay(1000);
}
