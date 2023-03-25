//Welcome to the orbiter alpha - early design phase orbital manager multi inbsturment unit
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
#include <GL/glut.h>
#include <gl/GLU.h>
#include <iostream>
int main()
{
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 16;
    settings.majorVersion = 3;
    settings.minorVersion = 0;

    // create the window
    sf::Window window(sf::VideoMode(800, 600), "OpenGL", sf::Style::Default, sf::ContextSettings(32));
    window.setVerticalSyncEnabled(true);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_LIGHTING);

 
    // Camera initalise
    sf::Vector3f cameraPosition(0.f, 0.f, 500.f);
    sf::Vector3f cameraTarget(0.f, 0.f, 0.f);
    sf::Vector3f cameraUp(0.f, 1.f, 0.f);




    // activate the window
    window.setActive(true);
    // load resources, initialize the OpenGL states, ...
    // initialize projection and view matrices


    static float anglex = 0.f;
    static float angley = 0.f;
    static float distance = 100.f;
    sf::Vector2f previousMousePos;
    sf::Vector2i lastPosition;

    float theta = 0.0f; // kąt wznoszenia kamery (pomiędzy 0 a 180 stopni)
    float phi = 0.0f; // kąt obrotu kamery (pomiędzy 0 a 360 stopni)
    float r = 100.f;



    bool running = true;
    while (running)
    {

        // light source
        GLfloat light_position[] = { 500.f, 0.f, 500.f, 1.f };
        GLfloat light_diffuse[] = { 1.f, 1.f, 1.f, 1.f };
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
        glEnable(GL_LIGHT0);

        // handle events
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
            else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
            {
                lastPosition = sf::Mouse::getPosition(window);
            }
            else if (event.type == sf::Event::MouseMoved && sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {

                // actual & last coursor position difference
                sf::Vector2i delta = sf::Mouse::getPosition(window) - lastPosition;
                // rotate
                anglex += delta.x * -0.01f;
                angley += delta.y * -0.01f;
                //window.setView(view);
                lastPosition = sf::Mouse::getPosition(window);

                //if (anglex > 360.f)
                //{
                //    anglex -= 360.f;
                //}
                //if (anglex < 0.f)
                //{
                //    anglex += 360.f;
                //}
                if (angley > 1.57f)
                {
                    angley = 1.57;
                }
                if (angley < -1.57f)
                {
                    angley = -1.57f;
                }
            }
            if (event.type == sf::Event::MouseWheelScrolled)
            {
                if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
                {
                    //window.zoom(1 - event.mouseWheelScroll.delta / 10.0f;
                    //window.setView(view);
                    r -= event.mouseWheelScroll.delta * 10.f;
                }
            }
        }

        std::cout << "angley: " << angley << std::endl;

        // clear the buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.f, (float)window.getSize().x / (float)window.getSize().y, 0.1f, 1000.f);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();


        //sphere equation
        cameraPosition.x = r * cos(anglex) * cos(angley);
        cameraPosition.y = r * sin(angley);
        cameraPosition.z = r * sin(anglex) * cos(angley);

        cameraTarget.x = 0;
        cameraTarget.y = 0;
        cameraTarget.z = 0;
        cameraUp.x = 0;
        cameraUp.y = 1;
        cameraUp.z = 0;

        gluLookAt(
            cameraPosition.x, cameraPosition.y, cameraPosition.z,
            cameraTarget.x, cameraTarget.y, cameraTarget.z,
            cameraUp.x, cameraUp.y, cameraUp.z
        );

        // draw 1st sphere
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glTranslatef(0.f, 0.f, 0.f);
        //glTranslatef(0.f, 0.f, distance);
        //glRotatef(angleX, 0.f, 2.f, 0.f);
        //glRotatef(angleZ, 2.f, 0.f, 0.f);
        glColor3f(1.f, 0.f, 0.f);
        glutSolidSphere(50.f, 50, 50);
        glPopMatrix();



        //2nd sphere just for prototypin
        glPushMatrix();
        glTranslatef(100.f, 0.f, 0.f);
        //glTranslatef(0.f, 0.f, distance);
        //glRotatef(angleX, 0.f, 2.f, 0.f);
        //glRotatef(angleZ, 2.f, 0.f, 0.f);
        glColor3f(1.f, 0.f, 0.f);
        glutSolidSphere(20.f, 50, 50);
        glPopMatrix();



        // end the current frame (internally swaps the front and back buffers)
        window.display();
    }

    // release resources...
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);

    // deactivate the window and its context
    window.setActive(false);

    // close the window
    window.close();

    return 0;
}















