#define BLYNK_TEMPLATE_ID "TMPLN4Gp-byQ"
#define BLYNK_DEVICE_NAME "Smart extractor fan"
#define BLYNK_AUTH_TOKEN "thD0xHSeUIEDHE_3MsBYWkcNJDQJlYGt"

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
void ICACHE_RAM_ATTR buttControl();

BlynkTimer timer;
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Galaxy S21 FE 5G";    // type your wifi name
char pass[] = "12345678";  // type your wifi password


#define DHT11PIN D1  //D1
#define DHTTYPE DHT11
DHT dht(DHT11PIN, DHTTYPE);

#define tempSensorPin A0
#define fanPin D7
bool override = false;

#define buttPin D8
volatile byte buttOverride = LOW;

void readSensors() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  int temp = analogRead(tempSensorPin);

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  float voltage = temp * 3.3;
  voltage /= 1024.0;
  float temperatureC = (voltage - 0.5) * 100;

  if (!buttOverride) {
    if (!override) {
      if (h > 60.00 || t > 30.00 || temperatureC > 31.00) {
        digitalWrite(fanPin, HIGH);
        Blynk.virtualWrite(V4, HIGH);
      } else {
        digitalWrite(fanPin, LOW);
        Blynk.virtualWrite(V4, LOW);
      }
    }
  } else {
    Blynk.virtualWrite(V4, buttOverride);
    digitalWrite(fanPin, buttOverride);
  }

  Blynk.virtualWrite(V0, h);
  Blynk.virtualWrite(V1, t);
  Blynk.virtualWrite(V2, temperatureC);

  Serial.print("Temperature : ");
  Serial.print(t);
  Serial.print("    Humidity : ");
  Serial.print(h);

  Serial.print("    Room Temp : ");
  Serial.println(temperatureC);
}


void setup() {
  Serial.begin(9600);
  dht.begin();

  Blynk.begin(auth, ssid, pass, "blynk.cloud", 8080);
  timer.setInterval(1000L, readSensors);

  pinMode(fanPin, OUTPUT);
  digitalWrite(fanPin, LOW);

  pinMode(buttPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(buttPin), buttControl, FALLING);

  pinMode(D0, OUTPUT);
  digitalWrite(D0, LOW);
  pinMode(D4, OUTPUT);
  digitalWrite(D4, LOW);
}

BLYNK_WRITE(V3) {
  int buttonState = param.asInt();
  if (buttonState == 1) {
    override = true;

  } else if (buttonState == 0) {
    override = false;
  }
  Blynk.virtualWrite(V4, override);
  digitalWrite(fanPin, buttonState);
}

void loop() {
  Blynk.run();
  timer.run();
}

void buttControl() {
  buttOverride = !buttOverride;
}