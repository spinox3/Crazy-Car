#define CLK 2
#define DT 3
//Distanz pro Impuls in Millimetern
const unsigned long distance_per_impulse = 5;
//Variablen zur Messung der Zeitinervallen
unsigned long lastMillis = 0;
unsigned long lastMovementMillis = 0;
//zählt die Impulse des Encoders
volatile bool debounceISR = false;
volatile long ImpulseReset = 0;
volatile long ImpulseCounter = 0;
unsigned long LastImpulseCounted = 0;
//Zeit die vergeht wo das Fahrzeug als "stillstehend" gesehen wird
const unsigned long NoMovementTime = 2000;


void setup() {
  Serial.begin(9600);
  pinMode(CLK, INPUT);
  //Interrupt für den Encoder setzen
  attachInterrupt(digitalPinToInterrupt(CLK), impulseA_get, FALLING);
  attachInterrupt(digitalPinToInterrupt(DT), impulseB_get, FALLING);
}

void loop() {
  unsigned long currentMillis = millis();
    //Geschwindigkeits und Distanzberechnung alle 1000ms
    //Der Timer ist 1Hz
    debounceISR = true;
    if (currentMillis - lastMillis >= 1000){
      lastMillis = currentMillis;
    //Neuen Impulse zählen
    unsigned long Impulses = ImpulseCounter - LastImpulseCounted;
    LastImpulseCounted = ImpulseCounter;
    //Berechnung der Geschwindigkeit
    float speed = Impulses * distance_per_impulse;
    float distance = ImpulseCounter * distance_per_impulse;
    //Die letzte Bewegung aktualisieren wenn Impulse erkannt werden
    if (Impulses > 0) {
      lastMovementMillis = currentMillis;
    }
    //Asugabe in Text
    Serial.print("Geschwindigkeit: ");
    Serial.print(speed);
    Serial.println("m/s");

    Serial.print("Strecke: ");
    Serial.print(distance);
    Serial.println("m");
    //Erkennen ob das Auto steht
    if (currentMillis - lastMovementMillis > NoMovementTime) {
    Serial.println("Das Fahrzeug steht still");
    debounceISR = false;
    }
  }
}  
   
void impulseA_get() {
  if (debounceISR == false) { 
    ImpulseReset++; 
    if (ImpulseReset == 2) {
      ImpulseCounter--; 
      ImpulseReset = 0;
    }
  } 
}

void impulseB_get() {
  if (debounceISR == false) {
    ImpulseReset++;
    if (ImpulseReset == 2) {
      ImpulseCounter++; 
      ImpulseReset = 0;
    }
  }     
}
