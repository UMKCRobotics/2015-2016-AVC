#include <Servo.h>

/* =============================================================================
  LIDAR-Lite v2: PWM operation

  This example demonstrates how to read measurements from LIDAR-Lite v2 "Blue
  Label" using PWM

  The library is in BETA, so subscribe to the github repo to recieve updates, or
  just check in periodically:
  https://github.com/PulsedLight3D/LIDARLite_v2_Arduino_Library

  To learn more read over lidarlite.cpp as each function is commented
=========================================================================== */

unsigned long pulse_width;
Servo LServo;

int degree = 180;

void setup()
{
  Serial.begin(9600); // Start serial communications
  pinMode(2, OUTPUT); // Set pin 2 as trigger pin
  pinMode(3, INPUT); // Set pin 3 as monitor pin
  digitalWrite(2, LOW); // Set trigger LOW for continuous read
  
  // Sets the pin with which it is connected to the Arduino by. Damn Kevin...
  LServo.attach(5);
  LServo.write(degree);
}

void loop()
{
  for(degree = 180; degree >= 0; degree--)
  {
    scan();
    LServo.write(degree);
    delay(25);
  }
  for(degree = 0; degree <= 180; degree++)
  {
    scan(); 
    LServo.write(degree);
    delay(25);
  }
}

void scan()
{
  pulse_width = pulseIn(3, HIGH); // Count how long the pulse is high in microseconds
  
  if(pulse_width != 0){ // If we get a reading that isn't zero, let's print it
        pulse_width = pulse_width/10; // 10usec = 1 cm of distance for LIDAR-Lite
        String send_string = String(degree-90) + "," + String(pulse_width) + "$";
  	Serial.print(send_string); // Print the distance
  }
}
