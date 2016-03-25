#include "Wire.h"
#include "HMC5883L.h"

HMC5883L compass; //Copy the folder "HMC5883L" in the folder "C:\Program Files\Arduino\libraries" and restart the arduino IDE.

double xv, yv, zv;
double bias[] = {0,0,0};
double scale[] = {0,0,0};

void setup()
{   
  Serial.begin(9600);
  Wire.begin();  
  compass = HMC5883L();  
  setupHMC5883L();   
  calibrateMag(bias,scale);
  Serial.flush();
  Serial.println("Bias(x,y,z):");
  Serial.print(bias[0]);
  Serial.print(',');
  Serial.print(bias[1]);
  Serial.print(',');
  Serial.println(bias[2]);
  Serial.println("Scale(x,y,z):");
  Serial.print(scale[0]);
  Serial.print(',');
  Serial.print(scale[1]);
  Serial.print(',');
  Serial.println(scale[2]);
}

void loop()
{
  /*getHeading();

  Serial.flush(); 
  Serial.print(xv); 
  Serial.print(",");
  Serial.print(yv);
  Serial.print(",");
  Serial.print(zv);
  Serial.println();

  delay(100); */

} 

void calibrateMag(double dest1[3], double dest2[3])
{
  int sample_count = 0;
  double mag_bias[3] = {0,0,0};
  double mag_scale[3] = {0,0,0};
  double mag_max[3] = {-16000,-16000,-16000};
  double mag_min[3] = {16000,16000,16000};
  double mag_temp[3] = {0,0,0};

  //get samples to determine max and min of each axis
  sample_count = 248;
  Serial.print("Going to: ");
  Serial.println(sample_count);
  for (int i = 0; i < sample_count; i++)
  {
    readMagData(mag_temp);
    for (int j = 0; j < 3; j++)
    {
      if (mag_temp[j] > mag_max[j])
        mag_max[j] = mag_temp[j];
      if (mag_temp[j] < mag_min[j])
        mag_min[j] = mag_temp[j];
    }
    delay(100);
  }
  Serial.println("");
  //hard iron correction
  mag_bias[0] = (mag_max[0] + mag_min[0])/2;
  mag_bias[1] = (mag_max[1] + mag_min[1])/2;
  mag_bias[2] = (mag_max[2] + mag_min[2])/2;

  dest1[0] = mag_bias[0]; //save mag bias
  dest1[1] = mag_bias[1];
  dest1[2] = mag_bias[2];

  //soft iron correction estimate
  mag_scale[0] = (mag_max[0] - mag_min[0])/2;
  mag_scale[1] = (mag_max[1] - mag_min[1])/2;
  mag_scale[2] = (mag_max[2] - mag_min[2])/2;

  double avg_rad = mag_scale[0] + mag_scale[1] + mag_scale[2];
  avg_rad /= 3.0;

  dest2[0] = avg_rad/((double)mag_scale[0]);
  dest2[1] = avg_rad/((double)mag_scale[1]);
  dest2[2] = avg_rad/((double)mag_scale[2]);

  Serial.println("Mag Calibration done!");
}

void readMagData(double mag_temp[3])
{
  getHeading();
  mag_temp[0] = xv;
  mag_temp[1] = yv;
  mag_temp[2] = zv;
  Serial.print(xv); 
  Serial.print(",");
  Serial.print(yv);
  Serial.print(",");
  Serial.print(zv);
  Serial.println();
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
