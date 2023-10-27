#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>

ESP8266WebServer server(8080);
DHT dht14(D4, DHT11);

#define LED_PIN D6

void setLedStatus(bool isOn) {
  digitalWrite(LED_PIN, isOn ? HIGH : LOW);
}


void getTempHum(float& temperature, float& humidity) {
  temperature = dht14.readTemperature();
  humidity = dht14.readHumidity();
}

void sendHtmlResponse(float temperature, float humidity, bool isLedOn) {
  String html = "<html><body style='display: flex; justify-content: left; height: 50vh;'>";
  html += "<div style='text-align: center; background-color: #fffff; border: 5px solid #ccc; padding: 20px; border-radius: 10px;'>";
  html += "<h1>Temperature: " + String(temperature, 2) + " C</h1>";
  html += "<h1>Humidity: " + String(humidity, 2) + "%</h1>";
  html += "<h1>LED Status: " + String(isLedOn ? "On" : "Off") + "</h1>";
   html += "<div style='display: flex; justify-content: center;'>";
  html += "<form action='/TurnOn' method='get'><input type='submit' value='Turn On'></form>";
  html += "<span style='margin: 0 5px;'></span>";
  html += "<form action='/TurnOff' method='get'><input type='submit' value='Turn Off'></form>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}


void sendJsonResponse(float temperature, float humidity) {
  String json = "{\"temperature\":" + String(temperature, 2) + ",\"humidity\":" + String(humidity, 2) + "}";

  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin("iPhone", "4321zszs");
  pinMode(LED_PIN, OUTPUT);
  dht14.begin();

  server.on("/", HTTP_GET, []() {
    float temperature, humidity;
    getTempHum(temperature, humidity);
    sendHtmlResponse(temperature, humidity, false);
  });

  server.on("/TurnOn", HTTP_GET, []() {
    setLedStatus(true);
    server.sendHeader("Location", "/");
    server.send(302, "text/plain", "");
  });

  server.on("/TurnOff", HTTP_GET, []() {
    setLedStatus(false);
    server.sendHeader("Location", "/");
    server.send(302, "text/plain", "");
  });

  server.on("/TempHum", HTTP_GET, []() {
    float temperature, humidity;
    getTempHum(temperature, humidity);
    sendJsonResponse(temperature, humidity);
  });

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}