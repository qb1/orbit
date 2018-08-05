#pragma once

#include <optional>
#include <cstdint>

#include <glm/vec2.hpp>

#include <graphics/camera.h>

#include "universe.h"

class SceneManager
{
public:
	SceneManager(const sf::RenderTarget& target);
	virtual ~SceneManager();

	virtual void update(float elapsed);
	virtual void draw(sf::RenderTarget& target);

	// Events
	virtual void handle_window_resize(int w, int h);
	virtual void handle_key(bool pressed, const sf::Event::KeyEvent& event);
	virtual void handle_click(bool pressed, const sf::Event::MouseButtonEvent& event);
	virtual void handle_mouse_move(const sf::Event::MouseMoveEvent& event);
	virtual void handle_mouse_wheel(const sf::Event::MouseWheelScrollEvent& event);

private:
	SceneUniverse scene_universe;
	std::vector<SceneBase*> scenes;

	SimUniverse simulation_;

	GrCamera camera;
	std::optional<SimUniverse::Collection::const_iterator> center_camera_on;

	SimUniverse::Collection::iterator ship_simu_;
	GrSpaceship* ship_visu_;
	double ship_accel_;

	std::optional<glm::dvec2> mouse_drag_;
	bool simu_running_;
	int simu_speed_;
};