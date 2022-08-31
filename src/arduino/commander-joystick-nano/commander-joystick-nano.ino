//ROS
#include "ros.h"
#include "geometry_msgs/Twist.h"
#include <std_msgs/UInt16.h>
#include <ros/time.h>

ros::NodeHandle nh;
geometry_msgs::Twist twist;
ros::Publisher joy_pub("cmd_vel", &twist);

//std_msgs::UInt16 button_msg;
//ros::Publisher pub_button("buttons", &button_msg);

// timers for the sub-main loop
unsigned long currentMillis;
long previousMillis = 0;    // set up timers
float loopTime = 20;

// values for reading sticks

float stick1;
float stick2;
float stick3;
float stick4;
float stick5;
float stick6;
int but1;
int but2;
int but3;
int but4;
int but1out;
int but2out;
int but3out;
int but4out;
uint16_t combo = 0;

void setup() {

  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);

//  nh.getHardware()->setBaud(115200);      // set baud rate to 115200
  nh.initNode();              // init ROS
  nh.advertise(joy_pub);      // advertise topic
//  nh.advertise(pub_button);   // advertise topic

}

// deadzone function for cheap joysticks

float deadzone(float value) {
     if (value > 50) {
        value = value - 50;
     }
     else if (value < -50) {
      value = value +50;
     }
     else {
      value = 0;
     }
     value = value / 500;   // scale so that we get 0.0 ~ 1.0
     return value;  
}

int invButtons(int value) {
  
    if (value == 0) {
      value = 1;
    }
    else if (value == 1) {
      value = 0;
    }
    return value;
}

// ** Main loop **

void loop() {  

  currentMillis = millis();
        if (currentMillis - previousMillis >= loopTime) {  // run a loop every 10ms          
            previousMillis = currentMillis;          // reset the clock to time it 

            // read sticks and buttons

//            but1 = digitalRead(2);
//            but2 = digitalRead(3);
//            but3 = digitalRead(4);
//            but4 = digitalRead(5);

            // invert buttons as they are pull_ip

//            but1out = invButtons(but1);
//            but2out = invButtons(but2);
//            but3out = invButtons(but3);
//            but4out = invButtons(but4);

            // combine buttons into single uint16_t value
/*
            combo = 0;    // reset value and add buttons in this loop only

            if (but1out == 1) {
              combo = combo + 1;
            }
            else if (but2out == 1) {
              combo = combo  + 2;
            }
            else if (but3out == 1) {
              combo = combo + 4;
            }
            else if (but4out == 1) {
              combo = combo + 8;
            }
            else {
              combo = 0;      // of no buttons are pressed then combo is zero
            }
*/          
            stick1 = analogRead(A0);
            stick2 = analogRead(A1);

            // remove offsets
          
            stick1 = stick1 - 5 - 512;
            stick1 = deadzone(stick1);
            stick2 = stick2 - 5 - 512;
            stick2 = deadzone(stick2);

            // *** broadcast cmd_vel twist message **

            // Update the Twist message
            twist.linear.x = 0;
            twist.linear.y = stick1;
            twist.linear.z = 0;
        
            twist.angular.x = 0;
            twist.angular.y = 0;
            twist.angular.z = stick2;

            joy_pub.publish(&twist);        // make the message ready to publish            

            // *** broadcast buttons ***

//            button_msg.data = combo;
//            pub_button.publish(&button_msg);  

            nh.spinOnce();                  // make sure we do ROS stuff every loop

        } // end of 10ms loop

} // end of main loop
