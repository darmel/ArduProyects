#include "FS.h"
#include "SD_MMC.h"
#include "esp_camera.h"

#define SD_CS_PIN 5

File myFile;

// Opciones de configuración de la cámara
const int CAMERA_WIDTH = 640;
const int CAMERA_HEIGHT = 480;
const int CAMERA_FRAMESIZE = FRAMESIZE_VGA;
const int CAMERA_JPEG_QUALITY = 10;

void setup() {
  Serial.begin(115200);

  // Inicializar SD
  if (!SD_MMC.begin("/sdcard", true)) {
    Serial.println("Error al inicializar SD");
    return;
  }

  // Inicializar cámara
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = 5;
  config.pin_d1 = 18;
  config.pin_d2 = 19;
  config.pin_d3 = 21;
  config.pin_d4 = 36;
  config.pin_d5 = 39;
  config.pin_d6 = 34;
  config.pin_d7 = 35;
  config.pin_xclk = 0;
  config.pin_pclk = 22;
  config.pin_vsync = 25;
  config.pin_href = 23;
  config.pin_sscb_sda = 26;
  config.pin_sscb_scl = 27;
  config.pin_pwdn = -1;
  config.pin_reset = 15;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_SVGA;
  config.jpeg_quality = CAMERA_JPEG_QUALITY;
  config.fb_count = 1;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Error al inicializar cámara: %d", err);
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
  // Capturar imagen
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Error al capturar imagen");
    return;
  }

  // Escribir imagen en la SD
  String filename = "/imagen_" + String(millis()) + ".jpg";
  myFile = SD_MMC.open(filename, FILE_WRITE);
  if (myFile) {
    myFile.write(fb->buf, fb->len);
    myFile.close();
    Serial.println("Imagen guardada correctamente: " + filename);
  } else {
    Serial.println("Error al crear archivo de imagen");
  }

  // Liberar memoria de la imagen
  esp_camera_fb_return(fb);

  // Esperar 5 segundos antes de tomar otra imagen
  delay(5000);
}
