#include "PID.h"

template<class T>
PID<T>::PID(){
  setPrevTimeNow();
  setGains(1,1,1);
  integral = 0;
  prev_error = 0;
}

template<class T>
PID<T>::PID(bigfloat p_gain, bigfloat i_gain, bigfloat d_gain){
  setPrevTimeNow();
  setGains(p_gain,i_gain,d_gain);
  integral = 0;
  prev_error = 0;
}

template<class T>
void PID<T>::setGains(bigfloat p_gain, bigfloat i_gain, bigfloat d_gain){
  proportional_gain = p_gain;
  integral_gain = i_gain;
  derivative_gain = d_gain;

}

template<class T>
time_point PID<T>::currentTime(){
  return pid_clock::now();
}

template<class T>
void PID<T>::setPrevTimeNow(){
  prev_time = currentTime();
}

//Pretty much taken directly from https://en.wikipedia.org/wiki/PID_controller#Pseudocode
template<class T>
T PID<T>::calculate(T measured_value, T set_point){
  dt = prev_time - currentTime();
  prev_time = currentTime();

  T error = set_point - measured_value;
  integral = integral + error * dt;
  derivative = (error - prev_error) / dt;
  T output = proportional_gain * error + integral_gain * integral + derivative_gain * derivative;
  prev_error = error;

  return output;
}
