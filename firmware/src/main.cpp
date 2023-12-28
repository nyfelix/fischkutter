#include <Arduino.h>
#include <heltec.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include "debug.h"
#include "config.h"
#include "secrets.h"


void drawWelcomeScreen();
void createWifiHotspot();
void listenWiFi();
void listenRC();
String handleRequest(String req);
void switchSteamingLight(bool on);
void switchFishingLight(bool on);

const char *ssid = SSID;
const char *password = PASSWORD;
bool steamingLightOn = false;
bool fishingLightOn = false;

WiFiServer server;
WiFiClient client;

void setup() {
  Serial.begin(115200);
  delay(1000);
  debugLn("Welcome on board of " + String(BOAT_NAME));
  debugLn("Starting up...");
  // Prepare GPIO for Light
  pinMode(PIN_STEAMING_LIGHT, OUTPUT);
  pinMode(PIN_FISHING_LIGHT, OUTPUT);
  pinMode(PIN_RC_LIGHT_SWITCH, INPUT);
  digitalWrite(PIN_STEAMING_LIGHT, LOW);
  digitalWrite(PIN_FISHING_LIGHT, LOW);
  debugLn("   ... Light pins set");
  // Prepare Display
  Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Disable*/, true /*Serial Enable*/);
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->flipScreenVertically();
  debugLn("   ... Display ready");
  // Prepare Wifi
  createWifiHotspot();
  drawWelcomeScreen();
}

void loop() {
  listenWiFi();
  listenRC();
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
  debugLn("   ... Configuring access point.");
  WiFi.mode(WIFI_AP);
  if (!WiFi.softAP(ssid, password)) {
    Serial.println("Failed to set up AP");
    return;
  }
  IPAddress IP = WiFi.softAPIP();
  debug("AP IP address: ");
  debugLn(IP);
  server.begin(HTTP_PORT);
}

void listenRC() {
  if (digitalRead(PIN_RC_LIGHT_SWITCH) == HIGH) {
    switchSteamingLight(true);
  } else {
    switchSteamingLight(false);
  }
}

void switchSteamingLight(bool on) {
  steamingLightOn = !steamingLightOn;
  digitalWrite(PIN_STEAMING_LIGHT, steamingLightOn ? HIGH : LOW);
}

void switchFishingLight(bool on) {
  fishingLightOn = !fishingLightOn;
  digitalWrite(PIN_FISHING_LIGHT, fishingLightOn ? HIGH : LOW);
}

void listenWiFi() {
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
