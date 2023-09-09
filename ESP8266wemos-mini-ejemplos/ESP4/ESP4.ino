/**********************************************************************************
 * TITULO: Código ESP8266 ejemplo de uso activar un MOC3020 y encender un bombillo.
 * AUTOR: Jhimmy Astoraque Durán
 * DESCRIPCION: Este es el código del capítulo 4 de la serie intermedia ESP82266 del canal.
 * CANAL YOUTUBE: https://www.youtube.com/c/jadsatv
 * © 2019
 * *******************************************************************************/

const int lightPin = 14; //D5
const int signalPin = 13; //D7

void setup() {
  Serial.begin(9600);
  pinMode(lightPin, OUTPUT);
}

void loop() {
  if (digitalRead(signalPin) == HIGH){
    digitalWrite(lightPin, HIGH);
    Serial.println("Luz encendida!");
  }
  else{
    digitalWrite(lightPin, LOW);
  }  
  delay(150);
}
