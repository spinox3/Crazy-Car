// ADC mit Timer-Triggerung (Wokwi, ATmega2560)

#define ADC_CHANNELS 4  // Anzahl der zu scannenden ADC-Kanäle
volatile uint16_t adcValues[ADC_CHANNELS]; // Speichert die gemessenen ADC-Werte
volatile uint8_t currentChannel = 0; // Aktueller ADC-Kanal

void setup() {
    cli();  // Interrupts deaktivieren
    
    // ADC-Initialisierung
    ADMUX  = (1 << REFS0); // AVCC als Referenzspannung
    ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // ADC aktivieren, Interrupt aktivieren, Prescaler 128
    ADCSRB = (1 << ADTS2) | (1 << ADTS1); // Trigger-Quelle: Timer1 Compare Match B

    // Timer1-Initialisierung für ADC-Triggerung
    TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10); // CTC-Modus, Prescaler 1024
    OCR1A = 15624; // Timer-Intervall für ca. 100 ms (16MHz / 1024 / 15624)
    TIMSK1 = (1 << OCIE1A); // Timer-Interrupt aktivieren
    
    sei();  // Interrupts aktivieren
}

// Timer1 ISR (löst ADC-Conversion aus)
ISR(TIMER1_COMPA_vect) {
    ADMUX = (ADMUX & 0xF0) | currentChannel; // Nächsten ADC-Kanal wählen
    ADCSRA |= (1 << ADSC); // ADC-Wandlung starten
}

// ADC ISR (speichert den Wert und wählt den nächsten Kanal)
ISR(ADC_vect) {
    adcValues[currentChannel] = ADC; // Wert speichern
    currentChannel = (currentChannel + 1) % ADC_CHANNELS; // Nächsten Kanal setzen
}

void loop() {
    // Hauptprogramm greift auf adcValues zu
}
