#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

using namespace std;

int main()
{
    // Ustawienia okna
    sf::RenderWindow window(sf::VideoMode(800, 600), "Orbita", sf::Style::Default);
    window.setFramerateLimit(60);

    // Wprowadzanie danych
    float a, b, eccentricity;
    sf::Vector2f center(400.f, 300.f);
    std::cout << "Podaj a, b i ekscentrycznoœæ elipsy: ";
    std::cin >> a >> b >> eccentricity;

    // Tworzenie elipsy
    sf::VertexArray ellipse(sf::LineStrip, 361);
    for (int i = 0; i <= 360; i++) {
        float x = a * cos(i * 3.14159265 / 180.f);
        float y = b * sin(i * 3.14159265 / 180.f);
        float angle = atan2(y, x);
        float r = sqrt(x * x + y * y) / (1.f - eccentricity * cos(angle));
        ellipse[i].position = center + sf::Vector2f(r * cos(angle), r * sin(angle));
        ellipse[i].color = sf::Color::White;
    }

    // Wyœwietlanie parametrów orbity
    sf::Font font;
    if (!font.loadFromFile("C:\\Users\\Devxd\\Desktop\\EurostileExtended.ttf")) {
        return -1;
    }
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(16);
    text.setPosition(10.f, 10.f);
    text.setFillColor(sf::Color::Black);
    text.setString("a: " + std::to_string(a) + "\n"
        "b: " + std::to_string(b) + "\n"
        "Ekscentrycznoœæ: " + std::to_string(eccentricity));

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        // Rysowanie elipsy
        window.clear(sf::Color::Black);
        window.draw(ellipse);

        // Rysowanie rzutów
        for (int i = 0; i <= 360; i += 10) {
            sf::Vertex top[] = { center, ellipse[i] };
            top[0].color = sf::Color::Red;
            top[1].color = sf::Color::Red;
            top[1].position.y -= 5.f;
            window.draw(top, 2, sf::Lines);

            sf::Vertex side[] = { center, ellipse[i] };
            side[0].color = sf::Color::Green;
            side[1].color = sf::Color::Green;
            side[1].position.x -= 5.f;
            window.draw(side, 2, sf::Lines);
        }

        // Rysowanie parametrów orbity
        window.draw(text);

        window.display();
    }

    return 0;
}
