#pragma once

#include <deque>

#include <glm/vec2.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/detail/func_matrix.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "object.h"
#include <common/computations.h>

class SimUniverse {
public:
	using Collection = std::deque<SimObject>;

	void step(double timestep) {
		constexpr double G = 6.67408e-11;

		for (auto & obj : objects_) {
			glm::dvec2 acceleration(0.0, 0.0);

			for (const auto & other : objects_) {
				if(&obj == &other)
					continue;

				auto posv = obj.position() - other.position();

				// Compute gravitation
				auto r = glm::length(posv);
				auto accv = - glm::normalize(posv);
				accv *= G * other.mass() / glm::pow(r, 2);

				acceleration += accv;
			}

			obj.step(timestep, acceleration);
		}

		for (auto & obj : objects_) {
			if(not obj.can_collide())
				continue;

			for (const auto & other : objects_) {
				if(&obj == &other)
					continue;

				auto posv = obj.position() - other.position();
				// Test for collisions
				if (glm::length(posv) <= obj.radius() + other.radius()) {
					// Cannot be inside
					posv = glm::normalize(posv) * (obj.radius() + other.radius());
					obj.set_position(other.position() + posv);

					// infinite friction
					auto vel = glm::normalize(posv);
					vel = glm::rotate(vel, glm::half_pi<double>());
					vel *= other.angular_velocity() * other.radius();
					obj.set_velocity(vel + other.velocity());

					obj.set_angular_velocity(other.angular_velocity());
				}
			}
		}
	}

	SimObject* add_object(SimObject object) {
		objects_.push_back(object);
		return &objects_.back();
	}

	const Collection& objects() const { return objects_; }

	std::pair<glm::dvec2, double> barycenter() const {
		glm::dvec2 barycenter;
		double mass = 0.0;
		for (const auto& obj: objects_) {
			barycenter = compute_barycenter(barycenter, mass, obj.position(), obj.mass());
			mass += obj.mass();
		}
		return std::make_pair(barycenter, mass);
	}

private:
	std::deque<SimObject> objects_;
};