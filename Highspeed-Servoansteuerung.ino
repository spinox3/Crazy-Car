// Code von Marino

#define Servo_Pin PB1  // PB1 entspricht Pin 13 auf deinem ATmega328P

void setup() {
  // Fast PWM Mode mit ICR1 als TOP-Wert (präzise 50 Hz / 20 ms Periode)
  TCCR1A = (1 << COM1A1) | (1 << WGM11);  // Clear OC1A on Compare Match, Fast PWM Mode
  TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11); // Prescaler 8 = 16/8=2MHz, Fast PWM and ICR1 is TOP

  ICR1 = 20000; // setting to 20k TOP, 0.5us is half periodlength meaning *20000 is 10ms, whole Period is then 20ms = 50Hz
  // Servo motor works with 50Hz = 20ms Period, first 1-2ms on-time determines rotation degree of servo
}


void loop() {
  OCR1A = 1000; // 1000 * 0.5us = 0.5ms whole Period = 1ms = 0°
  delay(1000);
  OCR1A = 1500;
  delay(1000);
  OCR1A = 2000;
  delay(1000);
}
