#pragma once

#include <optional>

#include <glm/vec2.hpp>

#include <graphics/camera.h>

#include "base.h"

class SceneUniverse : public SceneBase
{
public:
	SceneUniverse(const sf::RenderTarget& target);
	virtual ~SceneUniverse();

	virtual void update(float elapsed);
	virtual void draw(sf::RenderTarget& target);

	virtual void set_visible(bool visible) { visible_ = visible; }
	virtual void set_takes_input(bool takes_input) { takes_input_ = takes_input; }

	// Events
	virtual void handle_window_resize(int w, int h);
	virtual void handle_key(bool pressed, const sf::Event::KeyEvent& event);
	virtual void handle_click(bool pressed, const sf::Event::MouseButtonEvent& event);
	virtual void handle_mouse_move(const sf::Event::MouseMoveEvent& event);
	virtual void handle_mouse_wheel(const sf::Event::MouseWheelScrollEvent& event);

private:
	GrCamera camera;

	std::optional<glm::dvec2> mouse_drag_;
	bool takes_input_;
	bool visible_;
};