#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include "DHT.h"
#include "config.h"
#include <Ticker.h>
//para el bot de telegram
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

// ---------------------------
// Variables globales
// ---------------------------
WiFiServer server(80);

const int ledPin = LED_BUILTIN;
bool ledState = HIGH;

//BMP180
Adafruit_BMP085 bmp;
bool bmp_ok = false;
float bmp_temp = NAN;
float bmp_pressure = NAN;

//DHT11
#define DHT11_PIN D5  // GPIO14
DHT dht11(DHT11_PIN, DHT11);
bool dht11_ok = false;

float dht11_temp = NAN;
float dht11_hum = NAN;

//PIR
#define PIR_PIN D6        // GPIO12
bool pir_state = false;
unsigned int pir_trigger_count = 0;

//Radar
#define RADAR_PIN D7      // GPIO13
bool radar_state = false;
unsigned int radar_trigger_count = 0;

//Buzzer
#define BUZZER_PIN D0     // GPIO16
unsigned long buzzer_end_time = 0;
unsigned long buzzer_until = 0;

// --- Variables para Telegram ---
WiFiClientSecure clientTCP;
String BOTtoken = "***REMOVED***";
String CHAT_ID = "***REMOVED***";
UniversalTelegramBot bot(BOTtoken, clientTCP);

// Cola de mensajes
const int MAX_COLA = 5;
String colaMensajes[MAX_COLA];
int colaInicio = 0;
int colaFin = 0;
bool envioEnCurso = false;


//watchdog
Ticker watchdog;

bool loggin = true;

// Modo sentinela
bool modoSentinela = false;

// Control de actualización del bot de telegam
int botRequestDelay = 1000;
unsigned long lastTimeBotRan = 0;

// Para controlar el envío de alertas
unsigned long lastPIRAlertTime = 0;
unsigned long lastRadarAlertTime = 0;
unsigned long lastBothAlertTime = 0;
const unsigned long ALERT_INTERVAL = 60000;  // 1 minuto


//Funcion para loggin
void logInfo(String logInfo){
  if (loggin == true){
    Serial.println(logInfo);
  }
}

// ---------------------------
// SETUP
// ---------------------------
void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);

  pinMode(PIR_PIN, INPUT);
  pinMode(RADAR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW); // buzzer apagado al inicio


  connectWiFi();
  initSensors();

  // Configura NTP (UTC -3 )
  configTime(-3 * 3600, 0, "pool.ntp.org");

  server.begin();
  logInfo("fin setup");

  initTelegram();


}

// ---------------------------
// LOOP
// ---------------------------
void loop() {
  heartBeat();

  
  WiFiClient client = server.available();
  if (client) {
    handleClient(client);
  }

  updateSensorsAndBuzzer();

  updateTelegramBot();

}

