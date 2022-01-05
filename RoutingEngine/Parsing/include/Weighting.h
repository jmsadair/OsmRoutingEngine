#pragma once
#include <string>


// The purpose of this class is to calculate Edge weights that will be used in a weighted, undirected graph.
class Weighting {

public:

    /**
     * This method utilizes the haversine formula in order to determine the distance
     * between two pairs of coordinates in kilometers. The default distance is miles, but kilometers are also supported.
     * Throws an exception if an invalid distance unit is provided.
     * See https://en.wikipedia.org/wiki/Haversine_formula for further details.
     * @param lat1 The first latitude.
     * @param lon1 The first longitude.
     * @param lat2 The second latitude.
     * @param lon2 The second longitude.
     * @param distance_units The type of distance units to be used.
     * @return The distance between the two coordinates.
     */
    static double haversineDist(double lat1, double lon1, double lat2, double lon2, const std::string& distance_units = "miles");

    /**
     * This method determines the travel time between two coordinate pairs, given
     * a speed limit in miles per hour. The default time unit is minutes, but hours and seconds are also supported.
     * Throws an exception if an invalid time unit is provided.
     * @param lat1 The first latitude.
     * @param lon1 The first longitude.
     * @param lat2 The second latitude.
     * @param lon2 The second longitude.
     * @param speed_limit_mph The speed limit in miles per hour.
     * @param time_units The time units to be used (i.e. "seconds", "minutes", or "hours").
     * @return The amount of time to travel from the first coordinate pair to the second coordinate pair.
     */
    static double time(double lat1, double lon1, double lat2, double lon2, int speed_limit_mph, const std::string& time_units = "minutes");

private:

    /**
     * Converts degrees to radian.
     * @param degrees The number of degrees to be converted to radians.
     * @return degrees converted to radians.
     */
    static double degreesToRadians(double degrees);

    /**
     * Converts kilometers to miles.
     * @param km The number of kilometers to be converted to miles.
     * @return kilometers converted to miles.
     */
    static double kmToMiles(double km);
};