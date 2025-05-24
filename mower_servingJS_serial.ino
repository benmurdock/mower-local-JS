// mower_control.ino

#include <WiFi.h>       // for wifi
#include <WebServer.h>  // for web server
#include <ESPmDNS.h>    // for accessing site via [name].local rather than ip address MDNS.begin([name])
#include <esp_wifi.h>   // for rssi measurement via esp_wifi_ap_get_sta_list

// Include custom headers

#include "html_content.h"   // For the HTML page string
#include "web_handlers.h"   // For web server route handlers

// WebServer object on port 80
const char* ssid = "ESP32 WIFI";
const char* password = "11111111";
WebServer server(80);

// Global Variables for website content (these need to be accessible by web_handlers.cpp)
// It's good practice to declare them here and use 'extern' in web_handlers.h
unsigned long startTime; // To track uptime
String groundSpeedOption = "Ground Speed: Slow";
int groundSpeedValue = 100;
String bladeOption = "Blade: off";
int bladeValue = 0;
unsigned long lastClientInfoUpdateTime = 0;
const long clientInfoUpdateInterval = 5000; // Update client info every 5 seconds


void setup() {
    Serial.begin(115200);

    // Set up ESP32 as an access point
    Serial.print("Setting up AP...");
    WiFi.softAP(ssid, password);
    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);

    // Set up mDNS
    if (!MDNS.begin("mower")) {
        Serial.println("Error setting up MDNS responder!");
    }
    Serial.println("MDNS responder started, hostname: mower.local");

    startTime = millis(); // Record start time for uptime

    // Call function to set up web server routes
    setupWebRoutes();

    // Start the server
    server.begin();
    Serial.println("HTTP server started");
}

void loop() {
    server.handleClient(); // Handle incoming client requests

    // Periodically print client info to Serial Monitor
    if (millis() - lastClientInfoUpdateTime >= clientInfoUpdateInterval) {
        wifi_sta_list_t stationList;
        esp_wifi_ap_get_sta_list(&stationList);

        Serial.printf("\n--- Connected Clients (%d) ---\n", stationList.num);
        if (stationList.num == 0) {
            Serial.println("No clients connected.");
        } else {
            for (int i = 0; i < stationList.num; i++) {
                wifi_sta_info_t station = stationList.sta[i];
                Serial.printf("Client %d: MAC: %02x:%02x:%02x:%02x:%02x:%02x, RSSI: %d dBm\n",
                                i + 1,
                                station.mac[0], station.mac[1], station.mac[2],
                                station.mac[3], station.mac[4], station.mac[5],
                                station.rssi);
            }
        }
        Serial.println("------------------------------");
        lastClientInfoUpdateTime = millis();
    }
}