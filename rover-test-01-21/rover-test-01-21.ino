/*
 # Edited by:  Matt
 # Date:       2015.09.06
 # Version:    1.1
 # Product:    Cherokey 4WD Mobile Platform
 # SKU:        ROB0102/ROB0117

 # Description:
 # Drive 2 motors with this Cherokey 4WD Mobile Platform
 # Connect D4,D5,D6,D7,GND to UNO digital 4,5,6,7,GND

*/
//Motor Definitions
int E1 = 5;     //M1 Speed Control
int E2 = 6;     //M2 Speed Control
int M1 = 4;     //M1 Direction Control
int M2 = 7;     //M2 Direction Control

//DIRECTIONS

//STOP
void stop(void)
{
  digitalWrite(E1, 0);
  digitalWrite(M1, LOW);
  digitalWrite(E2, 0);
  digitalWrite(M2, LOW);
}

//ADVANCE
void advance(char a, char b)
{
  analogWrite (E1, a);
  digitalWrite(M1, HIGH);
  analogWrite (E2, b);
  digitalWrite(M2, HIGH);
}

//MOVE BACKWARDS
void back_off (char a, char b)
{
  analogWrite (E1, a);
  digitalWrite(M1, LOW);
  analogWrite (E2, b);
  digitalWrite(M2, LOW);
}


//TURN LEFT
void turn_L (char a, char b)
{
  analogWrite (E1, a);
  digitalWrite(M1, LOW);
  analogWrite (E2, b);
  digitalWrite(M2, HIGH);
}

//TURN RIGHT
void turn_R (char a, char b)
{
  analogWrite (E1, a);
  digitalWrite(M1, HIGH);
  analogWrite (E2, b);
  digitalWrite(M2, LOW);
}

void setup(void) {
  int i;
  for (i = 4; i <= 7; i++)
    pinMode(i, OUTPUT);
  Serial1.begin(38400);      //Set Baud Rate
  //Serial1.println("hello. w = forward, d = turn right, a = turn left, s = backward, x = stop, z = hello world"); //Display instructions in the serial monitor
  digitalWrite(E1, LOW);
  digitalWrite(E2, LOW);
}

void loop(void) {
  if (Serial1.available()) {
    char val = Serial1.read();
    if (val != -1)
    {
      switch (val)
      {
        case '1'://Move Forward
          Serial1.println("going forward");
          advance (255, 255);  //move forward at max speed
          delay (50);
          stop();
          break;
        case '3'://Move Backward
          Serial1.println("going backward");
          back_off (255, 255);  //move backwards at max speed
          delay (50);
          stop();
          break;
        case '4'://Turn Left
          Serial1.println("turning left");
          turn_L (255, 255);
          delay (50);
          stop();
          break;
        case '2'://Turn Right
          Serial1.println("turning right");
          turn_R (255, 255);
          delay (50);
          stop();
          break;
        case 'z':
          Serial1.println("hello world!");
          break;
        case 'x':
          Serial1.println("stopping");
          stop();
          break;
      }
    }
    else stop();
  }
}
