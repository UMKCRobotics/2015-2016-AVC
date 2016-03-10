#ifndef VISION_H
#define MOTOR_CONTROLLER_H

#include <string>
#include <iostream>
#include <map>
#include <cmath>
#include <thread>
#include <stdlib.h>
#include <stdexcept>
#include <sstream>

#include "Conf.hpp"
#include "Pathfinder.h"

#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

struct VisionReadings {
	float color_loc_x[4];
	float color_loc_y[4];
	int dim[2]; //image dimensions
};

class Vision : public Pathfinder {
	private:
		//Functions:
		void initialize_values(Conf c);
		VisionReadings readCamera();
		double getYDist(int y_raw, int y_dim);
		double getXDist(int x_raw, int y_raw, int x_dim, int y_dim);
		double getAngle(double x_d, double y_d);
		//Variables:
		bool threadContinue;
		int skip_frames;
		double scale;
		double unit;
		double c_y_dist[4];
		double c_x_dist[2];
		VisionReadings color_locations = {{0,0,0,0},{0,0,0,0},{0,0}}; //color container
		VideoCapture cap_main;
		thread vision_thread;
		vector<vector<Scalar>> colors_track = vector<vector<Scalar>>(4, vector<Scalar>(2));
	public:
		VisionReadings getLocations();
		Vision(Conf configuration);
		~Vision();
		//the thing we need
		Angle bestAvailableHeading(Angle desiredHeading);
};

#endif
