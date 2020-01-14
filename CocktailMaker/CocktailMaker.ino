#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "HX711.h"
LiquidCrystal_I2C lcd(0x3F, 16, 2);
#include <HX711_ADC.h>
#include <EEPROM.h>
#include "IRremote.h"

// (c) Copyright 2010-2012 MCQN Ltd.
// Released under Apache License, version 2.0
//
// Simple example to show how to use the HttpClient library
// Get's the web page given at http://<kHostname><kPath> and
// outputs the content to the serial port


//IRremote
int receiver = 11; // Signal Pin of IR receiver to Arduino Digital Pin 11

/*-----( Declare objects )-----*/
IRrecv irrecv(receiver);     // create instance of 'irrecv'
decode_results results;      // create instance of 'decode_results'

//HX711 constructor (dout pin, sck pin):
HX711_ADC LoadCell(5, 6);


long t;
#define clk 2
#define dt 3
#define sw 4
#define in1 7
#define in2 8
#define in3 9
#define in4 10
volatile boolean TurnDetected;
volatile boolean up;
bool doonce = 0;
char screen = 0;
char subscreen = 0;
int changestate = -1;

String shit;
int pump1ml = 100;
int pump2ml = 100;

void isr0 ()  {
  TurnDetected = true;
  up = (digitalRead(clk) == digitalRead(dt));
}

void calibrate (){
  float calValue; // calibration value
  calValue = 696.0; // uncomment this if you want to set this value in the sketch
  #if defined(ESP8266)
  //EEPROM.begin(sizeof calValue); // uncomment this if you use ESP8266 and want to fetch this value from eeprom
  #endif
  //EEPROM.get(eepromAdress, calValue); // uncomment this if you want to fetch this value from eeprom

  Serial.begin(9600); 
  delay(10);
//  Serial.println();
//  Serial.println("Starting...");
  LoadCell.begin();
  long stabilisingtime = 2000; // tare preciscion can be improved by adding a few seconds of stabilising time
  LoadCell.start(stabilisingtime);
  if (LoadCell.getTareTimeoutFlag()) {
//    Serial.println("Tare timeout, check MCU>HX711 wiring and pin designations");
  }
  else {
    LoadCell.setCalFactor(calValue); // set calibration factor (float)
//    Serial.println("Startup + tare is complete");
  }
  while (!LoadCell.update());
//  Serial.print("Calibration factor: ");
//  Serial.println(LoadCell.getCalFactor());  
//  Serial.print("HX711 measured conversion time ms: ");
//  Serial.println(LoadCell.getConversionTime());
//  Serial.print("HX711 measured sampling rate HZ: ");
//  Serial.println(LoadCell.getSPS());
//  Serial.print("HX711 measured settlingtime ms: ");
//  Serial.println(LoadCell.getSettlingTime());
//  Serial.println("Note that the settling time may increase significantly if you use delay() in your sketch!");
  if (LoadCell.getSPS() < 7) {
//    Serial.println("!!Sampling rate is lower than specification, check MCU>HX711 wiring and pin designations");
  }
  else if (LoadCell.getSPS() > 100) {
//    Serial.println("!!Sampling rate is higher than specification, check MCU>HX711 wiring and pin designations");
  }
}

