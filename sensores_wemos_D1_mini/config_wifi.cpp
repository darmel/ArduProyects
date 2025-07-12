#include "config_wifi.h"
#include <WiFiManager.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

//String BOTtoken = "";
//String CHAT_ID = "";


bool iniciarWiFiYConfig() {
       // Reinicia para lanzar el portal

  Serial.println("Inicia iniciaWiFiYConfig");
  WiFiManager wm;

  //wm.resetSettings();  // Borra credenciales WiFi anteriores
//ESP.restart();

  WiFiManagerParameter custom_token("bot_token", "Telegram Bot Token", "", 70);
  WiFiManagerParameter custom_chat("chat_id", "Telegram Chat ID", "", 30);

  wm.addParameter(&custom_token);
  wm.addParameter(&custom_chat);

  if (!LittleFS.begin()) {
    Serial.println("Error al montar LittleFS");
  }

  // Intenta leer config existente
  File f = LittleFS.open("/config.json", "r");
  if (f) {
    StaticJsonDocument<512> doc;
    DeserializationError err = deserializeJson(doc, f);
    if (!err) {
      BOTtoken = doc["bot_token"] | "";
      CHAT_ID = doc["chat_id"] | "";
    }
    f.close();
  }

    bool needsConfigPortal = false;


  // Comprobar SSID
  if (WiFi.SSID() == "" || WiFi.SSID() == "<unknown ssid>") {
    Serial.println("⚠️ No hay SSID válido guardado");
    needsConfigPortal = true;
  }

  // Comprobar Token y Chat ID
  if (BOTtoken == "" || CHAT_ID == "") {
    Serial.println("⚠️ No hay token/chat_id válidos guardados");
    needsConfigPortal = true;
  }

  if (needsConfigPortal) {
    Serial.println("🚀 Lanzando portal de configuración...");
    if (!wm.autoConnect("Wemos_Config")) {
      Serial.println("❌ Falló la conexión o el portal");
      return false;
    }

    BOTtoken = custom_token.getValue();
    CHAT_ID = custom_chat.getValue();

    StaticJsonDocument<512> doc;
    doc["bot_token"] = BOTtoken;
    doc["chat_id"] = CHAT_ID;

    File f = LittleFS.open("/config.json", "w");
    if (f) {
      serializeJson(doc, f);
      f.close();
      Serial.println("✅ Configuración guardada correctamente");
    } else {
      Serial.println("❌ No se pudo escribir config.json");
      return false;
    }
  } else {
    // Conectarse con credenciales guardadas
    Serial.print("Intentando conectar con SSID guardado: ");
    Serial.println(WiFi.SSID());

    wm.setConnectTimeout(20);
    if (!wm.autoConnect()) {
      Serial.println("❌ No se pudo conectar con el WiFi guardado");
      return false;
    }

//    BOTtoken = storedToken;
  //  CHAT_ID = storedChatID;

    Serial.println("✅ Configuración cargada de LittleFS");
    Serial.println(BOTtoken);
    Serial.println(CHAT_ID);
  }

  Serial.println("fin iniciarWiFiYConfig");
  return true;
}
