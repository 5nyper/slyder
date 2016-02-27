#include <Arduino.h>

#include <Servo.h>
#include <Timer.h>

#define CLOCKWISE 1800         // directions of the motor
#define COUNTER 1400

const int clkPin = 2;         // pins
const int dtPin = 3;
const int swPin = 4;
int vexPin = 7;
const int resetPin = 10;
const int camPin = 9;

int count = 0;               // ticks of the rotary encoder

int repeats = 0;             // number of repeats that took place
int event;                   // timer
Timer t;

class Slider {
public:
  int repeat;                  // number of times to stop and take pic
  int pictures;                // number of pictures to take at each stop
  int dur;                     // duration between each stop
  void init(int reps = 2, int phots = 1, int secs = 4, int pin = 7) {
    vexPin = pin;
    VEX.attach(vexPin);
    repeat = reps;
    pictures = 1;
    dur = secs;
  }
  void reset() {
    Serial.println("Resetting");
    while (count != 0) {
      monitorTurns();
      VEX.writeMicroseconds(COUNTER);
      Serial.println(count);
    }
    exit(1);
  }
  void resetTimer(int current) {
    t.stop(current);
    float calc = ((float)(dur + (2000 * pictures)) / dur);  // fixes timer offset
    event = t.every(dur*calc, takePicture, repeat);
  }
  void write(int micro) {
    VEX.writeMicroseconds(micro);
  }
private:
  Servo VEX;
};

Slider obj;

void setup() {
   Serial.begin(9600);
   getInput();
   pinMode(clkPin, INPUT);
   pinMode(dtPin, INPUT);
   pinMode(swPin, INPUT);
   pinMode(resetPin, INPUT);
   pinMode(camPin, OUTPUT);
   digitalWrite(swPin, HIGH);
   digitalWrite(resetPin, HIGH);
   event = t.every(obj.dur, takePicture, obj.repeat);
}

void loop() {
  if (digitalRead(resetPin) == LOW || repeats >= obj.repeat) {
    obj.reset();
  }
  monitorTurns();
  obj.write(CLOCKWISE);
  t.update();
}

void monitorTurns() {
  static int oldA = HIGH;
  static int oldB = HIGH;
  int newA = digitalRead(clkPin);
  int newB = digitalRead(dtPin);
  if (newA != oldA || newB != oldB) {
    if (oldA == HIGH && newA == LOW) {
      count += (oldB * 2 - 1); //oldB is either 1 or 0
    }
  }
  oldA = newA;
  oldB = newB;
  Serial.println(count);
}

void takePicture() {
  obj.write(0);
  for (int i = 0; i<obj.pictures; i++ ) {
    delay(1000);
    digitalWrite(camPin, HIGH);
    delay(300);
    digitalWrite(camPin, LOW);
    Serial.println("Took Picture!");
    delay(1000);
  }
  repeats++;
  obj.resetTimer(event);
}

void getInput() {
  while (!Serial.available()) {
    ;
  }
  char input1 = Serial.read();
  while (!Serial.available()) {
    ;
  }
  char input2 = Serial.read();
  while (!Serial.available()) {
    ;
  }
  char input3 = Serial.read();
  int repeat = input1 - '0';
  int pictures = input2 - '0';
  int dur = (input3 - '0') * 1000;
  obj.init(repeat, pictures, dur);
}
