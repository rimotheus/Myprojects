#include "secrets.h"
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include "WiFi.h"

// The MQTT topics that this device should publish/subscribe
#define AWS_IOT_PUBLISH_TOPIC   "cAIRE_1/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "cAIRE_1/sub"

// DHT22 sensor setup
#define DHT_PIN 4           // Pin where the DHT sensor is connected
#define DHT_TYPE DHT22      // Define the sensor type (DHT22)

// Create an instance of the DHT sensor
DHT dhtSensor(DHT_PIN, DHT_TYPE);

// MQTT client and WiFi client
WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(256);

void connectAWS() {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(1000);

    Serial.println("Scanning for WiFi networks...");
    int numNetworks = WiFi.scanNetworks();

    if (numNetworks == 0) {
        Serial.println("No WiFi networks found.");
        return;
    }

    Serial.println("Available WiFi networks:");
    String bestSSID = "";
    String bestPassword = "";
    int bestSignal = -1000;  // Start with a very low signal strength

    for (int i = 0; i < numNetworks; i++) {
        String foundSSID = WiFi.SSID(i);
        int signalStrength = WiFi.RSSI(i);

        Serial.printf("%d: %s (Signal Strength: %d dBm)\n", i + 1, foundSSID.c_str(), signalStrength);

        // Check if this SSID is in our list of known networks
        for (int j = 0; j < numWiFiNetworks; j++) {
            if (foundSSID == wifiNetworks[j].ssid && signalStrength > bestSignal) {
                bestSSID = wifiNetworks[j].ssid;
                bestPassword = wifiNetworks[j].password;
                bestSignal = signalStrength;
            }
        }
    }

    // If we found a known network, connect to the best one
    if (bestSSID != "") {
        Serial.printf("Connecting to best WiFi: %s\n", bestSSID.c_str());
        WiFi.begin(bestSSID.c_str(), bestPassword.c_str());

        int retries = 0;
        while (WiFi.status() != WL_CONNECTED && retries < 20) {
            delay(500);
            Serial.print(".");
            retries++;
        }

        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("\nFailed to connect to WiFi!");
            return;
        }

        Serial.println("\nWiFi Connected!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());

        // Configure TLS for AWS IoT
        net.setCACert(AWS_CERT_CA);
        net.setCertificate(AWS_CERT_CRT);
        net.setPrivateKey(AWS_CERT_PRIVATE);

        client.begin(AWS_IOT_ENDPOINT, 8883, net);
        client.onMessage(messageHandler);

        Serial.print("Connecting to AWS IoT...");
        while (!client.connect(THINGNAME)) {
            Serial.print(".");
            delay(100);
        }

        if (!client.connected()) {
            Serial.println("AWS IoT Timeout!");
            return;
        }

        client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
        Serial.println("AWS IoT Connected!");
    } else {
        Serial.println("No known WiFi networks found.");
    }
}

void publishMessage()
{
  // Check if the client is connected before trying to publish
  if (!client.connected()) {
    Serial.println("MQTT client not connected, reconnecting...");
    connectAWS(); // Reconnect to AWS IoT if not connected
  }

  // Read humidity and temperature from the DHT sensor
  float humidity = dhtSensor.readHumidity();
  float temperature = dhtSensor.readTemperature();

  // Debug: Print raw sensor values
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(", Temperature: ");
  Serial.println(temperature);

  // Check if any of the readings failed
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Prepare the JSON document
  StaticJsonDocument<200> doc;
  doc["time"] = millis();
  doc["humidity"] = humidity;
  doc["temperature"] = temperature;
  doc["DeviceID"] = "cAIRE_1"; // change according to its device id
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // Serialize JSON to buffer

  // Debug: Print the JSON message before publishing
  Serial.print("Publishing message: ");
  Serial.println(jsonBuffer);

  // Publish to AWS IoT
  if (!client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer)) {
    Serial.println("Failed to publish message");
  } else {
    Serial.println("Message published");
  }
}

void messageHandler(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
}

void setup() {
  Serial.begin(9600);
  dhtSensor.begin();  // Initialize the DHT sensor
  connectAWS();
}

void loop() {
  // Check if MQTT is connected and reconnect if necessary
  if (!client.connected()) {
    connectAWS();
  }

  // Publish sensor data every 30 seconds
  publishMessage();
  client.loop();
  delay(120000);  // Delay for 30 seconds (30000 milliseconds)
}