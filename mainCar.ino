// Crazy Car with Servo, ADC Triggering, and Speed Measurement
//@authors Marino Batarilo, Mario Bosnjak, Nevio Mautner
//@version 26.02.2025

#define CLK 2 // Encoder pin for speed measurement
#define DT 3  // Encoder pin for speed measurement

// Distance per encoder pulse in millimeters
const unsigned long distance_per_impulse = 5;

int grad = 0;

// Time tracking for speed calculation
unsigned long lastMillis = 0;
unsigned long lastMovementMillis = 0;

// Counter for encoder pulses
volatile unsigned long ImpulseCounter = 0;
unsigned long LastImpulseCounted = 0;

// Time after which the vehicle is considered stationary (in ms)
const unsigned long NoMovementTime = 2000;

// ADC variables (Nevio Mautner)
#define ADC_CHANNELS 4
volatile uint16_t adcValues[ADC_CHANNELS]; // Stores ADC values per channel
volatile uint8_t currentChannel = 0; // Starts at channel 0

void setup() {
    // Servo initialization (Marino Batarilo) -> MOVED TO TIMER4!
    pinMode(6, OUTPUT); // Servo pin (OC4A)

    // Timer4 for servo control
    TCCR4A = 0;
    TCCR4B = 0;
    TCCR4A = (1 << COM4A1) | (1 << WGM41);  
    TCCR4B = (1 << WGM43) | (1 << WGM42) | (1 << CS41); // Prescaler 8 -> 2MHz
    ICR4 = 20000; // PWM period 20ms (50Hz servo frequency)

    // Speed sensor (Mario Bosnjak)
    Serial.begin(9600);
    pinMode(CLK, INPUT);
    attachInterrupt(digitalPinToInterrupt(CLK), impulse_get, RISING);

    // ADC initialization (Nevio Mautner)
    cli();  // Disable interrupts

    ADMUX  = (1 << REFS0); // AVCC as reference voltage
    ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); 
    ADCSRB = (1 << ADTS2) | (1 << ADTS1); // Auto-trigger ADC conversion

    // Timer1 for ADC triggering (100ms intervals)
    TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);
    OCR1A = 15624; // 100ms interval (16MHz / 1024 / 15624)
    TIMSK1 = (1 << OCIE1A); // Enable Timer1 interrupt

    sei();  // Re-enable interrupts
}

void loop() {
    unsigned long currentMillis = millis();

    // Speed calculation every 1000ms
    if (currentMillis - lastMillis >= 1000) {
        control(grad);
        grad += 10;
        lastMillis = currentMillis;
        
        unsigned long Impulses = ImpulseCounter - LastImpulseCounted;
        LastImpulseCounted = ImpulseCounter;

        float speed = Impulses * distance_per_impulse;
        float distance = ImpulseCounter * distance_per_impulse;

        if (Impulses > 0) {
            lastMovementMillis = currentMillis;
        }

        Serial.print("Speed: ");
        Serial.print(speed);
        Serial.println(" mm/s");

        Serial.print("Distance: ");
        Serial.print(distance);
        Serial.println(" mm");

        if (currentMillis - lastMovementMillis > NoMovementTime) {
            Serial.println("The vehicle is stationary");
        }
    }
}

// ISR for encoder pulse counting
void impulse_get() {
    ImpulseCounter++;
}

// Servo control function (Marino Batarilo)
void control(int grad) {
    OCR4A = map(grad, 0, 180, 1000, 4950); // Map degrees (0-180) to PWM values (1000-4950)
}

// Timer1 ISR -> Triggers ADC conversion
ISR(TIMER1_COMPA_vect) {
    ADMUX = (ADMUX & 0xF0) | currentChannel;
    ADCSRA |= (1 << ADSC);
}

// ADC ISR -> Stores ADC value and selects next channel
ISR(ADC_vect) {
    adcValues[currentChannel] = ADC;
    currentChannel = (currentChannel + 1) % ADC_CHANNELS;
}
