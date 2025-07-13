#ifndef SENSORES_H
#define SENSORES_H

#include <Arduino.h>

void initSensors();
void updateData();
void updateDHT11();
void updateBMP180();
String readBMP180();
String readBMP180Json();
String readDHT11();
String readDHT11Json();

extern float bmp_temp;
extern float bmp_pressure;
extern float dht11_temp;
extern float dht11_hum;
extern bool bmp_ok;
extern bool dht11_ok;

#endif
