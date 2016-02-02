#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "40f0622b74eb4997b20c639e8a430797";

const int selenoidPin = 5;

void setup()
{
  Serial.begin(115200);
  Blynk.begin(auth, "kamar13", "");
  pinMode(selenoidPin, OUTPUT);
  digitalWrite(selenoidPin, LOW);
  
}

BLYNK_WRITE(V6){
  int a = param.asInt();
  if (a == 0) {
    digitalWrite(selenoidPin, LOW);
  } else {
    digitalWrite(selenoidPin, HIGH);
  }
}

void loop()
{
  Blynk.run();
}

