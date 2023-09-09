/*  Daft punk helmet code with Bluetooth control and microphone
 *  Tutorial webpage: https://www.electronoobs.com/eng_arduino_tut62.php
 *  Schematic: https://www.electronoobs.com/eng_arduino_tut62_sch1.php
 *  Part list: https://www.electronoobs.com/eng_arduino_tut62_parts1.php 
 */

#include <Adafruit_GFX.h>       //Downlaod: https://www.electronoobs.com/eng_arduino_Adafruit_GFX.php 
#include <Adafruit_NeoMatrix.h> //Downlaod: https://www.electronoobs.com/eng_arduino_Adafruit_NeoMatrix.php 
#include <Adafruit_NeoPixel.h>  //Downlaod: https://www.electronoobs.com/eng_arduino_Adafruit_NeoPixel.php 
#ifndef PSTR
#define PSTR // Make Arduino Due happy
#endif


//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////EDITABLE VARIABLES///////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
#define PIN 9                 //Select here the digital pin used for the LED strip data pin
#define NUMPIXELS      105    //Amount of LEDs. I have 7 row of 15 LEDs wit a total of 105 LEDs
  
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(
  15,                                              //Select LEDs of each row (15 in my case) 
  7,                                               //Select amount of rows (7 in my case)
  PIN,                                                
  NEO_MATRIX_BOTTOM + NEO_MATRIX_RIGHT +           //Define first data pin (right bottom corner is my first pin)
  NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG,             //Define the type of connection (in a zig zag connections and divided by rows not columns)
  NEO_GRB            + NEO_KHZ800);

String text = "ELECTRONOOBS SUBSCRIBE";                 //Put here the default text to be printed on the LED matrix when it's powered on
int Delay = 5000;                       //Delay for the eye movement
int brightness = 10;                    //Defaulst brightness
int min_adc = 690;                      //min range of the microphone ADC read
int max_adc = 840;                      //max range of the microphone ADC read
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

/* * I defien 2 matrices because I've used two different code exampels to create my scroll text, eyes, etc..
That's why below I define a NeoPixel matrix and a simple matrix controlled LED by LED, one by one. 
matrix 1 is "pixels" and matrix 2 is called "matrix"
*/

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);   
const uint16_t colors[] = {
  matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255) };        

//More variables
int string_length = 0;
static byte byteRead=0;
bool print_text = true;
bool print_eyes= false;
bool print_rainbow = false;
bool print_side_to_side = false;
bool print_spectrum = false; 
bool print_Chase = false;
bool print_off = false;
unsigned long previousMillis = 0; 

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////ADC config//////////////////////////////////////////////////
//ADC sampling variables
int adc_in = A0;
int adc = 0;
#define ADC_MULTISAMPLING 2
#define ADC_MULTISAMPLING_SAMPLES (1 << ADC_MULTISAMPLING)
//////////////////////////////////////////////////////////////////////////////////////////////////


void setup() {
  pinMode(adc_in,INPUT);
  Serial.begin(9600);           //Start the serial communication for the BT module
  
  matrix.begin();               //Start the LED matrix 1
  matrix.setTextWrap(false);
  matrix.setBrightness(20);
  pixels.setBrightness(20);
  matrix.setTextColor(colors[0]);
  
  pixels.begin(); // This initializes the NeoPixel library.  //Start the LED matrix 2
}

int x    = matrix.width();
int pass = 0;

