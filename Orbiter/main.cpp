#include <iostream>
#include <cmath>
#include <cstdlib>
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Text.hpp>
#include <sstream>
#include <iomanip>
#include <math.h>

# define M_PI           3.14159265358979323846



int main()
{
    sf::Clock clock;
    sf::RenderWindow window(sf::VideoMode(800, 600), "Orbiter");

    // orbit parameters set
    sf::VertexArray orbit(sf::LineStrip, 361);
    orbit[0].position = sf::Vector2f(300, 300);
    orbit[0].color = sf::Color::Green;
    float radius = 200.0f;
    for (int i = 0; i < 361; i++) {
        float angle = i * 3.14f / 180.0f;
        float x = radius * std::cos(angle);
        float y = radius * std::sin(angle);
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

        // Aktualizacja pozycji planety na orbicie
        float time = static_cast<float>(clock.getElapsedTime().asMilliseconds());
        float angle = time * 0.00001f;
        int index = static_cast<int>(angle / (2.f * M_PI) * orbit.getVertexCount());
        satellite.setPosition(orbit[index].position);

        window.clear();
        window.draw(orbit);
        window.draw(satellite);
        window.display();
    }

    return 0;
}

//int main()
//{
//    sf::RenderWindow window(sf::VideoMode(800, 600), "Orbit Display");
//
//    // ustawienia kamery
//    sf::Vector3f cameraPos(0.f, 0.f, -10.f);
//    sf::Vector3f cameraTarget(0.f, 0.f, 0.f);
//    sf::Vector3f cameraUp(0.f, 1.f, 0.f);
//
//    // ustawienia obiektu
//    sf::Vector3f objectPos(0.f, 0.f, 0.f);
//    sf::Vector3f objectScale(1.f, 1.f, 1.f);
//    sf::Color objectColor(sf::Color::Green);
//
//    //// utworzenie kuli
//    //sf::SphereShape sphere(1.f);
//    //sphere.setFillColor(objectColor);
//    //sphere.setScale(objectScale);
//    //sphere.setPosition(objectPos);
//
//    // utworzenie okrêgu symbolizuj¹cego orbitê
//    sf::CircleShape orbit(5.f);
//    orbit.setFillColor(sf::Color::Transparent);
//    orbit.setOutlineThickness(1.f);
//    orbit.setOutlineColor(objectColor);
//    orbit.setScale(objectScale);
//    orbit.setPosition(objectPos);
//
//    while (window.isOpen())
//    {
//        sf::Event event;
//        while (window.pollEvent(event))
//        {
//            if (event.type == sf::Event::Closed)
//                window.close();
//        }
//
//        // rysowanie sceny
//        window.clear(sf::Color::Black);
//
//        // ustawienia macierzy kamery
//        sf::Transform viewMatrix;
//        viewMatrix = sf::Glsl::LookAt(cameraPos, cameraTarget, cameraUp);
//
//        // ustawienia macierzy projekcji
//        sf::Transform projectionMatrix;
//        projectionMatrix = sf::Glsl::Perspective(45.f, 800.f / 600.f, 0.1f, 100.f);
//
//        // przemno¿enie macierzy projekcji i kamery
//        sf::Transform viewProjectionMatrix;
//        viewProjectionMatrix = projectionMatrix * viewMatrix;
//
//        // ustawienie macierzy modelu dla kuli i narysowanie jej
//        sf::Transform sphereTransform;
//        sphereTransform.combine(viewProjectionMatrix);
//        window.draw(sphere, sphereTransform);
//
//        // ustawienie macierzy modelu dla okrêgu orbituj¹cego i narysowanie go
//        sf::Transform orbitTransform;
//        orbitTransform.combine(viewProjectionMatrix);
//        window.draw(orbit, orbitTransform);
//
//        window.display();
//    }
//
//    return 0;
//}