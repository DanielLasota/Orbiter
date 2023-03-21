#include <SFML/Graphics.hpp>
#include <cmath>

int main()
{
    sf::Clock clock;
    sf::RenderWindow window(sf::VideoMode(800, 600), "Orbiter");

    // Ustawienie parametrów orbity
    sf::VertexArray orbit(sf::LineStrip, 360);
    orbit[0].position = sf::Vector2f(300, 300);
    orbit[0].color = sf::Color::Green;
    float radius = 200.0f;
    for (int i = 1; i < 360; i++) {
        float angle = i * 3.14f / 180.0f;
        float x = radius * std::cos(angle);
        float y = radius * std::sin(angle);
        orbit[i].position = sf::Vector2f(300 + x, 300 + y);
        orbit[i].color = sf::Color::Green;
    }

    // Ustawienie pocz¹tkowej pozycji myszy
    sf::Vector2f lastMousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    // Aktualny k¹t orbity
    float orbitAngle = 0.0f;

    // G³ówna pêtla programu
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::MouseMoved) {
                // Obliczanie ró¿nicy pozycji myszy
                sf::Vector2f newMousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                sf::Vector2f delta = newMousePos - lastMousePos;

                // Obracanie orbity na podstawie ró¿nicy pozycji myszy
                orbitAngle += delta.x * 0.01f;
                lastMousePos = newMousePos;
            }
        }

        // Aktualizacja pozycji planety na orbicie
        float time = static_cast<float>(clock.getElapsedTime().asMilliseconds());
        float angle = (time * 0.01f) + orbitAngle;
        float x = window.getSize().x / 2.f + std::cos(angle) * radius;
        float y = window.getSize().y / 2.f + std::sin(angle) * radius;
        window.clear();
        // Obracanie orbity
        sf::Transform transform;
        transform.rotate(orbitAngle);
        window.draw(orbit, transform);
        window.display();
    }

    return 0;
}