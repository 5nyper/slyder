#include <Arduino.h>

#include <Wire.h>
#include <I2CEncoder.h>

I2CEncoder encoder;

const int fPin = 3;  
const long len = 233280;// pins
const int sPin = 2;
const int vexPin = A7;
const int resetPin = 11;
const int camPin = 9;
int in1Pin = 7;
int in2Pin = 6;

int count = 1;               // ticks of the rotary encoder
int oldA = LOW;

int pictures;                // number of pictures to take at each stop
long dur;// duration between each stop
long secsBefore;
long secsAfter;

int repeats = 0;             // number of repeats that took place
int event;                   // timer

void setup() {
    Wire.begin();
   Serial.begin(9600);
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

void loop() {
  initslide();
  takePicture();
  Serial.println(-dur);
  for (int i = 0; i<pictures; i++) {
    while(encoder.getRawPosition() > -dur) {
      if(Serial.available()) {
        char input = Serial.read();
        if(input == '0') {
          setMotor(0, true);
          while (!Serial.available()) {
            ;
          }
          char input2 = Serial.read();
          if(input2 == '1') {
            continue;
          }
        }
        if (input == '2') {
          initslide();
        }
      }
      if (digitalRead(resetPin) == LOW) {
        reset();
      }
      setMotor(255, true);
    }
    takePicture();
  }
  while (true) {
    setMotor(0, true);
  }
}

void setMotor(int speed, boolean reverse)
{
  long var = encoder.getRawPosition();
  analogWrite(vexPin, speed);
  digitalWrite(in1Pin, ! reverse);
  digitalWrite(in2Pin, reverse);
  Serial.println(var);
}

void takePicture() {
    setMotor(0, true);
    delay((secsBefore * 1000));
    digitalWrite(camPin, HIGH);
    delay(300);
    digitalWrite(camPin, LOW);
    Serial.println("Took Picture!");
    delay((secsAfter * 1000));
    repeats++;
    encoder.zero();
}

void reset() {
  Serial.println("Resetting");
  while (encoder.getRawPosition() != 0) {
    setMotor(255, false);
  }
  while (true) {
    setMotor(0, false);
  }
}

void initslide() {
  setMotor(255, true);
  while(encoder.getRawPosition() > -3000) {
   setMotor(255, true);
  }
    
  while(digitalRead(resetPin) != 0) {
    setMotor(255, false);
  }
  encoder.zero();
  while(encoder.getRawPosition() > -3000) {
    setMotor(255, true);
  }
  setMotor(0, true);
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
    }
    else {
      sprintf(test, "%c%c", input4, input5);
      String pics = String(test);
      pictures = pics.toInt();
    }
  }
  else {
    sprintf(test, "%c%c%c", input3, input4, input5);
    String pics = String(test);
    pictures = pics.toInt();
  }
  Serial.println(pictures);
  dur = (long) len / pictures;
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
    }
    else {
      sprintf(test, "%c%c", input4, input5);
      String pics = String(test);
      secsBefore = (pics.toInt()) * 1000;
    }
  }
  else {
    sprintf(test, "%c%c%c", input3, input4, input5);
    String pics = String(test);
    secsBefore = (pics.toInt()) * 1000;
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
    }
    else {
      sprintf(test, "%c%c", input4, input5);
      String pics = String(test);
      secsAfter = (pics.toInt()) * 1000;
    }
  }
  else {
    sprintf(test, "%c%c%c", input3, input4, input5);
    String pics = String(test);
    secsAfter = (pics.toInt()) * 1000;
  }
  Serial.println(secsAfter);
}
