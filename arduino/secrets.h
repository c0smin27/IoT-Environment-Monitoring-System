// secrets.h
#pragma once

// --- Configurari WiFi ---
const char* ssid = "WIFI_SSID";
const char* password = "WIFI_PASSWORD";

// --- Configurari MQTT ---
const char* mqtt_server = "MQTT_BROKER_HOST"; // IP-ul PC-ului (broker Mosquitto)
const int mqtt_port = 8883; // port MQTT criptat cu TLS

// --- Certificat self-signed broker MQTT ('mosquitto.crt')
// folosit pentru validarea conexiunii TLS dintre ESP32 si broker ---
const char* certificate = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDljCCAn6gAwIBAgIUMnVI/OIaL+zs2/2ZBkW33+HWbwUwDQYJKoZIhvcNAQEL\n" \
"BQAwazELMAkGA1UEBhMCUk8xDTALBgNVBAgMBENsdWoxFDASBgNVBAcMC0NsdWot\n" \
(...)
"lB09aClw6g1NSg==\n" \
"-----END CERTIFICATE-----\n";
