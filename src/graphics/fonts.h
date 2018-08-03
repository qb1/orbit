#pragma once

#include <SFML/Graphics/Font.hpp>

struct FontList
{
	sf::Font text;
};

const FontList& GrFonts();

void gr_initialize_fonts();