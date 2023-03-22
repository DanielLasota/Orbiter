//Welcome to the orbiter alpha, early design phase.
//
//This Soft is a Project universal design pattern to finish 
//for target-satellite-ship whatever the fuck You want to throw out to orbit, 
//but basiaclly for satellites with propulsion, tanks, navs etc.
//The aim/goal is to combine common features of STS' FLIGHT INSTRUMENT MENU with Crew Dragon due to
//the ship is ought to be steered manevuered, programmed by human unit on earth. 
// 
//##############################################################################
//##############################################################################
// 
//There are two reasons why it is not ideal to use archaic Apollo steering low - level 
//managing programs with a count of 200. Firstly, we now have processors in our phones 
//that are orders of magnitude faster it gives us the possibility to have access to a lot more sensor inputs
// managing them more quickly, reliable etc, 
//and most importantly, we are now in the 2020s, so it would be better to use modern software 
//to render 3D orbits in the style of Crew Dragon, with options for 2D projections 
//from both above and the side.
// 
////##############################################################################
//##############################################################################
// 
//It's important to consider the fact that the ship will be controlled from 
//Earth by human resources, and SpaceX uses different software for managing 
//orbit programs from the control center, which are then sent to the ship. 
//There is also different software for onboard instruments to manage and 
//program orbits, maneuvers, and docking. Docking is usually automatic, 
//but astronauts are trained to dock manually, so there is an option for manual docking.
//
//Therefore, the two goals are to aim for simplicity and 
//minimalistic design, similar to the STS program, while also 
//embracing modernity and using software similar to that of Crew Dragon.
// 
// //##############################################################################
//##############################################################################
//
//PLANNED LAYERS:
//1. 3D orbit view with details /////// 2D 2 projections - upper and side vies(to realise inclination) - changeable
//2. Window with side data - orbit data, speed, time, inclination, nodes, time to apoapsis/periapsis etc
//3. Earth flat map with ship trajectory mode
// 
// 
// 
//4. Navball
//5. Manevuering Panel - a'la spacex
//6. log - events - event time - fault panel
//7. Engine Panel - with fuel, basic data such as hydraulic fluid temp, amount, etc
//8. Buses Data + APU + Energetics etc



#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <GL/glu.h>
#include <GL/glut.h>

int main()
{
    // Utw�rz okno renderowania
    sf::RenderWindow window(sf::VideoMode(800, 600), "Prosty silnik graficzny 3D");

    // Ustaw parametry OpenGL
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Utw�rz kulk�
    sf::CircleShape ball(50.f);
    ball.setFillColor(sf::Color::Red);

    // Utw�rz kamery
    sf::Vector3f cameraPosition(0.f, 0.f, 500.f);
    sf::Vector3f cameraTarget(0.f, 0.f, 0.f);
    sf::Vector3f cameraUp(0.f, 1.f, 0.f);

    // G��wna p�tla gry
    while (window.isOpen())
    {
        // Obs�u� zdarzenia okna
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        // Obr�� kul�
        static float angle = 0.f;
        angle += 0.1f;
        if (angle > 360.f)
        {
            angle = 0.f;
        }

        // Wyczy�� bufor koloru i bufor g��boko�ci
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Ustaw kamery
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(cameraPosition.x, cameraPosition.y, cameraPosition.z,
            cameraTarget.x, cameraTarget.y, cameraTarget.z,
            cameraUp.x, cameraUp.y, cameraUp.z);

        // Przemieszczaj kul� wzd�u� osi Z i obracaj j� wzd�u� osi Y
        glPushMatrix();
        glTranslatef(0.f, 0.f, -100.f);
        glRotatef(angle, 0.f, 1.f, 0.f);
        glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0.f, 0.f, 0.f);
        const int segments = 32;
        const float radius = 50.f;
        for (int i = 0; i <= segments; ++i)
        {
            float theta = i * 2.f * 3.14159f / segments;
            glVertex3f(radius * cosf(theta), radius * sinf(theta), 0.f);
        }
        glEnd();
        glPopMatrix();

        // Wy�wietl zawarto�� bufora
        window.display();
    }

    return 0;
}






//#include <iostream>
//#include <SFML/Graphics.hpp>
//
//# define M_PI           3.14159265358979323846
//
//int main()
//{
//    sf::Clock clock;
//    sf::RenderWindow window(sf::VideoMode(800, 600), "Orbiter");
//
//    // orbit parameters set
//    sf::VertexArray planet(sf::LineStrip, 361);
//    planet[0].position = sf::Vector2f(300, 300);
//    planet[0].color = sf::Color::Green;
//    float radiusp = 10.0f;
//    for (int i = 0; i < 361; i++) {
//        float angle = i * 3.14f / 180.0f;
//        float x = radiusp * std::cos(angle);
//        float y = radiusp * std::sin(angle);
//        planet[i].position = sf::Vector2f(300 + x, 300 + y);
//        planet[i].color = sf::Color::Green;
//    }
//
//    sf::VertexArray orbit(sf::LineStrip, 361);
//    orbit[0].position = sf::Vector2f(300, 300);
//    orbit[0].color = sf::Color::Green;
//    float radiuso = 200.0f;
//    for (int i = 0; i < 361; i++) {
//        float angle = i * 3.14f / 180.0f;
//        float x = radiuso * std::cos(angle);
//        float y = radiuso * std::sin(angle);
//        orbit[i].position = sf::Vector2f(300 + x, 300 + y);
//        orbit[i].color = sf::Color::Green;
//    }
//
//    // satellite
//    sf::CircleShape satellite(5.f);
//    satellite.setFillColor(sf::Color::Green);
//    satellite.setOrigin(satellite.getRadius(), satellite.getRadius());
//    satellite.setPosition(orbit[0].position);
//
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
//        // satellite position render
//        float time = static_cast<float>(clock.getElapsedTime().asMilliseconds());
//        float angle = time * 0.00001f;
//        int index = static_cast<int>(angle / (2.f * M_PI) * orbit.getVertexCount());
//        satellite.setPosition(orbit[index].position); //wyjatek 
//
//        window.clear();
//        window.draw(planet);
//        window.draw(orbit);
//        window.draw(satellite);
//        window.display();
//    }
//
//    return 0;
//}