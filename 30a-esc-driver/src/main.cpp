#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESP32Servo.h>
#include <SPIFFS.h>

// -------------------------------------------------------
// Configuration
// -------------------------------------------------------

// WiFi credentials -- update these for your network
const char* WIFI_SSID = "YOUR_SSID";
const char* WIFI_PASS = "YOUR_PASSWORD";

// ESC configuration
const int ESC_PIN = 13;
const int ESC_MIN_US = 1000;   // pulse width for off/minimum
const int ESC_MAX_US = 2000;   // pulse width for full throttle
const int ESC_ARM_DELAY_MS = 3000;

// Server
const int SERVER_PORT = 80;

// -------------------------------------------------------
// Global state
// -------------------------------------------------------

Servo esc;
AsyncWebServer server(SERVER_PORT);

int currentSpeed = 0;      // 0-100 percent
bool motorEnabled = false;  // on/off toggle state

// -------------------------------------------------------
// ESC control
// -------------------------------------------------------

// Convert a 0-100 percentage to a pulse width in microseconds
int speedToMicroseconds(int percent) {
    percent = constrain(percent, 0, 100);
    return map(percent, 0, 100, ESC_MIN_US, ESC_MAX_US);
}

// Apply the current speed to the ESC (respects motorEnabled)
void applySpeed() {
    if (motorEnabled) {
        esc.writeMicroseconds(speedToMicroseconds(currentSpeed));
    } else {
        esc.writeMicroseconds(ESC_MIN_US);
    }
}

// Arm the ESC by holding minimum throttle signal
void armEsc() {
    Serial.println("Arming ESC...");
    esc.writeMicroseconds(ESC_MIN_US);
    delay(ESC_ARM_DELAY_MS);
    Serial.println("ESC armed.");
}

// -------------------------------------------------------
// Web server routes
// -------------------------------------------------------

void setupRoutes() {
    // Serve the control panel HTML from SPIFFS
    server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(SPIFFS, "/index.html", "text/html");
    });

    // Set motor speed: GET /speed?value=0-100
    server.on("/speed", HTTP_GET, [](AsyncWebServerRequest* request) {
        if (request->hasParam("value")) {
            currentSpeed = request->getParam("value")->value().toInt();
            currentSpeed = constrain(currentSpeed, 0, 100);
            applySpeed();
            String json = "{\"speed\":" + String(currentSpeed) + ",\"enabled\":" + (motorEnabled ? "true" : "false") + "}";
            request->send(200, "application/json", json);
        } else {
            request->send(400, "application/json", "{\"error\":\"missing value parameter\"}");
        }
    });

    // Toggle motor on/off: GET /toggle?state=on|off
    server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest* request) {
        if (request->hasParam("state")) {
            String state = request->getParam("state")->value();
            motorEnabled = (state == "on");
            applySpeed();
            String json = "{\"speed\":" + String(currentSpeed) + ",\"enabled\":" + (motorEnabled ? "true" : "false") + "}";
            request->send(200, "application/json", json);
        } else {
            request->send(400, "application/json", "{\"error\":\"missing state parameter\"}");
        }
    });

    // Get current status: GET /status
    server.on("/status", HTTP_GET, [](AsyncWebServerRequest* request) {
        String json = "{\"speed\":" + String(currentSpeed) + ",\"enabled\":" + (motorEnabled ? "true" : "false") + "}";
        request->send(200, "application/json", json);
    });
}

// -------------------------------------------------------
// Setup and loop
// -------------------------------------------------------

void setup() {
    Serial.begin(115200);
    Serial.println("\n30A ESC Driver starting...");

    // Initialize SPIFFS
    if (!SPIFFS.begin(true)) {
        Serial.println("ERROR: Failed to mount SPIFFS");
        return;
    }
    Serial.println("SPIFFS mounted.");

    // Initialize ESC servo output
    esc.attach(ESC_PIN, ESC_MIN_US, ESC_MAX_US);
    armEsc();

    // Connect to WiFi
    Serial.print("Connecting to WiFi");
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.print("Connected. IP address: ");
    Serial.println(WiFi.localIP());

    // Start web server
    setupRoutes();
    server.begin();
    Serial.println("Web server started on port 80.");
    Serial.println("Open your browser to the IP address above.");
}

void loop() {
    // AsyncWebServer handles requests in the background.
    // Nothing needed here for Phase 1.
}
