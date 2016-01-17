#include <Bounce2.h>

#include <SimpleTimer.h>
// You could use a spare Hardware Serial on boards that have it (like Mega)
#include <SoftwareSerial.h>
SoftwareSerial SwSerial(2, 3); // RX, TX
#define BLYNK_PRINT SwSerial
#include <BlynkSimpleSerial.h>

char auth[] = "53834e9ebf3942ad95d533a4547efe3f";

#define moisturePin A5

// door 1 sensor and control
const int selePin = 13;
const int seleStatusPin = 3;

// door 1 status
int selePinStatus = 0;
int seleLedStatus = 0;
int moistureStatus;
bool seleSwitch = false;
bool seleBypass = false;

SimpleTimer timer;
Bounce  Bouncer  = Bounce();

void setup()
{
  SwSerial.begin(9600);
  Blynk.begin(auth);


  pinMode(selePin, OUTPUT);
  pinMode(moisturePin, INPUT);
  pinMode(seleStatusPin, INPUT);

  Bouncer.attach( seleStatusPin );
  Bouncer.interval(5);

  timer.setInterval(1000L, sendSeleStatus);

}

void sendSeleStatus() {
  Blynk.virtualWrite(V5, seleLedStatus);
}

void loop() {
  Blynk.run();

  moistureStatus = analogRead(moisturePin);
  //moistureStatus = digitalRead(moisturePin);
  //selePinStatus = digitalRead(seleStatusPin);


  if ( Bouncer.update() ) {
    if ( Bouncer.read() == HIGH) {
      if (seleSwitch) {
        seleLedStatus = 0;
        digitalWrite(selePin, LOW);
        seleBypass = false;
        seleSwitch = false;
      }
      else {
        seleLedStatus = 1023; // 100% brightness
        digitalWrite(selePin, HIGH);
        seleBypass = true;
        seleSwitch = true;
      }
    }
  }

  if (!seleBypass) {
    if (moistureStatus > 100) {
      seleLedStatus = 1023;
      digitalWrite(selePin, HIGH);

    }
    else {
      seleLedStatus = 0;
      digitalWrite(selePin, LOW);
    }

  }

  timer.run();
}
