#include <Arduino.h>
#include <DHT.h>
#include <driver/gpio.h>
#include <soc/gpio_reg.h>

const int DHT_PIN = 32;
const int LED_PIN = 14;
const int SUMMER_PIN = 25;
const int TASTER_PIN = 27;
const float GRENZWERT_FEUCHTE = 60.0;

const unsigned long INTERVALL_MS = 2000;

const int EBENE = 3; // 1 = digitalWrite, 2 = gpio_set_level, 3 = Register

DHT dht(DHT_PIN, DHT22);
unsigned long letzteMessung = 0;

volatile bool tasterGedrueckt = false;
volatile unsigned int anzahlInterrupts = 0;
unsigned int gueltigeDruecke = 0;
unsigned long letzterTastendruck = 0;
bool alarmAktiv = false; 
bool stumm = false;

void IRAM_ATTR tasterIsr()
{
  anzahlInterrupts = anzahlInterrupts + 1;
  tasterGedrueckt = true;
}

void ledSchalten(bool an)
{
  if (EBENE == 1)
  {
    digitalWrite(LED_PIN, an ? HIGH : LOW);
  }
  else if (EBENE == 2)
  {
    gpio_set_level((gpio_num_t)LED_PIN, an ? 1 : 0);
  }
  else if (an)
  {
    REG_WRITE(GPIO_OUT_W1TS_REG, 1UL << LED_PIN);
  }
  else
  {
    REG_WRITE(GPIO_OUT_W1TC_REG, 1UL << LED_PIN);
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(SUMMER_PIN, OUTPUT);
  pinMode(TASTER_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(TASTER_PIN), tasterIsr, FALLING);
  dht.begin();
}

void loop()
{
  if (tasterGedrueckt)
  {
    tasterGedrueckt = false;
    if (millis() - letzterTastendruck > 50)
    {
      letzterTastendruck = millis();
      gueltigeDruecke++;
      if (alarmAktiv)
        stumm = true;
    }
  }

    if (millis() - letzteMessung >= INTERVALL_MS)
    {
      letzteMessung = millis();

      float feuchte = dht.readHumidity();
      float temperatur = dht.readTemperature();

      if (isnan(feuchte) || isnan(temperatur))
      {
        Serial.println("Fehler: Sensor antwortet nicht");
        alarmAktiv = false;
        stumm = false;
      }
      else
      {
        alarmAktiv = feuchte > GRENZWERT_FEUCHTE;
        if (!alarmAktiv)
          stumm = false;

        Serial.printf("T = %.1f °C rF = %.1f %%  Status: %s%s  Interrupts: %u  gueltig: %u\n",
                      temperatur, feuchte,
                      alarmAktiv ? "ALARM" : "OK",
                      (alarmAktiv && stumm) ? " (stumm)" : "",
                      anzahlInterrupts, gueltigeDruecke);
      }
    }
  
  ledSchalten(alarmAktiv);
  digitalWrite(SUMMER_PIN, (alarmAktiv && !stumm) ? HIGH : LOW);
  }
