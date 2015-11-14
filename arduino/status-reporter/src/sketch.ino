#include<LiquidCrystal.h>

const int yellowpin = 8;
const int redpin = 9;
const int greenpin =10;


LiquidCrystal lcd(12, 11, 5, 4, 3, 2);


void setup()
{
	Serial.begin(9600);
        pinMode(redpin, OUTPUT);
        pinMode(yellowpin, OUTPUT);
        pinMode(greenpin, OUTPUT);
	//number of rows and columns

        digitalWrite(yellowpin, LOW);	
        digitalWrite(redpin, LOW);
        digitalWrite(greenpin, LOW);
        lcd.begin(16,2);
}

void loop()
{
	if(Serial.available() > 0)
	{
		String inputSerial = Serial.readString();
		String inputNoVowels;
		char code = inputSerial[0];
   
               Serial.println(code);
		if(code == 'i')
                  {
			digitalWrite(greenpin, HIGH);
                        digitalWrite(yellowpin, LOW);
                        digitalWrite(redpin, LOW);
		}
                else if (code == 'w')
                {
                        digitalWrite(greenpin, LOW);
                        digitalWrite(redpin, LOW);
			digitalWrite(yellowpin, HIGH);
                }
		else if (code == 'e')
                {
			digitalWrite(redpin, HIGH);
                        digitalWrite(greenpin, LOW);
                        digitalWrite(yellowpin, LOW);
		}	

		for(int i = 0; i <inputSerial.length(); i++)
		{
			if(inputSerial[i] == 'a' || inputSerial[i] == 'e' || inputSerial[i] == 'i' || inputSerial[i] == 'o' || inputSerial[i] == 'u')
			{}
			else
			{
				inputNoVowels += inputSerial[i];
			}
		}

                Serial.println(inputNoVowels);
                lcd.setCursor(1,1);
		lcd.print(inputNoVowels);
		lcd.display();
	}
}
