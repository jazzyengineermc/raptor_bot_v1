/*
    DIY Arduino based RC Transmitter Project
              == Receiver Code ==

  by Dejan Nedelkovski, www.HowToMechatronics.com
  Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/
#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <printf.h>
#include <ros.h>
#include <std_msgs/String.h>
#include <std_msgs/Int16.h>
#include <geometry_msgs/Twist.h>
#include <ros/time.h>

#define enA 8
#define enB 9
#define rmd 5
#define lmd 12
#define stp1 35
#define stp2 13

ros::NodeHandle  nh;

RF24 radio(10, 53);   // nRF24L01 (CE, CSN)
const byte address[6] = "00001";

unsigned long lastReceiveTime = 0;
unsigned long currentTime = 0;

// Max size of this struct is 32 bytes - NRF24L01 buffer limit
struct Data_Package {
  int j1PotX;
  int j1PotY;
  int j1Button;
};

Data_Package data; //Create a variable with the above structure

float wheel1;
float wheel2;

float wheel1a;
float wheel2a;

float demandx;
float demandz;

unsigned long currentMillis;
unsigned long previousMillis;
int loopTime = 10;

// ** ROS callback & subscriber **

void velCallback(  const geometry_msgs::Twist& vel)
{
     demandx = vel.linear.x;
     demandz = vel.angular.z;

     demandx = constrain(demandx,-1,1);     // try to keep it under control
     demandz = constrain(demandz,-1,1);

     demandx = demandx * 350;
     demandz = demandz * 75;
}

std_msgs::Int16 str_msg;
ros::Subscriber<geometry_msgs::Twist> sub("cmd_vel" , velCallback);     //create a subscriber for ROS cmd_vel topic


void setup() {
  Serial.begin(9600);
  printf_begin();
  Serial.println("RaptorBot");
//  radio.begin();
//  radio.openReadingPipe(0, address);
  //radio.setAutoAck(false);
//  radio.setDataRate(RF24_250KBPS);
//  radio.setPALevel(RF24_PA_LOW);
//  radio.setChannel(124);
//  radio.printDetails();
//  radio.startListening(); //  Set the module as receiver
//  resetData();
  pinMode(rmd, OUTPUT);
  pinMode(lmd, OUTPUT);
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(stp1, OUTPUT);
  pinMode(stp2,OUTPUT);
  nh.initNode();              // init ROS
  nh.subscribe(sub);          // subscribe to cmd_vel
}

void loop() {

  
  // Check whether there is data to be received
//  if (radio.available()) {
//    radio.read(&data, sizeof(Data_Package)); // Read the whole data and store it into the 'data' structure
//  lastReceiveTime = millis(); // At this moment we have received the data

  nh.spinOnce();
  
  // Check whether we keep receving data, or we have a connection between the two modules
  currentTime = millis();
  if ( currentTime - lastReceiveTime > 1000 ) { // If current time is more then 1 second since we have recived the last data, that means we have lost connection
    resetData(); // If connection is lost, reset the data. It prevents unwanted behavior, for example if a drone has a throttle up and we lose connection, it can keep flying unless we reset the values
  }  

        // put data into usable values
//        demandx = map(data.j1PotY, 0, 1023, -100, 100);
//        demandz = map(data.j1PotX, 0, 1023, -65, 65);

        // work out the two values for differential drive of each wheel

        wheel1 = demandx - (demandz);
        wheel2 = demandx + (demandz);

        wheel1 = constrain(wheel1,-255,255);
        wheel2 = constrain(wheel2,-255,255);

      
        // drive motors


        if (wheel1 > 30) {
          wheel1a = wheel1;
          analogWrite(enA, wheel1a);
          digitalWrite(rmd, HIGH);
          digitalWrite(stp1, LOW);
        }
        else if (wheel1 < -30) {
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

        if (wheel2 > 30) {
          wheel2a = wheel2;
          analogWrite(enB, wheel2a);
          digitalWrite(lmd, HIGH);
          digitalWrite(stp2, LOW);
        }
        else if (wheel2 < -30) {
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
//  Serial.print("j1PotX: ");
//  Serial.print(data.j1PotX);
//  Serial.print("  j1PotY: ");
//  Serial.print(data.j1PotY);
//  Serial.print("  enA: ");
//  Serial.print(wheel1a);
//  Serial.print("  enB: ");
//  Serial.print(wheel2a);
//  Serial.print("  j1Button: ");
//  Serial.println(data.j1Button);
  
  delay(10);
}

void resetData() {
  // Reset the values when there is no radio connection - Set initial default values
  data.j1PotX = 515;
  data.j1PotY = 523;
  data.j1Button = 1;
}
