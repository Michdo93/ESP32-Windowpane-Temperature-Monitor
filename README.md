# ESP32 Windowpane Temperature Monitor (DS18B20 + MQTT)

Ein intelligentes IoT-System zur präzisen Messung der Fensterscheibentemperatur mittels **ESP32** und **DS18B20** Temperatursensor. Die Messwerte werden zyklisch über WLAN per **MQTT** (z. B. an Home Assistant, Node-RED, Mosquitto oder ioBroker) übertragen.

---

## 📸 Systemübersicht & Aufbau

![DS18B20 Windowpane Setup](assets/hardware_setup.jpg)

---

## 🛠️ Benötigte Hardware & Bauteile

| Bauteil | Anzahl | Beschreibung / Empfehlung |
| :--- | :---: | :--- |
| **ESP32 NodeMCU Board** | 1× | Z. B. ESP32-WROOM-32 DevKit v1 |
| **DS18B20 Sensor** | 1× | TO-92 Transistor-Bauform oder wasserdichte Kabel-Sonde |
| **4,7 kΩ Widerstand** | 1× | Pull-Up-Widerstand (Farbcode: Gelb-Violett-Rot-Gold) |
| **Kapton-Klebeband** | 1 Roll | Polyimid-Folie (hitzebeständig, hinterlässt keine Klebereste) |
| **Dämmstoff** | 1 Stück | Ca. 2×2 cm Schaumstoff, Armaflex oder Neopren |
| **Breadboard & Jumper wire** | - | Zur schnellen Verkabelung (Dupont-Kabel) |
| **USB-Kabel** | 1× | Micro-USB oder USB-C (zur Stromversorgung/Flashen) |

---

## 🔌 Verkabelung & Schaltplan

Der DS18B20 nutzt das **OneWire-Protokoll**. Daher werden alle Daten über eine einzige Signalleitung übertragen. Zwischen der **Data-Leitung** und **3.3V (VCC)** muss zwingend ein **4,7 kΩ Pull-Up-Widerstand** geschaltet werden.

### Pinbelegung ESP32 & DS18B20

| DS18B20 Pin (TO-92) | Kabel-Sonde Farbe | ESP32 Board | Hinweis |
| :--- | :--- | :--- | :--- |
| **Pin 1 (GND)** | Schwarz | **GND** | Masse |
| **Pin 2 (DATA)** | Gelb / Weiß | **GPIO 4** | Signalleitung |
| **Pin 3 (VCC)** | Rot | **3V3** | 3.3 Volt Stromversorgung |

> ⚠️ **Wichtig beim TO-92 Gehäuse:** Wenn du von vorne auf die flache beschriftete Seite schaust, ist **Pin 1 links (GND)**, **Pin 2 Mitte (DATA)**, **Pin 3 rechts (VCC)**.

---

## 🪟 Richtige Montage an der Fensterscheibe

Um tatsächlich die **Oberflächentemperatur der Glasscheibe** zu messen (und nicht die Umgebungsluft des Raumes), folge dieser Schritt-für-Schritt-Anleitung:

1. **Reinigung:** Die Glasscheibe an der gewünschten Stelle gründlich mit Isopropanol oder Glasreiniger entfetten.
2. **Platzierung:** Die flache Seite des DS18B20 direkt auf das Glas drücken.
3. **Erste Fixierung:** Den Sensor mit einem Streifen **Kapton-Klebeband** stramm auf der Scheibe fixieren.
4. **Thermische Entkopplung (Essentiell!):** Ein ca. 2×2 cm großes Stück Dämmstoff (Armaflex, Schaumstoff oder Styropor) direkt über den Sensor legen.
5. **Finale Fixierung:** Den Dämmblock mit weiterem Kapton-Klebeband fest anpressen.
   > **Effekt:** Die Dämmung schirmt den Sensor ab. Er nimmt somit die echte Temperatur der Fensterscheibe an.

---

## 💻 Software & Flashen

### 1. Voraussetzungen (Arduino IDE)
1. Installiere die [Arduino IDE](https://www.arduino.cc/en/software) (Version 2.x empfohlen).
2. Füge die ESP32 Boardverwalter-URL hinzu (*Datei → Voreinstellungen → Zusätzliche Boardverwalter-URLs*):
   `https://dl.espressif.com/dl/package_esp32_index.json`
3. Installiere über den **Boardverwalter** (*Werkzeuge → Board → Boardverwalter*) das Paket **esp32** von Espressif Systems.
4. Installiere über den **Bibliotheksverwalter** (*Werkzeuge → Bibliotheken verwalten*) folgende Bibliotheken:
   * **OneWire** (by Jim Studt, Tom Pollard, etc.)
   * **DallasTemperature** (by Miles Burton)
   * **PubSubClient** (by Nick O'Leary)

---

### 2. Konfiguration anpassen

1. Benenne die Datei `config.example.h` um in `config.h`.
2. Öffne `config.h` und trage deine Zugangsdaten ein:

```cpp
// WLAN Konfiguration
const char* WIFI_SSID = "DEIN_WLAN_NAME";
const char* WIFI_PASSWORD = "DEIN_WLAN_PASSWORT";

// MQTT Server Konfiguration
const char* MQTT_SERVER = "192.168.1.100"; // IP deines MQTT Brokers
const int MQTT_PORT = 1883;
const char* MQTT_USER = "";               // Optional
const char* MQTT_PASSWORD = "";           // Optional

// MQTT Topics
const char* MQTT_TOPIC_TEMP = "home/window/temperature";
const char* MQTT_TOPIC_STATUS = "home/window/status";
```

---

### 3. Hochladen
1. Verbinde den ESP32 per USB-Kabel mit dem PC.
2. Wähle unter *Werkzeuge → Board* dein ESP32 Model (z. B. **ESP32 Dev Module**).
3. Wähle den passenden COM-Port aus (*Werkzeuge → Port*).
4. Klicke auf **Hochladen** (Pfeil-Symbol).

---

## 🛰️ MQTT Payload & Einbindung

Der ESP32 sendet die Daten standardmäßig alle **10 Sekunden** (einstellbar in `config.h`) im JSON-Format an das definierte Topic:

**Topic:** `home/window/temperature`  
**Payload (JSON):**
```json
{
  "temperature": 14.25,
  "unit": "°C",
  "wifi_rssi": -65
}
```

### Home Assistant Integration (YAML Beispiel)
Füge folgenden Block zu deiner `configuration.yaml` hinzu:

```yaml
mqtt:
  sensor:
    - name: "Fensterscheibe Temperatur"
      state_topic: "home/window/temperature"
      value_template: "{{ value_json.temperature }}"
      unit_of_measurement: "°C"
      device_class: "temperature"
      state_class: "measurement"
```

---

## 📁 Repository-Struktur

```text
.
├── assets/
│   └── hardware_setup.jpg     # Schaltplan & Anbringungs-Grafik
├── src/
│   ├── main.cpp               # Hauptprogramm für ESP32
│   ├── config.example.h       # Vorlage für WLAN & MQTT Konfiguration
│   └── config.h               # (Nicht im Git) Deine persönlichen Daten
├── .gitignore
└── README.md                  # Diese Dokumentation
```

---

## 📄 Lizenz
MIT License. Freie Verwendung für private und kommerzielle Projekte.
