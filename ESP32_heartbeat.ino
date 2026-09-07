#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>

// =========================
// WiFi Settings
// =========================
const char* ssid = "your_wifi_name";
const char* password = "your_wifi_pass";

// =========================
// Odoo Settings
// =========================
// Odoo server IP address
const char* odooServer = "http://192.168.50.176:8080";

const char* deviceName = "ESP32-S3";
const char* apiToken = "esp32-secret-token";

// =========================
// Web Server
// =========================
WebServer server(80);

// Virtual device state
bool deviceState = false;

// Heartbeat timer
unsigned long lastHeartbeat = 0;
const unsigned long heartbeatInterval = 10000; // 10 seconds


// =========================
// Root
// =========================
void handleRoot() {

  String html = "<!DOCTYPE html>";
  html += "<html>";
  html += "<head>";
  html += "<title>ESP32-S3</title>";
  html += "</head>";
  html += "<body>";

  html += "<h1>ESP32-S3 is Working!</h1>";

  html += "<p>Status: ";
  html += deviceState ? "ON" : "OFF";
  html += "</p>";

  html += "<p>";
  html += "<a href='/on'><button>ON</button></a> ";
  html += "<a href='/off'><button>OFF</button></a>";
  html += "</p>";

  html += "</body>";
  html += "</html>";

  server.send(200, "text/html", html);
}


// =========================
// ON
// =========================
void handleOn() {

  deviceState = true;

  server.send(
    200,
    "application/json",
    "{\"message\":\"Device turned ON\",\"status\":\"ON\"}"
  );
}


// =========================
// OFF
// =========================
void handleOff() {

  deviceState = false;

  server.send(
    200,
    "application/json",
    "{\"message\":\"Device turned OFF\",\"status\":\"OFF\"}"
  );
}


// =========================
// STATUS
// =========================
void handleStatus() {

  String statusText = deviceState ? "ON" : "OFF";

  String json = "{";
  json += "\"device\":\"ESP32-S3\",";
  json += "\"status\":\"" + statusText + "\",";
  json += "\"ip\":\"" + WiFi.localIP().toString() + "\"";
  json += "}";

  server.send(
    200,
    "application/json",
    json
  );
}


// =========================
// Send Heartbeat to Odoo
// =========================
void sendHeartbeat() {

  if (WiFi.status() != WL_CONNECTED) {

    Serial.println("WiFi disconnected!");
    return;
  }

  HTTPClient http;

  String url = String(odooServer) + "/iot/esp32/heartbeat";

  Serial.println();
  Serial.println("========== HEARTBEAT ==========");
  Serial.print("POST: ");
  Serial.println(url);

  http.begin(url);

  http.addHeader(
    "Content-Type",
    "application/json"
  );

  // Current ESP32 state
  String statusText = deviceState ? "ON" : "OFF";

  // ESP32 uptime
  unsigned long uptimeSeconds = millis() / 1000;

  // JSON
  String json = "{";

  json += "\"device\":\"";
  json += deviceName;
  json += "\",";

  json += "\"status\":\"";
  json += statusText;
  json += "\",";

  json += "\"ip\":\"";
  json += WiFi.localIP().toString();
  json += "\",";

  json += "\"uptime\":";
  json += String(uptimeSeconds);
  json += ",";

  json += "\"token\":\"";
  json += apiToken;
  json += "\"";

  json += "}";


  Serial.println("Request:");
  Serial.println(json);


  // POST to Odoo
  int httpCode = http.POST(json);

  Serial.print("HTTP Code: ");
  Serial.println(httpCode);


  if (httpCode > 0) {

    String response = http.getString();

    Serial.println("Odoo Response:");
    Serial.println(response);

  } else {

    Serial.print("Heartbeat failed: ");
    Serial.println(
      http.errorToString(httpCode)
    );
  }

  Serial.println("===============================");

  http.end();
}


// =========================
// Setup
// =========================
void setup() {

  Serial.begin(115200);

  delay(1000);

  Serial.println();
  Serial.println("==============================");
  Serial.println("ESP32-S3 IoT Starting...");
  Serial.println("==============================");


  // Connect WiFi
  WiFi.begin(
    ssid,
    password
  );

  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);

    Serial.print(".");
  }

  Serial.println();

  Serial.println("WiFi Connected!");

  Serial.print("ESP32 IP Address: ");
  Serial.println(
    WiFi.localIP()
  );


  // Web Server Routes
  server.on(
    "/",
    handleRoot
  );

  server.on(
    "/on",
    handleOn
  );

  server.on(
    "/off",
    handleOff
  );

  server.on(
    "/status",
    handleStatus
  );


  // Start Web Server
  server.begin();

  Serial.println("Web Server Started!");

  Serial.println();
  Serial.println("Endpoints:");

  Serial.println(
    "http://" + WiFi.localIP().toString() + "/"
  );

  Serial.println(
    "http://" + WiFi.localIP().toString() + "/on"
  );

  Serial.println(
    "http://" + WiFi.localIP().toString() + "/off"
  );

  Serial.println(
    "http://" + WiFi.localIP().toString() + "/status"
  );


  // Send first heartbeat immediately
  sendHeartbeat();

  lastHeartbeat = millis();
}


// =========================
// Loop
// =========================
void loop() {

  // Handle ESP32 Web Server
  server.handleClient();


  // Heartbeat every 10 seconds
  if (millis() - lastHeartbeat >= heartbeatInterval) {

    lastHeartbeat = millis();

    sendHeartbeat();
  }
}