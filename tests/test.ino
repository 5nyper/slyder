#include <Arduino.h>
#include <Timer.h>

#define CLOCKWISE 45           // directions of the motor
#define COUNTER 120

const int clkPin = 2;         // pins
const int dtPin = 3;
const int swPin = 4;
const int vexPin = 9;
const int resetPin = 10;

int count = 0;               // ticks of the rotary encoder

int repeat;                  // number of times to stop and take pic
int pictures;                // number of pictures to take at each stop
int dur;                     // duration between each stop

int repeats = 0;             // number of repeats that took place

class vex_t {
public:
  void attach(int put) {
    pinMode(put, OUTPUT);
    pin = put;
  }
  void write(int value) {
    analogWrite(pin, value);
  }
private:
  int pin;
};

Timer t;
vex_t VEX;

void setup() {
   Serial.begin(9600);
  // getInput();
   VEX.attach(vexPin);
   pinMode(clkPin, INPUT);
   pinMode(dtPin, INPUT);
   pinMode(swPin, INPUT);
   pinMode(resetPin, INPUT);
   digitalWrite(swPin, HIGH);
   digitalWrite(resetPin, HIGH);
  // Serial.println(repeat);
//Serial.println(pictures);
   t.every(dur, takePicture, repeat);
}

void loop() {
 /* if (digitalRead(resetPin) == LOW || repeats >= repeat) {
    reset();
  }*/
  monitorTurns();
  for(int i = -255;i<255; i++) {
    VEX.write(i);
  }
  //t.update();
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
  VEX.write(0);
  for (int i = 0; i<pictures; i++ ) {
    delay(1000);
    Serial.println("Took Picture!");
    delay(1000);
  }
  repeats++;
}

void reset() {
  Serial.println("Resetting");
  while (count != 0) {
    monitorTurns();
    VEX.write(-45);
    Serial.println(count);
  }
  exit(1);
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
  repeat = input1 - '0';
  pictures = input2 - '0';
  dur = (input3 - '0') * 1000;
}
