#include <Wire.h>


void setup() {
  // Open serial communications and wait for port to open:
  Wire.begin(); 
  
  // instruct chip we want to read it continuously (TODO: REPLACE WITH BETTER POWER SAVING MODE)
  Wire.beginTransmission(0x1e); // transmit to device 0x1e       
  Wire.write(0x02);              // sends one byte (select register to write to - MODE)
  Wire.write(0x00);              // sends one byte (configure mode - continuous)
  Wire.endTransmission();
}

void loop() {  // run over and over 
    // Now take a compass reading to supplement GPS information   
    int x,y,z; //triple axis data

    //Tell the HMC5883L where to begin reading data
    Wire.beginTransmission(0x1e);
    Wire.write(0x03); //select register 3, X MSB register - to start read from
    Wire.endTransmission();

    //Read data from each axis, 2 registers per axis
    Wire.requestFrom(0x1e, 6);
    if (6 <= Wire.available()) {
    x = Wire.read()<<8; //X msb
    x |= Wire.read(); //X lsb
    z = Wire.read()<<8; //Z msb
    z |= Wire.read(); //Z lsb
    y = Wire.read()<<8; //Y msb
    y |= Wire.read(); //Y lsb
    }

    //Print out values of each axis
    Serial.print("COMPASS: x: ");
    Serial.print(x);
    Serial.print("  y: ");
    Serial.print(y);
    Serial.print("  z: ");
    Serial.println(z);
}