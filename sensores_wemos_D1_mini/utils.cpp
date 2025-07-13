#include "utils.h"
//#include <Arduino.h>

bool loggin = true;

void logInfo(const String& mensaje) {
  if (loggin) {
    Serial.println(mensaje);
  }
}
