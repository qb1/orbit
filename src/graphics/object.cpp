#include "object.h"

#include <string>

#include <glm/gtc/constants.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/glm.hpp>

#include <SFML/Graphics/CircleShape.hpp>

#include <common/utils.h>

#include "utils.h"
#include "transform.h"
#include "misc_draw.h"
#include "shapes_routines.h"

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
	const auto& view = target.getView().getViewport();

	return (scr_pos.x - scr_radius >= view.width or scr_pos.x + scr_radius <= view.left or
	        scr_pos.y - scr_radius >= view.height or scr_pos.y + scr_radius <= view.top);
}

void GrObject::do_draw_label(sf::RenderTarget& target, const GrTransform& tr) const
{
	// bool fill_background = false;

	// auto scr_pos = tr.to_screen(position_);
	// auto scr_radius = tr.to_screen(radius());

	// const auto& view = target.getView();
	// auto rect = getBitmapStringBoundingBox(name_);

	// auto distance = glm::length(scr_pos - to_dvec2(view.getCenter()));
	// auto direction = glm::normalize(scr_pos - to_dvec2(view.getCenter()));
	// if (distance < 1)
	// 	direction = glm::normalize(scr_pos - glm::dvec2(view.getTopRight()));

	// auto center_in_view = scr_pos;
	// auto extrem1 = clamp_in_rect(scr_pos - direction * scr_radius, view);
	// auto extrem2 = clamp_in_rect(scr_pos + direction * scr_radius, view);
	// center_in_view = extrem1 + (extrem2 - extrem1) / 2.0;

	// rect.alignTo(center_in_view);

	// if (outside_view(target, tr)) {
	// 	auto intersect = point_on_rect(scr_pos, view).value();
	// 	auto arrow_perp = glm::rotate(direction, glm::half_pi<double>());
	// 	double arrow_width = tr.pix_to_res(5.0);

	// 	to_dvec2 a = intersect;
	// 	glm::dvec2 b = intersect - direction * arrow_width + arrow_perp * arrow_width;
	// 	glm::dvec2 c = intersect - direction * arrow_width - arrow_perp * arrow_width;
	// 	ofDrawTriangle(a, b, c);
	// 	align_rect_directional(rect, intersect - direction * arrow_width, -direction);
	// 	fill_background = true;

	// } else if (not rect_inside_sphere(rect, scr_pos, scr_radius)) {
	// 	auto closest = scr_pos - direction * scr_radius - direction * tr.pix_to_res(2);

	// 	auto a = closest;
	// 	auto b = closest - direction * tr.pix_to_res(10);
	// 	ofDrawLine(a, b);
	// 	align_rect_directional(rect, b, -direction);
	// 	fill_background = true;
	// } else

	//   if (rect.getLeft() <= view.getLeft())
	// 	rect.alignToHorz(view, OF_ALIGN_HORZ_LEFT);
	// else if (rect.getRight() >= view.getRight())
	// 	rect.alignToHorz(view, OF_ALIGN_HORZ_RIGHT);

	// if (rect.getTop() <= view.getTop())
	// 	rect.alignToVert(view, OF_ALIGN_VERT_TOP);
	// else if (rect.getBottom() >= view.getBottom())
	// 	rect.alignToVert(view, OF_ALIGN_VERT_BOTTOM);

	// ofPushStyle();
	// if (fill_background) {
	// 	ofDrawBitmapStringHighlight(name_, glm::vec2(rect.getBottomLeft()) + glm::vec2(4.0f, -4.0f));
	// 	ofNoFill();
	// 	ofDrawRectangle(rect);
	// } else {
	// 	ofSetColor(sf::Color::white);
	// 	ofDrawBitmapString(name_, glm::vec2(rect.getBottomLeft()) + glm::vec2(4.0f, -4.0f));
	// }
	// ofPopStyle();
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

	// auto points = screen_circle_points(position_, radius_, 200, tr);
	// ofSetPolyMode(OF_POLY_WINDING_NONZERO);
	// ofBeginShape();
	// ofVertex(tr.to_screen(position_));
	// for (const auto& p : points) {
	// 	ofVertex(tr.to_screen(p));
	// }
	// ofEndShape();

	// auto color = color_;
	// color.setBrightness(0.8 * color.getBrightness());
	// ofSetColor(color);

	// glm::dvec2 cross = glm::dvec2(1.0, 0.0) * scr_radius;
	// cross = glm::rotate(cross, angle_);
	// ofDrawLine(scr_pos + cross, scr_pos - cross);
	// cross = glm::rotate(cross, glm::half_pi<double>());
	// ofDrawLine(scr_pos + cross, scr_pos - cross);
	// ofPopStyle();
	sf::CircleShape shape(scr_radius);
	shape.setFillColor(color_);
	shape.setPosition(to_vector(scr_pos));
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
	// ofPushStyle();

	// // Draw outline to a bigger world size to make orientation visible at any zoom
	// double world_height = tr.to_world(tr.height_to_res(50.0));
	// double scale = world_height / height_;
	// scale = std::max(scale, 1.0);

	// // Draw exhaust plume
	// if (burn_) {
	// 	ofNoFill();
	// 	draw_shape(tr, plume_points_, position_, scale, angle_, sf::Color::yellow);
	// 	ofFill();
	// 	draw_shape(tr, plume_points_, position_, 1.0, angle_, sf::Color::yellow);
	// }

	// auto color = color_;
	// color.setBrightness(color.getBrightness() * 1.8);
	// draw_shape(tr, ship_points_, position_, 1.0, angle_, color);

	// ofNoFill();
	// draw_shape(tr, ship_points_, position_, scale, angle_, color_);

	// ofPopStyle();
}

void GrSpaceship::do_draw_label(sf::RenderTarget& target, const GrTransform& tr) const
{
	if (outside_view(target, tr)) {
		do_draw_label(target, tr);
	}
}
