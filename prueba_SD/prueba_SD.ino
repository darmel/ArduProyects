#include "FS.h"
#include "SD_MMC.h"

#define SD_CS_PIN 5

File myFile;

void setup() {
  Serial.begin(115200);

  // Inicializar SD
  if (!SD_MMC.begin("/sdcard", true)) {
    Serial.println("Error al inicializar SD");
    return;
  }

  // Crear archivo de texto en la SD
  myFile = SD_MMC.open("/archivo.txt", FILE_WRITE);
  if (myFile) {
    Serial.println("Archivo creado correctamente");
    myFile.println("Este es un ejemplo de texto en el archivo.");
    myFile.close();
  } else {
    Serial.println("Error al crear archivo");
  }
}

void loop() {
  // Nada que hacer aquí
}
