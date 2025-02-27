// Servo Motor controller without Servo.h library (using Timer4)
// @author Marino
// @version 27.02.2025

void setup() {
  pinMode(6, OUTPUT); // Pin 6 als Ausgang (OC4A)
  TCCR4A = 0;
  TCCR4B = 0;
  // Fast PWM Mode mit ICR4 als TOP-Wert (präzise 50 Hz / 20 ms Periode)
  TCCR4A = (1 << COM4A1) | (1 << WGM41);  // Clear OC4A on Compare Match, Fast PWM Mode
  TCCR4B = (1 << WGM43) | (1 << WGM42) | (1 << CS41); // Prescaler 8 = 16/8=2MHz, Fast PWM mit ICR4 als TOP

  ICR4 = 20000; // setting to 20k TOP, 0.5us is periodlength meaning *20000 is 10ms, whole Period is then 20ms = 50Hz
  // Servo motor works with 50Hz = 20ms Period; first 1-2ms on-time determines rotation degree of servo
}

void loop() {
  control(0); 
  control(90);
  control(180); 
}

void control(int grad) {
  OCR4A = map(grad, 0, 180, 1000, 4950); // map correctly to be able to enter degrees
  delay(1000);
}
