#include <Servo.h>

void swapInt(int &a, int &b);

// Pinout 
#define yAxisPin 5
#define zAxisPin 6

// Settings
int zAxisLowLimit = 0;
int zAxisHighLimit = 180;
int yAxisLowLimit = 0;
int yAxisHighLimit = 180;

bool invertedZ = false;
bool invertedY = false;

int alarmTime = 1000;

// Variables and objects
Servo yAxisServo;
Servo zAxisServo;

int zStartAng = 90;
int yStartAng = 90;

int yAxisAngle = 90;
int zAxisAngle = 90;

int angVal = 0;


// States
bool detected;

void setup() {
  // if(invertedY){
  //   swapInt(yAxisHighLimit, yAxisLowLimit);
  // }
  // if(invertedZ){
  //   swapInt(zAxisHighLimit, zAxisLowLimit);
  // }
  
  yAxisServo.attach(yAxisPin, yAxisLowLimit, yAxisHighLimit);
  zAxisServo.attach(zAxisPin, zAxisLowLimit, zAxisHighLimit);

  yAxisAngle = yStartAng;
  zAxisAngle = zStartAng;

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

  test_z_rot(zStartAng, zAxisHighLimit, zAxisLowLimit);


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

void test_z_rot(int start, int low, int high){
  int i;
  for(i=start; i<high; i++){
    move_camera(i, zAxisAngle);
    delay(50);
  }

  for(i=high; i>low; i--){
    move_camera(i, zAxisAngle);
    delay(50);
  }

  for(i=low; i<start; i++){
    move_camera(i, zAxisAngle);
    delay(50);
  }
}