void loop() 
{
  if(Serial.available()>0)
  {
    byteRead = Serial.read();     //Read data from the BT module
    if(byteRead=='T')             //If we receive a "T", then we gen into text scroll mode
    {
      text = Serial.readString(); //Store the bluetooth received text
      print_text = true;
      print_eyes= false;
      print_rainbow = false;
      print_side_to_side = false;
      print_spectrum = false;
      print_Chase = false;
      print_off = false;
    }    
    if(byteRead=='E')               //If we receive a "E", then we gen into eyes mode
    {
      print_eyes= true;
      print_text = false;
      print_rainbow = false;
      print_side_to_side = false;
      print_spectrum = false;
      print_Chase = false;
      print_off = false;
    }  
    if(byteRead=='B')               //If we receive a "B", then we change brightness to the received value
    {
      String Received = Serial.readString();
      int brightness = (int(Received[0])-48)*10;
      if(brightness == 0)
      {
        brightness = 100;
      }
      brightness = constrain(brightness,0,100);
      matrix.setBrightness(brightness);
      pixels.setBrightness(brightness);
      Serial.println(brightness);
    }  

    if(byteRead=='O')                             //If we receive a "O", then we turn off the LEDs
    {
      print_eyes= false;
      print_text = false;
      print_rainbow = false;
      print_side_to_side = false;
      print_spectrum = false;
      print_Chase = false;
      print_off = true;
    }

    if(byteRead=='R')                             //If we receive a "R", then we get into rainbow mode
    {
      print_eyes= false;
      print_text = false;
      print_rainbow = true;
      print_side_to_side = false;
      print_spectrum = false;
      print_Chase = false;
      print_off = false;
    }

    if(byteRead=='S')                              //If we receive a "S", then we get into robocop mode
    {
      print_eyes= false;
      print_text = false;
      print_rainbow = false;
      print_side_to_side = true;
      print_spectrum = false;
      print_Chase = false;
      print_off = false;
    }

    if(byteRead=='P')                               //If we receive a "P", then we get audio power mode (microphone control)
    {
      print_eyes= false;
      print_text = false;
      print_rainbow = false;
      print_side_to_side = false;
      print_spectrum = true;
      print_Chase = false;
      print_off = false;
    }

    if(byteRead=='C')                               //If we receive a "C", then we get into random mode
    {
      print_eyes= false;
      print_text = false;
      print_rainbow = false;
      print_side_to_side = false;
      print_spectrum = false;
      print_Chase = true;
      print_off = false;
    }
  }//end of serial read

  if(print_text && !print_off)
  {
  string_length = text.length();
  matrix.fillScreen(0);
  matrix.setCursor(x, 0);
  matrix.print(text);
  if(--x < -(string_length*6)) {
    x = matrix.width();
    if(++pass >= 3) pass = 0;
    matrix.setTextColor(colors[pass]);
  }
  matrix.show();
  delay(60);
  }//End of print text

  if(print_eyes && !print_off)
  {
    int y = -3;
    int i = 0;
    unsigned long currentMillis = millis();
    if(currentMillis - previousMillis >= Delay)
    {
      previousMillis += Delay;
      while(y < 3)
      {
        while (i < 105)
        {
        pixels.setPixelColor(i, pixels.Color(0,0,0)); 
        i = i + 1;
        }
        i=0;   
          
        pixels.setPixelColor(33+y, pixels.Color(255,0,0)); 
        pixels.setPixelColor(34+y, pixels.Color(255,0,0)); 
          pixels.setPixelColor(40+y, pixels.Color(255,0,0)); 
          pixels.setPixelColor(41+y, pixels.Color(255,0,0)); 
          pixels.setPixelColor(47-y, pixels.Color(255,0,0)); 
          pixels.setPixelColor(48-y, pixels.Color(255,255,255)); 
          pixels.setPixelColor(49-y, pixels.Color(255,255,255));
          pixels.setPixelColor(50-y, pixels.Color(255,0,0));   
        pixels.setPixelColor(54-y, pixels.Color(255,0,0)); 
        pixels.setPixelColor(55-y, pixels.Color(255,255,255)); 
        pixels.setPixelColor(56-y, pixels.Color(255,255,255)); 
        pixels.setPixelColor(57-y, pixels.Color(255,0,0)); 
        pixels.setPixelColor(63+y, pixels.Color(255,0,0)); 
        pixels.setPixelColor(64+y, pixels.Color(255,0,0)); 
          pixels.setPixelColor(70+y, pixels.Color(255,0,0)); 
          pixels.setPixelColor(71+y, pixels.Color(255,0,0)); 
          
        pixels.show(); // This sends the updated pixel color to the hardware.
        delay(150);
        y = y + 1;
      }
      //delay(1000);
      
      y = 2;
      
      while(y > -3)
      {
        while (i < 105)
        {
        pixels.setPixelColor(i, pixels.Color(0,0,0)); 
        i = i + 1;
        }
        i=0;
          
        pixels.setPixelColor(33+y, pixels.Color(255,0,0)); 
        pixels.setPixelColor(34+y, pixels.Color(255,0,0)); 
          pixels.setPixelColor(40+y, pixels.Color(255,0,0));
          pixels.setPixelColor(41+y, pixels.Color(255,0,0)); 
          pixels.setPixelColor(47-y, pixels.Color(255,0,0)); 
          pixels.setPixelColor(48-y, pixels.Color(255,255,255)); 
          pixels.setPixelColor(49-y, pixels.Color(255,255,255)); 
          pixels.setPixelColor(50-y, pixels.Color(255,0,0));   
        pixels.setPixelColor(54-y, pixels.Color(255,0,0)); 
        pixels.setPixelColor(55-y, pixels.Color(255,255,255)); 
        pixels.setPixelColor(56-y, pixels.Color(255,255,255)); 
        pixels.setPixelColor(57-y, pixels.Color(255,0,0));
        pixels.setPixelColor(63+y, pixels.Color(255,0,0)); 
        pixels.setPixelColor(64+y, pixels.Color(255,0,0)); 
          pixels.setPixelColor(70+y, pixels.Color(255,0,0)); 
          pixels.setPixelColor(71+y, pixels.Color(255,0,0)); 
          
        pixels.show(); // This sends the updated pixel color to the hardware.
        delay(150);
        y = y - 1;
      }
      
      
      y = -2 ;
      }//end of current millis
  }//end of print eyes


  if(print_rainbow && !print_off)
  {
    rainbow(1);
  }

  if(print_Chase && !print_off)
  {
    theaterChase(pixels.Color(127, 127, 127), 50); 
  }

  if(print_side_to_side && !print_off)
  {
    int x = -3;
    int i = 0;
    int k = 0;
    int Delay_time = 20;

    while (k < 105)
    {
      pixels.setPixelColor(k, pixels.Color(0,0,0)); 
      k = k + 1;
    }
    k=0;

    while(i < 15)
    {
      pixels.setPixelColor(60-i, pixels.Color(0,0,0)); 
      pixels.setPixelColor(59-i, pixels.Color(255,255,255));
      pixels.show(); // This sends the updated pixel color to the hardware.
      delay(Delay_time);
      i = i + 1;
    }
    
    i = 14;
    delay(100);
    
    while(i >= 0)
    {
      pixels.setPixelColor(58-i, pixels.Color(0,0,0));
      pixels.setPixelColor(59-i, pixels.Color(255,255,255)); 
      pixels.show(); // This sends the updated pixel color to the hardware.
      delay(Delay_time);
      i = i - 1;
    }
    
    i = 0;
    delay(100);

  }//end of side to side


  if(print_spectrum && !print_off)
  {
    int val = read_adc();
    int level = map(val,min_adc,max_adc,1,7);
    Serial.println(val);
    analogWrite(13,level);
    //delay(1);
  
  
    if(level==1)
    {
      int r = 0;
      while (r < 105)
      {
        pixels.setPixelColor(r, pixels.Color(0,0,0)); 
        r = r + 1;
      }
      r=0;
      while (r < 15)
      {
        pixels.setPixelColor(r, pixels.Color(0,255,0)); 
        r = r + 1;
      }
      r=0;
      pixels.show(); // This sends the updated pixel color to the hardware.
    }//end of level 1
  
  
    if(level==2)
    {
      int r = 0;
      while (r < 105)
      {
        pixels.setPixelColor(r, pixels.Color(0,0,0)); 
        r = r + 1;
      }
      r=0;
      while (r < 15)
      {
        pixels.setPixelColor(r, pixels.Color(0,255,0)); 
        r = r + 1;
      }    
  
      while (r < 30)
      {
        pixels.setPixelColor(r, pixels.Color(150,255,0)); 
        r = r + 1;
      }
      r=0;
      pixels.show(); // This sends the updated pixel color to the hardware.
    }//end of level 2
  
  
    if(level==3)
    {
      int r = 0;
      while (r < 105)
      {
        pixels.setPixelColor(r, pixels.Color(0,0,0));
        r = r + 1;
      }
      r=0;
      while (r < 15)
      {
        pixels.setPixelColor(r, pixels.Color(0,255,0)); 
        r = r + 1;
      }    
  
      while (r < 30)
      {
        pixels.setPixelColor(r, pixels.Color(150,255,0)); 
        r = r + 1;
      }
  
      while (r < 45)
      {
        pixels.setPixelColor(r, pixels.Color(255,255,0)); 
        r = r + 1;
      }
      r=0;
      pixels.show(); // This sends the updated pixel color to the hardware.
    }//end of level 3
  
  
    if(level==4)
    {
      int r = 0;
      while (r < 105)
      {
        pixels.setPixelColor(r, pixels.Color(0,0,0)); 
        r = r + 1;
      }
      r=0;
      while (r < 15)
      {
        pixels.setPixelColor(r, pixels.Color(0,255,0)); 
        r = r + 1;
      }    
  
      while (r < 30)
      {
        pixels.setPixelColor(r, pixels.Color(150,255,0)); 
        r = r + 1;
      }
  
      while (r < 45)
      {
        pixels.setPixelColor(r, pixels.Color(255,255,0)); 
        r = r + 1;
      }
  
      while (r < 60)
      {
        pixels.setPixelColor(r, pixels.Color(255,150,0)); 
        r = r + 1;
      }
  
      
      r=0;
      pixels.show(); // This sends the updated pixel color to the hardware.
    }//end of level 4
  
  
  if(level==5)
    {
      int r = 0;
      while (r < 105)
      {
        pixels.setPixelColor(r, pixels.Color(0,0,0)); 
        r = r + 1;
      }
      r=0;
      while (r < 15)
      {
        pixels.setPixelColor(r, pixels.Color(0,255,0)); 
        r = r + 1;
      }    
  
      while (r < 30)
      {
        pixels.setPixelColor(r, pixels.Color(150,255,0)); 
        r = r + 1;
      }
  
      while (r < 45)
      {
        pixels.setPixelColor(r, pixels.Color(255,255,0)); 
        r = r + 1;
      }
  
      while (r < 60)
      {
        pixels.setPixelColor(r, pixels.Color(255,150,0));
        r = r + 1;
      }
  
      while (r < 75)
      {
        pixels.setPixelColor(r, pixels.Color(255,75,0)); 
        r = r + 1;
      }
  
      
      r=0;
      pixels.show(); // This sends the updated pixel color to the hardware.
    }//end of level 5
  
  
  if(level==6)
    {
      int r = 0;
      while (r < 105)
      {
        pixels.setPixelColor(r, pixels.Color(0,0,0)); 
        r = r + 1;
      }
      r=0;
      while (r < 15)
      {
        pixels.setPixelColor(r, pixels.Color(0,255,0)); 
        r = r + 1;
      }    
  
      while (r < 30)
      {
        pixels.setPixelColor(r, pixels.Color(150,255,0)); 
        r = r + 1;
      }
  
      while (r < 45)
      {
        pixels.setPixelColor(r, pixels.Color(255,255,0)); 
        r = r + 1;
      }
  
      while (r < 60)
      {
        pixels.setPixelColor(r, pixels.Color(255,150,0)); 
        r = r + 1;
      }
  
      while (r < 75)
      {
        pixels.setPixelColor(r, pixels.Color(255,50,0)); 
        r = r + 1;
      }
  
  
      while (r < 90)
      {
        pixels.setPixelColor(r, pixels.Color(255,25,0)); 
        r = r + 1;
      }
      
      r=0;
      pixels.show(); // This sends the updated pixel color to the hardware.
    }//end of level 6
  
  
  if(level==7)
    {
      int r = 0;
      while (r < 105)
      {
        pixels.setPixelColor(r, pixels.Color(0,0,0)); 
        r = r + 1;
      }
      r=0;
      while (r < 15)
      {
        pixels.setPixelColor(r, pixels.Color(0,255,0)); 
        r = r + 1;
      }    
  
      while (r < 30)
      {
        pixels.setPixelColor(r, pixels.Color(150,255,0)); 
        r = r + 1;
      }
  
      while (r < 45)
      {
        pixels.setPixelColor(r, pixels.Color(255,255,0)); 
        r = r + 1;
      }
  
      while (r < 60)
      {
        pixels.setPixelColor(r, pixels.Color(255,150,0)); 
        r = r + 1;
      }
  
      while (r < 75)
      {
        pixels.setPixelColor(r, pixels.Color(255,50,0)); 
        r = r + 1;
      }
  
  
      while (r < 90)
      {
        pixels.setPixelColor(r, pixels.Color(255,25,0)); 
        r = r + 1;
      }
      
      while (r < 105)
      {
        pixels.setPixelColor(r, pixels.Color(255,0,0)); 
        r = r + 1;
      }
      
      r=0;
      pixels.show(); // This sends the updated pixel color to the hardware.
    }//end of level 7
  }//end of print spectrum

  if(print_off)
  {
    int r = 0;
    while (r < 105)
    {
      pixels.setPixelColor(r, pixels.Color(0,0,0)); 
      r = r + 1;
    }
     pixels.show();
    r=0;
  }




}//end of void loop


void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel((i+j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}


uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}



float read_adc()
{
   // Filter the ADC by multisampling with the values defined at the beginning
  adc = 0;
  for (int i = 0; i < ADC_MULTISAMPLING_SAMPLES; ++i)
    adc += analogRead(adc_in);
    adc = adc >> ADC_MULTISAMPLING; 
  
  return(adc);
}


//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < pixels.numPixels(); i=i+3) {
        pixels.setPixelColor(i+q, c);    //turn every third pixel on
      }
      pixels.show();

      delay(wait);

      for (uint16_t i=0; i < pixels.numPixels(); i=i+3) {
        pixels.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}
