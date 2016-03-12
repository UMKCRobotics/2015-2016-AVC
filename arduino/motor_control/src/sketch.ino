#include <Servo.h>
//#include <LiquidCrystal.h>

#define ESCPIN 10
#define STRPIN 9

#define STOPCMD 's'
#define FWRDCMD 'f'
#define BWRDCMD 'b'
#define TURNCMD 't'
#define CNTRCMD 'c'

/*NOTES:
 * THROTTLE
 * 1 unit of throttle = 1.232 inches/sec velocity
 * Throttle is neutral at 1500
 * Throttle has a deadzone around neutral of 76
 * Super fast vehicle, so throttle codes needs to be contrained based on testing environment
 * 
 * STEERING
 * max average turn in each direction is about 27 degrees
 * Turn is neutral at 1500
 * Turn deadzone is around neutral of close to 76, using the same deadzone for ease of use
 * Only turns, so turn code can allow full 1000-2000 range
*/

const int THROTTLE_DEADZONE = 100; //minimum throttle for wheel spin is 76
const int TURN_DEADZONE = 0;
const int NEUTRAL = 1500;
const int MAX_ABS_THROTTLE = 200;
const int MAX_ABS_INPUT = 500;
const int MAX_ANGLE = 27;

//LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

Servo ESC;
Servo STR;

int offset = 90;
int leftPos = -45;
int rightPos = 45;
bool notStarted = true;

void setup()
{
  //lcd.begin(16, 2);
  //UNCOMMENT WHEN READY TO USE
  ESC.attach(ESCPIN,1000,2000);
  ESC.write(NEUTRAL);

  STR.attach(STRPIN,1000,2000);
  STR.write(NEUTRAL);
	
  Serial.begin(57600);
  Serial.write('1');
  delay(1000);
}

void loop()
{
  if (Serial.available()) {
    notStarted = false;
    bool continueRead = true;
    char command;
    String value = "";
    //lcd.clear();
    command = (char)Serial.read();
    while (Serial.available() > 0 && continueRead)
    {
      char character = (char)Serial.read();
      if (character == '$')
      {
        continueRead = false;
        continue;
      }
      value += character;
    }

    parseMessage(command, value);

  }
  if (notStarted)
  {
    ESC.write(NEUTRAL);
    STR.write(NEUTRAL);
    delay(50);
  }
  else
  {
    delay(10);
  }
}

void parseMessage(char command, String value)
{
  if (command == STOPCMD)
  {
    //lcd.print("s = STOP");
    //lcd.setCursor(0, 1);
    //lcd.print(NEUTRAL);
    ESC.write(NEUTRAL);
  }
  else if (command == FWRDCMD)
  {
    int throttle;
    //lcd.print("f = FORWARD");
    //lcd.setCursor(0, 1);
    int intvalue = value.toInt();
    //CHANGE THIS TO CHANGE WHAT VALUES ARE ALLOWED
    intvalue = constrain(intvalue, 0, 99);
    if (intvalue != 0)
    {
      throttle = map(intvalue, 1, 99, NEUTRAL + THROTTLE_DEADZONE, NEUTRAL + MAX_ABS_THROTTLE);
    }
    else
    {
      throttle = NEUTRAL;
    }
    //lcd.print(throttle);
    ESC.write(throttle);
  }

  else if (command == BWRDCMD)
  {
    int throttle;
    //lcd.print("b = BACKWARD");
    //lcd.setCursor(0, 1);
    int intvalue = value.toInt();
    //CHANGE THIS TO CHANGE WHAT VALUES ARE ALLOWED
    intvalue = constrain(intvalue, 0, 99);
    if (intvalue != 0)
    {
      throttle = map(intvalue, 1, 99, NEUTRAL - THROTTLE_DEADZONE, NEUTRAL - MAX_ABS_THROTTLE);
    }
    else
    {
      throttle = NEUTRAL;
    }
    //lcd.print(throttle);
    ESC.write(throttle);
  }

  else if (command == TURNCMD)
  {
    int turn;
    //lcd.print("t = TURN");
    //lcd.setCursor(0,1);
    int intvalue = value.toInt();
    intvalue = constrain(intvalue,-MAX_ANGLE,MAX_ANGLE);
    if (intvalue > 0)
    {
      turn = map(intvalue, 1, MAX_ANGLE, NEUTRAL - TURN_DEADZONE, NEUTRAL - MAX_ABS_INPUT);
    }
    else if (intvalue < 0)
    {
      turn = map(intvalue, 1, -MAX_ANGLE, NEUTRAL + TURN_DEADZONE, NEUTRAL + MAX_ABS_INPUT);
    }
    else
    {
      turn = NEUTRAL;
    }
    //lcd.print(turn);
    STR.write(turn);
  }

  else if (command == CNTRCMD)
  {
    //lcd.print("c = CENTER");
    //lcd.setCursor(0, 1);
    //lcd.print(NEUTRAL);
    STR.write(NEUTRAL);
  }
}

