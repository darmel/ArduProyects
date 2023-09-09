void setup() {
  // put your setup code here, to run once:
  digitalWrite(49, HIGH);
  Serial.begin(9600);
  Serial1.begin(9600);  // HC-05 default speed in AT command more
}

void loop() {
  // put your main code here, to run repeatedly:
  analizarCadena();
  // Keep reading from Arduino Serial Monitor and send to HC-05
  if (Serial.available())
    Serial1.write(Serial.read());
}




   void SetVel(int v1, int v2)
    {
         MR.setSpeed(v1);
         ML.setSpeed(v2);         
    }

    void Avance()
    { 
      SetVel(Vel,Vel,Vel,Vel);
      MR.run(FORWARD) ;
      ML.run(FORWARD); 
    }
