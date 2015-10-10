#include "GPS.h"
#include <math.h>



double GPS::calculateDesiredHeading(double currentHeading, GPSNode current, GPSNode desired){
		double correction=desired.heading - current.heading
	}
	return correction
}

double GPS::calculateAngleToNode(GPSNode current, GPSNode desired){
  //taken from http://wikicode.wikidot.com/get-angle-of-line-between-two-points
  double xDiff = desired.longitude - current.longitude;
  double yDiff = desired.lattitude - current.lattitude;
  return atan2(yDiff,xDiff);
}

static initGPS(){


}

static char[] retrieveGPS(){


}

static char[] convertGPS(char rawData){

	
}