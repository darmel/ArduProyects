#include "movimiento.h"
#include "utils.h"
#include <ESP8266WiFi.h> // Por si usás bot->sendMessage
#include "config_wifi.h" // CHAT_ID
#include <UniversalTelegramBot.h>

#define PIR_PIN D6  // GPIO12
#define RADAR_PIN D7 // GPIO13
#define BUZZER_PIN D0 // GPIO16

extern UniversalTelegramBot* bot;

bool pir_state = false;
bool radar_state = false;

unsigned long lastPIRAlertTime = 0;
unsigned long lastRadarAlertTime = 0;
unsigned long lastBothAlertTime = 0;
unsigned long buzzer_until = 0;

const unsigned long ALERT_INTERVAL = 20000; //20 seg

bool modoCentinela = true;

void initMovimiento() {
  pinMode(PIR_PIN, INPUT);
  pinMode(RADAR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
}

void updateMovimiento() {
  pir_state = digitalRead(PIR_PIN);
  radar_state = digitalRead(RADAR_PIN);

  unsigned long now = millis();

  if (pir_state && radar_state) {
    buzzer_until = now + 30000;
  } else if (pir_state || radar_state) {
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

  if (pir_state && radar_state && now - lastBothAlertTime > ALERT_INTERVAL) {
    msg = "🥷 Alerta: PIR y radar detectaron movimiento";
    lastBothAlertTime = now;
  } else if (pir_state && now - lastPIRAlertTime > ALERT_INTERVAL) {
    msg = "👤 Alerta: PIR detectó movimiento";
    lastPIRAlertTime = now;
  } else if (radar_state && now - lastRadarAlertTime > ALERT_INTERVAL) {
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
  return pir_state;
}

bool getRadar() {
  return radar_state;
}

bool getBuzzer() {
  return digitalRead(BUZZER_PIN) == HIGH;
}
