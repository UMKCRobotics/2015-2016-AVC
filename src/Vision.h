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
};

class Vision : public Pathfinder {
	private:
		bool threadContinue;
		VisionReadings color_locations = {{0,0,0,0},{0,0,0,0}}; //color container
		void initialize_colors(Conf c);
		VideoCapture cap_main;
		VisionReadings readCamera();
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