#include <Arduino.h>
#include <DHT.h>


const int DHT_PIN = 32;  // aus der Pintabelle
const int LED_PIN = 14;  // aus der Pintabelle
const float GRENZWERT_FEUCHTE = 60.0; // in Prozent
const unsigned long INTERVALL_MS = 2000; // der DHT22 misst höchstens alle 2 s

DHT dht(DHT_PIN, DHT22);
unsigned long letzteMessung = 0;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  dht.begin();
}

void loop() {
  if (millis() - letzteMessung >= INTERVALL_MS) {
    letzteMessung = millis();

    float feuchte = dht.readHumidity();
    float temperatur = dht.readTemperature();

    if (isnan(feuchte) || isnan(temperatur)) {
      Serial.println("Fehler: Sensor antwortet nicht");
      digitalWrite(LED_PIN, LOW);
    } else {
      Serial.printf("T = %.1f °C rF = %.1f %%\n", temperatur, feuchte);
      digitalWrite(LED_PIN, feuchte > GRENZWERT_FEUCHTE ? HIGH : LOW);
    }
  }
}