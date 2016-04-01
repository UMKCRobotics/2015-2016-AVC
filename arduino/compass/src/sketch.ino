#include "Wire.h"
#include "HMC5883L.h"


//this code was obtained from MagMaster, specificalle Arduino_Radius_Stabilisation.ino

HMC5883L compass; //Copy the folder "HMC5883L" in the folder "C:\Program Files\Arduino\libraries" and restart the arduino IDE.

float xv, yv, zv;

const double pi = 3.14159265358979;

//calibrated_values[3] is the global array where the calibrated data will be placed
//calibrated_values[3]: [0]=Xc, [1]=Yc, [2]=Zc
float calibrated_values[3];   
//transformation(float uncalibrated_values[3]) is the function of the magnetometer data correction 
//uncalibrated_values[3] is the array of the non calibrated magnetometer data
//uncalibrated_values[3]: [0]=Xnc, [1]=Ync, [2]=Znc
void transformation(float uncalibrated_values[3])    
{
  //calibration_matrix[3][3] is the transformation matrix
  //replace M11, M12,..,M33 with your transformation matrix data
  double calibration_matrix[3][3] = 
  {
    /*{1.442, 0.002, 0.063},
    {0.23, 1.046, 0.073},
    {0.039, -0.002, 1.261}*/  
    /*{0.930, 0.000, 0.000},
    {0.000, 1.020, 0.000},
    {0.000, 0.000, 1.060}*/
    {0.950, 0.000, 0.000},
    {0.000, 0.990, 0.000},
    {0.000, 0.000, 1.070}
  };
  //bias[3] is the bias
  //replace Bx, By, Bz with your bias data
  double bias[3] = 
  {
    /*-46.654,
    -158.777,
    -4.513*/
    /*-47.50,
    -159.00,
    8.50*/
    -50.00,
    -143.00,
    -1.00

  };  
  //calculation
  for (int i=0; i<3; ++i) uncalibrated_values[i] = uncalibrated_values[i] - bias[i];
  float result[3] = {0, 0, 0};
  for (int i=0; i<3; ++i)
    for (int j=0; j<3; ++j)
      result[i] += calibration_matrix[i][j] * uncalibrated_values[j];
  for (int i=0; i<3; ++i) calibrated_values[i] = result[i];
}

//vector_length_stabilasation() - is the function of the magnetometer vector length stabilasation (stabilisation of the sphere radius)
float scaler;
boolean scaler_flag = false;
float normal_vector_length;
void vector_length_stabilasation(){
  //calculate the normal vector length
  if (scaler_flag == false)
  {
    getHeading();
    normal_vector_length = sqrt(calibrated_values[0]*calibrated_values[0] + calibrated_values[1]*calibrated_values[1] + calibrated_values[2]*calibrated_values[2]);
    scaler_flag = true;
  } 
  //calculate the current scaler
  scaler = normal_vector_length/sqrt(calibrated_values[0]*calibrated_values[0] + calibrated_values[1]*calibrated_values[1] + calibrated_values[2]*calibrated_values[2]);
  //apply the current scaler to the calibrated coordinates (global array calibrated_values)
  calibrated_values[0] = calibrated_values[0]*scaler;
  calibrated_values[1] = calibrated_values[1]*scaler;
  calibrated_values[2] = calibrated_values[2]*scaler;
}

void setup()
{   
  Serial.begin(9600);
  Wire.begin();  
  compass = HMC5883L();  
  setupHMC5883L();       
}

void loop()
{
  float values_from_magnetometer[3];
  
  getHeading();
  values_from_magnetometer[0] = xv;
  values_from_magnetometer[1] = yv;
  values_from_magnetometer[2] = zv;
  transformation(values_from_magnetometer);
  
  vector_length_stabilasation();

  Serial.print(calibrated_values[0]);
  Serial.print(',');
  Serial.print(calibrated_values[1]);
  Serial.print('$');
  //Serial.println(heading(calibrated_values[0],calibrated_values[1]));


  delay(1000); 
} 

void setupHMC5883L()
{  
  compass.SetScale(0.88);
  compass.SetMeasurementMode(Measurement_Continuous);
}
 
void getHeading()
{ 
  MagnetometerRaw raw = compass.ReadRawAxis();
  xv = (float)raw.XAxis;
  yv = (float)raw.YAxis;
  zv = (float)raw.ZAxis;
}

double heading(double x, double y)
{
  double head = atan2(y,x);
  head = head*180/pi;
  return head;
}





/*#include <Wire.h>
void setup() {
  // Open serial communications and wait for port to open:
  Wire.begin(); 
  
  // instruct chip we want to read it continuously (TODO: REPLACE WITH BETTER POWER SAVING MODE)
  Wire.beginTransmission(0x1e); // transmit to device 0x1e       
  Wire.write(0x02);              // sends one byte (select register to write to - MODE)
  Wire.write(0x00);              // sends one byte (configure mode - continuous)
  Wire.endTransmission();
  
  Serial.begin(9600);
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
      
      Serial.print(x);
      Serial.print(',');
      Serial.print(y);
      Serial.print('$');
      
   delay(1000); 
}*/
