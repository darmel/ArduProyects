#include <ESP8266WiFi.h>
#include "config.h"

WiFiServer server(80);

const int ledPin = LED_BUILTIN;  // D4 / GPIO2 (activo LOW)
bool ledState = HIGH;  // Apagado al inicio

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);

  Serial.begin(115200);
  delay(10);

  Serial.println();
  Serial.printf("Conectando a %s\n", ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado");
  Serial.print("IP local: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (!client) return;

  while (!client.available()) {
    delay(1);
  }

  String req = client.readStringUntil('\r');
  client.flush();

  if (req.indexOf("/ON") != -1) {
    ledState = LOW;
  } else if (req.indexOf("/OFF") != -1) {
    ledState = HIGH;
  }

  digitalWrite(ledPin, ledState);

  String html = "<!DOCTYPE html><html><head><meta charset='utf-8'>";
  html += "<meta http-equiv='refresh' content='5'>";
  html += "<title>LED Status</title></head><body>";
  html += "<h1>Estado del LED: ";
  html += (ledState == LOW) ? "ENCENDIDO" : "APAGADO";
  html += "</h1>";
  html += "<p><a href='/ON'>Encender</a></p>";
  html += "<p><a href='/OFF'>Apagar</a></p>";
  html += "</body></html>";

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();
  client.println(html);
}
