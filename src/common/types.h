#pragma once

#include <glm/vec2.hpp>

struct OrbitalParameters {
	double e; // eccentricity
	double rp; // periapsis distance

	double longitude; // reference is east direction (1, 0)
};
