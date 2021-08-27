#pragma once
#include <string>

/**
* The purpose of this class is to calculate edge weights that will be used in a weighted, undirected graph.
*/
class weighting {

public:

	/**
	* This function utilizes the haversine formula in order to determine the distance 
	* between two pairs of coordinates in kilometers.
	* See https://en.wikipedia.org/wiki/Haversine_formula for further details.
	*/
	static double haversine_dist(double lat1, double lon1, double lat2, double lon2);

	/**
	* This function determines the travel time in minutes between two coordinate pairs, given
	* a speed limit in miles per hour.
	*/
	static double time(double lat1, double lon1, double lat2, double lon2, int speed_limit_mph);

private:

	//Converts degrees to radians.
	static double degs_to_rads(double degrees);

	//Converts kilometers to miles.
	static double km_to_miles(double km);
};