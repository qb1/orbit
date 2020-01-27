#pragma once

#include <optional>
#include <cmath>
#include <glm/vec2.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>

#include <common/types.h>

class GrObject;
class GrTransform;
class SimObject;

void draw_orbit(sf::RenderTarget& target, const GrTransform& tr, const SimObject& prim_sim, const SimObject& sec_sim, sf::Color color, int point_nb=50);
void draw_orbit(sf::RenderTarget& target, const GrTransform& tr, const glm::dvec2& prim_pos, const OrbitalParameters& orbit, sf::Color color, int point_nb=50);
void draw_scale(sf::RenderTarget& target, const GrTransform& tr);
void draw_grid(sf::RenderTarget& target, const GrTransform& tr, const glm::dvec2& origin);
void draw_screen_arrow(sf::RenderTarget& target, const sf::Color& color, glm::dvec2 origin, glm::dvec2 vector, double arrow_end_size);

void draw_shape(sf::RenderTarget& target, const GrTransform& tr, const std::vector<glm::dvec2> points, glm::dvec2 world_pos, double scale, double object_rotate, const sf::Color& color);
void draw_shape(sf::RenderTarget& target, const GrTransform& tr, const std::vector<glm::dvec2> points, const sf::Color& color);
void draw_shape(sf::RenderTarget& target, const std::vector<glm::dvec2> points, const sf::Color& color);

void draw_lines(sf::RenderTarget& target, const std::vector<std::pair<glm::dvec2, glm::dvec2>> lines, const sf::Color& color);

void draw_lines(sf::RenderTarget& target, const GrTransform& tr, const std::vector<glm::dvec2> points, glm::dvec2 world_pos, double scale, double object_rotate, const sf::Color& color);


std::optional<glm::dvec2> point_on_rect(const glm::dvec2& position, const sf::FloatRect& rect);
glm::dvec2 clamp_in_rect(const glm::dvec2& position, const sf::FloatRect& rect);
void align_rect_directional(sf::FloatRect& rect, const glm::dvec2& position, const glm::dvec2& direction);
bool rect_inside_sphere(const sf::FloatRect& rect, const glm::vec2& position, float radius);