#define CATCH_CONFIG_MAIN //Only do this here, include other tests below

#include "GPS.h"
#include "catch.hpp"

//Testing framework tutorial provided below
//https://github.com/philsquared/Catch/blob/master/docs/tutorial.md

TEST_CASE("BLANK TEST","TEST"){
	REQUIRE( 1 == 1);
}
TEST_CASE("GPS","CALCULATE ANGLES WORKS")
{
		double angles = calculateAngleToNode(GPSNode(0,0),GPSNode(0,1));
		REQUIRE(angles == 0);
}
TEST_CASE("GPS_2","CALCULATE ANGLES WORKS 2")
{
		double angles = alculatingAngleToNode(GPSNode(10,20),GPSNode(10,30);
		REQUIRE(angles == 0);
}
