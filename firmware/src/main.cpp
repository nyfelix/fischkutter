#include <Arduino.h>
#include <heltec.h>
#include <WiFi.h>
#include <WiFiAP.h>


void drawWelcomeScreen();
void createWifiHotspot();
void listenForClients();
String handleRequest(String req);

//const char *ssid = "MSFishyAP";
//const char *password = "1234";
const char *ssid = "FerdinandAir";
const char *password = "Lychee2011";
#define BOAT_NAME "MS Fishy"

WiFiServer server;
WiFiClient client;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting up...");
  Serial.println("Welcome on board of " + String(BOAT_NAME));
  // Prepare GPIO for Light
  pinMode(13, OUTPUT);

  // Prepare Display
  Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Disable*/, true /*Serial Enable*/);
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->flipScreenVertically();
  // Prepare Wifi
  createWifiHotspot();
  drawWelcomeScreen();
}

void loop() {
  Serial.println("Hello World!");
  digitalWrite(13, HIGH);
  delay(1000);
  digitalWrite(13, LOW);
  delay(1000);
}

void drawWelcomeScreen() {
  Heltec.display->clear();
  Heltec.display->drawString(0, 0, "Welcome on board of:");
  Heltec.display->drawString(0, 12, BOAT_NAME);
  IPAddress IP = WiFi.softAPIP();
  Heltec.display->drawString(0, 24, "IP address: " + IP.toString()); 
  Heltec.display->display();
} 

void createWifiHotspot() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.println(F("Configuring access point."));
  WiFi.mode(WIFI_STA);
  if (!WiFi.begin(ssid, password)) {
    Serial.println("Failed to set up AP");
    return;
  }
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  server.begin(80);
}

void listenForClients() {
if (!client) {
    client = server.available();
  }
  else {
    if (client.available()) {
      // read request
      String req = client.readString();
      //handle Request
      String response = handleRequest(req);
      // send response
      client.print(response.c_str());
      client.stop();
    }
  }
}

String handleRequest(String req) {
  Serial.println(req);
  return "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\n<h1>Hello World!</h1>\r\n</html>\n";
}
