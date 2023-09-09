#include <AFMotor.h>

AF_DCMotor Motor1(1);  // Delante Derecha
AF_DCMotor Motor2(2);  // Delante Izquierda
AF_DCMotor Motor3(3);  // Detrás Izquierda
AF_DCMotor Motor4(4);  // Detrás Derecha

int Vel = 0 ;    // Define la velocidad base del Rover
float P = 0.25 ;      // Proporcion de giro

void SetVel(int v1, int v2, int v3, int v4)
    {
         Motor1.setSpeed(v1);
         Motor2.setSpeed(v2);
         Motor3.setSpeed(v3);
         Motor4.setSpeed(v4);         
    }
   
    
    void Retroceso()
    { 
      SetVel(Vel,Vel,Vel,Vel);
      Motor1.run(BACKWARD) ;
      Motor2.run(BACKWARD);
      Motor3.run(BACKWARD);
      Motor4.run(BACKWARD);
    }
    
    
    void Paro()
    { 
      Motor1.run(RELEASE);
      Motor2.run(RELEASE);
      Motor3.run(RELEASE);
      Motor4.run(RELEASE); 
    }
    
    void giroIzquierda()
    {  
        int v = Vel * P ;   
        SetVel( v, Vel, v, Vel) ;    
        Motor1.run(FORWARD) ;
        Motor2.run(FORWARD);
        Motor3.run(FORWARD);
        Motor4.run(FORWARD);    
    }
             
    void giroDerecha()
    {  
        int v = Vel * P ;
        SetVel( Vel, v, Vel, v) ;    
        Motor1.run(FORWARD) ;
        Motor2.run(FORWARD);
        Motor3.run(FORWARD);
        Motor4.run(FORWARD);    
    }
    
    void RetrocesoDerecha()
    {  
        int v = Vel * P ;   
        SetVel( v, Vel, v, Vel) ;
        Motor1.run(BACKWARD) ;
        Motor2.run(BACKWARD);
        Motor3.run(BACKWARD);
        Motor4.run(BACKWARD);
    }
             
    void RetrocesoIzquierda()
    {  
        int v = Vel * P ;
        SetVel( Vel, v, Vel, v) ;    
        Motor1.run(BACKWARD) ;
        Motor2.run(BACKWARD);
        Motor3.run(BACKWARD);
        Motor4.run(BACKWARD);
    }

  


 void Avance()
    { 
      SetVel(Vel,Vel,Vel,Vel);
      Motor1.run(FORWARD) ;
      Motor2.run(FORWARD);
      Motor3.run(FORWARD);
      Motor4.run(FORWARD); 
    }
/*
  Pasos a seguir para establecer el HC-05 en modo comando AT
  -------------------------------------------------------------

  1 - Conectar el HC-05 al Arduino
  
  2 - Antes de conectar el Arduino al USB hay que desconectar el cable de 5V del HC-05 para que no coga corriente del Arduino
  
  3 - Conectar el Arduino Uno con el cable USB al PC
  
  4 - Asegurarse de que el módulo HC-05 no está pareado con ningún otro dispositivo bluetooth, es decir, ningún dispositivo bluetooth debe estar conectado al módulo bluetooth
      para poder entrar en el modo comando AT.

  5 - Reconectar el cable de 5V desde el Arduino al módulo HC-05
      Al hacerlo el led del módulo HC-05 parpadeará a intervalos de 2 se3gundos. Ahora el módulo HC-05 está en modo comando AT listo para aceptar comandos para cambiar la 
      configuración y los parámetros.

  6- Para comprobar que todo está funcionando correctamente, debemos abrir el Monitor Serie desde el entorno de desarrollo de Arduino, teclear "AT" (sin las comillas) y despues 
     pulsar el botón enviar. Si aparece OK en la pantalla es que todo funciona correctamente

  Muy importante: 
  ---------------
  
  El Monitor Serie debe estar configurado como NL & CR, para que no de error todo el rato cuando introduzcamos los comandos

*/






void setup()
{
  pinMode(49, OUTPUT);  // this pin will pull the HC-05 pin 9 (key pin) HIGH to switch module to AT mode
  digitalWrite(49, HIGH);
  Serial.begin(9600);
  Serial.println("A la espera de ordenes");
  Serial.println("----------------------");
  Serial.println("");
  
  Serial1.begin(38400);  // HC-05 default speed in AT command more
}

void loop()
{
  analizarCadena();
  // Keep reading from Arduino Serial Monitor and send to HC-05
  if (Serial.available())
    Serial1.write(Serial.read());
}

void analizarCadena(){   
  if (Serial1.available()){    
      char c;                 
      String s="";     
      c = Serial1.read();     
      s += c;           
      if(s.startsWith("S")){
         // Modificamos la velocidad         
         long velocidad;
         velocidad = Serial1.parseInt();
         Vel = (int) velocidad;        
         return;
      }
      else if (s.toInt()>0 && s.toInt()<9){                            
          int m = s.toInt();         
            switch(m){
              case 1:        
                Serial.println("Arriba"); 
                Avance();
                break;
              case 5:
                Serial.println("Arriba a la Derecha"); 
                Avance();
                giroDerecha();
                break;
              case 2:          
                Serial.println("Derecha"); 
                giroDerecha();
                break;
              case 6:
                Serial.println("Abajo a la Derecha"); 
                RetrocesoIzquierda();
                break;
              case 3:
                Serial.println("Abajo"); 
                Retroceso();
                break;
              case 7:
                Serial.println("Abajo a la Izquierda"); 
                RetrocesoDerecha();
                break;
              case 4:
                Serial.println("Izquierda"); 
                giroIzquierda();
                break;
              case 8:
                Serial.println("Arriba a la Izquierda"); 
                Avance();
                giroIzquierda();
                break;
              case 0:
                Serial.println("Echa el freno"); 
                Paro();
                break;        
            }                              
      }
  }}
  
  
  
  



    
