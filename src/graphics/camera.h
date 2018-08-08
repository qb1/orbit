#pragma once

#include <glm/vec2.hpp>

#include "transform.h"

class GrCamera {
public:
	GrCamera() = default;

	GrCamera(int window_width, int window_height)
	: window_width_(window_width), window_height_(window_height), scale(1.0) {}

	GrTransform transform()
	{
		return GrTransform(origin + position, scale, window_width_, window_height_);
	}

	void set_viewable_width(double width) {
		scale = static_cast<double>(window_width_) / width;
	}

	void set_viewable_height(double height) {
		scale = static_cast<double>(window_height_) / height;
	}

	void set_viewable_distance(double distance) {
		scale = static_cast<double>(std::min(window_width_, window_height_)) / distance;
	}

	void update_window(int window_width, int window_height) {
		// Base new scaling on width. TODO: offset y to keep center if ratio changed?
		scale *= (static_cast<double>(window_height) / static_cast<double>(window_height_));
		window_width_ = window_width;
		window_height_ = window_height;
	}

	void translate_screen(glm::dvec2 offset) {
		// TODO rotation
		position.x -= offset.x / scale;
		position.y -= offset.y / scale;
	}

	glm::dvec2 origin;
	glm::dvec2 position;
	double scale;

protected:
	int window_width_;
	int window_height_;
};
