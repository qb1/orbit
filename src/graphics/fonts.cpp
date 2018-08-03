#include "fonts.h"

#include <stdexcept>

namespace {

void load_font(const std::string& font_name, sf::Font& font)
{
	if(!font.loadFromFile(font_name)) {
        throw std::runtime_error("Could not load font file " + font_name);
    }
}

FontList static_fonts;

}

const FontList& GrFonts()
{
	return static_fonts;
}

void gr_initialize_fonts()
{
	load_font("ressources/fonts/FiraMono-Regular.otf", static_fonts.text);
}