#include <Servo.h>

void swapInt(int &a, int &b);
void buzz_sound(int pin, int pitch, int time);
void test_z_rot(int start, int low, int high);

// Pinout 
#define yAxisPin 5
#define zAxisPin 6

#define ledPin 10
#define buzzPin 3 

// Settings
int zAxisLowLimit = 0;
int zAxisHighLimit = 180;
int yAxisLowLimit = 0;
int yAxisHighLimit = 180;

bool invertedZ = false;
bool invertedY = false;

int alarmTime = 1000;
int alarmPitch = 1000;
int alarmEnabled = true;

// Variables and objects
Servo yAxisServo;
Servo zAxisServo;

int zStartAng = 90;
int yStartAng = 90;

int yAxisAngle = 90;
int zAxisAngle = 90;

int angVal = 0;


// States
bool intruderDetected = false;

void setup() {
  // if(invertedY){
  //   swapInt(yAxisHighLimit, yAxisLowLimit);
  // }
  // if(invertedZ){
  //   swapInt(zAxisHighLimit, zAxisLowLimit);
  // }

  pinMode(ledPin, OUTPUT);
  
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

    if(angVal == 1){
      intruderDetected = true;
    }

    // zAxisAngle = angVal;
    // yAxisAngle = angVal;

    // move_camera(yAxisAngle, zAxisAngle);
  }

  // test_z_rot(zStartAng, zAxisHighLimit, zAxisLowLimit);

  if(alarmEnabled){
    if(intruderDetected == 1){
      digitalWrite(ledPin, HIGH);

      // buzzer 
      buzz_sound(buzzPin, alarmPitch, alarmTime);

      intruderDetected = false;

    } else {
      digitalWrite(ledPin, LOW);
    }
  }
  
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

void buzz_sound(int pin, int pitch, int time){
  tone(pin, pitch);
  delay(time);
  noTone(pin);
}
