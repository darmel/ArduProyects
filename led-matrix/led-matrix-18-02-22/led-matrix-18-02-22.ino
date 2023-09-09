#define MODO_BCD_MAX7219 0x09
#define BRILLO_MAX7219 0x0A
#define LIMITE_MAX7219 0x0B
#define ACTIVAR_MAX7219 0x0C
#define MODO_TEST_MAX7219 0x0F

#define LETRA_E 0x0B
#define LETRA_H 0x0C
#define LETRA_L 0x0D
#define LETRA_P 0x0E
#define BORRAR_SEGMENTOS 0x0F

#define NUMERO_DIGITOS 8
#define PIN_CS 10

#include <SPI.h>

void setup()
{
  SPI.begin();
  pinMode(PIN_CS,OUTPUT);
  digitalWrite(PIN_CS,LOW);

  enviar_MAX7219(ACTIVAR_MAX7219,0); // Desactivar MAX7219
  enviar_MAX7219(MODO_TEST_MAX7219,0); // Desactivar el modo de prueba
  enviar_MAX7219(BRILLO_MAX7219,0); // El brillo más bajo posible (de 0 a 15)
  enviar_MAX7219(LIMITE_MAX7219,NUMERO_DIGITOS-1); // Límite de escaneo de dígitos (último dígito empezando en cero)
  enviar_MAX7219(MODO_BCD_MAX7219,255); // Modo BCD (dígitos de 7 segmentos)
  enviar_MAX7219(ACTIVAR_MAX7219,1); // Activar MAX7219

  enviar_MAX7219(8,BORRAR_SEGMENTOS);
  enviar_MAX7219(7,BORRAR_SEGMENTOS);
  enviar_MAX7219(6,LETRA_H);
  enviar_MAX7219(5,LETRA_E);
  enviar_MAX7219(4,LETRA_L);
  enviar_MAX7219(3,LETRA_P);
  enviar_MAX7219(2,BORRAR_SEGMENTOS);
  enviar_MAX7219(1,BORRAR_SEGMENTOS);
}

void loop()
{
}

void enviar_MAX7219(byte registro, byte valor)
{
  digitalWrite(PIN_CS,LOW);
  SPI.transfer(registro);
  SPI.transfer(valor);
  digitalWrite(PIN_CS,HIGH);
}
