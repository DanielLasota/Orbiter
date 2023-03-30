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

#include <boost/asio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics/Text.hpp>
#include <GL/glut.h>
#include <gl/GLU.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <curl/curl.h>
#include <array>
#include <ctime>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

const uint32_t NTP_TIMESTAMP_DELTA = 2208988800ull;
const size_t recv_time_offset = 32;
const size_t xmit_time_offset = 40;

void xyz_axis_draw()
{
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    // X axis
    glColor3f(1.f, 0.f, 0.f);
    glVertex3f(0.f, 0.f, 0.f);
    glVertex3f(10000.f, 0.f, 0.f);
    // Y axis
    glColor3f(0.f, 1.f, 0.f);
    glVertex3f(0.f, 0.f, 0.f);
    glVertex3f(0.f, 10000.f, 0.f);
    // Z asix
    glColor3f(0.f, 0.f, 1.f);
    glVertex3f(0.f, 0.f, 0.f);
    glVertex3f(0.f, 0.f, 10000.f);
    glEnd();
    glEnable(GL_LIGHTING);
}
void earth_draw() {
    glPushMatrix();
    glTranslatef(0.f, 0.f, 0.f);
    glColor3f(1.f, 0.f, 0.f);
    glutSolidSphere(6738.f, 50, 50);
    glPopMatrix();
}
void moon_draw() {
        ////2nd sphere just for prototypin
        glPushMatrix();
        glTranslatef(80000.f, 0.f, 0.f);
        glColor3f(1.f, 0.f, 0.f);
        glutSolidSphere(2000.f, 50, 50);
        glPopMatrix();
}


time_t get_ntp_time(const std::string& server_address)
{
    try
    {
        boost::asio::io_context io_context;

        udp::resolver resolver(io_context);
        udp::resolver::query query(udp::v4(), server_address, "ntp");

        udp::endpoint receiver_endpoint = *resolver.resolve(query);

        udp::socket socket(io_context);
        socket.open(udp::v4());
        socket.connect(receiver_endpoint);

        std::array<uint8_t, 48> buffer;
        buffer.fill(0);
        buffer[0] = 0x1b;
        socket.send(boost::asio::buffer(buffer));

        std::array<uint8_t, 1024> reply_buffer;
        udp::endpoint sender_endpoint;
        size_t reply_length = socket.receive_from(boost::asio::buffer(reply_buffer), sender_endpoint);
        uint32_t receive_timestamp = (reply_buffer[recv_time_offset] << 24) |
            (reply_buffer[recv_time_offset + 1] << 16) |
            (reply_buffer[recv_time_offset + 2] << 8) |
            (reply_buffer[recv_time_offset + 3]);
        double receive_timestamp_seconds = static_cast<double>(receive_timestamp) - NTP_TIMESTAMP_DELTA;
        time_t receive_time = static_cast<time_t>(receive_timestamp_seconds);

        socket.close();

        return receive_time;
    }
    catch (std::exception& e)
    {
        std::cerr << "Wystąpił błąd: " << e.what() << std::endl;
        return 0;
    }
}

