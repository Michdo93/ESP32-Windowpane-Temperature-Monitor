#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Konfigurationsdatei einbinden
#include "config.h"

// Objekte initialisieren
WiFiClient espClient;
PubSubClient mqttClient(espClient);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Zeitsteuerung
unsigned long lastMeasureTime = 0;

// WLAN Verbindung aufbauen
void setupWiFi() {
  delay(10);
  Serial.println();
  Serial.print("Verbinde mit WLAN: ");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WLAN verbunden!");
  Serial.print("IP-Adresse: ");
  Serial.println(WiFi.localIP());
}

// MQTT Verbindung aufbauen / wiederherstellen
void reconnectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Versuche MQTT-Verbindung...");
    
    // Verbindung versuchen (mit Last Will Message)
    if (mqttClient.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD, MQTT_TOPIC_STATUS, 1, true, "offline")) {
      Serial.println(" verbunden!");
      // Status auf 'online' setzen
      mqttClient.publish(MQTT_TOPIC_STATUS, "online", true);
    } else {
      Serial.print(" Fehlgeschlagen, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" Neuer Versuch in 5 Sekunden...");
      delay(5000);
    }
  }
}

void setup() {
  // Serielle Schnittstelle starten
  Serial.begin(115200);
  Serial.println("\n--- ESP32 Fensterscheiben-Temperatursensor ---");

  // DS18B20 Sensoren starten
  sensors.begin();
  int deviceCount = sensors.getDeviceCount();
  Serial.print("Anzahl gefundener DS18B20 Sensoren: ");
  Serial.println(deviceCount);

  // Netzwerkeinrichtung
  setupWiFi();
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
}

void loop() {
  // Verbindung zu WLAN/MQTT sicherstellen
  if (WiFi.status() != WL_CONNECTED) {
    setupWiFi();
  }
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop();

  // Zyklische Messung und MQTT-Versand
  unsigned long now = millis();
  if (now - lastMeasureTime >= INTERVAL_MS) {
    lastMeasureTime = now;

    // Temperatur anfordern
    sensors.requestTemperatures();
    float tempC = sensors.getTempCByIndex(0);

    // Prüfen, ob der Sensor antwortet
    if (tempC == DEVICE_DISCONNECTED_C) {
      Serial.println("Fehler: DS18B20 konnte nicht gelesen werden!");
      mqttClient.publish(MQTT_TOPIC_STATUS, "sensor_error");
    } else {
      // Signalstärke auslesen
      long rssi = WiFi.RSSI();

      // JSON Payload zusammenbauen
      String payload = "{";
      payload += "\"temperature\":" + String(tempC, 2) + ",";
      payload += "\"unit\":\"°C\",";
      payload += "\"wifi_rssi\":" + String(rssi);
      payload += "}";

      Serial.print("Sende MQTT Payload: ");
      Serial.println(payload);

      // Über MQTT publizieren
      mqttClient.publish(MQTT_TOPIC_TEMP, payload.c_str());
    }
  }
}