void setup() {

  Serial.begin(9600);
  lcd.begin();
  pinMode(sw, INPUT_PULLUP);
  pinMode(clk, INPUT);
  pinMode(dt, INPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);

  attachInterrupt (0, isr0, RISING);
  irrecv.enableIRIn(); // Start the receiver
   
  
}
void startMachine(){
  lcd.clear();
          lcd.setCursor(5, 0);
          lcd.print("Wait!");
          delay(2000);
          calibrate();
          LoadCell.update();
          delay(2000);
          lcd.clear();
          lcd.print("Pump 1 ON");
          digitalWrite(in2, HIGH);
  while (((int)LoadCell.getData())*(-1) +7< pump1ml) {
            LoadCell.update();
            lcd.clear();
            lcd.print((-1)*(int)LoadCell.getData()+15);
            if(((int)LoadCell.getData())*(-1)+7 > pump1ml){
              digitalWrite(in2, LOW);
            }
            delay(1000);
            
          }
          digitalWrite(in4, HIGH);
          while (((int)LoadCell.getData())*(-1) +7 - pump1ml < pump2ml) {
            LoadCell.update();
            lcd.clear();
            lcd.print((-1)*(int)LoadCell.getData()+15);
            if(((int)LoadCell.getData())*(-1)+7 - pump1ml > pump2ml){
              digitalWrite(in4, LOW);
            }
            delay(500);
            
          }
          screen = 1;
          subscreen = 0;
          changestate = 0;
          doonce = 0;
}
void loop() {
  if(Serial.available()){
    shit = Serial.readString();
    String shit2 = Serial.readString();
  
    
     pump1ml = shit.toInt();
     pump2ml = shit2.toInt();
     startMachine();
    
    Serial.print(shit);
    Serial.print(shit2);
  }
  

   if (irrecv.decode(&results)) // have we received an IR signal?
   {
    translateIR(); 
    irrecv.resume(); // receive the next value
   }  
   LoadCell.update();

  //get smoothed value from data set + current calibration factor
  
  if (millis() > t + 250) {
    float i = LoadCell.getData();
//    Serial.print("Load_cell output val: ");
//    Serial.println(i);
    t = millis();
  }
  if (TurnDetected) {
    delay(200);
    doonce = 0;
  
      if (up) {
      if(changestate == 0){
         subscreen++;
      if(subscreen > 3){
        subscreen = 0;
        }
      }
       
      }
      else {
      if(changestate == 0){
        subscreen--;
      if(subscreen < 0){
        subscreen = 3;
      }
      }
        
      }
    
  
      if (up) {
          if(changestate == 2 && subscreen == 0){
            pump1ml+=10;
          }
          if(changestate == 2 && subscreen == 1){
            pump2ml+=10;
          }
      
      }
      else {
        if(changestate == 2 && subscreen == 0){
          pump1ml-=10;
        }
        if(changestate == 2 && subscreen == 1){
          pump2ml-=10;
        }
       
      }
      TurnDetected = false;
    }
    
  


  if (digitalRead(sw) == LOW) {
    delay(200);
    changestate ++;
    if(changestate > 2){
      changestate = 0;
    }
    doonce = 0;
  }
  switch(screen){
    case 0:
    lcd.clear();
    lcd.setCursor(5, 0);
    lcd.print("Welcome");
    
    delay(2000);
    screen++;
    changestate ++;
    break;
    case 1:
    if(doonce == 0){
      switch(subscreen){
        case 0:
        lcd.clear();
        lcd.setCursor(5, 0);
        lcd.print("Menu");
        lcd.setCursor(5,1);
        lcd.print("Water");
        doonce = 1;
        switch(changestate){
          case 1:
          lcd.clear();
          lcd.setCursor(5, 0);
          lcd.print("Water");
          lcd.setCursor(5,1);
          lcd.print(pump1ml);
          lcd.setCursor(8,1);
          lcd.print("ml");
          doonce = 1;
          break;
          
          case 2:
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Change Water ml");
          lcd.setCursor(5,1);
          lcd.print(pump1ml);
          lcd.setCursor(8,1);
          lcd.print("ml");
          doonce = 1;
          break;
        }
        break;
        case 1:
        
        lcd.clear();
        lcd.setCursor(5, 0);
        lcd.print("Menu");
        lcd.setCursor(2,1);
        lcd.print("Orange Juice");
        doonce = 1;
        switch(changestate){
          case 1:
          lcd.clear();
          lcd.setCursor(2,0);
          lcd.print("Orange Juice");
          lcd.setCursor(5,1);
          lcd.print(pump2ml);
          lcd.setCursor(8,1);
          lcd.print("ml");
          doonce = 1;
          break;
          case 2:
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Change Juice ml");
          lcd.setCursor(5,1);
          lcd.print(pump2ml);
          lcd.setCursor(8,1);
          lcd.print("ml");
          
          doonce = 1;
          
          break;
        }
        break;
       
        case 2:
      
        lcd.clear();
        lcd.setCursor(5, 0);
        lcd.print("Menu");
        lcd.setCursor(5,1);
        lcd.print("Start");
        doonce = 1;
        switch(changestate){
          case 1:
        
          startMachine();
          
          break;
        }
        break;
      }
    }
    break;
  }
}
void(* resetFunc) (void) = 0;//declare reset function at address 0
void translateIR() // takes action based on IR code received

// describing Remote IR codes 

{

  switch(results.value)

  {

  case 0xFF629D: 
//  Serial.println(" FORWARD");
  if(changestate == 2 && subscreen == 0){
    pump1ml+=10;
  }
  if(changestate == 2 && subscreen == 1){
    pump2ml+=10;
  }
  doonce = 0;
  break;
  case 0xFF22DD: 
//  Serial.println(" LEFT");
  changestate = 0;
   subscreen--;
  if(subscreen < 0){
    subscreen = 2;
  }
  doonce = 0;
  break;
  case 0xFF02FD: 
//  Serial.println(" -OK-");
  screen=1;
  changestate ++;
  if(changestate > 2){
      changestate = 0;
    }
    doonce = 0;
  break;
  case 0xFFC23D:
//  Serial.println(" RIGHT"); 
  changestate = 0;
   subscreen++;
  if(subscreen > 2){
    subscreen = 0;
  }
  doonce = 0;
  break;
  case 0xFFA857: 
//  Serial.println(" REVERSE"); 
  if(changestate == 2 && subscreen == 0){
    pump1ml-=10;
  }
  if(changestate == 2 && subscreen == 1){
    pump2ml-=10;
  }

  doonce = 0;
  break;
  case 0xFF6897: 
//  Serial.println(" 1");  
  
  break;
  case 0xFF9867:
  //Serial.println(" 2"); 
  
  break;
  case 0xFFB04F:
  //Serial.println(" 3"); 
  
  break;
  case 0xFF30CF: 
  //Serial.println(" 4"); 
  
  break;
  case 0xFF18E7:
  //Serial.println(" 5"); 
  
  break;
  case 0xFF7A85: 
 // Serial.println(" 6");  
  
  break;
  case 0xFF10EF: 
//  Serial.println(" 7"); 
  
  break;
  case 0xFF38C7:
//  Serial.println(" 8");
  
  break;
  case 0xFF5AA5:
//  Serial.println(" 9");
  
  break;
  case 0xFF42BD:
//  Serial.println(" *"); 
  changestate --;
  if(changestate < 0){
      changestate = 0;
    }
    doonce = 0;
  break;
  case 0xFF4AB5: 
//  Serial.println(" 0");
  
  break;
  case 0xFF52AD: 
//  Serial.println(" #"); 
  resetFunc(); //call reset 
  break;
  case 0xFFFFFFFF: 
//  Serial.println(" REPEAT");
  
  doonce = 0;
  break;  

//  default: 
//  
//    Serial.println(" other button   ");
//     Serial.println(results.value,HEX);
//
  }// End Case
    doonce = 0;
   delay(200);
   
} 
