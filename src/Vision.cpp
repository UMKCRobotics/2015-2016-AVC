#include "Vision.h"
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;


void Vision::initialize_values(Conf c)
{
	//BLUE INFO
	Scalar blueLower(c.data["pathfinding"]["camera"]["blue"]["lower"]["hue"],c.data["pathfinding"]["camera"]["blue"]["lower"]["saturation"],c.data["pathfinding"]["camera"]["blue"]["lower"]["value"]);
	Scalar blueUpper(c.data["pathfinding"]["camera"]["blue"]["upper"]["hue"],c.data["pathfinding"]["camera"]["blue"]["upper"]["saturation"],c.data["pathfinding"]["camera"]["blue"]["upper"]["value"]);

	//RED INFO
	Scalar redLower(c.data["pathfinding"]["camera"]["red"]["lower"]["hue"],c.data["pathfinding"]["camera"]["red"]["lower"]["saturation"],c.data["pathfinding"]["camera"]["red"]["lower"]["value"]);
	Scalar redUpper(c.data["pathfinding"]["camera"]["red"]["upper"]["hue"],c.data["pathfinding"]["camera"]["red"]["upper"]["saturation"],c.data["pathfinding"]["camera"]["red"]["upper"]["value"]);

	//GREEN INFO
	Scalar greenLower(c.data["pathfinding"]["camera"]["green"]["lower"]["hue"],c.data["pathfinding"]["camera"]["green"]["lower"]["saturation"],c.data["pathfinding"]["camera"]["green"]["lower"]["value"]);
	Scalar greenUpper(c.data["pathfinding"]["camera"]["green"]["upper"]["hue"],c.data["pathfinding"]["camera"]["green"]["upper"]["saturation"],c.data["pathfinding"]["camera"]["green"]["upper"]["value"]);

	//YELLOW INFO
	Scalar yellowLower(c.data["pathfinding"]["camera"]["yellow"]["lower"]["hue"],c.data["pathfinding"]["camera"]["yellow"]["lower"]["saturation"],c.data["pathfinding"]["camera"]["yellow"]["lower"]["value"]);
	Scalar yellowUpper(c.data["pathfinding"]["camera"]["yellow"]["upper"]["hue"],c.data["pathfinding"]["camera"]["yellow"]["upper"]["saturation"],c.data["pathfinding"]["camera"]["yellow"]["upper"]["value"]);

	colors_track[0][0] = blueLower;
	colors_track[0][1] = blueUpper;
	colors_track[1][0] = redLower;
	colors_track[1][1] = redUpper;
	colors_track[2][0] = greenLower;
	colors_track[2][1] = greenUpper;
	colors_track[3][0] = yellowLower;
	colors_track[3][1] = yellowUpper;

	//Y-Dist Coefficients
	c_y_dist[0] = c.data["pathfinding"]["camera"]["calc"]["y_dist"]["x3"].get<double>();
	c_y_dist[1] = c.data["pathfinding"]["camera"]["calc"]["y_dist"]["x2"].get<double>();
	c_y_dist[2] = c.data["pathfinding"]["camera"]["calc"]["y_dist"]["x1"].get<double>();
	c_y_dist[3] = c.data["pathfinding"]["camera"]["calc"]["y_dist"]["x0"].get<double>();

	//X-Dist Coefficients
	c_x_dist[0] = c.data["pathfinding"]["camera"]["calc"]["x_dist"]["x1"].get<double>();
	c_x_dist[1] = c.data["pathfinding"]["camera"]["calc"]["x_dist"]["x0"].get<double>();

	//Size of unit square in calibration
	unit = c.data["pathfinding"]["camera"]["calc"]["unit_cm"].get<double>();

	//Number of frames to skip before keeping one
	skip_frames = c.data["pathfinding"]["camera"]["skip_frames"].get<int>();

	//Get frame scaling
	scale = c.data["pathfinding"]["camera"]["scale"].get<double>();

	//Camera calibration constants
	camera_matrix_arr[0][0] = c.data["pathfinding"]["camera"]["cam"]["camera_matrix"]["00"].get<double>();
    camera_matrix_arr[0][1] = 0;
    camera_matrix_arr[0][2] = c.data["pathfinding"]["camera"]["cam"]["camera_matrix"]["02"].get<double>();
    camera_matrix_arr[1][0] = 0;
    camera_matrix_arr[1][1] = c.data["pathfinding"]["camera"]["cam"]["camera_matrix"]["11"].get<double>();
    camera_matrix_arr[1][2] = c.data["pathfinding"]["camera"]["cam"]["camera_matrix"]["12"].get<double>();
    camera_matrix_arr[2][0] = 0;
    camera_matrix_arr[2][1] = 0;
    camera_matrix_arr[2][2] = 1;
    camera_matrix = Mat(3,3,CV_64F,camera_matrix_arr);

    distort_coeffs_arr[0] = c.data["pathfinding"]["camera"]["cam"]["distortion_coeffs"]["0"].get<double>();
    distort_coeffs_arr[1] = c.data["pathfinding"]["camera"]["cam"]["distortion_coeffs"]["1"].get<double>();
    distort_coeffs_arr[2] = c.data["pathfinding"]["camera"]["cam"]["distortion_coeffs"]["2"].get<double>();
    distort_coeffs_arr[3] = c.data["pathfinding"]["camera"]["cam"]["distortion_coeffs"]["3"].get<double>();
    distort_coeffs_arr[4] = c.data["pathfinding"]["camera"]["cam"]["distortion_coeffs"]["4"].get<double>();
    distort_coeffs = Mat(1,5,CV_64F,distort_coeffs_arr);

}

