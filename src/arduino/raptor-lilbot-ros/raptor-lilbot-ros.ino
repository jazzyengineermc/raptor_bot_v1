
#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <printf.h>
#include <NewPing.h>
#include <ros.h>
#include <std_msgs/String.h>
#include <std_msgs/Int16.h>
#include <geometry_msgs/Twist.h>
#include <ros/time.h>
#include <DHT.h>

ros::NodeHandle  nh;

#define DHTPIN 11
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

float hum;
float temp;
float soundsp;
float soundcm;

#define TRIGGER_PIN 12
#define ECHO_PIN 13
#define MAX_DISTANCE 400

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

float duration;
float distance;
int iterations = 5;

//#define enA 8 // Right motor
//#define in2 27
//#define in1 25
//#define hs1A 5 // hall sensors
//#define hs1B 4

//#define enB 9 // Left motor
//#define in4 29
//#define in3 31
//#define hs2A 3 // hall sensors
//#define hs2B 2

#define enA 9 // Right motor
#define in2 29
#define in1 31
#define hs1A 3 // hall sensors
#define hs1B 2

#define enB 8 // Left motor
#define in4 27
#define in3 25
#define hs2A 5 // hall sensors
#define hs2B 4

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

float speed_act_left; // actual left wheel speed in m/s
float speed_act_right; // actual right wheel speed in m/s

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
ros::Publisher obstruction("obstruction", &str_msg);

void setup() {
//  Serial.begin(9600);
//  printf_begin();
//  Serial.println("RaptorBot");ll
  dht.begin();
//  radio.begin();
//  radio.openReadingPipe(0, address);
//  radio.setDataRate(RF24_250KBPS);
//  radio.setPALevel(RF24_PA_LOW);
//  radio.setChannel(124);
//  radio.printDetails();
//  radio.startListening(); //  Set the module as receiver
//  resetData();
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(hs1A, INPUT);
  pinMode(hs1B, INPUT);
  pinMode(hs2A, INPUT);
  pinMode(hs2B, INPUT);
  nh.initNode();              // init ROS
  nh.subscribe(sub);          // subscribe to cmd_vel
  nh.advertise(obstruction);
  
//  pinMode(4, OUTPUT);     // motor PWM pins
//  pinMode(5, OUTPUT);
//  pinMode(6, OUTPUT);
//  pinMode(7, OUTPUT);
}

void loop() {

  hum = dht.readHumidity();
  temp = dht.readTemperature();

  soundsp = 331.4 + (0.606 * temp) + (0.0124 * hum);
  soundcm = soundsp / 10000;
  
  duration = sonar.ping_median(iterations);

  distance = (duration / 2) * soundcm;

  nh.spinOnce();        // make sure we listen for ROS messages and activate the callback if there is one

     currentMillis = millis();  

     if (currentMillis - previousMillis >= loopTime) {  // start timed loop for everything else
         previousMillis = currentMillis;

        // work out the two values for differential drive of each wheel

        wheel1 = demandx - (demandz);
        wheel2 = demandx + (demandz);

        wheel1 = constrain(wheel1,-255,255);
        wheel2 = constrain(wheel2,-255,255);

      
        // drive motors


        if (wheel1 > 0) {
          wheel1a = wheel1;
          analogWrite(enA, wheel1a);
          digitalWrite(in2, LOW);
          digitalWrite(in1, HIGH);
        }
        else if (wheel1 < 0) {
          wheel1a = wheel1 * -1;
          analogWrite(enA, wheel1a);
          digitalWrite(in2, HIGH);
          digitalWrite(in1, LOW);
        }
        else {
          wheel1a = 0;
          analogWrite(enA, wheel1a);
        }

        // other motor

        if (wheel2 > 0) {
          wheel2a = wheel2;
          analogWrite(enB, wheel2a);
          digitalWrite(in4, LOW);
          digitalWrite(in3, HIGH);
        }
        else if (wheel2 < 0) {
          wheel2a = wheel2 * -1;
          analogWrite(enB, wheel2a);
          digitalWrite(in4, HIGH);
          digitalWrite(in3, LOW);
        }
        else {
          wheel2a = 0;
          analogWrite(enB, wheel2a);
        }


     } // end of timed loop    
   uint16_t obs = (distance);
   str_msg.data = obs;
   obstruction.publish( &str_msg );
}

void resetData() {
  // Reset the values when there is no radio connection - Set initial default values
  data.j1PotX = 515;
  data.j1PotY = 523;
  data.j1Button = 1;
}
