#include "telegram_bot.h"
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include "config_wifi.h"  // para BOTtoken y CHAT_ID
#include "utils.h"
#include "telegram_bot.h"
#include "movimiento.h"    // setModoCentinela
#include "sensores.h"       // sensores y updateData
#include "utils.h"         // getFormattedTime, logInfo


WiFiClientSecure clientTCP;
UniversalTelegramBot* bot = nullptr;

// Para limitar la frecuencia de polling al bot
int botRequestDelay = 3000; // 3 segundos
unsigned long lastTimeBotRan = 0;

// Cola circular de mensajes (útil para alertas múltiples)
const int MAX_COLA = 5;
String colaMensajes[MAX_COLA];
int colaInicio = 0;
int colaFin = 0;
bool envioEnCurso = false;

// Inicializa el cliente TLS del bot (sin certificado)
void initTelegram() {
  clientTCP.setInsecure(); // No se valida certificado (más simple en ESP8266)
}

// Crea el bot con el token ya leído desde el config
// y envía el mensaje de bienvenida por Telegram
void startTelegramBot() {
  bot = new UniversalTelegramBot(BOTtoken, clientTCP);
  logInfo("UniversalTelegramBot creado");
  Serial.print("Conectado a: ");
  Serial.println(WiFi.SSID());

  // Enviar mensaje inicial
  bot->sendMessage(CHAT_ID, "Inicio Completo. estado Centinela ACTIVO", "");
  logInfo("Mensaje de INICIO enviado por Telegram");
}

// Revisa si hay nuevos mensajes de Telegram, y los procesa uno por uno
void updateTelegramBot() {
  if (millis() > lastTimeBotRan + botRequestDelay) {
    logInfo("updateTelegramBot revisando nuevos mensajes...");
    int numNewMessages = bot->getUpdates(bot->last_message_received + 1);

    while (numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot->getUpdates(bot->last_message_received + 1);
    }

    lastTimeBotRan = millis();
  }
}

// Procesa mensajes nuevos del bot, uno por uno
void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot->messages[i].chat_id);

    if (chat_id != CHAT_ID) {
      bot->sendMessage(chat_id, "Usuario no autorizado", "");
      continue;
    }

    String text = bot->messages[i].text;
    String from_name = bot->messages[i].from_name;
    logInfo("Mensaje recibido: " + text);

    if (text == "/start") {
      String welcome = "Hola " + from_name + ", comandos disponibles:\n";
      welcome += "/centinela - activar modo centinela\n";
      welcome += "/descanso - desactivar modo centinela\n";
      welcome += "/datos - info actual\n";
      welcome += "/json - info en formato JSON\n";
      bot->sendMessage(CHAT_ID, welcome, "");
    }
    else if (text == "/centinela") {
      setModoCentinela(true);
      bot->sendMessage(CHAT_ID, "Modo centinela activado 🟢", "");
      logInfo("Modo centinela activado 🟢");
    }
    else if (text == "/descanso") {
      setModoCentinela(false);
      bot->sendMessage(CHAT_ID, "Modo descanso activado 💤", "");
      logInfo("Modo centinela desactivado 💤");
    }
    else if (text == "/datos") {
      updateData(); // refrescar sensores
      String datos = "📅 Fecha/Hora: " + getFormattedTime() + "\n";
      datos += "🌡 DHT11 - Temp: " + String(dht11_temp, 1) + " °C, Hum: " + String(dht11_hum, 1) + " %\n";
      datos += "🌡 BMP180 - Temp: " + String(bmp_temp, 1) + " °C, Presión: " + String(bmp_pressure, 1) + " hPa\n";
      datos += "💻 IP local: http://" + WiFi.localIP().toString() + "\n";
      bot->sendMessage(CHAT_ID, datos, "");
      logInfo("Enviado /datos: " + datos);
    }
    else if (text == "/json") {
      updateData();
      String json = "{";
      json += "\"hora\":\"" + getFormattedTime() + "\",";
      json += "\"dht11_temp\":" + String(dht11_temp, 1) + ",";
      json += "\"dht11_hum\":" + String(dht11_hum, 1) + ",";
      json += "\"bmp_temp\":" + String(bmp_temp, 1) + ",";
      json += "\"bmp_presion\":" + String(bmp_pressure, 1);
      json += "}";
      bot->sendMessage(CHAT_ID, json, "");
      logInfo("Enviado /json por Telegram: " + json);
    }
  }
}

// ======================
// Cola de mensajes (opcional pero útil para mejoras futuras)
// ======================

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