int main()
{
    sf::Font eurostile;
    if (!eurostile.loadFromFile("C:/Users/Devxd/Desktop/EurostileExtended.ttf"))
    {
        std::cerr << "Could not load font." << std::endl;
        return 1;
    }

    sf::RenderWindow window(sf::VideoMode(800, 600), "Orbiter", sf::Style::Default, sf::ContextSettings(32));
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

    // Camera initalise
    sf::Vector3f cameraPosition(0.f, 0.f, 500.f);
    sf::Vector3f cameraTarget(0.f, 0.f, 0.f);
    sf::Vector3f cameraUp(0.f, 1.f, 0.f);
    window.setActive(true);

    static float anglex = 0.f;
    static float angley = 0.f;
    sf::Vector2f previousMousePos;
    sf::Vector2i lastPosition;

    float r = 40000.f; // camera's distance from the origin


    // orbit parameters define 
    float ap = 1600.658f;
    float pe = 110.213f;
    
    float ra = 6738.f + ap; // promień perygeum w km
    float rp = 6738.f + pe; // promień apogeum w km
    float i_deg = 60; // inklinacja w stopniach
    float w_deg = 0; // argument perygeum w stopniach
    float W_deg = 60; // węzeł wstępujący w stopniach
    float T = 90 * 60; // czas okresu obiegu w sekundach
    float i = i_deg * 3.1415926f / 180;
    float w = w_deg * 3.1415926f / 180;
    float W = W_deg * 3.1415926f / 180;
    float a = (rp + ra) / 2; //semi-major axis
    float b = a * sqrt(1 - pow((ra - rp) / (ra + rp), 2)); //semi-minor axis
    float n = sqrt(398600.5 / pow((rp + ra) / 2, 3)); //mean motion
    float e = sqrt(1 - pow(b / a, 2)); //eccentricity
    //float E0 = 0;
    //float M = 2 * 3.1415926f * fmod((glfwGetTime() / T), 1);
    //float E = M;

    //while (abs(E - E0) > 0.0001) {
    //    E0 = E;
    //    E = M + (ra - rp) * sin(E) / (a * b * sqrt(1 - pow(e, 2))) - E0;
    //}

    if (pe > ap)
        return 0;
    float x, y, z, cosi = cos(i), sini = sin(i), cosw = cos(w), sinw = sin(w), cosW = cos(W), sinW = sin(W);
    //float x, y, z, cosi = cos(i_deg), sini = sin(i_deg), cosw = cos(w), sinw =

    std::cout << "rp(perigee radius): " << rp  << " m" << std::endl;
    std::cout << "ra(apogee radius): " << ra << " m" << std::endl;
    std::cout << "AP: " << ap * 1000 << " m" << std::endl;
    std::cout << "PE: " << pe * 1000 << " m" << std::endl;
    std::cout << "i (inclination): " << i_deg << " deg" << std::endl;
    std::cout << "w (perigee argument): " << w_deg << " deg" << std::endl;
    std::cout << "W (Ascending Node): " << W_deg << " deg" << std::endl;
    std::cout << "T (Period): " << T << " sec" << std::endl;
    std::cout << "a (semi-major axis): " << a << " km" << std::endl;
    std::cout << "b (semi-minor axis): " << b << " km" << std::endl;
    std::cout << "n (mean motion): " << n << " rad/sec" << std::endl;
    std::cout << "e (eccentricity): " << e << std::endl;


    sf::View textView(sf::FloatRect(0, 0, window.getSize().x * 0.38, window.getSize().y));
    sf::Text orbit_data;
    orbit_data.setFont(eurostile);
    orbit_data.setCharacterSize(12);
    orbit_data.setFillColor(sf::Color::Green);
    std::stringstream oss;
    oss << "usno time: " << std::endl
        << "RP: " << rp << " km" << std::endl
        << "RA: " << ra << " km" << std::endl
        << "AP: " << ap << " km" << std::endl
        << "PE: " << pe << " km" << std::endl
        << "i (inclination): " << i_deg << " deg" << std::endl
        << "w (perigee argument): " << w_deg << " deg" << std::endl
        << "W (Ascending Node): " << W_deg << " deg" << std::endl
        << "T (Period): " << T << " sec" << std::endl
        << "a (semi-major axis): " << a * 1000 << " km" << std::endl
        << "b (semi-minor axis): " << b * 1000 << " km" << std::endl
        << "n (mean motion): " << n << " rad/sec" << std::endl
        << "e (eccentricity): " << e << std::endl;
    orbit_data.setString(oss.str());
    


    bool running = true;
    while (running)
    {
        // light source
        GLfloat light_position[] = { 50000.f, 0.f, 50000.f, 1.f };
        GLfloat light_diffuse[] = { 1.f, 1.f, 1.f, 1.f };
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
        glEnable(GL_LIGHT0);
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
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
            else if (event.type == sf::Event::MouseMoved && sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                // actual & last coursor position difference
                sf::Vector2i delta = sf::Mouse::getPosition(window) - lastPosition;
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
            else if (event.type == sf::Event::MouseWheelScrolled)
            {
                if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
                {
                    r -= event.mouseWheelScroll.delta * 1000.f;
                }
            }
        }
        
        glViewport(window.getSize().x * 0.38, 0, window.getSize().x * 0.62, window.getSize().y);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(64.f, (float)window.getSize().x / (float)window.getSize().y * 0.62, 10.f, 200000.f);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        //camera sphere equation
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
       
        earth_draw();
        moon_draw();

        //circle render data - to transform into an orbit
        //glDisable(GL_LIGHT0);
        glPushMatrix();
        glTranslatef(0.f, 0.f, 0.f);
        glDisable(GL_LIGHTING);
        glColor3f(0.2f, 0.6f, 0.2f);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i <= 360; i++) {
            float theta = i * 3.1415926f / 180; // kąt w radianach
            float r = (a * (1 - pow(e, 2))) / (1 + e * cos(theta));
            x = r * (cosw * cos(theta) - sinw * sin(theta) * cosi);
            y = r * (sinw * cos(theta) + cosw * sin(theta) * cosi);
            z = r * (sini * sin(theta));
            //float temp_x = x * cosW - y * sinW;
            //float temp_y = x * sinW + y * cosW;
            //x = temp_x;
            //y = temp_y;
            glVertex3f(x, y, z);
        }
        // basic circle - orbit alpha version
        //for (int i = 0; i < 50; ++i)
        //{
        //    float theta = 2.0f * 3.1415926f * float(i) / float(50); // kąt w radianach
        //    float x = 80 * cosf(theta); // współrzędna x punktu na okręgu //80 to promien orbity
        //    float z = 80 * sinf(theta); // współrzędna z punktu na okręgu // 80 to promien orbity
        //    glVertex3f(x, 0.f, z); // wierzchołek okręgu
        //}
        glEnd();
        xyz_axis_draw();

        window.pushGLStates();
        window.draw(orbit_data);
        window.popGLStates();

        glPopMatrix();
        window.display();
    }
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    window.setActive(false);
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
//
//
//
//
//
//
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
//        satellite.setPosition(orbit[index].position); 
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