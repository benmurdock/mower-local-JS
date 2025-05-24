// web_handlers.cpp

#include "web_handlers.h"
#include "html_content.h" // Needed for htmlPage string
#include <esp_wifi.h>     // For esp_wifi_ap_get_sta_list

// Global variables declared in mower_control.ino and made accessible via extern in web_handlers.h
// No need to redeclare them here, just use them directly as they are extern.

// Handler for the root path
void handleRoot() {
    server.send(200, "text/html", htmlPage);
}

// Handler for ground speed button press
void handleGroundSpeed() {
    if (server.hasArg("state")) {
        int state = server.arg("state").toInt();
        if (state == 0) {
            groundSpeedOption = "Ground Speed: Slow";
            groundSpeedValue = 100;
        } else {
            groundSpeedOption = "Ground Speed: Fast";
            groundSpeedValue = 200;
        }
        Serial.print("Ground Speed: ");
        Serial.println(groundSpeedValue);
        server.send(200, "text/plain", "OK");
    } else {
        server.send(400, "text/plain", "Bad Request");
    }
}

// Handler for blade button press
void handleBlade() {
    if (server.hasArg("state")) {
        int state = server.arg("state").toInt();
        if (state == 0) {
            bladeOption = "Blade: off";
            bladeValue = 0;
        } else if (state == 1) {
            bladeOption = "Blade: 5000 fpm";
            bladeValue = 5000;
        } else {
            bladeOption = "Blade: 12000 fpm";
            bladeValue = 12000;
        }
        Serial.print("Blade: ");
        Serial.println(bladeValue);
        server.send(200, "text/plain", "OK");
    } else {
        server.send(400, "text/plain", "Bad Request");
    }
}

// Handler for uptime request
void handleUptime() {
    unsigned long uptimeSeconds = (millis() - startTime) / 1000;
    server.send(200, "text/plain", String(uptimeSeconds));
}

// Handler for joystick data
void handleJoystick() {
    if (server.hasArg("r") && server.hasArg("angle") && server.hasArg("left") && server.hasArg("right")) {
        float r = server.arg("r").toFloat();
        float angle = server.arg("angle").toFloat();
        float leftTrack = server.arg("left").toFloat();
        float rightTrack = server.arg("right").toFloat();

        Serial.printf("Joystick: R: %.2f, Angle: %.2f deg | Left Track: %.2f, Right Track: %.2f\n", r, angle, leftTrack, rightTrack);
        server.send(200, "text/plain", "OK");
    } else {
        server.send(400, "text/plain", "Bad Request");
    }
}

// Function to get client information (RSSI) and format as JSON
String getClientInfoJson() {
    wifi_sta_list_t stationList;
    esp_wifi_ap_get_sta_list(&stationList); // Get the list of connected stations

    String jsonOutput = "{";
    jsonOutput += "\"numClients\": " + String(stationList.num);
    jsonOutput += ", \"clients\": [";

    for (int i = 0; i < stationList.num; i++) {
        wifi_sta_info_t station = stationList.sta[i];
        if (i > 0) {
            jsonOutput += ",";
        }
        jsonOutput += "{";
        jsonOutput += "\"mac\": \"" + String(station.mac[0], HEX) + ":" +
                        String(station.mac[1], HEX) + ":" +
                        String(station.mac[2], HEX) + ":" +
                        String(station.mac[3], HEX) + ":" +
                        String(station.mac[4], HEX) + ":" +
                        String(station.mac[5], HEX) + "\"";
        jsonOutput += ", \"rssi\": " + String(station.rssi);
        jsonOutput += "}";
    }
    jsonOutput += "]}";
    return jsonOutput;
}

// This wrapper function is needed because server.on takes a void() callback,
// and getClientInfoJson() returns a String.
// Moved this definition above setupWebRoutes()
void handleClients() {
    String json = getClientInfoJson();
    server.send(200, "application/json", json);
}

// Function to set up all web server routes
void setupWebRoutes() {
    server.on("/", HTTP_GET, handleRoot);
    server.on("/groundSpeed", HTTP_GET, handleGroundSpeed);
    server.on("/blade", HTTP_GET, handleBlade);
    server.on("/uptime", HTTP_GET, handleUptime);
    server.on("/joystick", HTTP_GET, handleJoystick);
    server.on("/clients", HTTP_GET, handleClients);
}