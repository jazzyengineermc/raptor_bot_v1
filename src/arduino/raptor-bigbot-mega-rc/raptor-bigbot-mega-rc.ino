/*
    DIY Arduino based RC Transmitter Project
              == Receiver Code ==

  by Dejan Nedelkovski, www.HowToMechatronics.com
  Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/
#include <Arduino.h>
#include <SPI.h>
#include <printf.h>

#define enA 8
#define enB 9
#define rmd 5
#define lmd 12
#define stp1 35
#define stp2 13

#define RCPinY 2
#define RCPinX 3
volatile long PulsesY = 0;
volatile long PulsesX = 0;
int PulseWidthY = 0;
int PulseWidthX = 0;
volatile long StartTimeY = 0;
volatile long CurrentTimeY = 0;
volatile long StartTimeX = 0;
volatile long CurrentTimeX = 0;



float wheel1;
float wheel2;

float wheel1a;
float wheel2a;

float demandx;
float demandz;

void setup() {
  Serial.begin(9600);
  printf_begin();
  Serial.println("RaptorBot");
  pinMode(rmd, OUTPUT);
  pinMode(lmd, OUTPUT);
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(stp1, OUTPUT);
  pinMode(stp2,OUTPUT);
  pinMode(RCPinY, INPUT_PULLUP);
  pinMode(RCPinX, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(RCPinY),PulseTimerY,CHANGE);
  attachInterrupt(digitalPinToInterrupt(RCPinX),PulseTimerX,CHANGE);
}

void PulseTimerY(){
  CurrentTimeY = micros();
   if (CurrentTimeY > StartTimeY){
    PulsesY = CurrentTimeY - StartTimeY;
    StartTimeY = CurrentTimeY;
   }
  }

void PulseTimerX(){
  CurrentTimeX = micros();
   if (CurrentTimeX > StartTimeX){
    PulsesX = CurrentTimeX - StartTimeX;
    StartTimeX = CurrentTimeX;
   }
  }

void loop() {

  if (PulsesY < 2000){
    PulseWidthY = PulsesY;
  }  
  if (PulsesX < 2000){
    PulseWidthX = PulsesX;
  }  


        PulseWidthY = (PulseWidthY) - 20;
        PulseWidthX = (PulseWidthX) + 8;

        demandx = map(PulseWidthY, 1000, 2000, -255, 255); // Map lineular velocity
        demandz = map(PulseWidthX, 1000, 2000, -95, 95);
        
        // some logic to help smooth out rotational velocity
        if (demandx < -90) {
          demandz = map(PulseWidthX, 1000, 2000, -25, 25);
        }
       
        if (demandx > 90) {
          demandz = map(PulseWidthX, 1000, 2000, -30, 30);
        }

        if (demandx > 120) {
          demandz = map(PulseWidthX, 1000, 2000, -10, 10);
        }

        demandx = constrain(demandx, -255,255);
        demandz = constrain(demandz, -100,100);
        
        // work out the two values for differential drive of each wheel

        wheel1 = demandx - (demandz);
        wheel2 = demandx + (demandz);

        wheel1 = constrain(wheel1,-245,245);
        wheel2 = constrain(wheel2,-245,245);

      
        // drive motors


        if (wheel1 > 25) {
          wheel1a = wheel1;
          analogWrite(enA, wheel1a);
          digitalWrite(rmd, HIGH);
          digitalWrite(stp1, LOW);
        }
        else if (wheel1 < -25) {
          wheel1a = wheel1 * -1;
          analogWrite(enA, wheel1a);
          digitalWrite(rmd, LOW);
          digitalWrite(stp1, LOW);
        }
        else {
          wheel1a = 0;
          analogWrite(enA, wheel1a);
          digitalWrite(stp1, HIGH);
        }

        // other motor

        if (wheel2 > 25) {
          wheel2a = wheel2;
          analogWrite(enB, wheel2a);
          digitalWrite(lmd, HIGH);
          digitalWrite(stp2, LOW);
        }
        else if (wheel2 < -25) {
          wheel2a = wheel2 * -1;
          analogWrite(enB, wheel2a);
          digitalWrite(lmd, LOW);
          digitalWrite(stp2, LOW);
        }
        else {
          wheel2a = 0;
          analogWrite(enB, wheel2a);
          digitalWrite(stp2, HIGH);
        }


//  }  
  // Print the data in the Serial Monitor
  Serial.print("X: ");
  Serial.print(PulseWidthX);
  Serial.print("  Y: ");
  Serial.print(PulseWidthY);
  Serial.print("  enA: ");
  Serial.print(wheel1a);
  Serial.print("  enB: ");
  Serial.println(wheel2a);
//  Serial.print("  j1Button: ");
//  Serial.println(data.j1Button);
  
  delay(5);
}
