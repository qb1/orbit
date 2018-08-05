#include "object.h"

#include <string>

#include <glm/gtc/constants.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/glm.hpp>

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include <common/utils.h>

#include "utils.h"
#include "transform.h"
#include "misc_draw.h"
#include "shapes_routines.h"
#include "fonts.h"

GrObject::GrObject(const std::string& name, const sf::Color& color)
  : name_(name)
  , color_(color)
{
}

GrObject::~GrObject() = default;

void GrObject::update(const glm::dvec2& new_pos, double angle)
{
	position_ = new_pos;
	angle_ = angle;
}

void GrObject::draw(sf::RenderTarget& target, const GrTransform& tr) const
{
	do_draw(target, tr);
}

void GrObject::draw_label(sf::RenderTarget& target, const GrTransform& tr) const
{
	do_draw_label(target, tr);
}

bool GrObject::outside_view(const sf::RenderTarget& target, const GrTransform& tr) const
{
	auto scr_pos = tr.to_screen(position_);
	auto scr_radius = tr.to_screen(radius());
	auto view = tr.screen_view();

	return (scr_pos.x - scr_radius >= view.width or scr_pos.x + scr_radius <= view.left or
	        scr_pos.y - scr_radius >= view.height or scr_pos.y + scr_radius <= view.top);
}

void GrObject::do_draw_label(sf::RenderTarget& target, const GrTransform& tr) const
{
	const int margin = 4;
	bool fill_background = false;

	const auto scr_pos = tr.to_screen(position_);
	const auto scr_radius = tr.to_screen(radius());

	auto view = tr.screen_view();
	auto view_center = glm::dvec2(view.left + view.width / 2, view.top + view.height / 2);
	auto text = sf::Text(name_, GrFonts().text, tr.height_to_res(13));
	auto rect = text.getLocalBounds();
	rect.width += 2 * margin;
	rect.height += 2 * margin;

	auto distance = glm::length(scr_pos - view_center);
	auto direction = glm::normalize(scr_pos - view_center);
	if (distance < 1)
		direction = glm::normalize(scr_pos);

	auto center_in_view = scr_pos;
	auto extrem1 = clamp_in_rect(scr_pos - direction * scr_radius, view);
	auto extrem2 = clamp_in_rect(scr_pos + direction * scr_radius, view);
	center_in_view = extrem1 + (extrem2 - extrem1) / 2.0;

	rect.left = center_in_view.x - rect.width / 2;
	rect.top = center_in_view.y - rect.height / 2;

	if (outside_view(target, tr)) {
		auto intersect = point_on_rect(scr_pos, view).value();
		auto arrow_perp = glm::rotate(direction, glm::half_pi<double>());
		double arrow_width = tr.pix_to_res(8.0);

		glm::dvec2 a = intersect;
		glm::dvec2 b = intersect - direction * arrow_width + arrow_perp * arrow_width;
		glm::dvec2 c = intersect - direction * arrow_width - arrow_perp * arrow_width;
		draw_shape(target, {a, b, c}, color_);
		align_rect_directional(rect, intersect - direction * arrow_width, -direction);
		fill_background = true;

	} else if (not rect_inside_sphere(rect, scr_pos, scr_radius)) {
		auto closest = scr_pos - direction * scr_radius - direction * tr.pix_to_res(2);

		auto a = closest;
		auto b = closest - direction * tr.pix_to_res(10);
		draw_lines(target, { { a, b } }, color_);
		align_rect_directional(rect, b, -direction);
		fill_background = true;
	}

	rect.left = std::max(rect.left, view.left);
	rect.top = std::max(rect.top, view.top);
	rect.left = std::min(rect.left + rect.width, view.left + view.width) - rect.width;
	rect.top = std::min(rect.top + rect.height, view.top + view.height) - rect.height;

	text.setPosition((int)rect.left + margin, (int)rect.top + margin / 2);

	if (fill_background) {
		sf::RectangleShape background({rect.width, rect.height});
		background.setPosition(rect.left, rect.top);
		background.setFillColor(sf::Color(0x000000b0));
		background.setOutlineColor(color_);
		background.setOutlineThickness(1.5);
		target.draw(background);

		text.setOutlineColor(color_);
		text.setOutlineThickness(0.4f);
	} else {
		text.setOutlineColor(sf::Color(0x00000080));
		text.setOutlineThickness(1.0f);
	}

	target.draw(text);
}

GrPlanet::GrPlanet(const std::string& name, const sf::Color& color, double radius)
  : GrObject(name, color)
  , radius_(radius)
{
}

GrPlanet::~GrPlanet() = default;

void GrPlanet::do_draw(sf::RenderTarget& target, const GrTransform& tr) const
{
	auto scr_pos = tr.to_screen(position_);
	auto scr_radius = tr.to_screen(radius_);

	auto points = screen_circle_points(position_, radius_, 200, tr);
	draw_shape(target, tr, points, color_);

	auto darker_color = sf::Color(0x00000020);
	glm::dvec2 cross = glm::dvec2(1.0, 0.0) * scr_radius;
	auto cross_1 = glm::rotate(cross, angle_);
	auto cross_2 = glm::rotate(cross_1, glm::half_pi<double>());
	draw_lines(target, { { scr_pos + cross_1, scr_pos - cross_1 }, { scr_pos + cross_2, scr_pos - cross_2 } },
	           darker_color);

}

GrSpaceship::GrSpaceship(const sf::Color& color, double width, double height)
  : GrObject("Spaceship", color)
  , width_(width)
  , height_(height)
  , ship_points_({
      { 0, height / 2 },
      { width / 4, -height / 6 * 2 },
      { width / 2, -height / 2 },
      { 0, -height / 10 * 4 },
      { -width / 2, -height / 2 },
      { -width / 4, -height / 6 * 2 },
      { 0, height / 2 },
    })
  , plume_points_({
      { -width / 8, -height / 10 * 4 },
      { 0, -height / 2 * 1.4 },
      { width / 8, -height / 10 * 4 },

    })
{
}

GrSpaceship::~GrSpaceship() = default;

void GrSpaceship::do_draw(sf::RenderTarget& target, const GrTransform& tr) const
{
	// Draw outline to a bigger world size to make orientation visible at any zoom
	double world_height = tr.to_world(tr.height_to_res(50.0));
	double scale = world_height / height_;
	scale = std::max(scale, 1.0);

	// Draw exhaust plume
	if (burn_) {
		draw_lines(target, tr, plume_points_, position_, scale, angle_, sf::Color::Yellow);
		draw_shape(target, tr, plume_points_, position_, 1.0, angle_, sf::Color::Yellow);
	}

	draw_shape(target, tr, ship_points_, position_, 1.0, angle_, color_);
	draw_lines(target, tr, ship_points_, position_, scale, angle_, color_);
}

void GrSpaceship::do_draw_label(sf::RenderTarget& target, const GrTransform& tr) const
{
	if (outside_view(target, tr)) {
		GrObject::do_draw_label(target, tr);
	}
}
