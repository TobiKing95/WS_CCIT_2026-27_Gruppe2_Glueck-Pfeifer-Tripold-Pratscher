#include <Arduino.h>
#include <DHT.h>
#include <driver/gpio.h>
#include <soc/gpio_reg.h>

const int DHT_PIN = 32;
const int LED_PIN = 14;
const float GRENZWERT_FEUCHTE = 60.0;
const float GRENZWERT_TEMPERATUR = 27.0;
const unsigned long INTERVALL_MS = 2000;

const int EBENE = 3;  // 1 = digitalWrite, 2 = gpio_set_level, 3 = Register

DHT dht(DHT_PIN, DHT22);
unsigned long letzteMessung = 0;

void ledSchalten(bool an) {
  if (EBENE == 1) {
    digitalWrite(LED_PIN, an ? HIGH : LOW);
  } else if (EBENE == 2) {
    gpio_set_level((gpio_num_t)LED_PIN, an ? 1 : 0);
  } else if (an) {
    REG_WRITE(GPIO_OUT_W1TS_REG, 1UL << LED_PIN);
  } else {
    REG_WRITE(GPIO_OUT_W1TC_REG, 1UL << LED_PIN);
  }
}

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
      ledSchalten(false);
    } else {
      bool feuchteZuHoch = feuchte > GRENZWERT_FEUCHTE;
      bool temperaturZuHoch = temperatur > GRENZWERT_TEMPERATUR;

      Serial.printf("T = %.1f °C rF = %.1f %%", temperatur, feuchte);
      if (feuchteZuHoch) Serial.print("  ALARM: Feuchte");
      if (temperaturZuHoch) Serial.print("  ALARM: Temperatur");
      Serial.println();

      ledSchalten(feuchteZuHoch || temperaturZuHoch);
    }
  }
}
