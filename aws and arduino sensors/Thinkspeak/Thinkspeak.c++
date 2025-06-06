#include <WiFiS3.h>
#include <DHT.h>
#include <ThingSpeak.h>

// Struct to hold WiFi credentials
struct WiFiCredentials {
  const char* ssid;
  const char* pass;
};

// WiFi Credentials List
WiFiCredentials wifiList[] = {

};
const int numWiFiNetworks = sizeof(wifiList) / sizeof(wifiList[0]); // Number of networks

// ThingSpeak Settings
const unsigned long myChannelNumber =;  // Replace with your ThingSpeak Channel ID
const char* myWriteAPIKey = ""; // Replace with your ThingSpeak Write API Key
const char* server = "";      // ThingSpeak Server Address

// DHT Sensor Settings
#define DHTPIN1 4       
#define DHTPIN2 2      
#define DHTTYPE DHT11   

DHT dht1(DHTPIN1, DHTTYPE); 
DHT dht2(DHTPIN2, DHTTYPE); 
WiFiClient client;        

// Timer Variables
unsigned long lastUpdate = 0;                 
const unsigned long updateInterval = 15000;   
const unsigned long retryDelay = 5000;        
const int maxRetries = 2;                     

// Function to connect to WiFi with fallback
void connectToWiFi() {
  Serial.println("Connecting to WiFi...");

  for (int i = 0; i < numWiFiNetworks; i++) {
    Serial.print("Attempting to connect to: ");
    Serial.println(wifiList[i].ssid);
    
    WiFi.begin(wifiList[i].ssid, wifiList[i].pass);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 10) {
      delay(1000);
      Serial.print(".");
      attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nWiFi connected!");
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
      return;
    }
    
    Serial.println("\nFailed to connect. Trying next network...");
  }

  Serial.println("All WiFi networks failed. Check credentials or signal strength.");
}

// Function to send data to ThingSpeak with retry logic
bool sendToThingSpeak(float temperature1, float humidity1, float temperature2, float humidity2) {
  ThingSpeak.setField(1, temperature1);  
  ThingSpeak.setField(2, humidity1);     
  ThingSpeak.setField(3, temperature2);  
  ThingSpeak.setField(4, humidity2);     

  int attempt = 0;
  int result;

  // Retry loop
  while (attempt <= maxRetries) {
    result = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    
    if (result == 200) {
      Serial.println("Data successfully sent to ThingSpeak.");
      Serial.print("Temperature1: "); Serial.print(temperature1);
      Serial.print("°C, Humidity1: "); Serial.print(humidity1);
      Serial.print("% | Temperature2: "); Serial.print(temperature2);
      Serial.print("°C, Humidity2: "); Serial.println(humidity2);
      return true; // Success
    } 
    
    Serial.print("Failed to send data (Attempt "); Serial.print(attempt + 1);
    Serial.print("). HTTP error code: "); Serial.println(result);

    if (attempt < maxRetries) {
      Serial.println("Retrying in 5 seconds...");
      delay(retryDelay);
    }

    attempt++;
  }

  Serial.println("Max retries reached. Data could not be sent.");
  return false; // Failed after retries
}

// Arduino Setup Function
void setup() {
  Serial.begin(9600);  
  dht1.begin();        
  dht2.begin();        
  connectToWiFi();      
  ThingSpeak.begin(client);
}

// Arduino Loop Function
void loop() {
  unsigned long currentTime = millis();

  if (currentTime - lastUpdate >= updateInterval) {
    lastUpdate += updateInterval; // Maintain fixed intervals

    float temperature1 = dht1.readTemperature(); 
    float humidity1 = dht1.readHumidity();       
    float temperature2 = dht2.readTemperature(); 
    float humidity2 = dht2.readHumidity();       

    if (isnan(temperature1) || isnan(humidity1) || isnan(temperature2) || isnan(humidity2)) {
      Serial.println("Failed to read from one or both DHT sensors!");
      return;
    }

    // Try sending data, but interval remains consistent
    sendToThingSpeak(temperature1, humidity1, temperature2, humidity2);
  }
}