#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <BH1750.h>
#include <DHT.h>

const char* ssid = "MERCUSYS_18C1";
const char* password = "18460513";
const char* server = "172.31.0.51";
#define ledpin D4
#define ledpin1 D5
BH1750 lightMeter;
WiFiClient client;
#define DHTPIN D3      // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11  // DHT 22 (AM2302)
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  pinMode(ledpin, OUTPUT);
  pinMode(ledpin1, OUTPUT);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Wire.begin();
  lightMeter.begin();
  dht.begin();

  Serial.println("Connected to Wi-Fi");
}

void loop() {
  int light_intensity = random(0, 500);
  int temperature = random(10, 30);
  int humidity = random(10, 100);
  // float temperature = dht.readTemperature();
  // float humidity = dht.readHumidity();
  // uint16_t light_intensity = lightMeter.readLightLevel();
  StaticJsonDocument<200> doc;
  doc["temperature"] = temperature;
  doc["humidity"] = humidity;
  doc["light_intensity"] = light_intensity;
  String json;
  serializeJson(doc, json);
  if (client.connect(server, 8000)) {
    client.println("POST /Group2CTTT HTTP/1.1");
    client.println("Host: " + String(server));
    client.println("Content-Type: application/json");
    client.println("Content-Length: " + String(json.length()));
    client.println();
    client.println(json);
  }
  while (client.connected()) {
    if (client.available()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        break;
      }
    }
  }
  String response = client.readString();
  Serial.print(response);
  int start_index = response.indexOf("{\"state\":");
  if (start_index != -1) {
    int end_index = response.indexOf("}", start_index);
    if (end_index != -1) {
      String json_data = response.substring(start_index, end_index + 1);
      StaticJsonDocument<200> doc;
      deserializeJson(doc, json_data);
      int state = doc["state"];
      set_lights(state);  // function to set the number of lights to turn on
    }
  }
  delay(5000);
}
void set_lights(int num_lights) {
  if (num_lights == 0) {
    if (digitalRead(ledpin) == 0)
      digitalWrite(ledpin, HIGH);
    else digitalWrite(ledpin, LOW);
  } else if (num_lights == 1) {
    if (digitalRead(ledpin1) == 0)
      digitalWrite(ledpin1, HIGH);
    else digitalWrite(ledpin1, LOW);
  }
}