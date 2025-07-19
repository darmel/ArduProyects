#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include "DHT.h"
//#include "config_secrets.h" 

#include <Ticker.h>
//para el bot de telegram
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
//para el wifi manager
#include <LittleFS.h>
#include "config_wifi.h"
//para los sensores de temperatura, humedad y presion
#include "sensores.h"
//para sensores de movimiento
#include "movimiento.h"

//para las fucniones utilitarias
#include "utils.h"

//para el manejo del bot y todo lo que involucra
#include "telegram_bot.h"

// ---------------------------
// Variables globales
// ---------------------------
WiFiServer server(80);

const int ledPin = LED_BUILTIN;
bool ledState = HIGH;

//PIR
unsigned int pir_trigger_count = 0;

//Radar
unsigned int radar_trigger_count = 0;

//Buzzer
unsigned long buzzer_end_time = 0;

//watchdog
Ticker watchdog;

// ---------------------------
// SETUP
// ---------------------------
void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);

  Serial.println("desde el setUp, inicia iniciarWiFiYConfig");
  //connectWiFi();
  if (!iniciarWiFiYConfig()) {
    Serial.println("No se pudo configurar WiFi. Reiniciando...");
    ESP.restart();
  }
  Serial.println("desde el setUp, termino iniciarWiFiYConfig");
  Serial.println(BOTtoken);
  Serial.println(CHAT_ID);

  initTelegram();
  Serial.println("desde el setUp, termino init telegram");
  
  //Serial.print("Conectado a: ");
  //Serial.println(WiFi.SSID());

  initSensors(); //sensores de temperatura

  initMovimiento(); //sensores de movimiento

  // Configura NTP (UTC -3 )
  configTime(-3 * 3600, 0, "pool.ntp.org");

  server.begin();
    delay(1000);
    Serial.print("Esperando conexión WiFi estable...");
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
  Serial.println("✅ WiFi conectado");

  startTelegramBot();

  logInfo("fin setup");

}

// ---------------------------
// LOOP
// ---------------------------
void loop() {
  heartBeat();

  checkWiFiAndReconnect();

  resetWatchdog();

  WiFiClient client = server.available();
  if (client) {
    handleClient(client);
  }

  //logInfo("-----Update MOvimiento empieza desde el loop");
  updateMovimiento();       // PIR y radar
  
  //logInfo("---------empieza evaluar alerta desde el loop");
  evaluarAlerta();          // Telegram y buzzer

  //logInfo("----------------empieza update telegramBot desde el loop");
  updateTelegramBot();

}

// ---------------------------
// FUNCIONES DE SETUP
// ---------------------------
void connectWiFi() {
  Serial.printf("Conectando a wifi \n");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  logInfo("\nWiFi conectado");
  Serial.print("IP local: ");
  Serial.println(WiFi.localIP());

  } 

//funcion para el watchdog
void resetFunc() {
  logInfo("⚠️ Watchdog activado: reiniciando...");
  ESP.restart();
}

// ---------------------------
// FUNCIONES WEB
// ---------------------------
//maneja el enceder/apagar el led
void handleRequest(String req) { 
  Serial.println("Procesando request: " + req);

  if (req.indexOf("/ON") != -1) {
    ledState = LOW;
  } else if (req.indexOf("/OFF") != -1) {
    ledState = HIGH;
  }

  digitalWrite(ledPin, ledState);
}

