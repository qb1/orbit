#pragma once

#include <SFML/Graphics/Font.hpp>

struct FontList
{
	sf::Font label;
	sf::Font info;
};

const FontList& GrFonts();

void gr_initialize_fonts();