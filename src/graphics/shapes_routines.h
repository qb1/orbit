#include <vector>
#include <glm/vec2.hpp>

#include <common/types.h>
#include "transform.h"

std::vector<glm::dvec2> circle_points(glm::dvec2 center, double radius, double angle_start, double angle_end, int points_nb);
std::vector<glm::dvec2> orbit_points(const glm::dvec2& focal_point, const OrbitalParameters& orbit, double angle_start, double angle_end, int points_nb);
std::vector<glm::dvec2> screen_orbit_points(
	const glm::dvec2& focal_point, const OrbitalParameters& orbit, int points_nb, const GrTransform& tr);
std::vector<glm::dvec2> screen_circle_points(
	const glm::dvec2& center, double radius, int points_nb, const GrTransform& tr);