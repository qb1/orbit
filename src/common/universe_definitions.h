#pragma once

#include <string>
#include <vector>
#include <glm/vec2.hpp>
#include <SFML/Graphics/Color.hpp>

struct SpaceObjectDefinition {
	std::string name;
	sf::Color color;
	double mass;
	double radius;
	double rotation_period;
	double perihelion;
	double velocity;
	std::string primary_name = "";
};

using UniverseDefinition = std::vector<SpaceObjectDefinition>;

static const UniverseDefinition solar_system_definition = {
	{ "Sun", sf::Color(255, 130, 0), 1.98847e30, 6.95508e8, 0.0, 0.0, 0.0 },
	{ "Mercury", sf::Color(129, 137, 150), 0.330e24, 4879.0e3 / 2, 1407.6 * 3600, 46.0e9, 58.98e3, "Sun" },
	{ "Venus", sf::Color(209, 197, 119), 4.87e24, 12104.0e3 / 2, -5832.5 * 3600, 107.5e9, 35.26e3, "Sun" },
	{ "Earth", sf::Color(159, 207, 249), 5.97e24, 12756.0e3 / 2, 23.9 * 3600, 147.1e9, 30.29e3, "Sun" },
	{ "Moon", sf::Color(196, 196, 196), 7.34767309e22, 1737e3, 655.7 * 3600, 0.3633e9, 1082, "Earth" },
	{ "Mars", sf::Color(229, 151, 87), 0.642e24, 6792.0e3 / 2, 24.6 * 3600, 206.6e9, 26.50e3, "Sun" },
	{ "Jupiter", sf::Color(239, 202, 172), 1898.0e24, 142984.0e3 / 2, 9.9 * 3600, 740.5e9, 13.72e3, "Sun" },
	{ "Saturn", sf::Color(237, 232, 203), 568.0e24, 120536.0e3 / 2, 10.7 * 3600, 1352.6e9, 10.18e3, "Sun" },
	{ "Uranus", sf::Color(209, 253, 255), 86.8e24, 51118.0e3 / 2, -17.2 * 3600, 2741.3e9, 7.11e3, "Sun" },
	{ "Neptune", sf::Color(117, 176, 255), 102.0e24, 49528.0e3 / 2, 16.1 * 3600, 4444.5e9, 5.50e3, "Sun" },
	{ "Pluto", sf::Color(216, 188, 153), 0.0146e24, 2370.0e3 / 2, -153.3 * 3600, 4436.8e9, 6.10e3, "Sun" },
};