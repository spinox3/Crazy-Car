// Servo Motor controller without Servo.h library, ADC-Interrupts and Velocity Measurement for the Crazy Car.
//@author Marino Batarilo, Mario Bosnjak, Nevio Mautner
//@version 26.02.2025

#define CLK 2 // Encoder input pin for velocity measurement
#define DT 3 // Encoder input pin for velocity measurement

// Distance per encoder pulse in millimeters
const unsigned long distance_per_impulse = 5;

// Variables for measuring time intervals
unsigned long lastMillis = 0; // Stores the last time velocity was measured
unsigned long lastMovementMillis = 0; // Stores the last time movement was detected

// Counts the number of encoder pulses
volatile unsigned long ImpulseCounter = 0;
unsigned long LastImpulseCounted = 0;

// Time after which the vehicle is considered stationary (in milliseconds)
const unsigned long NoMovementTime = 2000;

#define ADC_CHANNELS 4  // Number of ADC channels to be scanned
volatile uint16_t adcValues[ADC_CHANNELS]; // Stores the measured ADC values per channel
volatile uint8_t currentChannel = 0; // Starts at ADC channel 0

void setup() {
  // High-speed Servo Controller (Marino Batarilo)
  pinMode(9, OUTPUT); // Set pin 9 as output (OC1A register)

  // Reset Timer1 registers
  TCCR1A = 0;
  TCCR1B = 0;

  // Configure Timer1 for Fast PWM mode with ICR1 as TOP value
  TCCR1A = (1 << COM1A1) | (1 << WGM11);  // Clear OC1A on Compare Match, Fast PWM Mode
  TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11); // Prescaler 8: 16MHz/8 = 2MHz

  ICR1 = 20000; // Set TOP to 20000 (corresponding to 20ms period, i.e., 50Hz PWM)
  // The servo motor operates at 50Hz (20ms period), with 1-2ms pulse width controlling its angle

  // Velocity Measurement (Mario Bosnjak)
  Serial.begin(9600); // Initialize Serial communication at 9600 baud
  pinMode(CLK, INPUT);
  attachInterrupt(digitalPinToInterrupt(CLK), impulse_get, RISING); // Attach interrupt to encoder

  // ADC Interrupt (Nevio Mautner)
  cli();  // Disable interrupts during configuration

  // Configure ADC
  ADMUX  = (1 << REFS0); // Use AVCC (5V) as reference voltage
  ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Enable ADC, enable interrupt, set prescaler to 128 (16MHz / 128 = 125kHz)
  ADCSRB = (1 << ADTS2) | (1 << ADTS1); // Configure ADC auto-trigger

  // Configure Timer1 to trigger ADC conversion every 100ms
  TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10); // CTC mode, prescaler 1024
  OCR1A = 15624; // Timer interval for ~100ms (16MHz / 1024 / 15624)
  TIMSK1 = (1 << OCIE1A); // Enable Timer1 compare match interrupt

  sei();  // Re-enable interrupts after configuration
}

void loop() {
  // Velocity Measurement (Mario Bosnjak)
  unsigned long currentMillis = millis();

  // Calculate speed and distance every 1000ms (1Hz interval)
  if (currentMillis - lastMillis >= 1000) {
    lastMillis = currentMillis;
    
    // Calculate new impulses since last measurement
    unsigned long Impulses = ImpulseCounter - LastImpulseCounted;
    LastImpulseCounted = ImpulseCounter;

    // Calculate velocity (mm/s) and total distance traveled (mm)
    float speed = Impulses * distance_per_impulse;
    float distance = ImpulseCounter * distance_per_impulse;

    // Update last movement time if impulses were detected
    if (Impulses > 0) {
      lastMovementMillis = currentMillis;
    }

    // Print speed and distance to Serial Monitor
    Serial.print("Speed: ");
    Serial.print(speed);
    Serial.println(" mm/s");

    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" mm");

    // Detect if the vehicle has stopped moving
    if (currentMillis - lastMovementMillis > NoMovementTime) {
      Serial.println("The vehicle is stationary");
    }
  }
}

// Interrupt Service Routine (ISR) for encoder pulse counting
void impulse_get() {
  ImpulseCounter++;
}

// Servo motor control function (High-speed Servo Control)
void control(int grad) {
  OCR1A = map(grad, 0, 180, 1000, 4950); // Map degrees (0-180) to PWM values (1000-4950)
}

// Timer1 ISR - Triggers ADC conversion
ISR(TIMER1_COMPA_vect) {
  ADMUX = (ADMUX & 0xF0) | currentChannel; // Select next ADC channel
  ADCSRA |= (1 << ADSC); // Start ADC conversion
}

// ADC ISR - Stores ADC value and selects the next channel
ISR(ADC_vect) {
  adcValues[currentChannel] = ADC; // Store ADC value
  currentChannel = (currentChannel + 1) % ADC_CHANNELS; // Cycle through ADC channels
}
