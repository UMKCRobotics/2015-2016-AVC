#include "AngleMath.h"

double customModulous(double a, double n){
    //mod without having the same sign as dividend
    return a - floor(a/n) * n;
}
double AngleMath::angleBetweenTwoAngles(double source, double target){
    //https://stackoverflow.com/questions/1878907/the-smallest-difference-between-2-angles
    double a  = target - source;
    return customModulous((a+180),360) - 180;
}

