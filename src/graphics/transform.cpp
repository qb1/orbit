#include "transform.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/detail/func_matrix.hpp>

GrTransform::GrTransform(glm::dvec2 position, double scale, int window_width, int window_height)
    : scale_(scale)
    , window_width_(window_width)
    , window_height_(window_height)
{
	world_to_screen_ = glm::translate(glm::dvec3(
		static_cast<double>(window_width) / 2 / scale - position.x,
	    static_cast<double>(window_height) / 2 / scale - position.y, 0.0));

	screen_to_world_ = glm::affineInverse(world_to_screen_);

	// TODO: Why does scale cause glm::inverse not to work?
	world_to_screen_ = glm::scale(glm::dvec3(scale_, scale_, 0.0)) * world_to_screen_;
	screen_to_world_ = screen_to_world_ * glm::scale(glm::dvec3(1 / scale_, 1 / scale_, 0.0));
}

glm::dvec2 GrTransform::world_top_left() const { return to_world(glm::vec2(0.0, 0.0)); }
glm::dvec2 GrTransform::world_top_right() const { return to_world(glm::vec2(window_width_, 0.0)); }
glm::dvec2 GrTransform::world_bottom_right() const { return to_world(glm::vec2(window_width_, window_height_)); }
glm::dvec2 GrTransform::world_bottom_left() const { return to_world(glm::vec2(0.0, window_height_)); }
glm::dvec2 GrTransform::world_center() const { return to_world(glm::vec2(window_width_ / 2, window_height_ / 2)); }
double GrTransform::world_top() const { return world_top_left().y; }
double GrTransform::world_left() const { return world_top_left().x; }
double GrTransform::world_right() const { return world_bottom_right().x; }
double GrTransform::world_bottom() const { return world_bottom_right().y; }

glm::dvec2 GrTransform::to_screen(glm::dvec2 world) const {
	auto vec = world_to_screen_ * glm::dvec4(world.x, world.y, 0.0, 1.0);
	double x = vec.x / vec.w;
	double y = vec.y / vec.w;
	return glm::dvec2(x, y);
}

glm::dvec2 GrTransform::to_world(glm::dvec2 screen) const {
	double x = screen.x;
	double y = screen.y;

	auto vec = screen_to_world_ * glm::dvec4(x, y, 0.0, 1.0);
	return glm::dvec2(vec.x / vec.w, vec.y / vec.w);
}

glm::dvec2 GrTransform::to_world(int x, int y) const {
	return to_world(glm::dvec2(x, y));
}

double GrTransform::to_screen(double length) const {
	return length * scale_;
}

double GrTransform::to_world(double length) const {
	return length / scale_;
}

double GrTransform::pix_to_res(double pix) const {
	if (window_width_ < window_height_)
		return width_to_res(pix);
	else
		return height_to_res(pix);
}

double GrTransform::width_to_res(double pix) const {
	return pix * static_cast<double>(window_width_) / 1024.0;
}

double GrTransform::height_to_res(double pix) const {
	return pix * static_cast<double>(window_height_) / 768.0;
}

bool GrTransform::is_world_on_camera(glm::dvec2 world_pos) const {
	auto screen_pos = to_screen(world_pos);
	return screen_pos.x >= 0 and screen_pos.y >= 0 and
	       screen_pos.x < window_width_ and screen_pos.y < window_height_;
}

glm::dvec2 GrTransform::world_farthest_view_point_from(glm::dvec2 world_pos) const {
	glm::dvec2 farthest = world_top_left();
	double length = glm::length(farthest - world_pos);
	auto keep_if_farther = [&farthest, &length, &world_pos] (const glm::dvec2& point) {
		auto new_length = glm::length(point - world_pos);
		if (new_length >= length) {
			farthest = point;
			length = new_length;
		}
	};

	keep_if_farther(world_top_right());
	keep_if_farther(world_bottom_right());
	keep_if_farther(world_bottom_left());

	return farthest;
}

glm::dvec2 GrTransform::world_closest_view_point_from(glm::dvec2 world_pos) const {
	glm::dvec2 closest = world_top_left();
	double length = glm::length(closest - world_pos);
	auto keep_if_closer = [&closest, &length, &world_pos] (const glm::dvec2& point) {
		auto new_length = glm::length(point - world_pos);
		if (new_length <= length) {
			closest = point;
			length = new_length;
		}
	};

	keep_if_closer(world_top_right());
	keep_if_closer(world_bottom_right());
	keep_if_closer(world_bottom_left());

	return closest;
}
