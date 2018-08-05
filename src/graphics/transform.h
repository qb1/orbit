#pragma once

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

#include <SFML/Graphics/RenderTarget.hpp>

class GrTransform {
public:
	GrTransform(glm::dvec2 position, double scale, int window_width, int window_height);

	sf::FloatRect screen_view() const { return sf::FloatRect(0, 0, window_width_, window_height_); }

    glm::dvec2 world_top_left() const;
    glm::dvec2 world_top_right() const;
    glm::dvec2 world_bottom_right() const;
    glm::dvec2 world_bottom_left() const;
    glm::dvec2 world_center() const;
    double world_top() const;
    double world_left() const;
    double world_right() const;
    double world_bottom() const;

	glm::dvec2 to_screen(glm::dvec2 world) const;
	glm::dvec2 to_world(glm::dvec2 screen) const;
	glm::dvec2 to_world(int x, int y) const;
	double to_screen(double length) const;
	double to_world(double length) const;
	double pix_to_res(double pix) const;
	double width_to_res(double pix) const;
	double height_to_res(double pix) const;
	bool is_world_on_camera(glm::dvec2 world_pos) const;
	glm::dvec2 world_farthest_view_point_from(glm::dvec2 world_pos) const;
	glm::dvec2 world_closest_view_point_from(glm::dvec2 world_pos) const;

protected:
	double scale_;
	glm::dmat4 world_to_screen_;
	glm::dmat4 screen_to_world_;
	int window_width_;
	int window_height_;
};
