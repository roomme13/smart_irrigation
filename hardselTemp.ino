#include <DHT.h>
#include <Bounce2.h>
#include <SimpleTimer.h>
#include <ESP8266_HardSer.h>
#include <BlynkSimpleShieldEsp8266_HardSer.h>

#define EspSerial Serial1
#define DHTTYPE DHT11

//Blynk Auth
char auth[] = "53834e9ebf3942ad95d533a4547efe3f";

//Pin Mapping
const int moisturePin = A3; //Pin for mouisture sensor
const int dhtPin = 2; //Pin; for DHT11/DHT22
const int lightSensorPin = A2; //Pin for Light Sensor / LDR
const int selenoidPin = 4; //Pin for Selenoid
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
ESP8266 wifi(EspSerial);

void setup()
{
  EspSerial.begin(115200);
  Blynk.begin(auth, wifi, "kamar13", ""); //Blynk Server Load

  //Pin Mode Initiate
  pinMode(selenoidPin, OUTPUT);
  pinMode(lightSensorPin, OUTPUT);
  pinMode(moisturePin, INPUT);
  pinMode(selenoidSwitchPin, INPUT);
  pinMode(14, OUTPUT);
  pinMode(16, OUTPUT);



  digitalWrite(selenoidPin, LOW);
  digitalWrite(14, HIGH);
  digitalWrite(16, HIGH);


  //Make Bouncher for Selenoid switch
  bouncerSelenoid.attach( selenoidSwitchPin );
  bouncerSelenoid.interval(5);

  dht.begin(); //begin DHT

  //Pooling timer for Blynk Server
  timer.setInterval(1000L, sendSelenoidStatus);
  timer.setInterval(5000L, checkDHTValue);
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

void loop() {
  Blynk.run();
  moistureStatus = analogRead(moisturePin);

  //Bypass Switch for Selenoid Switch
  if ( bouncerSelenoid.update() ) {
    if ( bouncerSelenoid.read() == HIGH) {
      if (seleSwitch) {
        selenoidLedStatus = 0;
        digitalWrite(selenoidPin, LOW);
        seleBypass = false;
        seleSwitch = false;
      }
      else {
        selenoidLedStatus = 1023; // 100% brightness
        digitalWrite(selenoidPin, HIGH);
        seleBypass = true;
        seleSwitch = true;
      }
    }
  }


  if (!seleBypass) {
    if (moistureStatus > 100) {
      selenoidLedStatus = 1023;
      digitalWrite(selenoidPin, HIGH);

    }
    else {
      selenoidLedStatus = 0;
      digitalWrite(selenoidPin, LOW);
    }

  }

  //Routin Moisture sensor checking
  timer.run();
}
