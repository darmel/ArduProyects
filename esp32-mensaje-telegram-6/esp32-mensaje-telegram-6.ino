#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "";
const char* password = "";
const char* botToken = "";
const char* chatId = "";

void setup() {
  Serial.begin(9600);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  HTTPClient http;

  String url = "https://api.telegram.org/bot" + String(botToken) + "/sendMessage";

  http.begin(url);

  http.addHeader("Content-Type", "application/json");

  String requestBody = "{\"chat_id\":\"" + String(chatId) + "\",\"text\":\"Hello, world!\"}";
  
  int httpResponseCode = http.POST(requestBody);

  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println(httpResponseCode);
    Serial.println(response);
  } else {
    Serial.println("Error on HTTP request");
  }

  http.end();

  delay(5000);
}
