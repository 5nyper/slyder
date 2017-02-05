#include <Arduino.h>
#include <Wire.h>
#include <I2CEncoder.h>
#include <EEPROMex.h>
#include <EEPROMVar.h>

#define CACHE_LENGTH_ADDRESS 0

I2CEncoder encoder;

int data[][2] = {
  {0, 0}
};
int cacheLen;
int loadedData[10][2];

int offset = 100;

const int fPin = 3;
const long len = 233280; // pins
const int sPin = 2;
const int vexPin = A2;
const int resetPin = 11;
const int endPin = 12;
const int camPin = 9;
int in1Pin = 7;
int in2Pin = 6;

int count = 1; // ticks of the rotary encoder
int oldA = LOW;

int pictures; // number of pictures to take at each stop
long dur;     // duration between each stop
long secsBefore;
long secsAfter;
long offsetLength = 0;
bool flag = false;

int repeats = 0; // number of repeats that took place
int event;       // timer

void setup() {
  Wire.begin();
  Serial.begin(9600); // initializing bluetooth serial connection
  //EEPROM.writeInt(CACHE_LENGTH_ADDRESS, -1);
  if(EEPROM.readInt(CACHE_LENGTH_ADDRESS) > 4) {
    // offset += machineLearning(params)
  }
  pinMode(19, INPUT_PULLUP);
  pinMode(18, INPUT_PULLUP);
  pinMode(vexPin, OUTPUT);
  pinMode(fPin, INPUT);
  pinMode(sPin, INPUT);
  pinMode(resetPin, INPUT);
  pinMode(camPin, OUTPUT);
  digitalWrite(resetPin, HIGH);
  digitalWrite(endPin, HIGH);
  encoder.init(MOTOR_393_SPEED_ROTATIONS, MOTOR_393_TIME_DELTA);
  getInput1();
  getInput2();
  getInput3();
}

int loadDataSet() {
  int len = EEPROM.readInt(CACHE_LENGTH_ADDRESS);
  if (len == -1) {
    Serial.println("No data saved before");
    return -1;
  }
  else {
    Serial.println(len);
    int address = 2;
    for(int i=0;i<len;i++){
      for(int j=0;j<2;j++){
        loadedData[i][j] = EEPROM.readInt(address);
        address+=sizeof(EEPROM.readInt(address));
      }
    }
    Serial.println();
    Serial.println("Dataset has been loaded"); 
    EEPROM.writeInt(CACHE_LENGTH_ADDRESS, (address-2)/4);
    //Serial.println(address/4);
    for(int i=0;i<(sizeof(loadedData)/sizeof(loadedData[0]));i++){
      Serial.println();
      for(int j=0;j<2;j++){
        Serial.print(loadedData[i][j]);
        Serial.print("\t");
      }
    } 
    return 0;
  }
}

void updateDataSet(int x, int y) {
  if(loadDataSet() == -1) {
    int data[][2] = {
      {x, y}
    };
    int address = 2;
    for(int i=0;i<1;i++){
      for(int j=0;j<2;j++){
        EEPROM.writeInt(address, data[i][j]);
        address+=sizeof(data[i][j]);
      }
    }
    EEPROM.writeInt(CACHE_LENGTH_ADDRESS, (address-2)/4);
  }
  else {
    int len = EEPROM.readInt(CACHE_LENGTH_ADDRESS);
    loadedData[len][0] = x;
    loadedData[len][1] = y;
    int address = 2;
    for(int i=0;i<len+1;i++){
      for(int j=0;j<2;j++){
        EEPROM.writeInt(address, loadedData[i][j]);
        address+=sizeof(loadedData[i][j]);
      }
    }
    EEPROM.writeInt(CACHE_LENGTH_ADDRESS, (address-2)/4);
  }
}

void loop() {
  Serial.print("offset is: ");
  Serial.println(offset);
  initslide();
  Serial.println("in loop");
  Serial.print("1 of ");
  Serial.print(pictures);
  takePicture();
  Serial.println(-dur);
  for (int i = 0; i < pictures - 1; i++) {
    while (encoder.getRawPosition() > -dur + offset) {
      if(digitalRead(endPin) == LOW) {
          setMotor(0, true);
          Serial.println();
          updateDataSet(pictures, (pictures-1-i*dur)+encoder.getRawPosition());
          while (true) {
            setMotor(0, true);
            //Serial.println("Saved Data");
          }
      }
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
  //encoder.zero();
  while(true) {
    if(digitalRead(endPin) == LOW) {
        Serial.print("from while loop ");
        Serial.println(offsetLength);
        setMotor(0, true);
        updateDataSet(pictures, offsetLength+3000);
        while (true) {
          setMotor(0, true);
          //Serial.println("Saved Data Here");
        }
    } 
    offsetLength = encoder.getRawPosition();
    setMotor(255, true);
  } 
}

int polynomialRegression(int x) {
  int len = EEPROM.readInt(CACHE_LENGTH_ADDRESS);
  long double x_mean = 0.0,
        y_mean = 0.0,
        x_sq_mean = 0.0,
        x_sum = 0.0,
        y_sum = 0.0,
        xy_sum = 0.0,
        x_cb_sum = 0.0,
        x_4_sum = 0.0,
        x2y_sum = 0.0;
  for(int i = 0; i< len; i++) {
    x_mean += (long double)loadedData[i][0];
    y_mean += (long double)loadedData[i][1];
    x_sq_mean += (long double)pow(loadedData[i][0],2);
    x_sum += (long double)loadedData[i][0];
    y_sum += (long double)loadedData[i][1];
    xy_sum += (long double)(loadedData[i][0] * loadedData[i][1]);
    x_cb_sum += (long double)pow(loadedData[i][0], 3);
    x_4_sum += (long double)pow(loadedData[i][0], 4);
    x2y_sum += (long double)(pow(loadedData[i][0],2) * loadedData[i][1]);
  }
  x_mean /= (long double)len;
  y_mean /= (long double)len;
  x_sq_mean /= (long double)len;

  long double sxx = x_sq_mean - pow(x_mean, 2);
  long double sxy = (xy_sum/len) - (x_mean * y_mean);
  long double sxx2 = (x_cb_sum/len) - (x_mean * x_sq_mean);
  long double sx2x2 = (x_4_sum/len) - (x_sq_mean * x_sq_mean);
  long double sx2y = (x2y_sum/len) - (x_sq_mean * y_mean);
  long double B = ((sxy*sx2x2)-(sx2y*sxx2))/((sxx*sx2x2)-pow(sxx2, 2));
  long double C = ((sx2y*sxx)-(sxy*sxx2))/((sxx*sx2x2)-pow(sxx2,2));
  long double A = y_mean-(B*x_mean)-(C*x_sq_mean);

  return ((int)(A + B*x + C*pow(x, 2)));
}

void setMotor(int speed, boolean reverse) {
  long var = encoder.getRawPosition();
  analogWrite(vexPin, speed);
  digitalWrite(in1Pin, !reverse);
  digitalWrite(in2Pin, reverse);
  //Serial.println(var);
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
  while (digitalRead(resetPin) != 0) {
    Serial.println("Resetting");
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

/*
 * int offset =
      floor(60 - ((7776 * (0.24339217 + 0.0020943471 * pictures +
                           0.0000048879012 * (pictures * pictures) - .5)) /
                  (pictures - 1)));
                  */
