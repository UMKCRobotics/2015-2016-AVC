#define CATCH_CONFIG_MAIN //Only do this here, include other tests below

#include "catch.hpp"
#include "../src/GPS.h"
#include "../src/Pathfinding.h"

//Testing framework tutorial provided below
//https://github.com/philsquared/Catch/blob/master/docs/tutorial.md

TEST_CASE("BLANK TEST","TEST"){
	REQUIRE( 1 == 1);
}

TEST_CASE("GPS angle",""){
  REQUIRE(0 == GPS::angleBetweenTwoAngles(0,0));
  double behindOutput = GPS::angleBetweenTwoAngles(0,180);
  REQUIRE(-180 == behindOutput);
  REQUIRE(30 == GPS::angleBetweenTwoAngles(0,30));
  REQUIRE(-30 == GPS::angleBetweenTwoAngles(30,0));
  REQUIRE(45 == GPS::angleBetweenTwoAngles(315,0));
  REQUIRE(90 == GPS::angleBetweenTwoAngles(315,45));
  REQUIRE(-90 == GPS::angleBetweenTwoAngles(45,315));
}

TEST_CASE("GPS calcuatedesired heading",""){
  REQUIRE(0 == GPS::calculateDesiredHeading(0,GPSNode(0,0),GPSNode(0,1)));
  REQUIRE(180 == GPS::calculateDesiredHeading(180,GPSNode(0,0),GPSNode(0,1)));
}
TEST_CASE("GPS calcuatedesired heading",""){
  REQUIRE(1 == Pathfinding::rayHeuristic(0,0,1));
  REQUIRE(1 == Pathfinding::rayHeuristic(45,0,1));
  REQUIRE(1 == Pathfinding::rayHeuristic(0,45,1));
  REQUIRE(1 == Pathfinding::rayHeuristic(0,241,1));
}
