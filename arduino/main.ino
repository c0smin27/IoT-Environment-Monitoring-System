// main.ino
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <Wire.h>
#include <SparkFun_SGP40_Arduino_Library.h>
#include <esp_task_wdt.h>
#include "secrets.h"

// --- Topicuri MQTT ---
const char* temperature_topic = "cosmin/temperature";
const char* humidity_topic = "cosmin/humidity";
const char* air_quality_topic = "cosmin/air_quality";

// --- Senzori ---
#define DHTPIN 13
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
SGP40 sgp40;

// --- Client MQTT TLS ---
WiFiClientSecure wifiClientSecure; // client WiFi cu TLS/SSL
PubSubClient mqttClient(wifiClientSecure); // client MQTT cu TLS

// --- Variabile globale ---
float temperature = NAN;
float humidity = NAN;
int vocIndex = -100;
bool sgp40Initialized = false;
unsigned long lastSGP40Retry = 0; // ultima incercare de initializare SGP40
const unsigned long sensorRetryInterval = 10000; // 10 sec

// --- Initializare si gestionare conexiune WiFi ---
void connectWiFi() {
  Serial.print("Connecting to WiFi..");
  WiFi.begin(ssid, password);
  unsigned long startAttemptTime = millis();
  // limitare timp de conectare WiFi la 20 sec
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 20000) {
    Serial.print(".");
    delay(500);
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(" Connected.");
    Serial.print("ESP32-S2 IP address: "); // adresa IP a ESP32
    Serial.println(WiFi.localIP());
  } else {
    Serial.println(" Connection failed!");
  }
}

// --- Conectare ESP32 la brokerul MQTT ---
bool connectMQTT() {
  if (mqttClient.connected()) return true;
  Serial.print("Connecting to MQTT broker with TLS... ");
  // creare clientId unic folosind adresa MAC a ESP32
  String clientId = "ESP32Client-" + WiFi.macAddress();
  // se incearca conectarea la broker folosind clientId
  if (mqttClient.connect(clientId.c_str())) {
    Serial.println("Connected.");
    return true;
  } else {
    Serial.print("Connection failed!, rc=");
    Serial.println(mqttClient.state());
    return false;
  }
}

// --- Initializare senzor SGP40 ---
bool initSGP40() {
  bool ok = sgp40.begin();
  if (!ok) {
    Wire.end();
    delay(100);
    Wire.begin();
    delay(100);
    ok = sgp40.begin();
  }
  if (ok) Serial.println("SGP40 initialized.");
  else Serial.println("SGP40 initialization failed.");
  return ok;
}

// --- Citire date de la senzori ---
void readSensors() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (!isnan(t) && !isnan(h)) {
    temperature = t;
    humidity = h;
    Serial.printf("DHT22 - Temp: %.2f °C, Hum: %.2f %%\n", temperature, humidity);
  } else {
    Serial.println("Failed to read DHT22.");
    temperature = NAN;
    humidity = NAN;
  }

  if (sgp40Initialized) {
    int voc = sgp40.getVOCindex();
    if (voc != -100) {
      vocIndex = voc;
      Serial.printf("SGP40 - VOC Index: %d\n", vocIndex);
    } else {
      Serial.println("Failed to read SGP40 VOC.");
      sgp40Initialized = false; // se incearca reinitializarea
      vocIndex = -100;
    }
  }
}

// --- Publicare date senzori pe MQTT ---
void publishSensorData() {
  char buf[16]; // 15 caractere + terminatorul \0
  bool success = true;

  if (!isnan(temperature)) {
    snprintf(buf, sizeof(buf), "%.2f", temperature);
    if (!mqttClient.publish(temperature_topic, buf)) {
      Serial.println("Failed to publish temperature");
      success = false;
    }
  }

  if (!isnan(humidity)) {
    snprintf(buf, sizeof(buf), "%.2f", humidity);
    if (!mqttClient.publish(humidity_topic, buf)) {
      Serial.println("Failed to publish humidity");
      success = false;
    }
  }

  if (vocIndex != -100) {
    snprintf(buf, sizeof(buf), "%d", vocIndex);
    if (!mqttClient.publish(air_quality_topic, buf)) {
      Serial.println("Failed to publish air quality");
      success = false;
    }
  }

  if (success) {
    Serial.println("All sensor data published.");
    Serial.println("---------------------------------");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Watchdog hardware
  esp_task_wdt_deinit(); // dezactiveaza orice configuratie watchdog anterioara
  esp_task_wdt_config_t wdt_config = {
    .timeout_ms = 30000, // timp maxim de inactivitate permis 30 sec
    .idle_core_mask = (1 << 0), // monitorizare core 0
    .trigger_panic = true // declansare resetare daca watchdog-ul expira
  };
  esp_task_wdt_init(&wdt_config); // initializare watchdog cu setarile specificate
  esp_task_wdt_add(NULL); // adaugare task curent la monitorizarea watchdog

  dht.begin();
  Wire.begin();
  sgp40Initialized = initSGP40();

  connectWiFi();
  wifiClientSecure.setCACert(certificate); // certificat conexiune TSL
  mqttClient.setServer(mqtt_server, mqtt_port); // adresa si portul serverului MQTT
}

void loop() {
  esp_task_wdt_reset(); // resetare watchdog

  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  if (!mqttClient.connected()) {
    if (!connectMQTT()) {
      delay(2000);
    }
  }
  mqttClient.loop(); // se mentine conexiunea MQTT si se proceseaza mesajele primite

  // citirea si publicarea datelor la fiecare 5 sec
  static unsigned long lastSend = 0;
  if (millis() - lastSend > 5000) {
    lastSend = millis();
    readSensors();
    publishSensorData();
  }

  // reinitializare SGP40 daca se deconecteaza
  if (!sgp40Initialized) {
    if (millis() - lastSGP40Retry > sensorRetryInterval) {
      Serial.println("Retrying SGP40 initialization...");
      sgp40Initialized = initSGP40();
      lastSGP40Retry = millis();
    }
  }

  delay(10); // reduce utilizarea CPU
}
