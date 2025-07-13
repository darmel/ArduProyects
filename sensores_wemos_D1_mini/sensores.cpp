#include "sensores.h"
#include <Adafruit_BMP085.h>
#include "DHT.h"
#include "utils.h"


#define DHT11_PIN D5  // GPIO14

Adafruit_BMP085 bmp;
bool bmp_ok = false;
float bmp_temp = NAN;
float bmp_pressure = NAN;

DHT dht11(DHT11_PIN, DHT11);
bool dht11_ok = false;
float dht11_temp = NAN;
float dht11_hum = NAN;

void initSensors() {
  if (bmp.begin()) {
    bmp_ok = true;
  } else {
    bmp_ok = false;
  }

  dht11.begin();
}

void updateData() {
  updateDHT11();
  updateBMP180();
}

void updateBMP180() {
  logInfo("Leyendo BMP180...");
  if (!bmp_ok) return; //el sensor no esta disponible
  bmp_temp = bmp.readTemperature();
  yield();
  bmp_pressure = bmp.readPressure() / 100.0;
  yield();
}

//solo formatea data del bmp180 para htm
String readBMP180() {
  if (!bmp_ok) return "<h2>BMP180</h2><p style='color:red;'>Sensor no disponible</p>";
  if (isnan(bmp_temp) || isnan(bmp_pressure)) return "<h2>BMP180</h2><p style='color:red;'>Error en la lectura</p>";

  String data = "<h2>BMP180</h2>";
  data += "<p>Temperatura: " + String(bmp_temp, 1) + " &deg;C</p>";
  data += "<p>Presión: " + String(bmp_pressure, 1) + " hPa</p>";
  return data;
}

//solo fortmatea data del BMP180 para el json
String readBMP180Json() {
  if (!bmp_ok) return "{\"error\":\"Sensor no disponible\"}";
  if (isnan(bmp_temp) || isnan(bmp_pressure)) return "{\"error\":\"Lectura invalida\"}";

  String json = "{";
  json += "\"temperatura\":" + String(bmp_temp, 1) + ",";
  json += "\"presion\":" + String(bmp_pressure, 1);
  json += "}";
  return json;
}

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
  if (isnan(dht11_temp) || isnan(dht11_hum)) return "{\"error\":\"Lectura invalida\"}";
  String json = "{";
  json += "\"temperatura\":" + String(dht11_temp, 1) + ",";
  json += "\"humedad\":" + String(dht11_hum, 1);
  json += "}";
  return json;
}
