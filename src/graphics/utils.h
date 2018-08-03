#pragma once

#include <glm/vec2.hpp>
#include <SFML/Graphics/Vertex.hpp>

template <typename T>
sf::Vertex to_vertex(const T& from)
{
	return sf::Vertex(sf::Vector2f(from.x, from.y));
}

template <typename T>
sf::Vertex to_vertex(const T& from, const sf::Color& color)
{
	return sf::Vertex(sf::Vector2f(from.x, from.y), color);
}

template <typename T>
sf::Vector2f to_vector(const T& from)
{
	return sf::Vector2f(from.x, from.y);
}

template <typename T>
glm::dvec2 to_dvec2(const T& from)
{
	return glm::dvec2(from.x, from.y);
}