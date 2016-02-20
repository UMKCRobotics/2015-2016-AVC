#define CATCH_CONFIG_MAIN //Only do this here, include other tests below

#include "catch.hpp"
#include "../src/GPS.h"
#include "../src/Lidar.h"
#include "../src/AngleMath.h"
#include "../src/nmea/minmea.h"
#include "../src/GPSParser.h"

INITIALIZE_EASYLOGGINGPP//Don't remove this

//Testing framework tutorial provided below
//https://github.com/philsquared/Catch/blob/master/docs/tutorial.md

TEST_CASE("BLANK TEST","TEST"){
	REQUIRE( 1 == 1);
}

TEST_CASE("GPS","CALCULATE ANGLES WORKS")
{
  double angles = GPS::calculateAngleToNode(GPSNode(0,0),GPSNode(0,1));
		REQUIRE(angles == 0);

		angles = GPS::calculateAngleToNode(GPSNode(10,20),GPSNode(10,30));
		REQUIRE(angles == 0);
}
TEST_CASE("ANGLEMath","Angle between angles"){
  REQUIRE(0 == AngleMath::angleBetweenTwoAngles(0,0));
  double behindOutput = AngleMath::angleBetweenTwoAngles(0,180);
  REQUIRE(-180 == behindOutput);
  REQUIRE(30 == AngleMath::angleBetweenTwoAngles(0,30));
  REQUIRE(-30 == AngleMath::angleBetweenTwoAngles(30,0));
  REQUIRE(45 == AngleMath::angleBetweenTwoAngles(315,0));
  REQUIRE(90 == AngleMath::angleBetweenTwoAngles(315,45));
  REQUIRE(-90 == AngleMath::angleBetweenTwoAngles(45,315));
}

TEST_CASE("GPS calcuatedesired heading",""){
  REQUIRE(0 == GPS::calculateDesiredHeading(0,GPSNode(0,0),GPSNode(0,1)));
  REQUIRE(-180 == GPS::calculateDesiredHeading(180,GPSNode(0,0),GPSNode(0,1)));
  REQUIRE(45 == Approx(GPS::calculateDesiredHeading(0,GPSNode(0,0),GPSNode(1,1)))); 
  REQUIRE(-45 == Approx(GPS::calculateDesiredHeading(0,GPSNode(0,0),GPSNode(-1,1)))); 
}
TEST_CASE("Pathfinding","ray heuristic"){
  Lidar pathfinding;
  REQUIRE(1 == pathfinding.rayHeuristic(0,0,1));
  REQUIRE(1 == pathfinding.rayHeuristic(45,0,1));
  REQUIRE(1 == pathfinding.rayHeuristic(0,45,1));
  REQUIRE(1 == pathfinding.rayHeuristic(0,241,1));
}

TEST_CASE("PARSER","vtg parser"){
  //$GPVTG,054.7,T,034.4,M,005.5,N,010.2,K*48
  custom_sentence_vtg frame;
  char* vtg_string = "$GPVTG,054.7,T,034.4,M,005.5,N,010.2,K*48";
  custom_parse_vtg(&frame,vtg_string);
  REQUIRE(547 == frame.true_track.value);
  REQUIRE(344 == frame.magnetic_track.value);
  REQUIRE(55 == frame.ground_speed_knots.value);
  REQUIRE(102 == frame.ground_speed_kmh.value);
}
