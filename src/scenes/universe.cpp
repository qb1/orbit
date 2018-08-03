#include "universe.h"

#include <simulation/object.h>
#include <simulation/universe.h>
#include <simulation/universe_definitions.h>

#include <graphics/camera.h>
#include <graphics/misc_draw.h>
#include <graphics/object.h>


SceneUniverse::~SceneUniverse() = default;

SceneUniverse::SceneUniverse(const sf::RenderTarget& target)
    : camera(target.getSize().x, target.getSize().y)
    , visible_(true)
    , takes_input_(true)
{
	camera.center_on(glm::dvec2(0.0, 0.0));
}

void SceneUniverse::update(float elapsed)
{
}

void SceneUniverse::draw(sf::RenderTarget& target)
{
    // ---------- Test bed
    auto tr = camera.begin();

    draw_scale(target, tr);
    draw_grid(target, tr, glm::dvec2(0.0, 0.0));

    // ---------- Test bed
}

void SceneUniverse::handle_window_resize(int w, int h)
{
	camera.update_window(w, h);
}

void SceneUniverse::handle_key(bool pressed, const sf::Event::KeyEvent& key)
{
}

void SceneUniverse::handle_click(bool pressed, const sf::Event::MouseButtonEvent& event)
{
	if (not takes_input_)
		return;
	if (event.button != sf::Mouse::Button::Left)
		return;

	if (pressed) {
		mouse_drag_ = glm::dvec2(event.x, event.y);
	} else {
		mouse_drag_.reset();
	}
}

void SceneUniverse::handle_mouse_move(const sf::Event::MouseMoveEvent& event)
{
	if (not takes_input_)
		return;

	if (mouse_drag_) {
		glm::dvec2 new_vec(event.x, event.y);
		camera.translate_screen(new_vec - mouse_drag_.value());
		mouse_drag_ = new_vec;
	}
}

void SceneUniverse::handle_mouse_wheel(const sf::Event::MouseWheelScrollEvent& event)
{
	if (not takes_input_)
		return;

	camera.scale += camera.scale * 0.1 * event.delta;
}
