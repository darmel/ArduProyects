#ifndef MOVIMIENTO_H
#define MOVIMIENTO_H

#include <Arduino.h>

void initMovimiento();       // Configura pines
void updateMovimiento();     // Lee sensores y actualiza estados
void evaluarAlerta();        // Maneja el buzzer y Telegram
void setModoCentinela(bool);
bool getPIR();
bool getRadar();
bool getBuzzer();  // devuelve true si el buzzer está encendido


extern bool modoSentinela;

#endif
