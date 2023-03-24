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
#include <iostream>
#include <vector>



std::vector<float> vertices;
std::vector<float> normals;
std::vector<float> texCoords;
std::vector<unsigned int> indices;
std::vector<unsigned int> lineIndices;
//bool smooth;
std::vector<float> interleavedVertices;
int upAxis;
int interleavedStride = 32;
const int MIN_SECTOR_COUNT = 3;
const int MIN_STACK_COUNT = 2;
int sectorCount;                        // longitude, # of slices
int stackCount;                         // latitude, # of stacks



float lineColor[] = { 0.2f, 0.2f, 0.2f, 1 };

void changeUpAxis();
void buildInterleavedVertices();
void addIndices();
void addTexCoord();
void addNormal();
void addVertex();
void clearArrays();
void buildVerticesSmooth();
void drawLines();
void displayCB();
void set();
void Sphere();

void changeUpAxis(int from, int to)
{
    // initial transform matrix cols
    float tx[] = { 1.0f, 0.0f, 0.0f };    // x-axis (left)
    float ty[] = { 0.0f, 1.0f, 0.0f };    // y-axis (up)
    float tz[] = { 0.0f, 0.0f, 1.0f };    // z-axis (forward)

    // X -> Y
    if (from == 1 && to == 2)
    {
        tx[0] = 0.0f; tx[1] = 1.0f;
        ty[0] = -1.0f; ty[1] = 0.0f;
    }
    // X -> Z
    else if (from == 1 && to == 3)
    {
        tx[0] = 0.0f; tx[2] = 1.0f;
        tz[0] = -1.0f; tz[2] = 0.0f;
    }
    // Y -> X
    else if (from == 2 && to == 1)
    {
        tx[0] = 0.0f; tx[1] = -1.0f;
        ty[0] = 1.0f; ty[1] = 0.0f;
    }
    // Y -> Z
    else if (from == 2 && to == 3)
    {
        ty[1] = 0.0f; ty[2] = 1.0f;
        tz[1] = -1.0f; tz[2] = 0.0f;
    }
    //  Z -> X
    else if (from == 3 && to == 1)
    {
        tx[0] = 0.0f; tx[2] = -1.0f;
        tz[0] = 1.0f; tz[2] = 0.0f;
    }
    // Z -> Y
    else
    {
        ty[1] = 0.0f; ty[2] = -1.0f;
        tz[1] = 1.0f; tz[2] = 0.0f;
    }

    std::size_t i, j;
    std::size_t count = vertices.size();
    float vx, vy, vz;
    float nx, ny, nz;
    for (i = 0, j = 0; i < count; i += 3, j += 8)
    {
        // transform vertices
        vx = vertices[i];
        vy = vertices[i + 1];
        vz = vertices[i + 2];
        vertices[i] = tx[0] * vx + ty[0] * vy + tz[0] * vz;   // x
        vertices[i + 1] = tx[1] * vx + ty[1] * vy + tz[1] * vz;   // y
        vertices[i + 2] = tx[2] * vx + ty[2] * vy + tz[2] * vz;   // z

        // transform normals
        nx = normals[i];
        ny = normals[i + 1];
        nz = normals[i + 2];
        normals[i] = tx[0] * nx + ty[0] * ny + tz[0] * nz;   // nx
        normals[i + 1] = tx[1] * nx + ty[1] * ny + tz[1] * nz;   // ny
        normals[i + 2] = tx[2] * nx + ty[2] * ny + tz[2] * nz;   // nz

        // trnasform interleaved array
        interleavedVertices[j] = vertices[i];
        interleavedVertices[j + 1] = vertices[i + 1];
        interleavedVertices[j + 2] = vertices[i + 2];
        interleavedVertices[j + 3] = normals[i];
        interleavedVertices[j + 4] = normals[i + 1];
        interleavedVertices[j + 5] = normals[i + 2];
    }
}

void buildInterleavedVertices()
{
    std::vector<float>().swap(interleavedVertices);

    std::size_t i, j;
    std::size_t count = vertices.size();
    for (i = 0, j = 0; i < count; i += 3, j += 2)
    {
        interleavedVertices.push_back(vertices[i]);
        interleavedVertices.push_back(vertices[i + 1]);
        interleavedVertices.push_back(vertices[i + 2]);

        interleavedVertices.push_back(normals[i]);
        interleavedVertices.push_back(normals[i + 1]);
        interleavedVertices.push_back(normals[i + 2]);

        interleavedVertices.push_back(texCoords[j]);
        interleavedVertices.push_back(texCoords[j + 1]);
    }
}

void addIndices(unsigned int i1, unsigned int i2, unsigned int i3)
{
    indices.push_back(i1);
    indices.push_back(i2);
    indices.push_back(i3);
}

void addTexCoord(float s, float t)
{
    texCoords.push_back(s);
    texCoords.push_back(t);
}

