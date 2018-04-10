const int seqLed [] = {3, 4, 5, 6, 7, 8, 9, 10};
const int statusLed1 = 13;
const int buttonPin [] = {11, 12, 2};

const int pot1 = A0, pot2 = A1;
const int bodySwitch [] = {A2, A3, A4, A5, A6, A7, A8, A9};

unsigned long lastPrint = millis();
int printFreq = 50;

int ledCount = 0;

void setup() {
  
  for (int i = 0; i < 8; i++) {
    pinMode(seqLed[i], OUTPUT);
  }
  pinMode(statusLed1, OUTPUT);
  
  for (int i = 0; i < 3; i++) {
    pinMode(buttonPin[i], INPUT_PULLUP);
  }

}

void loop() {
  if (lastPrint + printFreq < millis() ) {

    // pots
    Serial.print("potentiometers: ");
    Serial.print(analogRead(pot1));
    Serial.print("\t");
    Serial.println(analogRead(pot2));
    lastPrint = millis();

    // switches
    Serial.print("bodyswitches: \t");
    for (int i = 0; i < 8; i++) {
      Serial.print(analogRead(bodySwitch[i]));
      Serial.print("\t");
    }
    Serial.println();

    // buttons
    Serial.print("buttons: ");
    for (int i = 0; i < 3; i++) {
      if (digitalRead(buttonPin[i]) == HIGH) {
        Serial.print("1 ");
      } else {
        Serial.print("0 ");
      }
    }
    Serial.println();

    // leds
    for (int i = 0; i < 8; i++) {
      if (ledCount == i) {
        digitalWrite(seqLed[i], HIGH);
      } else {
        digitalWrite(seqLed[i], LOW);
      }
      if (ledCount % 2 == 0) {
        digitalWrite(statusLed1, HIGH);
      } else {
        digitalWrite(statusLed1, LOW);
      }
    }
    ledCount++;
    ledCount = ledCount % 8;

  }
}
