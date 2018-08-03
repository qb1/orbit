#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>

class SceneBase
{
public:
	SceneBase() = default;
	virtual ~SceneBase() = default;

	virtual void update(float elapsed) {}
	virtual void draw(sf::RenderTarget& target) {}

	virtual void set_visible(bool) = 0;
	virtual void set_takes_input(bool) = 0;

	// Events
	virtual void handle_window_resize(int w, int h) {}
	virtual void handle_key(bool pressed, const sf::Event::KeyEvent& event) {}
	virtual void handle_click(bool pressed, const sf::Event::MouseButtonEvent& event) {}
	virtual void handle_mouse_move(const sf::Event::MouseMoveEvent& event) {}
	virtual void handle_mouse_wheel(const sf::Event::MouseWheelScrollEvent& event) {}
};