void addNormal(float nx, float ny, float nz)
{
    normals.push_back(nx);
    normals.push_back(ny);
    normals.push_back(nz);
}

void addVertex(float x, float y, float z)
{
    vertices.push_back(x);
    vertices.push_back(y);
    vertices.push_back(z);
}

void clearArrays()
{
    std::vector<float>().swap(vertices);
    std::vector<float>().swap(normals);
    std::vector<float>().swap(texCoords);
    std::vector<unsigned int>().swap(indices);
    std::vector<unsigned int>().swap(lineIndices);
}

void buildVerticesSmooth()
{
    const float PI = acos(-1.0f);

    // clear memory of prev arrays
    clearArrays();

    float radius = 1.0f;
    int sectorCount = 36;
    int stackCount = 18;
    bool smooth = true;
    int up = 3;

    float x, y, z, xy;                              // vertex position
    float nx, ny, nz, lengthInv = 1.0f / radius;    // normal
    float s, t;                                     // texCoord

    float sectorStep = 2 * PI / sectorCount;
    float stackStep = PI / stackCount;
    float sectorAngle, stackAngle;

    for (int i = 0; i <= stackCount; ++i)
    {
        stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);             // r * cos(u)
        z = radius * sinf(stackAngle);              // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // the first and last vertices have same position and normal, but different tex coords
        for (int j = 0; j <= sectorCount; ++j)
        {
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi

            // vertex position
            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
            addVertex(x, y, z);

            // normalized vertex normal
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;
            addNormal(nx, ny, nz);

            // vertex tex coord between [0, 1]
            s = (float)j / sectorCount;
            t = (float)i / stackCount;
            addTexCoord(s, t);
        }
    }

    // indices
    //  k1--k1+1
    //  |  / |
    //  | /  |
    //  k2--k2+1
    unsigned int k1, k2;
    for (int i = 0; i < stackCount; ++i)
    {
        k1 = i * (sectorCount + 1);     // beginning of current stack
        k2 = k1 + sectorCount + 1;      // beginning of next stack

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector excluding 1st and last stacks
            if (i != 0)
            {
                addIndices(k1, k2, k1 + 1);   // k1---k2---k1+1
            }

            if (i != (stackCount - 1))
            {
                addIndices(k1 + 1, k2, k2 + 1); // k1+1---k2---k2+1
            }

            // vertical lines for all stacks
            lineIndices.push_back(k1);
            lineIndices.push_back(k2);
            if (i != 0)  // horizontal lines except 1st stack
            {
                lineIndices.push_back(k1);
                lineIndices.push_back(k1 + 1);
            }
        }
    }

    // generate interleaved vertex array as well
    buildInterleavedVertices();

    // change up axis from Z-axis to the given
    if (upAxis != 3)
        changeUpAxis(3, upAxis);
}

void drawLines(const float lineColor[4]) {
    // set line colour
    glColor4fv(lineColor);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, lineColor);

    // draw lines with VA
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices.data());

    glDrawElements(GL_LINES, (unsigned int)lineIndices.size(), GL_UNSIGNED_INT, lineIndices.data());

    glDisableClientState(GL_VERTEX_ARRAY);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
}

void draw()
{
    // interleaved array
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(3, GL_FLOAT, interleavedStride, &interleavedVertices[0]);
    glNormalPointer(GL_FLOAT, interleavedStride, &interleavedVertices[3]);
    glTexCoordPointer(2, GL_FLOAT, interleavedStride, &interleavedVertices[6]);

    glDrawElements(GL_TRIANGLES, (unsigned int)indices.size(), GL_UNSIGNED_INT, indices.data());

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void drawWithLines(const float lineColor[4])
{
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0, 1.0f); // move polygon backward
    draw();
    glDisable(GL_POLYGON_OFFSET_FILL);

    // draw lines with VA
    drawLines(lineColor);
}

