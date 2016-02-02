

#include <SimpleTimer.h>

#include <WiFiManager.h>
#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
//#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

WiFiManager wifi;

char auth[] = "53834e9ebf3942ad95d533a4547efe3f";
char auth1[] = "40f0622b74eb4997b20c639e8a430797";
WidgetBridge bridge1(V1);

const int moisturePin = A0;

int moistureStatus;

SimpleTimer timer;

void sendMoisStatus() {
  bridge1.virtualWrite(V6, moistureStatus);
}

void setup()
{
  Serial.begin(115200);
  wifi.autoConnect("Blynk");
  String ssid = wifi.getSSID();
  String pass = wifi.getPassword();

  Blynk.begin(auth1, ssid.c_str(), pass.c_str());

  while (Blynk.connect() == false) {
    
  }
  bridge1.setAuthToken(auth);
  pinMode(moisturePin, INPUT);

  timer.setInterval(1000L, sendMoisStatus);
}

void loop() {
  Blynk.run();
  timer.run();

  moistureStatus = analogRead(moisturePin);


}

