#include "universe.h"

#include <glm/detail/func_matrix.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/trigonometric.hpp>
#include <glm/vec2.hpp>

#include <common/computations.h>

void SimUniverse::step(double timestep)
{
	date_ += timestep;

	constexpr double G = 6.67408e-11;

	for (auto& item : objects_) {
		auto& obj = item.second;
		glm::dvec2 acceleration(0.0, 0.0);

		for (const auto& other_item : objects_) {
			auto& other = other_item.second;
			if (&obj == &other)
				continue;

			auto posv = obj.position() - other.position();

			// Compute gravitation
			auto r = glm::length(posv);
			auto accv = -glm::normalize(posv);
			accv *= G * other.mass() / glm::pow(r, 2);

			acceleration += accv;
		}

		obj.step(timestep, acceleration);
	}

	for (auto& item : objects_) {
		auto& obj = item.second;
		if (not obj.can_collide())
			continue;

		for (const auto& other_item : objects_) {
			auto& other = other_item.second;
			if (&obj == &other)
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

void SimUniverse::add_object(std::string name, SimObject object)
{
	objects_.insert(std::make_pair(name, object));
}

void init_simulation_randomize(SimUniverse& simulation, const UniverseDefinition& definition)
{
	for (const auto& obj : definition) {
		glm::dvec2 position = glm::dvec2(obj.perihelion, 0.0);
		glm::dvec2 velocity = glm::dvec2(0.0, -obj.velocity); // Counter clock-wise

		double orbit_angle = static_cast<double>(std::rand()) / static_cast<double>(RAND_MAX) * glm::two_pi<double>();
		position = glm::rotate(position, orbit_angle);
		velocity = glm::rotate(velocity, orbit_angle);

		double object_angle = static_cast<double>(std::rand()) / static_cast<double>(RAND_MAX) * glm::two_pi<double>();

		if (obj.primary_name.size()) {
			auto primary = simulation.objects().find(obj.primary_name);
			if (primary == simulation.objects().end())
				throw std::runtime_error("Error in definition: primary " + obj.primary_name + " does not exist.");

			position += primary->second.position();
			velocity += primary->second.velocity();
		}

		simulation.add_object(obj.name, SimObject(position, velocity, obj.mass, obj.radius, object_angle,
		                                          -glm::two_pi<double>() / obj.rotation_period));
		// printf("%s\n", name.c_str());
	}
}