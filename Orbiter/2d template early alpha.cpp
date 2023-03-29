//Welcome to the orbiter alpha - early design phase orbital manager multi inbsturment unit
//This Soft is a Project universal design pattern to finish 
//for target-satellite-ship whatever the heck You want to throw out to orbit, 
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
#include <GL/glut.h>
#include <gl/GLU.h>
#include <iostream>
//#include <GLFW/glfw3.h>

int main()
{

    sf::Window window(sf::VideoMode(800, 600), "OpenGL", sf::Style::Default, sf::ContextSettings(32));
    window.setVerticalSyncEnabled(true);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_LIGHTING);

    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 8;
    settings.majorVersion = 3;
    settings.minorVersion = 0;

    window.setActive(true);

    static float anglex = 0.f;
    static float angley = 0.f;
    sf::Vector2f previousMousePos;
    sf::Vector2i lastPosition;


    bool running = true;
    while (running)
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                // end the program
                running = false;
            }
            else if (event.type == sf::Event::Resized)
            {
                // adjust the viewport when the window is resized
                glViewport(0, 0, event.size.width, event.size.height);
            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
            {
                lastPosition = sf::Mouse::getPosition(window);
            }
            else if (event.type == sf::Event::MouseWheelScrolled)
            {
                if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
                {
                    r -= event.mouseWheelScroll.delta * 10.f;
                }
            }
        }

    }
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    window.setActive(false);
    window.close();

    return 0;
}