VisionReadings Vision::readCamera() {
	VisionReadings current_frame = {{0,0,0,0},{0,0,0,0},{0,0}};

	Size size = Size(0,0);
	Mat frame;
	for (int d = 0; d <= skip_frames; d++) // waste some frames before continuing
	{
		cap_main >> frame; // get a new frame from camera
	}
	resize(frame, frame, size, scale, scale); //resize image by desired scaling factor (<1 = shrink)
	//fix distortion
	Mat tempframe = frame.clone();
    undistort(tempframe, frame, camera_matrix, distort_coeffs);
    //continue with blurs
	Mat blurred;
	GaussianBlur(frame, blurred, Size(11,11), 0);
	Mat hsv;
	cvtColor(blurred, hsv, COLOR_BGR2HSV);

	Size kernel_size = Size(3,3);

	//MASKING BEGINS
	for(int c = 0; c < colors_track.size(); c++)
	{
		Mat mask;
		inRange(hsv, colors_track[c][0], colors_track[c][1], mask);
		erode(mask, mask, getStructuringElement(MORPH_RECT, kernel_size), Point(-1,-1), 2);
		dilate(mask, mask, getStructuringElement(MORPH_RECT, kernel_size), Point(-1,-1), 2);
		vector<vector<Point> > contours;
		vector<Point> contour_poly;
		Rect boundRect;
		int max_area_index;
		findContours( mask.clone(), contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0) );

		double contour_area_max = 0;
		if (contours.size() > 0)
		{
			for ( int i = 0; i < contours.size(); i++ ) {
				double current_area = contourArea(contours[i]);
				if (current_area > contour_area_max) {
					contour_area_max = current_area;
					max_area_index = i;
				}
			}

			if (contour_area_max > 3000) {
				Moments area_moments = moments(contours[max_area_index]);
				//Get centroid of countour
				float x_loc = area_moments.m10/area_moments.m00;
				float y_loc = area_moments.m01/area_moments.m00;
				//Create bounding rectangle
				approxPolyDP(Mat(contours[max_area_index]),contour_poly, 3, true);
				boundRect = boundingRect(Mat(contours[max_area_index]));
				//Get bottom of rectangle
				float y_loc_bot = boundRect.y+boundRect.height;
				//Save coordinates of point (x = centroid x, y = bottom of rectangle)
				current_frame.color_loc_x[c] = x_loc;
				current_frame.color_loc_y[c] = y_loc_bot;
			}

			else {
				current_frame.color_loc_x[c] = -1;
				current_frame.color_loc_y[c] = -1;
			}
		}

		else {
			current_frame.color_loc_x[c] = -1;
			current_frame.color_loc_y[c] = -1;
		}

	}

	current_frame.dim[0] = frame.cols;
	current_frame.dim[1] = frame.rows;
	return current_frame;

}

VisionReadings Vision::getLocations() {
	return color_locations;
}

Vision::Vision(Conf configuration) {
	initialize_values(configuration);
	cap_main = VideoCapture(configuration.data["pathfinding"]["camera"]["index"].get<int>());
	//if(!cap.isOpened()) // use this to log camera NOT opening
	threadContinue = true;
	vision_thread = thread([this]{
		while(threadContinue){
			color_locations = readCamera();
		}
	});
}

Vision::~Vision() {
	threadContinue = false;
	vision_thread.join();
}

Angle Vision::bestAvailableHeading(Angle desiredHeading) {
	int x = color_locations.color_loc_x[3]; //yellow
	int y = color_locations.color_loc_y[3];
	int x_dim = color_locations.dim[0];
	int y_dim = color_locations.dim[1];
	if (x == -1) {
		return 666;
	}
	return (int)getAngle(getXDist(x,y,x_dim,y_dim),getYDist(y,y_dim));
}

double Vision::getYDist(int y_raw, int y_dim)
{
	float y = y_dim - y_raw;
	float y_dist = 0;
	for (int i = 0; i < 4; i++)
	{
		y_dist += c_y_dist[i]*(pow(y,3-i));
	}
	return abs(y_dist);
}

double Vision::getXDist(int x_raw, int y_raw, int x_dim, int y_dim)
{
	int y = y_dim - y_raw;
	double ratio = c_x_dist[0]*y + c_x_dist[1];
	return (x_raw-(x_dim/2))/ratio;
}

double Vision::getAngle(double x_d, double y_d)
{
	double pi = 3.14159265358979323846;
	return atan(x_d/y_d)*180/pi;
}
