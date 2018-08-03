#pragma once

#include <cmath>
#include <glm/vec2.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>

class GrObject;
class GrTransform;
class SimObject;

void draw_orbit(sf::RenderTarget& target, const GrTransform& tr, const SimObject& prim_sim, const SimObject& sec_sim, sf::Color color, int point_nb=200);
void draw_scale(sf::RenderTarget& target, const GrTransform& tr);
void draw_grid(sf::RenderTarget& target, const GrTransform& tr, const glm::dvec2& origin);
void draw_screen_arrow(sf::RenderTarget& target, const sf::Color& color, glm::dvec2 origin, glm::dvec2 direction, double length, double arrow_end_size);
void draw_shape(sf::RenderTarget& target, const GrTransform& tr, const std::vector<glm::dvec2> points, glm::dvec2 world_pos, double scale, double object_rotate, const sf::Color& color);