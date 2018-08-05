#pragma once

#include <glm/vec2.hpp>

#include "types.h"

OrbitalParameters compute_orbit_from_state(const glm::dvec2& prim_pos, const glm::dvec2& prim_vel, double prim_mass,
                                           glm::dvec2 sat_pos, glm::dvec2 sat_vel);
glm::dvec2 compute_barycenter(glm::dvec2 a, double mass_a, glm::dvec2 b, double mass_b);