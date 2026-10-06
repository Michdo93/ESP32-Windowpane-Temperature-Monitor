#ifndef CONFIG_H
#define CONFIG_H

// ============================================================================
// WLAN KONFIGURATION
// ============================================================================
const char* WIFI_SSID     = "DEIN_WLAN_NAME";
const char* WIFI_PASSWORD = "DEIN_WLAN_PASSWORT";

// ============================================================================
// MQTT BROKER KONFIGURATION
// ============================================================================
const char* MQTT_SERVER   = "192.168.1.100";  // IP-Adresse deines Brokers (z.B. Home Assistant / Mosquitto)
const int   MQTT_PORT     = 1883;             // Standard-MQTT-Port
const char* MQTT_USER     = "";               // Leer lassen, falls keine Authentifizierung
const char* MQTT_PASSWORD = "";               // Leer lassen, falls keine Authentifizierung

// ============================================================================
// MQTT TOPICS & EINSTELLUNGEN
// ============================================================================
const char* MQTT_CLIENT_ID    = "ESP32_Window_Temp_Sensor";
const char* MQTT_TOPIC_TEMP   = "home/window/temperature";
const char* MQTT_TOPIC_STATUS = "home/window/status";

// ============================================================================
// HARDWARE & INTERVALL EINSTELLUNGEN
// ============================================================================
#define ONE_WIRE_BUS 4                    // GPIO-Pin für DS18B20 DATA
const unsigned long INTERVAL_MS = 10000;  // Sendeintervall in Millisekunden (10 Sek.)

#endif // CONFIG_H
