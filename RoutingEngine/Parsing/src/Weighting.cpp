#include <cmath>
#include <stdexcept>
#include "Weighting.h"

static const double EARTH_RADIUS_KM = 6371.0;
static const double PI = 3.14159265359;

double Weighting::degreesToRadians(const double degrees) {
    return degrees * PI / 180.0;
}

double Weighting::kmToMiles(double km) {
    return km * 0.621371;
}

double Weighting::haversineDist(const double lat1, const double lon1, const double lat2, const double lon2, const std::string& distance_units) {

    // Converting the latitudes and longitudes into radians.
    const double rad_lat1 = degreesToRadians(lat1);
    const double rad_lon1 = degreesToRadians(lon1);
    const double rad_lat2 = degreesToRadians(lat2);
    const double rad_lon2 = degreesToRadians(lon2);
    const double d_lat = std::abs(rad_lat1 - rad_lat2);
    const double d_lon = std::abs(rad_lon1 - rad_lon2);

    const double a = pow(sin(d_lat / 2), 2) + cos(rad_lat1) * cos(rad_lat2) * pow(sin(d_lon / 2), 2);
    const double d_sigma = 2 * asin(sqrt(a));

    if (distance_units == "km") {
        return EARTH_RADIUS_KM * d_sigma;
    } else if (distance_units == "miles") {
        return kmToMiles(EARTH_RADIUS_KM * d_sigma);
    } else {
        throw std::logic_error("Distance units not supported. Did you mean kilometers/miles?");
    }
}

double Weighting::time(double lat1, double lon1, double lat2, double lon2, int speed_limit_mph, const std::string& time_units) {
    double distance_km = haversineDist(lat1, lon1, lat2, lon2);
    double distance_miles = kmToMiles(distance_km);
    if (time_units == "hours") {
        return distance_miles / speed_limit_mph;
    } else if (time_units == "minutes") {
        return 60 * (distance_miles / speed_limit_mph);
    } else if (time_units == "seconds") {
        return 60 * 60 * (distance_miles / speed_limit_mph);
    } else {
        throw std::logic_error("Time units not supported. Did you mean hours/minutes/seconds?");
    }
}