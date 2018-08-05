#include "shapes_routines.h"

#include <experimental/optional>
#include <cmath>
#include <string>

#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <common/utils.h>

namespace {

double normalized_angle(double x) {
    x = std::fmod(x + glm::pi<double>(), glm::two_pi<double>());
    if (x < 0.0)
        x += glm::two_pi<double>();
    return x - glm::pi<double>();
}

void cap_angle_range(double& start, double& end, double min, double max)
{
	double range = end - start;
	start = normalized_angle(start);
	end = start + range;

	double max_range = max - min;
	min = normalized_angle(min);
	max = min + max_range;

	if (normalized_angle(end) > min)
		start = min;
	else
		start = std::max(start, min);
	end = std::min(end, max);
}

std::experimental::optional<std::pair<double, double>> find_viewable_angle(
	const glm::dvec2& center, double smallest_radius, const GrTransform& tr)
{
	double screen_diagonal = tr.width_to_res(1024) * 2;
	double world_diagonal = tr.to_world(screen_diagonal);


	// Simple case: mostly in view
	if ((tr.is_world_on_camera(center)))/* and tr.to_screen(smallest_radius) <= screen_diagonal) or
	    world_diagonal >= smallest_radius * 2)*/ {
		return std::make_pair(0.0, glm::two_pi<double>());
	}

	// Compute max possible arc angle

	// Angle of object center to center of view
	auto center_offset = tr.world_center() - center;
	double center_angle = glm::acos(center_offset.x / glm::length(center_offset));
	if (center_offset.y < 0.0)
		center_angle = -center_angle;

	// Max angle with corners
	double half_v;
	double candidate_half_v;
	half_v = glm::angle(glm::normalize(center_offset), glm::normalize(tr.world_top_left() - center));
	candidate_half_v = glm::angle(glm::normalize(center_offset), glm::normalize(tr.world_top_right() - center));
	half_v = std::max(candidate_half_v, half_v);
	candidate_half_v = glm::angle(glm::normalize(center_offset), glm::normalize(tr.world_bottom_left() - center));
	half_v = std::max(candidate_half_v, half_v);
	candidate_half_v = glm::angle(glm::normalize(center_offset), glm::normalize(tr.world_bottom_right() - center));
	half_v = std::max(candidate_half_v, half_v);

	return std::make_pair(center_angle - half_v, center_angle + half_v);
}

}

std::vector<glm::dvec2> circle_points(glm::dvec2 center, double radius, double angle_start, double angle_end, int points_nb)
{
	std::vector<glm::dvec2> points;
	for (int i = 0; i < points_nb; ++i) {
		double angle = linear_space(angle_start, angle_end, points_nb, i);
		glm::dvec2 point(glm::cos(angle) * radius, glm::sin(angle) * radius);
		points.push_back(point + center);
	}
	points.push_back(center);
	return points;
}

// Clock-wise
std::vector<glm::dvec2> orbit_points(const glm::dvec2& focal_point, const OrbitalParameters& orbit, double angle_start, double angle_end, int points_nb)
{
	std::vector<glm::dvec2> points;

	angle_start -= orbit.longitude;
	angle_end -= orbit.longitude;

	// Compute the semi-latus rectum depending on orbit type
	double p;
	if (orbit.e == 1.0) {
		p = orbit.rp * 2;
	} else {
		// Signs will compensate, p will always be positive
		double a = orbit.rp / (1 - orbit.e);
		p = a * (1 - glm::pow(orbit.e, 2));
	}

	// Restrict drawing to valid angles.
	if(orbit.e == 1) {
		cap_angle_range(angle_start, angle_end, -glm::pi<double>(), glm::pi<double>());
	} else if(orbit.e > 1) {
		// we cannot reach n, so ensure floating point calculation are just after
		double n = glm::acos(-1 / orbit.e + std::numeric_limits<double>::epsilon());
		cap_angle_range(angle_start, angle_end, -n, n);
	}

	for (int i = 0; i < points_nb; ++i) {
		double angle = linear_space(angle_start, angle_end, points_nb, i);

		double radius = p / (1 + orbit.e * glm::cos(angle));

		glm::dvec2 point(glm::cos(angle + orbit.longitude) * radius, glm::sin(angle + orbit.longitude) * radius);
		points.push_back(point + focal_point);
	}
	return points;
}

std::vector<glm::dvec2> screen_orbit_points(
	const glm::dvec2& focal_point, const OrbitalParameters& orbit, int points_nb, const GrTransform& tr)
{
	auto res = find_viewable_angle(focal_point, orbit.rp, tr);
	if (not res) {
		return {};
	}

	return orbit_points(focal_point, orbit, res.value().first, res.value().second, points_nb);
}

std::vector<glm::dvec2> screen_circle_points(
	const glm::dvec2& center, double radius, int points_nb, const GrTransform& tr)
{
	auto res = find_viewable_angle(center, radius, tr);
	if (not res) {
		return {};
	}

	return circle_points(center, radius, res.value().first, res.value().second, points_nb);
}