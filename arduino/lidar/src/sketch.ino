#include <Servo.h>

#define TRIGGER_PIN 2
#define INPUT_PIN 3 
#define TIMEOUT 100000
#define CENTER_DEGREE 98
#define POS_SWEEP_ANGLE 27

unsigned long pulse_width;
Servo LServo;

int degree = CENTER_DEGREE;
void setup()
{
  Serial.begin(115200); // Start serial communications
  pinMode(TRIGGER_PIN, OUTPUT); // Set pin 2 as trigger pin
  pinMode(INPUT_PIN, INPUT); // Set pin 3 as monitor pin
  digitalWrite(TRIGGER_PIN, LOW); // Set trigger LOW for continuous read
  
  // Sets the pin with which it is connected to the Arduino by. Damn Kevin...
  LServo.attach(5);
  LServo.write(CENTER_DEGREE);
}

void loop()
{

  for(degree = CENTER_DEGREE + POS_SWEEP_ANGLE; degree >= CENTER_DEGREE - POS_SWEEP_ANGLE; degree--)
  {
    scan();
    LServo.write(degree);
  }
  for(degree = CENTER_DEGREE - POS_SWEEP_ANGLE; degree <= CENTER_DEGREE + POS_SWEEP_ANGLE; degree++)
  {
    scan(); 
    LServo.write(degree);
  }
}

void scan()
{
  pulse_width = pulseIn(INPUT_PIN, HIGH); // Count how long the pulse is high in microseconds

  if(pulse_width != 0){ // If we get a reading that isn't zero, let's print it
        pulse_width = pulse_width/10; // 10usec = 1 cm of distance for LIDAR-Lite
        String send_string = String(degree-CENTER_DEGREE) + "," + String(pulse_width) + "$";
  	Serial.print(send_string); // Print the distance
  }
}
