#pragma once

#include <glm/vec2.hpp>
#include <glm/gtc/constants.hpp>

class SimObject {
public:
	SimObject(glm::dvec2 position, glm::dvec2 velocity, double mass, double radius, double angle, double angular_velocity)
	    : position_(position)
	    , velocity_(velocity)
	    , inherent_acceleration_()
	    , mass_(mass)
	    , radius_(radius)
	    , angle_(angle)
	    , angular_velocity_(angular_velocity)
	    , can_collide_(false) {}

	void step(double timestep, glm::dvec2 acceleration) {
		position_ += velocity_ * timestep;
		velocity_ += (acceleration + inherent_acceleration_) * timestep;
		angle_ += angular_velocity_ * timestep;
		if (angle_ > glm::two_pi<double>())
			angle_ -= glm::two_pi<double>();
		if (angle_ < 0.0)
			angle_ += glm::two_pi<double>();
	}

	void set_position(glm::dvec2 position) { position_ = position; }
	void set_velocity(glm::dvec2 velocity) { velocity_ = velocity; }
	void set_inherent_acceleration(glm::dvec2 inherent_acceleration) { inherent_acceleration_ = inherent_acceleration; }
	void set_angle(double angle) { angle_ = angle; }
	void set_angular_velocity(double angular_velocity) { angular_velocity_ = angular_velocity; }

	glm::dvec2 position() const { return position_; }
	glm::dvec2 velocity() const { return velocity_; }
	glm::dvec2 inherent_acceleration() const { return inherent_acceleration_; }
	double mass() const { return mass_; }
	double radius() const { return radius_; }
	double angle() const { return angle_; }
	double angular_velocity() const { return angular_velocity_; }

	void set_can_collide(bool collide) { can_collide_ = collide; }
	bool can_collide() const { return can_collide_; }
protected:
	glm::dvec2 position_;
	glm::dvec2 velocity_;
	glm::dvec2 inherent_acceleration_;
	double mass_;
	double radius_;
	double angle_;
	double angular_velocity_;
	bool can_collide_;
};