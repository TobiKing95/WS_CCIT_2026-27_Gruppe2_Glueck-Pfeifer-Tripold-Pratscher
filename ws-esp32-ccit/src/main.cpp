#include <Arduino.h>

unsigned long letzteAusgabe = 0;
int zaehler = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 bereit.");
}

void loop() {
  // 1. Alles, was du im Monitor tippst, wird zurückgeschickt
  if (Serial.available()) {
    String eingabe = Serial.readStringUntil('\n');
    eingabe.trim();
    Serial.println("Du hast gesendet: " + eingabe);
  }

  // 2. Jede Sekunde meldet sich der ESP32 von selbst
  if (millis() - letzteAusgabe >= 1000) {
    letzteAusgabe = millis();
    zaehler++;
    Serial.println("Lebenszeichen Nr. " + String(zaehler));
  }
}