#include <Arduino.h>

#include <Servo.h>
#include <Timer.h>

#define CLOCKWISE 45
#define COUNTER 120

const int clkPin = 2;
const int dtPin = 3;
const int swPin = 4;
const int vexPin = 7;
const int resetPin = 10;

int count = 0;

Servo VEX;
Timer t;

void setup()
{
   VEX.attach(vexPin);
   pinMode(clkPin, INPUT);
   pinMode(dtPin, INPUT);
   pinMode(swPin, INPUT);
   pinMode(resetPin, INPUT);
   digitalWrite(swPin, HIGH);
   digitalWrite(resetPin, HIGH);
   Serial.begin(9600);
   t.every(30000, takePicture);
}

void loop() {
  if (digitalRead(resetPin) == LOW) {
    reset();
  }
  monitorTurns();
  VEX.write(45);
  t.update();
}

void monitorTurns()
{
  static int oldA = HIGH;
  static int oldB = HIGH;
  int newA = digitalRead(clkPin);
  int newB = digitalRead(dtPin);
  if (newA != oldA || newB != oldB)
  {
    if (oldA == HIGH && newA == LOW)
    {
      count += (oldB * 2 - 1); //oldB is either 1 or 0
    }
  }
  oldA = newA;
  oldB = newB;
  Serial.println(count);
}

void takePicture() {
  VEX.write(0);
  delay(1000);
  Serial.println("Took Picture!");
  delay(1000);
}

void reset() {
  Serial.println("Resetting");
  while (count != 0) {
    monitorTurns();
    VEX.write(COUNTER);
    Serial.println(count);
  }
  exit(1);
}