void displayCB()
{
    // clear buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // save the initial ModelView matrix before modifying ModelView matrix
    glPushMatrix();

    // tramsform modelview matrix
    glTranslatef(0, 0, -500.f);

    // set material
    float ambient[] = { 0.5f, 0.5f, 0.5f, 1 };
    float diffuse[] = { 0.7f, 0.7f, 0.7f, 1 };
    float specular[] = { 1.0f, 1.0f, 1.0f, 1 };
    float shininess = 128;
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

    // line color
    float lineColor[] = { 0.2f, 0.2f, 0.2f, 1 };

    //// draw left flat sphere with lines
    //glPushMatrix();
    //glTranslatef(-2.5f, 0, 0);
    //glRotatef(cameraAngleX, 1, 0, 0);   // pitch
    //glRotatef(cameraAngleY, 0, 1, 0);   // heading
    //glBindTexture(GL_TEXTURE_2D, 0);
    //sphere1.drawWithLines(lineColor);
    ////sphere1.drawLines(lineColor);
    //glPopMatrix();

    /*static float angleX = 0.f;
    static float angleZ = 0.f;*/

    // draw centre smooth sphere with line
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse); // reset diffuse
    glPushMatrix();
    glRotatef(0.f, 1, 0, 0); // change to angleX, angleZ
    glRotatef(0.f, 0, 1, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    drawWithLines(lineColor);
    glPopMatrix();

    //// draw right sphere with texture
    //glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse); // reset diffuse
    //glPushMatrix();
    //glTranslatef(2.5f, 0, 0);
    //glRotatef(cameraAngleX, 1, 0, 0);
    //glRotatef(cameraAngleY, 0, 1, 0);
    //glBindTexture(GL_TEXTURE_2D, texId);
    //sphere2.draw();
    //glPopMatrix();

    /*
    // using GLU quadric object
    GLUquadricObj* obj = gluNewQuadric();
    gluQuadricDrawStyle(obj, GLU_FILL); // GLU_FILL, GLU_LINE, GLU_SILHOUETTE, GLU_POINT
    gluQuadricNormals(obj, GL_SMOOTH);
    gluQuadricTexture(obj, GL_TRUE);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    gluSphere(obj, 2.0, 50, 50); // radius, slice, stack
    */

    glBindTexture(GL_TEXTURE_2D, 0);

    //showInfo();     // print max range of glDrawRangeElements

    glPopMatrix();

    glutSwapBuffers();
}

void set(float radius, int sectors, int stacks, bool smooth, int up)
{
    radius = 1.0f;
    sectorCount = 36;
    stackCount = 18;
    smooth = true;
    up = 3;




    if (radius > 0)
        radius = radius;
    sectorCount = sectors;
    if (sectors < MIN_SECTOR_COUNT)
        sectorCount = MIN_SECTOR_COUNT;
    stackCount = stacks;
    if (stacks < MIN_STACK_COUNT)
        stackCount = MIN_STACK_COUNT;
    smooth = smooth;
    upAxis = up;
    if (up < 1 || up > 3)
        upAxis = 3;
    if (smooth)
        buildVerticesSmooth();
}

void Sphere(float radius, int sectors, int stacks, bool smooth, int up)
{
    set(radius, sectors, stacks, smooth, up);
}

int main()
{
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 8;
    settings.majorVersion = 3;
    settings.minorVersion = 0;
    // Utwórz okno renderowania
    sf::RenderWindow window(sf::VideoMode(800, 600), "Prosty silnik graficzny 3D");

    // openGL parameters
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_LIGHTING);



    // light source
    GLfloat light_position[] = { 0.f, 0.f, 500.f, 1.f };
    GLfloat light_diffuse[] = { 1.f, 1.f, 1.f, 1.f };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glEnable(GL_LIGHT0);

    // Ball initialise
    sf::CircleShape ball(50.f);
    //sf::SphereShape ball(50.f, 32, 32);
    //ball.setFillColor(sf::Color::Green);

    // Camera initalise
    sf::Vector3f cameraPosition(0.f, 0.f, 500.f);
    sf::Vector3f cameraTarget(0.f, 0.f, 0.f);
    sf::Vector3f cameraUp(0.f, 1.f, 0.f);

    window.setActive(true);
    //window.setVerticalSyncEnabled(true); //doesnt work, dunno, why

    static float angleX = 0.f;
    static float angleZ = 0.f;
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
                angleX += delta.x * -0.1f;
                angleZ += delta.y * -0.1f;
                //window.setView(view);
                lastPosition = sf::Mouse::getPosition(window);

                if (angleX > 360.f)
                {
                    angleX -= 360.f;
                }
                if (angleX < 0.f)
                {
                    angleX += 360.f;
                }
                if (angleZ > 360.f)
                {
                    angleZ -= 360.f;
                }
                if (angleZ < 0.f)
                {
                    angleZ += 360.f;
                }
            }
        }

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(90.f, (float)window.getSize().x / (float)window.getSize().y, 1.f, 1000.f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Camera Setup
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(cameraPosition.x, cameraPosition.y, cameraPosition.z,
            cameraTarget.x, cameraTarget.y, cameraTarget.z,
            cameraUp.x, cameraUp.y, cameraUp.z);

        //RotateBall

        displayCB();

        //glPushMatrix();
        //glTranslatef(0.f, 0.f, -100.f);
        //glRotatef(angleX, 0.f, 2.f, 0.f);
        //glRotatef(angleZ, 2.f, 0.f, 0.f);
        //glBegin(GL_TRIANGLE_FAN);
        ////glVertex3f(0.f, 0.f, 0.f);
        //const int segments = 32;
        //const float radius = 50.f;
        //for (int i = 0; i <= segments; ++i)
        //{
        //    float theta = i * 2.f * 3.14159f / segments;
        //    glVertex3f(radius * cosf(theta), radius * sinf(theta), 0.f);
        //}
        //glEnd();
        //glPopMatrix();
      

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