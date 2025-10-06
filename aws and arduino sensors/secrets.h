// secrets.h
#include <pgmspace.h>

#define SECRET
#define THINGNAME ""
// Define a struct to store WiFi credentials
struct WiFiCredential {
    const char* ssid;
    const char* password;
};

// List of known WiFi networks
WiFiCredential wifiNetworks[] = {

};

// Number of WiFi networks
const int numWiFiNetworks = sizeof(wifiNetworks) / sizeof(wifiNetworks[0]);

const char AWS_IOT_ENDPOINT[] = "";

// Amazon Root CA 1
static const char AWS_CERT_CA[] PROGMEM = ;
 
// Device Certificate                                               //change this
static const char AWS_CERT_CRT[] PROGMEM = ;
 
// Device Private Key                                               //change this
static const char AWS_CERT_PRIVATE[] PROGMEM = ;