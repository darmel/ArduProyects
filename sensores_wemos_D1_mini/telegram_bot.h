#ifndef TELEGRAM_BOT_H
#define TELEGRAM_BOT_H

#include <Arduino.h>
#include <UniversalTelegramBot.h>


// El puntero al bot debe seguir siendo accesible globalmente (por ejemplo, desde movimiento.cpp)
extern UniversalTelegramBot* bot;

// Inicializa el cliente seguro del bot
void initTelegram();

// Crea el bot con el token y chat ID ya cargados, y envía mensaje de arranque
void startTelegramBot();

// Revisa nuevos mensajes y los procesa (debe llamarse periódicamente desde loop)
void updateTelegramBot();

// Manejo interno de una pequeña cola de mensajes para enviar si se quiere en el futuro
void encolarMensaje(String msg);

// telegram_bot.cpp
void handleNewMessages(int numNewMessages);


String desencolarMensaje();
bool colaEstaVacia();
bool colaEstaLlena();

#endif