//#include <SFML/Graphics.hpp>
//#include <SFML/OpenGL.hpp>
//#include <GL/glut.h>
//#include <iostream>
//#include <vector>
//#include <gl/GLU.h>
//
//GLUquadric* sphere = gluNewQuadric();
//
//
//void WINAPI gluSphere(
//    GLUquadric* qobj,
//    GLdouble   radius,
//    GLint      slices,
//    GLint      stacks
//);
//
//void drawSphere() {
//    gluSphere(sphere, 50.0f, 36, 18);
//}
//
//
//int main()
//{
//
//
//    sf::ContextSettings settings;
//    settings.depthBits = 24;
//    settings.stencilBits = 8;
//    settings.antialiasingLevel = 8;
//    settings.majorVersion = 3;
//    settings.minorVersion = 0;
//    // Utwórz okno renderowania
//    sf::RenderWindow window(sf::VideoMode(800, 600), "Prosty silnik graficzny 3D");
//
//    // openGL parameters
//    glEnable(GL_DEPTH_TEST);
//    glDepthFunc(GL_LESS);
//    glEnable(GL_LIGHTING);
//
//
//    // light source
//    GLfloat light_position[] = { 0.f, 0.f, 500.f, 1.f };
//    GLfloat light_diffuse[] = { 1.f, 1.f, 1.f, 1.f };
//    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
//    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
//    glEnable(GL_LIGHT0);
//
//    // Ball initialise
//    //sf::CircleShape ball(50.f);
//    //sf::SphereShape ball(50.f, 32, 32);
//    //ball.setFillColor(sf::Color::Green);
//
//    // Camera initalise
//    sf::Vector3f cameraPosition(0.f, 0.f, 500.f);
//    sf::Vector3f cameraTarget(0.f, 0.f, 0.f);
//    sf::Vector3f cameraUp(0.f, 1.f, 0.f);
//
//    window.setActive(true);
//    //window.setVerticalSyncEnabled(true); //doesnt work, dunno, why
//
//    static float angleX = 0.f;
//    static float angleZ = 0.f;
//    static float distance = 100.f;
//    sf::Vector2f previousMousePos;
//    sf::Vector2i lastPosition;
//
//    bool running = true;
//    /*sf::Vector2i lastMousePos = sf::Mouse::getPosition();*/
//    while (running)
//    {
//        // Obs³u¿ zdarzenia okna
//        sf::Event event;
//        while (window.pollEvent(event))
//        {
//            if (event.type == sf::Event::Closed)
//            {
//                running = false;
//            }
//            else if (event.type == sf::Event::Resized)
//            {
//                // adjust the viewport when the window is resized
//                glViewport(0, 0, event.size.width, event.size.height);
//            }
//            else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
//            {
//                lastPosition = sf::Mouse::getPosition(window);
//            }
//            else if (event.type == sf::Event::MouseMoved && sf::Mouse::isButtonPressed(sf::Mouse::Left))
//            {
//
//                // actual & last coursor position difference
//                sf::Vector2i delta = sf::Mouse::getPosition(window) - lastPosition;
//                // rotate
//                angleX += delta.x * -0.1f;
//                angleZ += delta.y * -0.1f;
//                //window.setView(view);
//                lastPosition = sf::Mouse::getPosition(window);
//
//                if (angleX > 360.f)
//                {
//                    angleX -= 360.f;
//                }
//                if (angleX < 0.f)
//                {
//                    angleX += 360.f;
//                }
//                if (angleZ > 360.f)
//                {
//                    angleZ -= 360.f;
//                }
//                if (angleZ < 0.f)
//                {
//                    angleZ += 360.f;
//                }
//            }
//            if (event.type == sf::Event::MouseWheelScrolled)
//            {
//                if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
//                {
//                    //window.zoom(1 - event.mouseWheelScroll.delta / 10.0f;
//                    //window.setView(view);
//                    distance += event.mouseWheelScroll.delta * 10.f;
//                }
//            }
//
//        }
//
//
//
//        glMatrixMode(GL_PROJECTION);
//        glLoadIdentity();
//        gluPerspective(90.f, (float)window.getSize().x / (float)window.getSize().y, 1.f, 1000.f);
//
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//        // Camera Setup
//        glMatrixMode(GL_MODELVIEW);
//        glLoadIdentity();
//        gluLookAt(cameraPosition.x, cameraPosition.y, cameraPosition.z,
//            cameraTarget.x, cameraTarget.y, cameraTarget.z,
//            cameraUp.x, cameraUp.y, cameraUp.z);
//
//
//
//        //RotateBall
//        //gluSphere(sphere, 10.0f, 36, 18);
//
//
//        glPushMatrix();
//        //glTranslatef(0.f, 0.f , -100.f);
//        glTranslatef(0.f, 0.f, distance);
//        glRotatef(angleX, 0.f, 2.f, 0.f);
//        glRotatef(angleZ, 2.f, 0.f, 0.f);
//        glBegin(GL_TRIANGLE_FAN);
//        //glVertex3f(0.f, 0.f, 0.f);
//        const int segments = 32;
//        const float radius = 50.f;
//        for (int i = 0; i <= segments; ++i)
//        {
//            float theta = i * 2.f * 3.14159f / segments;
//            glVertex3f(radius * cosf(theta), radius * sinf(theta), 0.f);
//        }
//
//        //rawSphere();
//        glEnd();
//        glPopMatrix();
//
//        window.display();
//
//    }
//
//    return 0;
//}






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