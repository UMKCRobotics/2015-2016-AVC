#include <Servo.h>

#define TRIGGER_PIN 2
#define INPUT_PIN 3 
#define TIMEOUT 100000
#define CENTER_DEGREE 0
#define POS_SWEEP_ANGLE 20

unsigned long pulse_width;
Servo LServo;
String command;
bool lightOn = false;
int counter;
int divisor;

int degree = CENTER_DEGREE;
void setup()
{
  pinMode(13,OUTPUT);
  LServo.attach(5);
  Serial.begin(9600); // Start serial communications
  Serial.write(5);
  counter = 0;
  divisor = 250;
}

void loop()
{
	command = "";
	if (Serial.available())
	{
		while(Serial.available() > 0)
		{
			command += (char)Serial.read();
			delay(1);
		}
		LServo.write(command.toInt());
		Serial.println(command);
		divisor = command.toInt();
	}
	delay(10);
}


