#include "Vision.h"
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;


void Vision::initialize_colors(Conf c)
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
}

VisionReadings Vision::readCamera() {
	VisionReadings current_frame = {{0,0,0,0},{0,0,0,0}};

	Size size = Size(0,0);
    Mat frame;
    for (int d = 0; d < 8; d++) // waste some frames before continuing
    {
    	cap_main >> frame; // get a new frame from camera
    }
    resize(frame, frame, size, 0.5, 0.5);
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
        Point center;
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
				float x_loc = area_moments.m10/area_moments.m00;
				float y_loc = area_moments.m01/area_moments.m00;
				center = Point(x_loc,y_loc);
				circle(frame, center, 5, colors_track[c][2], -1);
				current_frame.color_loc_x[c] = x_loc;
				current_frame.color_loc_y[c] = y_loc;
			}

			else {
				current_frame.color_loc_x[c] = 0;
				current_frame.color_loc_y[c] = 0;
			}
        }

        else {
        	current_frame.color_loc_x[c] = 0;
			current_frame.color_loc_y[c] = 0;
        }

    }

	return current_frame;

}

VisionReadings Vision::getLocations() {
	return color_locations;
}

Vision::Vision(Conf configuration) {
	initialize_colors(configuration);
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
	float blue_x = color_locations.color_loc_x[0]; //blue
	if (blue_x == 0) {
		return 666;
	}
	return ((blue_x/320)*90)-45;
}