void sendWebPage(WiFiClient client) {
  String html = "<!DOCTYPE html><html><head><meta charset='utf-8'>";
  
  // JavaScript con fetch inicial y botones
  html += "<script>";
  html += "function fetchData() {";
  html += "fetch('/data').then(r => r.json()).then(data => {";
  html += "document.getElementById('led').innerText = data.led;";
  html += "document.getElementById('bmp').innerHTML = 'Temperatura: ' + data.bmp.temperatura + ' °C<br>Presión: ' + data.bmp.presion + ' hPa';";
  html += "document.getElementById('dht').innerHTML = 'Temperatura: ' + data.dht.temperatura + ' °C<br>Humedad: ' + data.dht.humedad + ' %';";
  html += "document.getElementById('hora').innerText = data.hora;";

  html += "document.getElementById('pir').innerText = data.pir;";
  html += "document.getElementById('radar').innerText = data.radar;";
  html += "document.getElementById('alarma').innerText = data.alarma;";

  html += "});";
  html += "}";
  html += "function toggleLed(cmd) {";
  html += "fetch('/' + cmd).then(() => fetchData());";
  html += "}";
  html += "setInterval(fetchData, 2000);";
  html += "window.onload = fetchData;";
  html += "</script>";
  
  html += "<title>Wemos Dashboard</title></head><body>";
  html += "<h1>Dashboard Wemos</h1>";

  html += "<h2>Hora</h2>";
  html += "<p id='hora'>Cargando...</p>";

  html += "<h2>LED</h2>";
  html += "<p id='led'>" + String((ledState == LOW) ? "ENCENDIDO" : "APAGADO") + "</p>";
  html += "<p>";
  html += "<button onclick=\"toggleLed('ON')\">Encender</button>";
  html += "<button onclick=\"toggleLed('OFF')\">Apagar</button>";
  html += "</p>";

  html += "<h2>BMP180</h2>";
  html += "<p id='bmp'>Cargando...</p>";

  html += "<h2>DHT11</h2>";
  html += "<p id='dht'>Cargando...</p>";

  html += "<h2>Detección</h2>";
  html += "<p>PIR: <span id='pir'>Cargando...</span></p>";
  html += "<p>Radar: <span id='radar'>Cargando...</span></p>";
  html += "<p>Alarma: <span id='alarma'>Cargando...</span></p>";

  
  html += "</body></html>";

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.print("Content-Length: ");
  client.println(html.length());
  client.println();
  client.print(html);
  logInfo("web enviada");
}

void heartBeat()
{
  static unsigned long lastLog = 0;
  if (millis() - lastLog > 300000)   // cada 5 min
  {
    logInfo("Servidor activo, esperando cliente...");
    lastLog = millis();
  }
}

void sendJson(WiFiClient client) 
{
  String json = "{";
  json += "\"hora\":\"" + getFormattedTime() + "\",";
  json += "\"led\":\"" + String((ledState == LOW) ? "ENCENDIDO" : "APAGADO") + "\",";
  json += "\"bmp\":" + readBMP180Json() + ",";
  json += "\"dht\":" + readDHT11Json() + ",";
  json += "\"pir\":\"" + String(getPIR() ? "👤" : "❌") + "\",";
  json += "\"radar\":\"" + String(getRadar() ? "🚶‍♂️" : "❌") + "\",";
  json += "\"alarma\":\"" + String((getPIR() && getRadar()) ? "🥷" : (getBuzzer() ? "🔔" : "🔕")) + "\"";
  json += "}";

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: application/json");
  client.println("Connection: close");
  client.print("Content-Length: ");
  client.println(json.length());
  client.println();
  client.print(json);
  client.flush();
  delay(10);
  client.stop();
  logInfo("JSON enviado");
}

//Funcion para manejar las requests, si es de json solo o de todo el sitio
void handleClient(WiFiClient client) {
  logInfo("Cliente conectado. Esperando datos...");
  unsigned long timeout = millis() + 5000; // 5 segundos máximo
  while (!client.available() && millis() < timeout) {
    delay(1);
    yield();
  }

  if (!client.available()) {
    logInfo("⚠️ Timeout esperando datos, cerrando cliente");
    client.stop();
    return;
  }

  String req = client.readStringUntil('\r');
  client.flush();

  logInfo("Procesando request: " + req);
  handleRequest(req); //request del led enceder o apagar
  updateData(); //actualizo datos de los sensores


  // Decide qué responder según el request
  if (req.indexOf("/data") != -1) {
    sendJson(client);
  } else if (req.indexOf("/favicon.ico") != -1) {
    // Responder algo mínimo para el favicon, así no deja conexiones colgadas
    client.println("HTTP/1.1 204 No Content");
    client.println("Connection: close");
    client.println();
  } else {
    sendWebPage(client);
  }

  client.flush(); //asegura el envio
  client.stop();
  logInfo("Cliente cerrado");

  watchdog.detach();
  watchdog.attach(60, resetFunc);

}

void resetWatchdog() {
  watchdog.detach();
  watchdog.attach(60, resetFunc);
}

void checkWiFiAndReconnect() {
  if (WiFi.status() != WL_CONNECTED) {
    logInfo("⚠️ WiFi desconectado. Intentando reconectar...");
    WiFi.disconnect();  // fuerza reinicio de la conexión
    WiFi.reconnect();

    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
      delay(500);
      Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
      logInfo("\n✅ WiFi reconectado");
      Serial.print("Nueva IP: ");
      Serial.println(WiFi.localIP());
      bot->sendMessage(CHAT_ID, "Conexión reestabelcida", "");
    } else {
      logInfo("\n❌ No se pudo reconectar a WiFi");
    }
  }
}
