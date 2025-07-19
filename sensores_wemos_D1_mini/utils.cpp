#include "utils.h"
//#include <Arduino.h>

bool loggin = true;

void logInfo(const String& mensaje) {
  if (loggin) {
    Serial.println(mensaje);
  }
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