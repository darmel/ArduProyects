/**********************************************************************************
 * TITULO: Código ESP8266 ejemplo de uso de puerto analógico A0.
 * AUTOR: Jhimmy Astoraque Durán
 * DESCRIPCION: Este es el código del capítulo 3 de la serie intermedia ESP82266 del canal.
 * CANAL YOUTUBE: https://www.youtube.com/c/jadsatv
 * © 2018
 * *******************************************************************************/

const byte ledPin = 13; // D7
int lecturaAnalogica;

void setup() {
  Serial.begin(115200);
  // A0 default es entrada
  pinMode(ledPin, OUTPUT);
}

void loop() {
  lecturaAnalogica = analogRead(A0);
  Serial.println(lecturaAnalogica);

  if (lecturaAnalogica >= 500){
    digitalWrite(ledPin, HIGH); // encendemos el Led
  }
  else{
    digitalWrite(ledPin, LOW);
  }
  //retardo
  delay(250);
}// EOF loop
