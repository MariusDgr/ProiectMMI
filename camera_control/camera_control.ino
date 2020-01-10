#include <Servo.h>

void swapInt(int &a, int &b);

// Pinout 
#define yAxisPin 5
#define zAxisPin 6

// Settings
int zAxisRightLimit = 0;
int zAxisLeftLimit = 180;
int yAxisUpLimit = 300;
int yAxisDownLimit = -50;

bool invertedZ = false;
bool invertedY = false;

int alarmTime = 1000;

// Variables and objects
Servo yAxisServo;
Servo zAxisServo;

int yAxisAngle = 150;
int zAxisAngle = 90;

int angVal = 0;


// States
bool detected;

void setup() {
  if(invertedY){
    swapInt(yAxisDownLimit, yAxisUpLimit);
  }
  if(invertedZ){
    swapInt(zAxisLeftLimit, zAxisRightLimit);
  }
  
  yAxisServo.attach(yAxisPin, yAxisDownLimit, yAxisUpLimit);
  zAxisServo.attach(zAxisPin, zAxisLeftLimit, zAxisRightLimit);

  yAxisServo.write(yAxisAngle);
  zAxisServo.write(zAxisAngle);

  Serial.begin(9600);
  Serial.println("Setup done");

  delay(1000);

}

void loop() {
  if (Serial.available() > 0) {
    angVal = Serial.parseInt();
    Serial.print("Received: ");
    Serial.println(angVal);

    // zAxisAngle = angVal;
    yAxisAngle = angVal;

    move_camera(yAxisAngle, zAxisAngle);
  }

// int i = 150;
// for(i=150; i<200; i++){
//   move_camera(i, zAxisAngle);
//   delay(50);
// }

// for(i=200; i>0; i--){
//   move_camera(i, zAxisAngle);
//   delay(50);
// }

// for(i=0; i<150; i++){
//   move_camera(i, zAxisAngle);
//   delay(50);
// }

}

void move_camera(int yAngle, int zAngle){
  yAxisServo.write(yAngle);
  zAxisServo.write(zAngle);
}

void swapInt(int &a, int &b) {
  int c = a;

  a = b;
  b = c;
}
