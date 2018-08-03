#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <iostream>
#include <string>
#include <vector>

#include <imgui.h>
#include <imgui-SFML.h>

#include <graphics/fonts.h>
#include <scenes/base.h>
#include <scenes/universe.h>

using namespace std;

int main()
{
	sf::ContextSettings settings;
	// settings.antialiasingLevel = 4;
	sf::RenderWindow window(sf::VideoMode(1024, 768), "Orbit", sf::Style::Default, settings);
	window.setVerticalSyncEnabled(true);

    ImGui::SFML::Init(window, false);
    // ImGui::GetIO().Fonts->
    ImGui::SFML::UpdateFontTexture();

    gr_initialize_fonts();

    std::vector<SceneBase*> scenes;
    SceneUniverse scene_universe(window);

    scenes.push_back(&scene_universe);

    sf::Clock deltaClock;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Local necessary handling
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed and event.key.code == sf::Keyboard::Key::Escape)
                window.close();
            if (event.type == sf::Event::Resized) {
                // Ensure view is not stretched
                auto view = window.getView();
                view.setSize(window.getSize().x, window.getSize().y);
                view.setCenter(window.getSize().x / 2, window.getSize().y / 2);
                window.setView(view);
            }

            // Handle for scenes
            if (event.type == sf::Event::KeyPressed or event.type == sf::Event::KeyReleased) {
                for (auto& scene: scenes)
                    scene->handle_key(event.type == sf::Event::KeyPressed, event.key);
            }
            if (event.type == sf::Event::MouseButtonPressed or event.type == sf::Event::MouseButtonReleased) {
                for (auto& scene: scenes)
                    scene->handle_click(event.type == sf::Event::MouseButtonPressed, event.mouseButton);
            }
            if (event.type == sf::Event::MouseWheelScrolled) {
                for (auto& scene: scenes)
                    scene->handle_mouse_wheel(event.mouseWheelScroll);
            }
            if (event.type == sf::Event::MouseMoved) {
                for (auto& scene: scenes)
                    scene->handle_mouse_move(event.mouseMove);
            }
            if (event.type == sf::Event::Resized) {
                auto size = window.getView().getSize();
                for (auto& scene: scenes)
                    scene->handle_window_resize(size.x, size.y);
            }
        }

        // Update
        ImGui::SFML::Update(window, deltaClock.restart());
        float elapsed = deltaClock.getElapsedTime().asSeconds();
        for (auto& scene: scenes)
            scene->update(elapsed);

        // Render
        window.clear();
        for (auto& scene: scenes)
            scene->draw(window);
        ImGui::SFML::Render(window); // UI always on top

        window.display();
    }

    return 0;
}

