/**********************************************************************************
 * TITULO: Conexión a la API de openweathermap.org
 * AUTHOR: Jhimmy Astoraque Durán
 * DESCRIPCION: Este es el código del capítulo 1 de la serie intermedia ESP82266 del canal.
 * CANAL YOUTUBE: https://www.youtube.com/c/jadsatv
 * © 2018
 * *******************************************************************************/

#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

const char* ssid     = "DIRECTVNET_10E197";
const char* password = "0pt10n.5";

// en host cambia la parte de la ciudad por la ciudad que desees
const String host = "http://api.openweathermap.org/data/2.5/weather?q=Cordoba,Ar&units=metric&APPID=";
const String privateKey = "c47661847b14f3c569e3f15cf52d0101";
const int delayTime = 10000; // sugerencia 1 minuto -> 60000

String temperature;

void setup() {
  Serial.begin(115200);
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
        //Serial.println(httpCode); // esperamos que sea 200
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
      Serial.println("Error en la petición HTTP");
    }
 
    http.end(); //cerramos conexión y liberamos recursos
  }
  delay(delayTime);
} // EOF loop()
