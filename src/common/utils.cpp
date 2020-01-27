#include "utils.h"

#include <vector>
#include <sstream>
#include <iomanip>

#include <glm/detail/func_geometric.hpp>
#include <glm/gtc/constants.hpp>

double linear_space(double min, double max, int step_count, int step)
{
	double step_size = (max - min) / (step_count - 1);
	return min + step_size * step;
}

std::string distance_to_string(double distance, int precision)
{
	std::string unit;
	std::stringstream s;

	s << std::defaultfloat;

	auto abs = std::abs(distance);

	if (abs < 0.5) {
		distance = 0;
		unit = "m";
	} else if (abs < 1.0e3) {
		unit = "m";
	} else if (abs < 1.0e9) {
		unit = "km";
		distance /= 1.0e3;
	// } else if (abs < 1.0e9) {
	// 	unit = "Kkm";
	// 	distance /= 1.0e6;
	} else if (distance < 1.0e12) {
		unit = "Mkm";
		distance /= 1.0e9;
	} else {
		unit = "Bkm";
		distance /= 1.0e12;

		if (distance >= 1.0e6) {
			s << std::scientific << std::setprecision(precision);
		}
	}

	s << distance << " " << unit;
	return s.str();
}

std::string time_to_string(std::uint64_t seconds)
{
	std::stringstream ss;

	std::uint64_t years = seconds / (3600 * 24 * 365);
	seconds %= 3600 * 24 * 365;

	std::uint64_t days = seconds / (3600 * 24);
	seconds %= 3600 * 24;
	std::uint64_t hours = seconds / 3600;
	seconds %= 3600;
	std::uint64_t minutes = seconds / 60;
	seconds %= 60;

	if (years)
		ss << years << "y ";
	if (days)
		ss << days << "d ";

	ss.fill('0');
	ss << std::setw(2) << hours << ":" << std::setw(2) << minutes << ":" << std::setw(2) << seconds;
	return ss.str();
}

double to_degrees(double angle)
{
	return angle / glm::two_pi<double>() * 360.0;
}