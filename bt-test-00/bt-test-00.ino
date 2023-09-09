char recibido[5];
int velocidad=0;
int flag=0;
int velact=0;
int velant=0;
void setup() {
  // put your setup code here, to run once:
       Serial.begin(9600);
       Serial.println("Iniciando");
       Serial.println("Conexión Serie 1 iniciada");
       Serial.println("Iniciando conexión BTen Serial 1");
       Serial1.begin(9600);
       Serial.println("Conexion con BT iniciada");    
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.flush();
  if (Serial1.available()){
      recibido[0]=Serial1.read();

    switch (recibido[0]){
      case 'm':
        velocidad=Serial1.parseInt();
        flag=1;
      break;
      default:
        flag=0;
    }

     if (velocidad != velact)
      {
         velact=velocidad;
         flag=1;
      }
 /* Serial.print("bandera ");
  Serial.println(flag);*/
   compc();
    }
}

void compc(){
  if(flag==0)
       {
       Serial.print("direccion: "); 
       Serial.println(recibido);
       }
      else
      {
       Serial.print("Velocidad: ");
       Serial.println(velact);
       flag=0;
      }
}

