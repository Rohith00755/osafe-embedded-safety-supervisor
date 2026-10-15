#include <WiFi.h>

// Bench-only demo: replace with credentials for an isolated lab network.
const char* WIFI_SSID = "REPLACE_WITH_LAB_WIFI";
const char* WIFI_PASSWORD = "REPLACE_WITH_LAB_PASSWORD";
const char* SUPERVISOR_HOST = "192.168.1.100"; // Replace with supervisor IP
const uint16_t SUPERVISOR_PORT = 5000;
WiFiClient client;
unsigned long beatAt = 0, statusAt = 0;

void connectSupervisor() {
  if (client.connected()) return;
  client.stop();
  if (client.connect(SUPERVISOR_HOST, SUPERVISOR_PORT, 3000))
    Serial.println("TCP connected");
  else
    Serial.println("TCP connection failed");
}
void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA); WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println(); Serial.print("Node IP: "); Serial.println(WiFi.localIP());
  connectSupervisor();
}
void loop() {
  if (WiFi.status() != WL_CONNECTED) { client.stop(); delay(500); return; }
  connectSupervisor();
  if (!client.connected()) { delay(500); return; }
  unsigned long now = millis();
  if (now - beatAt >= 1000) { client.println("HEARTBEAT"); beatAt = now; }
  if (now - statusAt >= 3000) { client.println("STATUS|node|esp32_demo"); statusAt = now; }
  delay(10);
}
