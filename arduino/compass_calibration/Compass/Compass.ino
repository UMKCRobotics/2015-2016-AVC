#include "Wire.h"
#include "HMC5883L.h"

HMC5883L compass; //Copy the folder "HMC5883L" in the folder "C:\Program Files\Arduino\libraries" and restart the arduino IDE.

double xv, yv, zv;
double xb = -81.5;
double yb = -78.0;
double zb = 12.0;
double xs = 1.12;
double ys = 0.93;
double zs = 0.97;

//calibrated_values[3] is the global array where the calibrated data will be placed
//calibrated_values[3]: [0]=Xc, [1]=Yc, [2]=Zc
double calibrated_values[3];   
//transformation(float uncalibrated_values[3]) is the function of the magnetometer data correction 
//uncalibrated_values[3] is the array of the non calibrated magnetometer data
//uncalibrated_values[3]: [0]=Xnc, [1]=Ync, [2]=Znc
void transformation(double uncalibrated_values[3])    
{
  //calibration_matrix[3][3] is the transformation matrix
  //replace M11, M12,..,M33 with your transformation matrix data
  double calibration_matrix[3][3] = 
  {
    {xs, 0, 0},
    {0, ys, 0},
    {0, 0, zs}  
  };
  //bias[3] is the bias
  //replace Bx, By, Bz with your bias data
  double bias[3] = 
  {
    xb,
    yb,
    zb
  };  
  //calculation
  for (int i=0; i<3; ++i) uncalibrated_values[i] = uncalibrated_values[i] - bias[i];
  double result[3] = {0, 0, 0};
  for (int i=0; i<3; ++i)
    for (int j=0; j<3; ++j)
      result[i] += calibration_matrix[i][j] * uncalibrated_values[j];
  for (int i=0; i<3; ++i) calibrated_values[i] = result[i];
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
  double values_from_magnetometer[3];
  
  getHeading();
  values_from_magnetometer[0] = xv;
  values_from_magnetometer[1] = yv;
  values_from_magnetometer[2] = zv;
  transformation(values_from_magnetometer);

  /*Serial.flush(); 
  Serial.print(calibrated_values[0]); 
  Serial.print(",");
  Serial.print(calibrated_values[1]);
  Serial.print(",");
  Serial.print(calibrated_values[2]);
  Serial.println();

  delay(100); */
  Serial.print(calibrated_values[0]);
  Serial.print(",");
  Serial.print(calibrated_values[1]);
  Serial.print("$");
  delay(1000);
} 

void setupHMC5883L()
{  
  compass.SetScale(1.0);
  compass.SetMeasurementMode(Measurement_Continuous);
}
 
void getHeading()
{ 
  MagnetometerRaw raw = compass.ReadRawAxis();
  xv = (double)raw.XAxis;
  yv = (double)raw.YAxis;
  zv = (double)raw.ZAxis;
}



