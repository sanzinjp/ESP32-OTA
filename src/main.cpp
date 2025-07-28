#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <PubSubClient.h>
#include <Update.h>
#include <WiFi.h>

#define WIFI_SSID "TP-Link_60_502"
#define WIFI_PASSWORD "isgm1234"

#define MQTT_BROKER "https://thingsboard.cloud"
#define MQTT_PORT 1883
#define TOKEN "blyrDnUgs1dSqxTMjUp7"

#define CURRENT_FW_VERSION "1.0.2"

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

const char *OTA_TOPIC = "v1/devices/me/attributes/response/+";
const char *REQUEST_ATTRS = "{\"sharedKeys\":\"fw_title,fw_version,fw_url\"}";

void performOTA(const char *url);

void connectWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println(" connected");
}

void connectMQTT() {
  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  mqttClient.setCallback([](char *topic, byte *payload, unsigned int length) {
    payload[length] = '\0';
    ArduinoJson::JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload);
    if (error) {
      Serial.println("JSON parse error");
      return;
    }

    const char *fw_title = doc["fw_title"];
    const char *fw_version = doc["fw_version"];
    const char *fw_url = doc["fw_url"];

    Serial.printf("FW title: %s\nFW version: %s\nFW URL: %s\n", fw_title,
                  fw_version, fw_url);

    if (strcmp(fw_version, CURRENT_FW_VERSION) != 0) {
      Serial.println("New firmware available. Starting OTA...");
      performOTA(fw_url);
    } else {
      Serial.println("Firmware is up-to-date.");
    }
  });

  while (!mqttClient.connected()) {
    Serial.print("Connecting to MQTT...");
    if (mqttClient.connect("ESP32_Client", TOKEN, nullptr)) {
      Serial.println(" connected");
      mqttClient.subscribe(OTA_TOPIC);
      mqttClient.publish("v1/devices/me/attributes/request/1", REQUEST_ATTRS);
    } else {
      Serial.print(" failed, rc=");
      Serial.print(mqttClient.state());
      delay(2000);
    }
  }
}

void performOTA(const char *url) {
  HTTPClient http;
  http.begin(url);
  int httpCode = http.GET();
  if (httpCode == 200) {
    int len = http.getSize();
    WiFiClient &stream = http.getStream();

    if (Update.begin(len)) {
      size_t written = Update.writeStream(stream);
      if (written == len && Update.end() && Update.isFinished()) {
        Serial.println("OTA done. Rebooting...");
        delay(1000);
        ESP.restart();
      } else {
        Serial.println("OTA failed.");
      }
    } else {
      Serial.println("Not enough space for OTA");
    }
  } else {
    Serial.printf("HTTP error: %d\n", httpCode);
  }
  http.end();
}

void setup() {
  Serial.begin(115200);
  connectWiFi();
  connectMQTT();
}

void loop() { mqttClient.loop(); }
