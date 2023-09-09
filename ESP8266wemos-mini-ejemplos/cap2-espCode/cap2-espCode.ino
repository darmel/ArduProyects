/**********************************************************************************
 * TITULO: Código ESP de conexión API y posterior comunicación con Arduino UNO (MEGA O NANO)
 * AUTOR: Jhimmy Astoraque Durán
 * DESCRIPCION: Este es el código del capítulo 2 de la serie intermedia ESP82266 del canal.
 * CANAL YOUTUBE: https://www.youtube.com/c/jadsatv
 * © 2018
 * *******************************************************************************/

#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

const char* ssid     = "AS-08387";
const char* password = "Tu password";

// en host cambia la parte de la ciudad por la ciudad que desees
const String host = "http://api.openweathermap.org/data/2.5/weather?q=Santa%20Cruz%20de%20la%20Sierra,%20BO&units=metric&APPID=";
const String privateKey = "Tu API key";
const int delayTime = 10000; // sugerencia 1 minuto -> 60000

String temperature;

void setup() {
  Serial.begin(9600);
  delay(20);

  // Nos intentamos conectar a la red wifi
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA); // modo wifi-client
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Conectado a la red WiFi!!!");
  Serial.println("");
} // EOF setup()


void loop() {
  if ((WiFi.status() == WL_CONNECTED)) { 
    HTTPClient http;
 
    http.begin(host + privateKey); //armamos la URL
    int httpCode = http.GET();  //Realizamos la petición
 
    if (httpCode > 0) { //código de retorno
 
        String data = http.getString();
        // Manipulamos la string data
        //Serial.println(data);
        int index = data.indexOf("\"temp\""); //+7
        if (index != -1){
          index += 7;
          int index2 = data.indexOf(",", index);
          temperature = data.substring(index, index2);
        }
        Serial.println(temperature); // unica q empieza en numeros
      }
 
    else {
      Serial.println("Error petición HTTP");
    }
 
    http.end(); //cerramos conexión y liberamos recursos
  }
  delay(delayTime);
} // EOF loop()

