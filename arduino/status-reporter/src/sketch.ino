#include <LiquidCrystal.h>
#include <FastLED.h>
#include <pins_arduino.h>
#include <math.h>
#define LCD_COLS 16
#define LCD_ROWS 2

const int MAX_MESSAGE_LENGTH = LCD_ROWS * LCD_COLS;
const int led_pin = 10;
const int button_pin = 3;



volatile bool finished_init = false;

LiquidCrystal lcd(2, 11, 5, 4, 6, 7);
CRGB led[1];


void setColor(int r, int g, int b){
  FastLED.showColor(CRGB(r,g,b));
}

void setup()
{
    Serial.begin(9600);
    lcd.begin(16,2);
    FastLED.addLeds<WS2811, led_pin, RGB>(led,1);
    attachInterrupt(button_pin - 2,goButtonInterrupt, CHANGE);
    lcdWrite("Starting...");
}

void lcdWrite(String message){
  lcd.clear();
  lcd.setCursor(0,0);
  if(message.length() > MAX_MESSAGE_LENGTH){ //Message wont fit uncompressed
    message = removeVowels(message);
  }
  int chars_written = 0;
  for(int i = 0; i < min(message.length(),MAX_MESSAGE_LENGTH); i++){
    lcd.setCursor(chars_written % LCD_COLS, floor(((double) chars_written) / LCD_COLS) );
    lcd.write(message[chars_written]);
    chars_written++;
  }
}

boolean recieved_first_message = false;
unsigned long old_time;

void loop()
{
  unsigned long new_time = millis();
  if(!recieved_first_message && new_time - old_time > 120){
    setColor(random(256),random(256),random(256));
    old_time = new_time;
  }
  if(Serial.available() > 0){
    recieved_first_message = true;
    String inputSerial = Serial.readString();
    char code = inputSerial[0];
    lightOnColor(code);
    lcdWrite(inputSerial.substring(1));
  }
}

String removeVowels(const String& input){
  String inputNoVowels;
  for(int i = 0; i < input.length(); i++){
    if(isNotVowel(input[i])){
            inputNoVowels += input[i];
    }
  }
  return inputNoVowels;
}

boolean isNotVowel(char c){
  return c != 'a' && c != 'A' 
    && c != 'e' && c != 'E'
    && c != 'i' && c != 'I'
    && c != 'o' && c != 'O'
    && c != 'u' && c != 'U';
}

void lightOnColor(char color){
  switch (color) {
    case 'i': 
      setColor(0,255,0);
      break;
    case 'w': 
      setColor(255,255,0);
      break;
    case 'e':
      setColor(255,0,0);
      break;
    default:
      setColor(255,255,255);
    }
}

void goButtonInterrupt(){
    Serial.write(1);
    lcdWrite("Sent go command to main board");
}
