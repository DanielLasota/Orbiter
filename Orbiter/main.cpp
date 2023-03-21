#include <iostream>
#include <SFML/Graphics.hpp>

# define M_PI           3.14159265358979323846

int main()
{
    sf::Clock clock;
    sf::RenderWindow window(sf::VideoMode(800, 600), "Orbiter");

    // orbit parameters set
    sf::VertexArray planet(sf::LineStrip, 361);
    planet[0].position = sf::Vector2f(300, 300);
    planet[0].color = sf::Color::Green;
    float radiusp = 10.0f;
    for (int i = 0; i < 361; i++) {
        float angle = i * 3.14f / 180.0f;
        float x = radiusp * std::cos(angle);
        float y = radiusp * std::sin(angle);
        planet[i].position = sf::Vector2f(300 + x, 300 + y);
        planet[i].color = sf::Color::Green;
    }

    sf::VertexArray orbit(sf::LineStrip, 361);
    orbit[0].position = sf::Vector2f(300, 300);
    orbit[0].color = sf::Color::Green;
    float radiuso = 200.0f;
    for (int i = 0; i < 361; i++) {
        float angle = i * 3.14f / 180.0f;
        float x = radiuso * std::cos(angle);
        float y = radiuso * std::sin(angle);
        orbit[i].position = sf::Vector2f(300 + x, 300 + y);
        orbit[i].color = sf::Color::Green;
    }

    // satellite
    sf::CircleShape satellite(5.f);
    satellite.setFillColor(sf::Color::Green);
    satellite.setOrigin(satellite.getRadius(), satellite.getRadius());
    satellite.setPosition(orbit[0].position);

 
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // satellite position render
        float time = static_cast<float>(clock.getElapsedTime().asMilliseconds());
        float angle = time * 0.00001f;
        int index = static_cast<int>(angle / (2.f * M_PI) * orbit.getVertexCount());
        satellite.setPosition(orbit[index].position);

        window.clear();
        window.draw(planet);
        window.draw(orbit);
        window.draw(satellite);
        window.display();
    }

    return 0;
}