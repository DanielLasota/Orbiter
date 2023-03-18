#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

int main()
{
    // Utworzenie okna aplikacji
    sf::RenderWindow window(sf::VideoMode(800, 600), "Orbit Viewer");

    // Ustawienie perspektywy
    sf::View view(sf::Vector2f(0.f, 0.f), sf::Vector2f(800.f, 600.f));
    view.setCenter(0.f, 0.f);
    view.setFov(90.f);
    window.setView(view);

    // Ustawienie czcionki
    sf::Font font;
    if (!font.loadFromFile("C:\\Users\\Devxd\\Desktop\\EurostileExtended.ttf")) {
        std::cout << "Nie mozna zaladowac czcionki." << std::endl;
        return 1;
    }

    // Wprowadzenie danych orbity przez u¿ytkownika
    double eccentricity, semimajorAxis, pericenterDistance, inclination, longitudeAscendingNode, argumentPericenter, trueAnomaly;
    std::cout << "Podaj mimosrod orbity: ";
    std::cin >> eccentricity;
    std::cout << "Podaj polose wielka orbity (w km): ";
    std::cin >> semimajorAxis;
    std::cout << "Podaj odleglosc perycentrum (w km): ";
    std::cin >> pericenterDistance;
    std::cout << "Podaj inklinacje orbity (w stopniach): ";
    std::cin >> inclination;
    std::cout << "Podaj dlugosc wezla wstegujacego (w stopniach): ";
    std::cin >> longitudeAscendingNode;
    std::cout << "Podaj argument perycentrum (w stopniach): ";
    std::cin >> argumentPericenter;
    std::cout << "Podaj dlugosc perycentrum (w stopniach): ";
    std::cin >> trueAnomaly;

    // Obliczenie parametrów orbity
    double periapsisDistance = pericenterDistance * (1.0 + eccentricity);
    double apoapsisDistance = pericenterDistance * (1.0 - eccentricity);
    double period = 2.0 * M_PI * sqrt(std::pow(semimajorAxis, 3.0) / (6.674e-11 * 5.972e24));

    // Ustawienie parametrów ziemi
    sf::CircleShape earth(50.f);
    earth.setFillColor(sf::Color::Blue);
    earth.setOrigin(50.f, 50.f);

    // Ustawienie parametrów orbity
    sf::VertexArray orbit(sf::LineStrip, 360);
    orbit.setPrimitiveType(sf::LineStrip);
    orbit.setFillColor(sf::Color::Transparent);
    orbit.setOutlineColor(sf::Color::Green);
    orbit.setOutlineThickness(1.f);

    // Wygenerowanie punktów orbity
    for (int i = 0; i < 360; i++)
    {
        double angle = i * M_PI / 180.0;
        double radius = (semimajorAxis * (1.0 - eccentricity * eccentricity)) / (1.0 + eccentricity * cos(angle));
        double x = radius * cos(angle);
        double y = radius * sin(angle);
        orbit[i].position = sf::Vector2f(x, y);
    }

    // Utworzenie tekstu opisuj¹cego parametry orbity
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(16);
    text.setFillColor(sf::Color::White);
    text.setPosition(10.f, 10.f);

    // Pêtla g³ówna aplikacji
    while (window.isOpen())
    {
        // Obs³uga zdarzeñ
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Aktualizacja tekstu opisuj¹cego parametry orbity
        text.setString("Mimosrod: " + std::to_string(eccentricity) + "\n"
            "Poloœ wielka: " + std::to_string(semimajorAxis) + " km\n"
            "Odleglosc perycentrum: " + std::to_string(pericenterDistance) + " km\n"
            "Odleglosc apocentrum: " + std::to_string(apoapsisDistance) + " km\n"
            "Okres obiegu: " + std::to_string(period) + " s\n"
            "Inklinacja: " + std::to_string(inclination) + " stopni\n"
            "Dlugosc wezla wstegujacego: " + std::to_string(longitudeAscendingNode) + " stopni\n"
            "Argument perycentrum: " + std::to_string(argumentPericenter) + " stopni\n"
            "Dlugosc perycentrum: " + std::to_string(trueAnomaly) + " stopni");

        // Wyczyszczenie ekranu
        window.clear(sf::Color::Black);

        // Wyrenderowanie ziemi
        window.draw(earth);

        // Wyrenderowanie orbity
        window.draw(orbit);

        // Wyrenderowanie tekstu opisuj¹cego parametry orbity
        window.draw(text);

        // Wyœwietlenie wszystkiego na ekranie
        window.display();
    }

    return 0;
}