// ---------------------------
// FUNCIONES DE SETUP
// ---------------------------
void connectWiFi() {
  Serial.printf("Conectando a %s\n", ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  logInfo("\nWiFi conectado");
  //Serial.println("\nWiFi conectado");
  Serial.print("IP local: ");
  Serial.println(WiFi.localIP());
  //logInfo(WiFi.localIP());

  //watchdog
  //watchdog.attach(60, resetFunc);  // 60 segundos sin desactivarse = reinicio

}

  void initTelegram() {
  clientTCP.setInsecure(); // Si no querés el certificado (más simple para el Wemos)
  // clientTCP.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Si querés validar certificado
}

void initSensors() {
  //BMP180
  if (bmp.begin()) {
    bmp_ok = true;
  } else {
    logInfo("⚠️ BMP180 no encontrado o fallando.");
//    Serial.println("⚠️ BMP180 no encontrado o fallando.");
    bmp_ok = false;
  }

//init DHT11
  dht11.begin();

}

//funcion para el watchdog
void resetFunc() {
  logInfo("⚠️ Watchdog activado: reiniciando...");
  ESP.restart();
}


// ---------------------------
// FUNCIONES DE LECTURA
// ---------------------------
//////////
//BMP180//
//////////
//update valores leidos
void updateBMP180() {
  if (!bmp_ok) {
    return;  // El sensor no está disponible, no hacemos nada
  }

  logInfo("Leyendo BMP180...");
  bmp_temp = bmp.readTemperature();
  yield();
  bmp_pressure = bmp.readPressure() / 100.0; // hPa
  yield();

  logInfo("BMP180 leído ok");
}

//solo formatea data del BMP180 para html
String readBMP180() {
  if (!bmp_ok) {
    return "<h2>BMP180</h2><p style='color:red;'>Sensor no disponible</p>";
  }

  if (isnan(bmp_temp) || isnan(bmp_pressure)) {
    return "<h2>BMP180</h2><p style='color:red;'>Error en la lectura</p>";
  }

  String data = "<h2>BMP180</h2>";
  data += "<p>Temperatura: " + String(bmp_temp, 1) + " &deg;C</p>";
  data += "<p>Presión: " + String(bmp_pressure, 1) + " hPa</p>";
  return data;
}

//solo fortmatea data del BMP180 para el json
String readBMP180Json() {
  if (!bmp_ok) {
    return "{\"error\":\"Sensor no disponible\"}";
  }

  if (isnan(bmp_temp) || isnan(bmp_pressure)) {
    return "{\"error\":\"Lectura invalida\"}";
  }

  String json = "{";
  json += "\"temperatura\":" + String(bmp_temp, 1) + ",";
  json += "\"presion\":" + String(bmp_pressure, 1);
  json += "}";

  return json;
}

///////////
// DHT11 //
//////////
//udapte valores leidos del DHT11
void updateDHT11() {
  logInfo("Leyendo DHT11...");
  dht11_temp = dht11.readTemperature();
  yield();
  dht11_hum = dht11.readHumidity();
  yield();

  if (isnan(dht11_temp) || isnan(dht11_hum)) {
    logInfo("DHT11 error de lectura");
  } else {
    logInfo("DHT11 leído ok");
  }
}

//solo formatea data del DHT11 para html
String readDHT11() {
  if (isnan(dht11_temp) || isnan(dht11_hum)) {
    return "<h2>DHT11</h2><p style='color:red;'>Error en la lectura</p>";
  }
  String data = "<h2>DHT11</h2>";
  data += "<p>Temperatura: " + String(dht11_temp, 1) + " &deg;C</p>";
  data += "<p>Humedad: " + String(dht11_hum, 1) + " %</p>";
  return data;
}

//solo formatea data del DHT11 para el JSON
String readDHT11Json() {
  if (isnan(dht11_temp) || isnan(dht11_hum)) {
    return "{\"error\":\"Lectura invalida\"}";
  }
  String json = "{";
  json += "\"temperatura\":" + String(dht11_temp, 1) + ",";
  json += "\"humedad\":" + String(dht11_hum, 1);
  json += "}";
  return json;
}

///////////
// HORA ///
///////////
//formateo de la hora
String getFormattedTime() {
  time_t now = time(nullptr);
  struct tm * timeinfo = localtime(&now);

  char buffer[30];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

  return String(buffer);
}



// ---------------------------
// FUNCIONES WEB
// ---------------------------
//maneja el enceder/apagar el led
void handleRequest(String req) { 
  //logInfo("Procesando request: " + req");
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
    //Serial.println("Servidor activo, esperando cliente...");
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
  json += "\"pir\":\"" + String(pir_state ? "👤" : "❌") + "\",";
  json += "\"radar\":\"" + String(radar_state ? "🚶‍♂️" : "❌") + "\",";
  json += "\"alarma\":\"" + String((pir_state && radar_state) ? "🥷" : (digitalRead(BUZZER_PIN) ? "🔔" : "🔕")) + "\"";
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

void updateData(){
  updateDHT11();
  updateBMP180();
}


void updateSensorsAndBuzzer() {
  bool pir_now = digitalRead(PIR_PIN);
  bool radar_now = digitalRead(RADAR_PIN);
  pir_state = pir_now;
  radar_state = radar_now;

  unsigned long now = millis();

  // --- Manejo del buzzer ---
  if (pir_now && radar_now) {
    buzzer_until = now + 30000;  // 30 segundos si ambos
  } 
  else if (pir_now || radar_now) {
    if (buzzer_until < now) {
      buzzer_until = now + 10000;  // 10 segundos si uno solo
    }
  }

  if (now < buzzer_until) {
    digitalWrite(BUZZER_PIN, HIGH);
  } else {
    digitalWrite(BUZZER_PIN, LOW);
  }

  // --- Manejo de alertas por Telegram ---
  if (modoSentinela) {
    if (pir_now && radar_now) {
      if (now - lastBothAlertTime > ALERT_INTERVAL) {
        enviarAlertaMovimiento("🥷 Alerta: PIR y radar detectaron movimiento");
        lastBothAlertTime = now;
      }
    }
    else if (pir_now) {
      if (now - lastPIRAlertTime > ALERT_INTERVAL) {
        enviarAlertaMovimiento("👤 Alerta: PIR detectó movimiento");
        lastPIRAlertTime = now;
      }
    }
    else if (radar_now) {
      if (now - lastRadarAlertTime > ALERT_INTERVAL) {
        enviarAlertaMovimiento("🚶‍♂️ Alerta: Radar detectó movimiento");
        lastRadarAlertTime = now;
      }
    }
  }
}



void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Usuario no autorizado", "");
      continue;
    }

    String text = bot.messages[i].text;
    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String welcome = "Hola " + from_name + ", comandos disponibles:\n";
      welcome += "/sentinela - activar modo sentinela\n";
      welcome += "/descanso - desactivar modo sentinela\n";
      bot.sendMessage(CHAT_ID, welcome, "");
    }
    else if (text == "/sentinela") {
      modoSentinela = true;
      bot.sendMessage(CHAT_ID, "Modo sentinela activado 🟢", "");
    }
    else if (text == "/descanso") {
      modoSentinela = false;
      bot.sendMessage(CHAT_ID, "Modo descanso activado 💤", "");
    }
  }
}


void updateTelegramBot() {
  if (millis() > lastTimeBotRan + botRequestDelay) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}


void enviarAlertaMovimiento(String mensaje) {
  if (modoSentinela) {
    bot.sendMessage(CHAT_ID, mensaje, "");
  }
}


bool colaEstaVacia() {
  return colaInicio == colaFin;
}

bool colaEstaLlena() {
  return ((colaFin + 1) % MAX_COLA) == colaInicio;
}

void encolarMensaje(String msg) {
  if (!colaEstaLlena()) {
    colaMensajes[colaFin] = msg;
    colaFin = (colaFin + 1) % MAX_COLA;
    logInfo("Mensaje encolado: " + msg);
  } else {
    logInfo("⚠️ Cola de mensajes llena. Se descarta: " + msg);
  }
}

String desencolarMensaje() {
  if (!colaEstaVacia()) {
    String msg = colaMensajes[colaInicio];
    colaInicio = (colaInicio + 1) % MAX_COLA;
    return msg;
  }
  return "";
}

