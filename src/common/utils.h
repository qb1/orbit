#pragma once

#include <cstdint>
#include <string>

// #include <glm/vec2.hpp>
// #include <SFML/Graphics/Rect.hpp>

// sf::Rect getBitmapStringBoundingBox(std::string text);

// std::experimental::optional<glm::dvec2>
// point_on_rect(glm::dvec2 position, const sf::Rect& rect);

// glm::dvec2 clamp_in_rect(glm::dvec2 position, const sf::Rect& rect);

// bool rect_inside_sphere(const sf::Rect& rect, glm::vec2 position, float radius);

// void align_rect_directional(sf::Rect& rect, const glm::dvec2& position, const glm::dvec2& direction);

double linear_space(double min, double max, int step_count, int step);

std::string distance_to_string(double distance, int precision = 2);
std::string time_to_string(std::uint64_t seconds);
double to_degrees(double angle);

constexpr double grav_const = 6.67408e-11;