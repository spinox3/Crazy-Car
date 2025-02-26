// ADC mit Timer-Triggerung von Nevio Mautner (mit ATmega2560)

#define ADC_CHANNELS 4  // Es werden 4 ADC Kanäle "gescannt"
volatile uint16_t adcValues[ADC_CHANNELS]; // Speichert die gemessenen ADC-Werte für jeweiligen Kanal
volatile uint8_t currentChannel = 0; // Aktueller ADC-Kanal = 0 als start

void setup() {
    cli();  // Interrupts deaktivieren
    
    // ADC-Initialisierung
    ADMUX  = (1 << REFS0); // AVCC als Referenzspannung, legt 5V fest
    ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // ADC aktivieren, Interrupt aktivieren, Prescaler 128
    //ADEN Aktiviert den ADC; ADIE aktiviert den den Interrupt
    ADCSRB = (1 << ADTS2) | (1 << ADTS1); // ADC-Takt = 16 MHz / 128 = 125 kHz

    // Timer1-Initialisierung für ADC-Triggerung
    TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10); // CTC-Modus von WGM
    //Prescaler = 1024 (der Timer zählt mit 16 MHz / 1024 deshalb 15.625 kHz)
    OCR1A = 15624; // Timer-Intervall für ca. 100 ms (16MHz / 1024 / 15624)
    TIMSK1 = (1 << OCIE1A); // Timer-Interrupt aktivieren
    
    sei();  // aktiviert wieder die Interrupts, nachdem die Konfig abgeschlossen ist
}

// Timer1 ISR (löst ADC-Conversion aus)
ISR(TIMER1_COMPA_vect) {
    ADMUX = (ADMUX & 0xF0) | currentChannel; // Nächsten ADC-Kanal wählen
    ADCSRA |= (1 << ADSC); // ADC-Wandlung starten
}
// Wird alle 100 ms ausgelös t 
// Wählt den nächsten ADC-Kanal (ADMUX & 0xF0 setzt die unteren 4 Bits neu)
// Startet  neue ADC-Wandlung (ADSC = 1).

// ADC ISR (speichert den Wert und wählt den nächsten Kanal)
ISR(ADC_vect) {
    adcValues[currentChannel] = ADC; // Wert speichern
    currentChannel = (currentChannel + 1) % ADC_CHANNELS; // Nächsten Kanal setzen
}
// Speichert den ADC-Wert im "Array adcValues"
// Wechselt zum nächsten ADC-Kanal (0 → 1 → 2 → 3 → 0 und so weiter)

void loop() {
    // Hauptprogramm greift auf adcValues zu
}
