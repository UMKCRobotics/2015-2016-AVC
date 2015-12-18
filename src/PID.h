#ifndef PID_H
#define PID_H
#include <chrono>


typedef long double bigfloat;
typedef std::chrono::high_resolution_clock pid_clock;
typedef std::chrono::time_point<pid_clock> time_point;
typedef std::chrono::duration<bigfloat> duration;

template<class T> class PID{
 private:
 
  time_point prev_time;
  duration dt; //fuck your continuous math
  bigfloat integral;
  bigfloat derivative;
  T prev_error;
  bigfloat integral_gain;
  bigfloat derivative_gain;
  bigfloat proportional_gain;
  void setPrevTimeNow();
  time_point currentTime();
 public:
  //Constructors set previous time so start calling the calculation soon
  PID();
  PID(bigfloat p_gain, bigfloat i_gain, bigfloat d_gain);
  /*
   * Performs a PID control calculation. The output value is where you should put as the input to your system
   * see: https://en.wikipedia.org/wiki/PID_controller
   *
   * @param measured_value: The value you are currently measuring.
   * @param set_point: The value you would ideally be at.
   */
  T calculate(T measured_value, T set_point);
};

#endif
