#include "computations.h"

#include <algorithm>
#include <string>
#include <glm/trigonometric.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/detail/func_geometric.hpp>

OrbitalParameters compute_orbit_from_state(const glm::dvec2& prim_pos, const glm::dvec2& prim_vel, double prim_mass,
                                           glm::dvec2 sat_pos, glm::dvec2 sat_vel)
{
	constexpr double G = 6.67408e-11;
	sat_pos -= prim_pos;
	sat_vel -= prim_vel;
	auto sat_pos_dir = glm::normalize(sat_pos);
	auto sat_vel_dir = glm::normalize(sat_vel);
	auto r = glm::length(sat_pos);
	auto v = glm::length(sat_vel);

	auto gamma = glm::orientedAngle(sat_pos_dir, sat_vel_dir);

	OrbitalParameters orbit;

	auto rv2_GM = (r * v*v) / (G * prim_mass);
	auto C = (2 * G * prim_mass) / (r * v*v);
	auto sinY = glm::sin(gamma);
	auto cosY = glm::cos(gamma);

	orbit.e = glm::sqrt(glm::pow(rv2_GM - 1, 2) * sinY*sinY + cosY*cosY);

	double tan_nu = (rv2_GM * sinY * cosY) / (rv2_GM * glm::pow(sinY, 2) - 1);
	double nu = glm::atan(tan_nu);

	double Rp = (-C + glm::sqrt(C*C - 4 * (1 - C) * (-sinY*sinY))) / (2 * (1 - C)) * r;
	// double Ra = (-C - glm::sqrt(C*C - 4 * (1 - C) * (-sinY*sinY))) / (2 * (1 - C)) * r;

	// auto new_e = glm::pow(r * v *sinY, 2) / (G * prim_mass * Rp) - 1;

	// ofDrawBitmapString(std::to_string(new_e) + " new_e", 10, 480);
	if ((gamma >= 0.0 and gamma <= glm::half_pi<double>()) or gamma < -glm::half_pi<double>()) {
		// First half of the ellipsis
		if (nu < 0.0) {
			nu = glm::pi<double>() + nu;
			// ofDrawBitmapString("first half inverted", 10, 500);
		} else {
			// ofDrawBitmapString("first half", 10, 500);
		}
	} else {
		if (nu > 0.0) {
			nu = glm::pi<double>() + nu;
			// ofDrawBitmapString("second half inverted", 10, 500);
		} else {
			// ofDrawBitmapString("second half", 10, 500);

		}
	}

	orbit.rp = glm::length(Rp);

	orbit.longitude = glm::orientedAngle(glm::dvec2(1.0, 0.0), sat_pos_dir) - nu;

	return orbit;
}

glm::dvec2 compute_barycenter(glm::dvec2 a, double mass_a, glm::dvec2 b, double mass_b)
{
	if (mass_a == 0.0) return b;
	if (mass_b == 0.0) return a;

	double distance = glm::length(b - a);
	auto a_to_b = glm::normalize(b - a);
	double r = distance / (1 + mass_a / mass_b);
	return a_to_b * r;
}