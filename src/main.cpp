#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

// #include <imgui.h>
#include <imgui-SFML.h>

#include <graphics/fonts.h>
#include <scenes/scene_manager.h>

using namespace std;

// void add(const UniverseDefinition& definition)
// {
//     for (const auto& obj : definition) {
//         glm::dvec2 position = glm::dvec2(obj.perihelion, 0.0);
//         glm::dvec2 velocity = glm::dvec2(0.0, - obj.velocity); // Counter clock-wise
//         Object *primary = nullptr;

//         double orbit_angle = static_cast<double>(std::rand()) / static_cast<double>(RAND_MAX) * glm::two_pi<double>();
//         position = glm::rotate(position, orbit_angle);
//         velocity = glm::rotate(velocity, orbit_angle);

//         double object_angle = static_cast<double>(std::rand()) / static_cast<double>(RAND_MAX) * glm::two_pi<double>();

//         if (obj.primary_name.size()) {
//             primary = find(obj.primary_name);

//             position += primary->simu->position();
//             velocity += primary->simu->velocity();
//         }

//         add(std::make_unique<GrPlanet>(obj.name, obj.color, obj.radius),
//                 SimObject(position, velocity, obj.mass, obj.radius, object_angle,
//                           -glm::two_pi<double>() / obj.rotation_period),
//                 primary);
//     }
// }

int main()
{
	sf::ContextSettings settings;
	settings.antialiasingLevel = 4;
	sf::RenderWindow window(sf::VideoMode(1024, 768), "Orbit", sf::Style::Default, settings);
	window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);

    ImGui::SFML::Init(window, false);
    // ImGui::GetIO().Fonts->
    ImGui::SFML::UpdateFontTexture();

    gr_initialize_fonts();

    SceneManager scene_manager(window);

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
                scene_manager.handle_key(event.type == sf::Event::KeyPressed, event.key);
            }
            if (event.type == sf::Event::MouseButtonPressed or event.type == sf::Event::MouseButtonReleased) {
                scene_manager.handle_click(event.type == sf::Event::MouseButtonPressed, event.mouseButton);
            }
            if (event.type == sf::Event::MouseWheelScrolled) {
                scene_manager.handle_mouse_wheel(event.mouseWheelScroll);
            }
            if (event.type == sf::Event::MouseMoved) {
                scene_manager.handle_mouse_move(event.mouseMove);
            }
            if (event.type == sf::Event::Resized) {
                auto size = window.getView().getSize();
                scene_manager.handle_window_resize(size.x, size.y);
            }
        }

        // Update
        ImGui::SFML::Update(window, deltaClock.restart());
        float elapsed = deltaClock.getElapsedTime().asSeconds();
        scene_manager.update(elapsed);

        // Render
        scene_manager.draw(window);
        ImGui::SFML::Render(window); // UI always on top

        window.display();
    }

    return 0;
}

