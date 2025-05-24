// web_handlers.h
#ifndef WEB_HANDLERS_H
#define WEB_HANDLERS_H

#include <WebServer.h> // Required for WebServer object

// Forward declaration of the WebServer object
extern WebServer server;

// Declare global variables that are modified by handlers
extern unsigned long startTime;
extern String groundSpeedOption;
extern int groundSpeedValue;
extern String bladeOption;
extern int bladeValue;

// Function to get client info as JSON (declared here, defined in .cpp)
String getClientInfoJson();

// Function to set up all web server routes (declared here, defined in .cpp)
void setupWebRoutes();

#endif