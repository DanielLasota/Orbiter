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
//#include <SFML/OpenGL.hpp>
//#include <SFML/Graphics/Text.hpp>
#include <GL/glut.h>
//#include <gl/GLU.h>
#include <iostream>
//#include <sstream>
//#include <string>
#include <thread>
#include <iomanip>
#include <conio.h>
//#include <ctime>
//#include <ios>
//#include <chrono>

using boost::asio::ip::udp;
using udp = boost::asio::ip::udp;
const uint32_t NTP_TIMESTAMP_DELTA = 2208988800ull;
const size_t recv_time_offset = 32;
const size_t xmit_time_offset = 40;
std::atomic<time_t> downloaded_time = 0;
//std::atomic<std::chrono::high_resolution_clock> downloaded_time = 0;

void xyz_axis_draw()
{
    glPushMatrix();
    glDisable(GL_LIGHTING);
    //glBegin(GL_LINES);
    //// X axis
    //glColor3f(1.f, 0.f, 0.f);
    //glVertex3f(0.f, 0.f, 0.f);
    //glVertex3f(10000.f, 0.f, 0.f);
    //// Y axis
    //glColor3f(0.f, 1.f, 0.f);
    //glVertex3f(0.f, 0.f, 0.f);
    //glVertex3f(0.f, 10000.f, 0.f);
    //// Z asix
    //glColor3f(0.f, 0.f, 1.f);
    //glVertex3f(0.f, 0.f, 0.f);
    //glVertex3f(0.f, 0.f, 10000.f);
    //glEnd();
    glEnable(GL_LIGHTING);
}void earth_draw() {
    glPushMatrix();
    glTranslatef(0.f, 0.f, 0.f);
    glColor3f(1.f, 0.f, 0.f);
    glutSolidSphere(6738.f, 50, 50);
    glPopMatrix();
}
void moon_draw() {
    glPushMatrix();
    glTranslatef(80000.f, 0.f, 0.f);
    glColor3f(1.f, 0.f, 0.f);
    glutSolidSphere(2000.f, 50, 50);
    glPopMatrix();
}
void sat_draw() {
    glPushMatrix();
    glTranslatef(0.f, 0.f, 0.f);
    glColor3f(1.f, 0.f, 0.f);
    glutSolidSphere(100.f, 50, 50);
    glPopMatrix();
}
std::string sys_time()
{
    auto now = std::chrono::system_clock::now();
    std::time_t time_now = std::chrono::system_clock::to_time_t(now);
    std::tm timeinfo;
    localtime_s(&timeinfo, &time_now);
    char buffer[80];
    std::strftime(buffer, 80, "%H:%M:%S", &timeinfo);
    return buffer;
}
time_t get_ntp_time(const std::string& server_address)
{
    try
    {
        boost::asio::io_context io_context;

        udp::resolver resolver(io_context);
        udp::resolver::query query(udp::v4(), server_address, "ntp"); //time-a-g.nist.gov

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
        downloaded_time = static_cast<time_t>(receive_timestamp_seconds);
        time_t receive_time = static_cast<time_t>(receive_timestamp_seconds);
        socket.close();

        return receive_time;
    }
    catch (std::exception& e)
    {
        std::cerr << "Err whilst gettin time..." << e.what() << std::endl;
        return 0;
    }
}
std::string tohms(time_t n) {
    time_t ntp_time = n;
    struct tm timeinfo;
    localtime_s(&timeinfo, &ntp_time);
    char buffer[80];
    strftime(buffer, 80, "%H:%M:%S", &timeinfo);
    return buffer;
}
void clock_engine() {
    while (true) {
        downloaded_time++;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main()
{
    std::thread t(clock_engine);

    sf::Font eurostile;
    if (!eurostile.loadFromFile("C:/Users/Devxd/Desktop/EurostileExtended.ttf"))
    {
        std::cerr << "Could not load font." << std::endl;
        return 1;
    }


    std::string link = "time-a-g.nist.gov";
    get_ntp_time(link); // ntp time donwload main, first initialistion

    std::cout << "NIST TIME:" << tohms(downloaded_time) << std::endl;
    std::cout << "SYS TIME : " << sys_time() << std::endl;
    time_t t_zero = downloaded_time;

    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 8;
    settings.majorVersion = 4;
    settings.minorVersion = 6;


    //sf::RenderWindow window(sf::VideoMode(800, 600), "Orbiter", sf::Style::Default, sf::ContextSettings(32));
    sf::RenderWindow window(sf::VideoMode(800, 600), "Orbiter", sf::Style::Default, settings);
    window.setVerticalSyncEnabled(true);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_LIGHTING);

    // Camera initalise
    sf::Vector3f cameraPosition(0.f, 0.f, 500.f);
    sf::Vector3f cameraTarget(0.f, 0.f, 0.f);
    sf::Vector3f cameraUp(0.f, 1.f, 0.f);
    window.setActive(true);

    static float anglex = 0.f;
    static float angley = 0.f;
    sf::Vector2f previousMousePos;
    sf::Vector2i lastPosition;

    float r = 40000.f; // camera's distance (radius) from the origin

    // orbit parameters define 
    double earth_m = 5.97e24;
    float earth_r = 6738;
    float earth_gm = 398600.5;

    float ap = 400.f;
    float pe = 1900.f;

    float ra = 6738.f + ap; // promień perygeum w km
    float rp = 6738.f + pe; // promień apogeum w km
    float i_deg = 60; // inklinacja w stopniach
    float w_deg = 0; // argument perygeum w stopniach
    float W_deg = 60; // węzeł wstępujący w stopniach
    float a = (rp + ra) / 2; //semi-major axis
    float b = a * sqrt(1 - pow((ra - rp) / (ra + rp), 2)); //semi-minor axis
    float T = 2 * 3.1415926f * sqrt(pow(a, 3) / earth_gm); // ookres orbitalny
    float i = i_deg * 3.1415926f / 180;
    float w = w_deg * 3.1415926f / 180;
    float W = W_deg * 3.1415926f / 180;
    float n = sqrt(398600.5 / pow((rp + ra) / 2, 3)); //mean motion
    float e = sqrt(1 - pow(b / a, 2)); //eccentricity
    


    // przyjmujemy kąt E równy 30 stopni
    float E_deg = 0;
    float E = E_deg * 3.1415926f / 180;
    float cr = (a * (1 - pow(e, 2))) / (1 + e * cos(E)); //odległość od środka Ziemi
    float cr_actual_height = cr - earth_r; //aktualna wysokosc
    float vkms = sqrt(earth_gm * ((2 / cr) - (1 / a))); //aktualna prędkość orbitalna
    float vkmh = vkms * 3600.0 / 1000.0; // km/s to km/h
    float vms = vkms * 1000.0; // km/s to m/s

    float x, y, z, cosi = cos(i), sini = sin(i), cosw = cos(w), sinw = sin(w), cosW = cos(W), sinW = sin(W);

    int total_seconds = (int)T;
    int minutes = total_seconds / 60;
    int seconds = total_seconds % 60;
    int milliseconds = (T - total_seconds) * 1000;

    std::stringstream T_format;
    T_format << std::setw(2) << std::setfill('0') << minutes << ":";
    T_format << std::setw(2) << std::setfill('0') << seconds << ":";
    T_format << std::setw(3) << std::setfill('0') << milliseconds;

    std::string formatted_time = T_format.str();

    //float E0 = 0;
    //float E1 = 1;
    //float epsilon = 0.0001f;
    //while (fabs(E1 - E0) > epsilon) {
    //    E0 = E1;
    //    E1 = M + e * sin(E0);
    //}

    //float v = 2 * atan(sqrt((1 + e) / (1 - e)) * tan(E1 / 2));
    //float r = a * (1 - e * cos(E1));
    
    //float E0 = 0;
    //float M = 2 * 3.1415926f * fmod((glfwGetTime() / T), 1);
    //float E = M;

    //while (abs(E - E0) > 0.0001) {
    //    E0 = E;
    //    E = M + (ra - rp) * sin(E) / (a * b * sqrt(1 - pow(e, 2))) - E0;
    //}

    /*if (pe > ap)
        return 0;*/
    

    std::cout << "earth mass " << earth_m << " kg" << std::endl;
    std::cout << "stala grawitacyjna dla ziemi " << earth_gm << " km^3/s^2" << std::endl;
    std::cout << "rp(perigee radius): " << rp << " m" << std::endl;
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
    int i_frame = 0;

    float xs, ys, zs;
    float diff;

    float chx;
    float ch;
    int view_button = 0;

    bool running = true;
    while (running)
    {

        if (_kbhit()) {
            int chx = _getch();
            int ch = (int)((char)chx - '0');
            if (chx == 'v')
                view_button += 1;
        }
        if (view_button >= 2)
            view_button = 0;



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
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Delete)
            {
                orbit_data.setString("");
            }
        }

        glViewport(window.getSize().x * 0.38, 0, window.getSize().x * 0.62, window.getSize().y);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(64.f, (float)window.getSize().x / (float)window.getSize().y * 0.62, 10.f, 200000.f);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        if (view_button == 0)
        {        
            //camera sphere equations
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

        }
        else if (view_button = 1) //ustawienie kamery z focusem na satelite - to do 
        {
            //camera sphere equations
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
        }


        earth_draw();
        moon_draw();




        // VARIABLES FOR CHANGIN SATELITE POSITION
        diff = downloaded_time - t_zero;
        E_deg = (-diff / T) * 360;

        E = E_deg * 3.1415926f / 180;
        cr = (a * (1 - pow(e, 2))) / (1 + e * cos(E)); //odległość od środka Ziemi
        cr_actual_height = cr - earth_r; //aktualna wysokosc
        vkms = sqrt(earth_gm * ((2 / cr) - (1 / a))); //aktualna prędkość orbitalna

        vkmh = vkms * 3600.0 / 1000.0; // km/s to km/h
        vms = vkms * 1000.0; // km/s to m/s



        //satelite position render data
        float theta = E_deg * 3.1415926f / 180;
        float r = (a * (1 - pow(e, 2))) / (1 + e * cos(theta));
        xs = r * (cosw * cos(theta) - sinw * sin(theta) * cosi);
        ys = r * (sinw * cos(theta) + cosw * sin(theta) * cosi);
        zs = r * (sini * sin(theta));
        //float temp_x = x * cosW - y * sinW;
        //float temp_y = x * sinW + y * cosW;
        //x = temp_x;
        //y = temp_y;
        glPushMatrix();
        glTranslatef(xs, ys, zs);
        glDisable(GL_LIGHTING);
        glColor3f(0.f, 1.f, 0.f);
        glutSolidSphere(50.f, 50, 50);
        glPopMatrix();
        //end of satelite position render data


        //circle render data - to transform into an orbit
        //glDisable(GL_LIGHT0);
        glPushMatrix();
        glTranslatef(0.f, 0.f, 0.f);
        glDisable(GL_LIGHTING);
        glColor3f(0.2f, 0.6f, 0.2f);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i <= 360; i++) {
            float theta = i * 3.1415926f / 180;
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

        glEnd();
        xyz_axis_draw();
        window.pushGLStates();


        if (i_frame % 2 == 0)
        {
            oss.str("");
            oss << std::fixed << std::setprecision(3)
                << "downloaded_time: " << downloaded_time << " " << std::endl
                << "t_zero: " << t_zero << std::endl
                << "diff: " << diff << std::endl
                << "NIST time: " << tohms(downloaded_time) << std::endl
                << "RP: " << rp << " km" << std::endl
                << "RA: " << ra << " km" << std::endl
                << "AP: " << ap << " km" << std::endl
                << "PE: " << pe << " km" << std::endl
                << std::endl

                << std::defaultfloat
                << "i (inclination): " << i_deg << " deg" << std::endl
                << "w (perigee argument): " << w_deg << " deg" << std::endl
                << "W (Ascending Node): " << W_deg << " deg" << std::endl

                << std::fixed << std::setprecision(3)
                << "T (Period): " << T << " sec" << std::endl
                << "T (Period): " << formatted_time << "     min:sec:ms" << std::endl
                << "a (semi-major axis): " << a * 1000 << " km" << std::endl
                << "b (semi-minor axis): " << b * 1000 << " km" << std::endl
                << std::fixed << std::setprecision(7)
                << "n (mean motion): " << n << " rad/sec" << std::endl
                << "e (eccentricity): " << e << std::endl
                << std::endl

                << std::fixed << std::setprecision(3)
                << "actual height: " << cr_actual_height << " km" << std::endl
                << "v (m/s): " << vms << " m/s" << std::endl
                << "v (km/h): " << vkmh << " km/h" << std::endl
                //<< "v button:  " << view_button << std::endl
                << "Period status - since periapsis(%):" << std::endl << diff << "/" << T << std::endl << (diff / T) * 100 << "%" << std::endl
                << "E_deg: " << E_deg << std::endl;
            orbit_data.setString(oss.str());
        }

        window.draw(orbit_data);
        oss << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);

        window.popGLStates();
        glPopMatrix();
        window.display();

        i_frame++;

        if (i_frame >= 60)
            i_frame = 0;
        if (diff >= T)
            diff = 0;
    }
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    window.setActive(false);
    window.close();


    t.join();
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