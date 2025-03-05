#define CLK 2
#define DT 3
// Distance per impulse in mm
const float distance_per_impulse = 5;
//counting impulses of encoder
volatile long ImpulseCounter = 0;
unsigned long LastImpulseCounted = 0;
//counting the time when the programm started
unsigned long lastMillis = 0;
unsigned long lastMovementMillis = 0;
//time it takes to detect if the vehicle is standing
const unsigned long NoMovementTime = 2000; 

void setup() {
  Serial.begin(9600);
  pinMode(CLK, INPUT_PULLUP);
  pinMode(DT, INPUT_PULLUP);
  
  // setting interrupt for
  attachInterrupt(digitalPinToInterrupt(CLK), impulse_get, FALLING);
}

void loop() {
  unsigned long currentMillis = millis();

  // calculating distance and speed every 1000ms
  if (currentMillis - lastMillis >= 1000) {
    lastMillis = currentMillis;

    // counting new impulses
    long Impulses = ImpulseCounter - LastImpulseCounted;
    LastImpulseCounted = ImpulseCounter;

    // calculating speed
    float speed = (Impulses * distance_per_impulse); // Meter pro Sekunde
    float distance = ImpulseCounter * distance_per_impulse; // Gesamtstrecke in Meter

    // if movement is detected counts impulse
    if (Impulses != 0) {
      lastMovementMillis = currentMillis;
    }

    // writing the values in serial monitor
    Serial.print("Geschwindigkeit: ");
    Serial.print(speed, 3);
    Serial.println(" m/s");

    Serial.print("Strecke: ");
    Serial.print(distance, 3);
    Serial.println(" m");

    // detecting if vehicle stands still
    if (currentMillis - lastMovementMillis > NoMovementTime) {
      Serial.println("Das Fahrzeug steht still");
    }
  }
}

// Interrupt Service Routing for encoder
void impulse_get() {
  if (digitalRead(DT) == LOW) {
    ImpulseCounter--; // backwards
  } else {
      ImpulseCounter++;
  }
}
