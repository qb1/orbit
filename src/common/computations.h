#pragma once

#include <optional>

#include <glm/vec2.hpp>

#include "types.h"

OrbitalParameters compute_orbit_from_state(const glm::dvec2& prim_pos, const glm::dvec2& prim_vel, double prim_mass,
                                           glm::dvec2 sat_pos, glm::dvec2 sat_vel);
glm::dvec2 compute_barycenter(glm::dvec2 a, double mass_a, glm::dvec2 b, double mass_b);
glm::dvec2 velocity_for_circular_orbit(const glm::dvec2& prim_pos, const glm::dvec2& prim_vel, double prim_mass,
                                       glm::dvec2 sat_pos);
glm::dvec2 acceleration_from_attraction(const glm::dvec2& prim_pos, double prim_mass, const glm::dvec2& sat_pos);

// Return std::nullopt work on e == 1. Did not test on e > 1
std::optional<double> compute_dt_from_true_anomaly(const OrbitalParameters& orbit, double prim_mass, double angle_start,
                                                   double angle_end);
double compute_true_anomaly_from_dt(const OrbitalParameters& orbit, double prim_mass, double angle_start, double dt);

double angle_from_primary(const glm::dvec2 prim_pos, const glm::dvec2 point);
double true_anomaly_from_primary(const OrbitalParameters& orbit, const glm::dvec2 prim_pos, const glm::dvec2 point);

double flight_path_angle(const OrbitalParameters& orbit, double true_anomaly);