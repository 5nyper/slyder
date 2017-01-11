
#include <Arduino.h>

#include <Wire.h>
#include <I2CEncoder.h>

I2CEncoder encoder;

const int fPin = 3;
const long len = 233280; // pins
const int sPin = 2;
const int vexPin = A2;
const int resetPin = 11;
const int camPin = 9;
int in1Pin = 7;
int in2Pin = 6;

int count = 1; // ticks of the rotary encoder
int oldA = LOW;

int pictures; // number of pictures to take at each stop
long dur;     // duration between each stop
long secsBefore;
long secsAfter;

int repeats = 0; // number of repeats that took place
int event;       // timer

void setup() {
  Wire.begin();
  Serial.begin(9600); // initializing bluetooth serial connection
  pinMode(19, INPUT_PULLUP);
  pinMode(18, INPUT_PULLUP);
  pinMode(vexPin, OUTPUT);
  pinMode(fPin, INPUT);
  pinMode(sPin, INPUT);
  pinMode(resetPin, INPUT);
  pinMode(camPin, OUTPUT);
  digitalWrite(resetPin, HIGH);
  encoder.init(MOTOR_393_SPEED_ROTATIONS, MOTOR_393_TIME_DELTA);
  getInput1();
  getInput2();
  getInput3();
}

void testBt() {
  Serial.println("Test3");
  //  Serial2.println("Test2");
  //  Serial1.println("Test1");
}

void loop() {
  initslide();

  int offset =
      floor(60 - ((7776 * (0.24339217 + 0.0020943471 * pictures +
                           0.0000048879012 * (pictures * pictures) - .5)) /
                  (pictures - 1)));

  Serial.println("in loop");
  Serial.print("1 of ");
  Serial.print(pictures);
  takePicture();
  Serial.println(-dur);
  for (int i = 0; i < pictures - 1; i++) {
    while (encoder.getRawPosition() > -dur + offset) {
      if (Serial.available()) {
        char input = Serial.read();
        if (input == '0') {
          setMotor(0, true);
          while (!Serial.available()) {
            ;
          }
          char input2 = Serial.read();
          if (input2 == '1') {
            continue;
          }
        }
        if (input == '2') {
          reset();
        }
      }
      if (digitalRead(resetPin) == LOW) {
        reset();
      }
      setMotor(255, true);
    }

    setMotor(150, false); // moving motor backwards after each stop to counter
                          // the "drift", was 200, just changed to 150
    delay(150); // the time the motor moves backwards to counter the "drift"

    Serial.print(i + 2);
    Serial.print(" of ");
    Serial.print(pictures);
    takePicture();
  }
  while (true) {
    setMotor(0, true);
  }
}

void setMotor(int speed, boolean reverse) {
  long var = encoder.getRawPosition();
  analogWrite(vexPin, speed);
  digitalWrite(in1Pin, !reverse);
  digitalWrite(in2Pin, reverse);
  Serial.println(var);
}

void takePicture() {
  setMotor(70, false);
  delay((secsBefore * 1000));
  digitalWrite(camPin, HIGH);
  delay(300);
  digitalWrite(camPin, LOW);
  Serial.println("Took Picture!");
  Serial.println(encoder.getRawPosition());
  Serial.println(-dur);
  delay((secsAfter * 1000));
  repeats++;
  encoder.zero();
}

void reset() {
  Serial.println("Resetting");
  while (digitalRead(resetPin) != 0) {
    setMotor(255, false);
  }
}

void initslide() {
  setMotor(255, true);
  while (encoder.getRawPosition() > -500) {
    setMotor(255, true);
  }

  while (digitalRead(resetPin) != 0) {
    setMotor(255, false);
  }
  encoder.zero();
  while (encoder.getRawPosition() > -1500) { // changed from -2500 to 1500 to
                                             // give more bar length in
                                             // beginning...
    setMotor(255, true);
  }
  setMotor(150, false);
  encoder.zero();
}

void getInput1() {
  char test[10];
  while (!Serial.available()) {
    ;
  }
  char input3 = Serial.read();
  while (!Serial.available()) {
    ;
  }
  char input4 = Serial.read();
  while (!Serial.available()) {
    ;
  }
  char input5 = Serial.read();
  if (input3 == '0') {
    if (input4 == '0') {
      pictures = input5 - '0';
    } else {
      sprintf(test, "%c%c", input4, input5);
      String pics = String(test);
      pictures = pics.toInt();
    }
  } else {
    sprintf(test, "%c%c%c", input3, input4, input5);
    String pics = String(test);
    pictures = pics.toInt();
  }
  Serial.println(pictures);
  dur = (long)len / (pictures - 1); // changed this to pictures minus 1 because
                                    // that should be the number of
                                    // stops....OOPS!!!! this will change
                                    // everything!
}
void getInput2() {
  char test[10];
  while (!Serial.available()) {
    ;
  }
  char input3 = Serial.read();
  while (!Serial.available()) {
    ;
  }
  char input4 = Serial.read();
  while (!Serial.available()) {
    ;
  }
  char input5 = Serial.read();
  if (input3 == '0') {
    if (input4 == '0') {
      secsBefore = input5 - '0';
    } else {
      sprintf(test, "%c%c", input4, input5);
      String pics = String(test);
      secsBefore = (pics.toInt()) * 1; // think 1000 is too much, trying 1
    }
  } else {
    sprintf(test, "%c%c%c", input3, input4, input5);
    String pics = String(test);
    secsBefore = (pics.toInt()) * 1; // think 1000 isn't right, trying 1
  }
  Serial.println(secsBefore);
}
void getInput3() {
  char test[10];
  while (!Serial.available()) {
    ;
  }
  char input3 = Serial.read();
  while (!Serial.available()) {
    ;
  }
  char input4 = Serial.read();
  while (!Serial.available()) {
    ;
  }
  char input5 = Serial.read();
  if (input3 == '0') {
    if (input4 == '0') {
      secsAfter = input5 - '0';
    } else {
      sprintf(test, "%c%c", input4, input5);
      String pics = String(test);
      secsAfter = (pics.toInt()) * 1; // changed from 1000 to 1
    }
  } else {
    sprintf(test, "%c%c%c", input3, input4, input5);
    String pics = String(test);
    secsAfter = (pics.toInt()) *
                1; // changed from 1000 to 1, not needed as it is 3 digits
  }
  Serial.println(secsAfter);
}


Format! Style:

C++ online code formatter © 2014 by KrzaQ

Powered by vibe.d, the D language and clang-format
