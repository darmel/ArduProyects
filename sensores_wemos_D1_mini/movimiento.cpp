#include "movimiento.h"
#include "utils.h"
#include <ESP8266WiFi.h> // Por si usás bot->sendMessage
#include "config_wifi.h" // CHAT_ID
#include <UniversalTelegramBot.h>
#include <Ticker.h> //para usar las interrupciones por tiempo
 
#define PIR_PIN D6  // GPIO12
#define RADAR_PIN D7 // GPIO13
#define BUZZER_PIN D0 // GPIO16

extern UniversalTelegramBot* bot;

bool pir_state = false;
bool radar_state = false;

// Flags de persistencia
unsigned long pir_until = 0;
unsigned long radar_until = 0;

unsigned long lastPIRAlertTime = 0;
unsigned long lastRadarAlertTime = 0;
unsigned long lastBothAlertTime = 0;
unsigned long buzzer_until = 0;

const unsigned long ALERT_INTERVAL = 20000; //20 seg

//badera para modo centinela
bool modoCentinela = true;

// Intervalos
const unsigned long SENSOR_CHECK_INTERVAL_MS = 500;  // cada cuánto leer
const unsigned long DETECTION_PERSIST_MS = 10000;    // cuánto tiempo mantener bandera en alto

Ticker tickerMovimiento;

// Función periodica para leer sensores
void leerSensoresMovimiento() {
  //logInfo("###---leer sensores.---###");
  unsigned long now = millis();
  if (digitalRead(PIR_PIN)) {
    pir_until = now + DETECTION_PERSIST_MS;
    logInfo("👤📡 PIR detectado");
  }
  if (digitalRead(RADAR_PIN)) {
    radar_until = now + DETECTION_PERSIST_MS;
    logInfo("🚶‍♂️📡 RADAR detectado");
  }
}

void initMovimiento() {
  pinMode(PIR_PIN, INPUT);
  pinMode(RADAR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  tickerMovimiento.attach_ms(SENSOR_CHECK_INTERVAL_MS, leerSensoresMovimiento);

}

void updateMovimiento() {
  unsigned long now = millis();
  bool pir = getPIR();
  bool radar = getRadar();

  if (pir && radar) {
    buzzer_until = now + 30000;
  } else if (pir || radar) {
    if (buzzer_until < now) {
      buzzer_until = now + 10000;
    }
  }

  if (now < buzzer_until) {
    logInfo("prendo el buzzer");
    digitalWrite(BUZZER_PIN, HIGH);
  } else {
    digitalWrite(BUZZER_PIN, LOW);
  }
}

void evaluarAlerta() {
  if (!modoCentinela || !bot) return;

  unsigned long now = millis();
  String msg;

  if (getPIR() && getRadar() && now - lastBothAlertTime > ALERT_INTERVAL) {
    msg = "🥷 Alerta: PIR y radar detectaron movimiento";
    lastBothAlertTime = now;
  } else if (getPIR() && now - lastPIRAlertTime > ALERT_INTERVAL) {
    msg = "👤 Alerta: PIR detectó movimiento";
    lastPIRAlertTime = now;
  } else if (getRadar() && now - lastRadarAlertTime > ALERT_INTERVAL) {
    msg = "🚶‍♂️ Alerta: Radar detectó movimiento";
    lastRadarAlertTime = now;
  }

  if (msg.length() > 0) {
    bot->sendMessage(CHAT_ID, msg, "");
    logInfo("Alerta enviada: " + msg);
  }
}

void setModoCentinela(bool valor) {
  modoCentinela = valor;
}

bool getPIR() {
  return millis() < pir_until;
}

bool getRadar() {
  return millis() < radar_until;
}

bool getBuzzer() {
  return digitalRead(BUZZER_PIN) == HIGH;
}
