#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Replace with your network credentials
const char* ssid = "olimpo24";
const char* password = "R3n3w4.l";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", -3 * 60 * 60, 60000); // -3 hours offset

void setup() {
  Serial.begin(9600);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize NTP client
  timeClient.begin();
  timeClient.update(); // Get current time
}

void loop() {
  timeClient.update(); // Update time

  // Get date and time
  struct tm timeInfo;
  time_t now = timeClient.getEpochTime();
  localtime_r(&now, &timeInfo);

  // Print date and time
  char timeString[20];
  strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", &timeInfo);
  Serial.println(timeString);

  delay(1000);
}
