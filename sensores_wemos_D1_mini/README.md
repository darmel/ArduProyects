# Proyecto: Centinela IoT (ESP8266 - Wemos D1 Mini)

Sistema de monitoreo con sensores de movimiento, temperatura, y reporte vía Web y Telegram.

---

## 📁 Archivos y responsabilidades

### sensores_wemos_D1_mini.ino

Archivo principal, contiene:

- `setup()` y `loop()`
- Inicialización de componentes (WiFi, sensores, Telegram)
- Servidor web básico
- Manejo de requests (`handleClient`, `sendWebPage`, `sendJson`)
- Heartbeat del sistema
- Watchdog

---

### config_wifi.h / config_wifi.cpp

Gestiona:

- Conexión WiFi (usando WiFiManager)
- Portal de configuración inicial para ingresar token de Telegram y Chat ID
- Lectura y escritura de archivo `/config.json` usando LittleFS

Funciones clave:

- `iniciarWiFiYConfig()`: conecta WiFi o lanza portal, guarda configuración

---

### movimiento.h / movimiento.cpp

Manejo de sensores de movimiento:

- Pines: `PIR`, `RADAR`, `BUZZER`
- Persistencia de detección con `Ticker`
- Control de alarma (`modoCentinela`)
- Lógica de alerta con temporización

Funciones clave:

- `initMovimiento()`
- `updateMovimiento()`
- `evaluarAlerta()`
- `getPIR()`, `getRadar()`, `getBuzzer()`
- `setModoCentinela()`

---

### sensores.h / sensores.cpp

Sensado de:

- Temperatura y presión: BMP180
- Temperatura y humedad: DHT11

Funciones:

- `initSensors()`
- `updateData()`, `updateDHT11()`, `updateBMP180()`
- `readDHT11()`, `readBMP180()`: para HTML
- `readDHT11Json()`, `readBMP180Json()`: para JSON REST

---

### utils.h / utils.cpp

Funciones utilitarias comunes:

- `logInfo(msg)`: imprime mensajes si `loggin == true`
- `loggin`: variable global para habilitar/deshabilitar logs

---

### (A CREAR) telegram_bot.h / telegram_bot.cpp

Encargado de:

- Inicialización del cliente Telegram
- Manejo de comandos `/start`, `/centinela`, `/datos`, `/json`
- Encolado y envío de mensajes con lógica de control
- Funciones: `initTelegram()`, `updateTelegramBot()`, `encolarMensaje()`, `desencolarMensaje()`, `colaEstaLlena()`, `colaEstaVacia()`

---

## 🌐 Interfaz Web

- Servidor HTTP responde a `/` con dashboard HTML
- `/data` responde con JSON para la UI
- `/ON` y `/OFF` controlan el LED integrado

---

## 🤖 Telegram Bot

Comandos disponibles:

- `/start`: muestra ayuda
- `/centinela`: activa las alertas
- `/descanso`: las desactiva
- `/datos`: muestra estado actual
- `/json`: datos crudos JSON

---

## 💾 Almacenamiento

- Se guarda configuración en `/config.json` usando `LittleFS`
- Contiene token y chat ID de Telegram ingresados vía portal WiFiManager

---

## 🔄 Watchdog

- Reinicia el sistema si se cuelga alguna parte crítica
- Función `resetWatchdog()` se llama regularmente

---

## 🛠 Ideas para mejoras futuras

- Modularizar también la parte Web en `web_server.h/.cpp`
- Registrar eventos de movimiento en LittleFS
- Enviar los datos a un servidor remoto (API REST)
