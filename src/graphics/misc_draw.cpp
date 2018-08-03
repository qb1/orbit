#include "misc_draw.h"

#include <cmath>

#include <glm/gtc/constants.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <SFML/Graphics/ConvexShape.hpp>

#include <common/computations.h>
#include <common/utils.h>
#include <simulation/object.h>

#include "utils.h"
#include "fonts.h"
#include "transform.h"
#include "shapes_routines.h"

namespace {

std::pair<double, double> find_scale_distance(const GrTransform& tr)
{
	double scale_distance = glm::pow(10.0, glm::floor(std::log10(tr.to_world(tr.pix_to_res(200)))));
	double scr_scale_distance = tr.to_screen(scale_distance);

	if (scr_scale_distance < tr.width_to_res(100)) {
		scr_scale_distance *= 2.5;
		scale_distance *= 2.5;
	}
	if (scr_scale_distance < tr.width_to_res(100)) {
		scr_scale_distance *= 2;
		scale_distance *= 2;
	}

	return std::make_pair(scale_distance, scr_scale_distance);
}

}

void draw_orbit(sf::RenderTarget& target, const GrTransform& tr, const SimObject& prim_sim, const SimObject& sec_sim, sf::Color color, int point_nb)
{
	auto orbit = compute_orbit_from_state(prim_sim.position(), prim_sim.velocity(), prim_sim.mass(), sec_sim.position(), sec_sim.velocity());
	auto points = screen_orbit_points(prim_sim.position(), orbit, point_nb, tr);
	std::vector<sf::Vertex> lines;
	color.a = 200;
	for (const auto& p : points) {
		auto t = tr.to_screen(p);
		lines.push_back(to_vertex(t, color));
	}
	target.draw(lines.data(), lines.size(), sf::LineStrip);
}

void draw_scale(sf::RenderTarget& target, const GrTransform& tr)
{
	auto d = find_scale_distance(tr);

	glm::dvec2 center(tr.width_to_res(1024 - 150), tr.height_to_res(750));
	glm::dvec2 left(center.x - d.second / 2, center.y);
	glm::dvec2 right(center.x + d.second / 2, center.y);

	glm::dvec2 seg_limit(0, tr.height_to_res(5));

	sf::Vertex lines[] = {
		to_vertex(left), to_vertex(right),
		to_vertex(left - seg_limit), to_vertex(left + seg_limit),
		to_vertex(right - seg_limit), to_vertex(right + seg_limit),
	};
	target.draw(lines, sizeof(lines) / sizeof(lines[0]), sf::Lines);

	sf::Text text(distance_to_string(d.first, 1), GrFonts().text, tr.height_to_res(10));
	auto rect = text.getLocalBounds();
	text.setPosition(center.x, center.y);
	text.setPosition(center.x - rect.width / 2, center.y - rect.height * 2);
	target.draw(text);
}

void draw_grid(sf::RenderTarget& target, const GrTransform& tr, const glm::dvec2& origin)
{
	auto d = find_scale_distance(tr);

	sf::Color color(0xffffff50);
	std::vector<sf::Vertex> lines;
	auto top_left = tr.world_top_left() - origin;
	int offset = tr.to_screen(std::fmod(top_left.x, d.first));
	int step = d.second;
	for (int x = - offset; x < tr.width_to_res(1024); x += step) {
		lines.push_back(sf::Vertex(sf::Vector2f(x, 0), color));
		lines.push_back(sf::Vertex(sf::Vector2f(x, (int)tr.height_to_res(768)), color));
	}
	offset = tr.to_screen(std::fmod(top_left.y, d.first));
	for (int y = - offset; y < tr.height_to_res(768); y += step) {
		lines.push_back(sf::Vertex(sf::Vector2f(0, y), color));
		lines.push_back(sf::Vertex(sf::Vector2f((int)tr.width_to_res(1024), y), color));
	}
	target.draw(lines.data(), lines.size(), sf::Lines);
}

void draw_screen_arrow(sf::RenderTarget& target, const sf::Color& color, glm::dvec2 origin, glm::dvec2 direction, double length, double arrow_end_size)
{
	length = std::max(arrow_end_size, length);
	auto line_end = origin + direction * (length - arrow_end_size);
	sf::Vertex lines[] = { to_vertex(origin), to_vertex(line_end) };
	target.draw(lines, sizeof(lines) / sizeof(lines[0]), sf::Lines);

	auto arrow_build = glm::rotate(direction, glm::half_pi<double>()) * arrow_end_size / 2.0;
	auto a = origin + direction * length;
	auto b = line_end + arrow_build;
	auto c = line_end - arrow_build;

	sf::ConvexShape triangle(3);
	triangle.setPoint(0, to_vector(a));
	triangle.setPoint(1, to_vector(b));
	triangle.setPoint(2, to_vector(c));
	triangle.setOutlineColor(color);
	triangle.setFillColor(color);
	target.draw(triangle);
}

void draw_shape(sf::RenderTarget& target, const GrTransform& tr, const std::vector<glm::dvec2> points, glm::dvec2 world_pos, double scale, double object_rotate, const sf::Color& color)
{
	sf::ConvexShape shape(points.size());
	for (std::size_t i = 0; i < points.size(); ++i) {
		auto p = points[i];
		p *= scale;
	 	p = glm::rotate(p, object_rotate - glm::half_pi<double>());
		shape.setPoint(i, to_vector(tr.to_screen(p + world_pos)));
	}
	shape.setFillColor(color);
	target.draw(shape);
}