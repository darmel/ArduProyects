//sketch para juego de "Rescate de la Boveda"
/*La idea es sensores de luz(fototransistores o LDR) que son alcanzados por lasers dispuestos de tal forma que sea
 * un desafio atravesarlos.
 * Cuando alguno de los lasers es cortado se activa la alarma. El jugador pierde
 *
 */

const int sensor[] = {A0, A1, A2, A3};    // pin that the sensor is attached to

const int ledpin = 9;        // pin that the LED is attached to

// variables:
int sensorValue[] = {0, 0, 0, 0};         // the sensor value
int sensorMin[] = {1023, 1023, 1023, 1023};        // minimum sensor value
int sensorMax[] = {0, 0, 0, 0};           // maximum sensor value
int ldd[] = {0, 0, 0, 0}; //limite de disparo
int i=0;


void setup() {
  // put your setup code here, to run once:
   Serial.begin(9600);
   pinMode(13, OUTPUT);
   digitalWrite(13, HIGH);//para mostrar tiempo de seteo
   
   while (millis() < 3000) //seteo de valores maximo y minimo de luz
   {
    calibracion0;
    }

   for(i=0;i<=3;i++)
    {
      ldd[i]=(sensorMax[i]-sensorMin[i])/2;
    }
     
     digitalWrite(13, LOW); //termino la calibracion

    
    Serial.print("SensorMin:"); Serial.println(sensorMin); 
    Serial.print("SensorMax:"); Serial.println(sensorMax); 
    Serial.print("Limite de disparo:"); Serial.println(ldd0); 

}
void loop() {
  // put your main code here, to run repeatedly:
  
  sensorValue[i] = analogRead(sensor0); // read the sensor:
    
  if (sensorValue<ldd0)
    {
    digitalWrite(ledpin, HIGH);
    Serial.print(sensorValue); Serial.print("<"); Serial.println(ldd0);
    Serial.println("Has sido detectado");
    Serial.print("valor del sensor:"); Serial.println(sensorValue); //muestro el valor del sensor por serie(solo para debug)
    }
    else
    digitalWrite(ledpin, LOW);
}

//FUNCIONES

//Calibracion
int calibracion0()
  {
    for(i=0;i<=3;i++)
      {
      sensorValue[i] = analogRead(sensor[i]); //leo el sensor (A0)
  
        // record the minimum sensor value - Luz Ambiente
        if (sensor[i] < sensorMin[i]) 
          sensorMin[i] = sensorValue[i];
          
      // record the maximum sensor value - Con el laser incidiendo
        if (sensorValue[i] > sensorMax[i]) 
          sensorMax[i] = sensorValue[i];
      }
  }


