#include <DHT.h>
#include <Bounce2.h>
#include <SimpleTimer.h>
// You could use a spare Hardware Serial on boards that have it (like Mega)
#include <SoftwareSerial.h>
SoftwareSerial SwSerial(2, 3); // RX, TX
#define BLYNK_PRINT SwSerial
#include <BlynkSimpleSerial.h>

#define DHTTYPE DHT11

//Blynk Auth
char auth[] = "53834e9ebf3942ad95d533a4547efe3f";
WidgetBridge bridgeToESP(V0);

//Pin Mapping
const int moisturePin = A5; //Pin for mouisture sensor
const int dhtPin = 2; //Pin; for DHT11/DHT22
const int lightSensorPin = A0; //Pin for Light Sensor / LDR
const int selenoidPin = 13; //Pin for Selenoid
const int selenoidSwitchPin = 3; //Pin for Selenoid Switch

//
int moistureStatus;
int selenoidLedStatus;
int lightSensorValue;
float humidity;
float temperature;
float heatIndex;

bool seleSwitch = false; //starting condition for selenoid switch status
bool seleBypass = false; //starting condition for bypass status

SimpleTimer timer; //timer initiate
Bounce  bouncerSelenoid  = Bounce(); //bouncher initiate
DHT dht(dhtPin, DHTTYPE); //DHT initiate

void setup()
{
  SwSerial.begin(9600);
  Blynk.begin(auth); //Blynk Server Load
  if (Blynk.connect()) {
    bridgeToESP.setAuthToken("40f0622b74eb4997b20c639e8a430797");
  }
  //Pin Mode Initiate
  pinMode(selenoidPin, OUTPUT);
  pinMode(lightSensorPin, OUTPUT);
  pinMode(moisturePin, INPUT);
  pinMode(selenoidSwitchPin, INPUT);

  digitalWrite(selenoidPin, LOW);

  //Make Bouncher for Selenoid switch
  bouncerSelenoid.attach( selenoidSwitchPin );
  bouncerSelenoid.interval(5);

  dht.begin(); //begin DHT

  //Pooling timer for Blynk Server
  timer.setInterval(1000L, sendSelenoidStatus);
  timer.setInterval(5000L, checkDHTValue);
  timer.setInterval(5000L, checkMoistureStatus);


}

void checkDHTValue() {
  lightSensorValue = analogRead(lightSensorPin);
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  heatIndex = dht.computeHeatIndex(temperature, humidity, false);

  Blynk.virtualWrite(V10, temperature);
  Blynk.virtualWrite(V11, humidity);
  Blynk.virtualWrite(V12, heatIndex);
  Blynk.virtualWrite(V9, lightSensorValue);
  Blynk.virtualWrite(V8, moistureStatus);

}

void sendSelenoidStatus() {
  Blynk.virtualWrite(V5, selenoidLedStatus);
}

void checkMoistureStatus() {
  moistureStatus = analogRead(moisturePin);
  if (!seleBypass) {
    if (moistureStatus > 100) {
      selenoidLedStatus = 1023;
      digitalWrite(selenoidPin, HIGH);
      bridgeToESP.digitalWrite(5, HIGH);

    }
    else {
      selenoidLedStatus = 0;
      digitalWrite(selenoidPin, LOW);
      bridgeToESP.digitalWrite(5, LOW);
    }

  }
}

void loop() {
  Blynk.run();

  //Bypass Switch for Selenoid Switch
  if ( bouncerSelenoid.update() ) {
    if ( bouncerSelenoid.read() == HIGH) {
      if (seleSwitch) {
        selenoidLedStatus = 0;
        digitalWrite(selenoidPin, LOW);
        bridgeToESP.digitalWrite(5, LOW);
        seleBypass = false;
        seleSwitch = false;
      }
      else {
        selenoidLedStatus = 1023; // 100% brightness
        digitalWrite(selenoidPin, HIGH);
        bridgeToESP.digitalWrite(5, HIGH);
        seleBypass = true;
        seleSwitch = true;
      }
    }
  }

  //Routin Moisture sensor checking


  timer.run();
}
