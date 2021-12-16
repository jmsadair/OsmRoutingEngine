#include <cmath>
#include "weighting.h"

static const double EARTH_RADIUS_KM = 6371.0;
static const double PI = 3.14159265359;

double weighting::degs_to_rads(const double degrees) {
	return degrees * PI / 180.0;
}

double weighting::km_to_miles(double km) {
	return km * 0.621371;
}

double weighting::haversine_dist(const double lat1, const double lon1, const double lat2, const double lon2) {

	// Converting the latitudes and longitudes into radians.
	const double rad_lat1 = degs_to_rads(lat1);
	const double rad_lon1 = degs_to_rads(lon1);
	const double rad_lat2 = degs_to_rads(lat2);
	const double rad_lon2 = degs_to_rads(lon2);
	const double d_lat = std::abs(rad_lat1 - rad_lat2);
	const double d_lon = std::abs(rad_lon1 - rad_lon2);

	const double a = pow(sin(d_lat / 2), 2) + cos(rad_lat1) * cos(rad_lat2) * pow(sin(d_lon / 2), 2);
	const double d_sigma = 2 * asin(sqrt(a));

	return EARTH_RADIUS_KM * d_sigma;
}

double weighting::time(double lat1, double lon1, double lat2, double lon2, int speed_limit_mph) {
	double distance_km = haversine_dist(lat1, lon1, lat2, lon2);
	double distance_miles = km_to_miles(distance_km);
	double hours = distance_miles / speed_limit_mph;

	return hours * 60;
}