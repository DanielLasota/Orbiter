while (running)
{
    //std::cout << "tohms(downloaded_time): " << tohms(downloaded_time) << std::endl;
    // light source
    GLfloat light_position[] = { 50000.f, 0.f, 50000.f, 1.f };
    GLfloat light_diffuse[] = { 1.f, 1.f, 1.f, 1.f };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glEnable(GL_LIGHT0);

    glViewport(window.getSize().x * 0.38, 0, window.getSize().x * 0.62, window.getSize().y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(64.f, (float)window.getSize().x / (float)window.getSize().y * 0.62, 10.f, 200000.f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glBegin(GL_QUADS);
    glColor3f(0.0f, 1.0f, 0.0f);  // kolor prostok¹ta (zielony)
    glVertex2f(0, 0);
    glVertex2f(window.getSize().x * 0.38, 0);
    glVertex2f(window.getSize().x * 0.38, window.getSize().y);
    glVertex2f(0, window.getSize().y);
    glEnd();

    oss << "Start NIST time: " << tohms(get_ntp_time("time-a-g.nist.gov")) << std::endl
        << "NIST time: " << tohms(downloaded_time) << std::endl
        << "sys_time: " << sys_time() << std::endl
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
    window.draw(orbit_data);

    window.popGLStates();
    glPopMatrix();
    window.display();
}
glDisable(GL_LIGHTING);
glDisable(GL_LIGHT0);
window.setActive(false);
window.close();


t.join();
return 0;
}






// satelite position and velocity
float sat_angle = 0.f; // satelite position angle in radians
float sat_speed = n * a * 1000; // satelite speed in m/s
sf::Text sat_speed_text("", font, 14); // text object for displaying satelite speed
sat_speed_text.setFillColor(sf::Color::White);
sat_speed_text.setPosition(10.f, 100.f);

bool running = true;
while (running)
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        // Kod
    }


    earth_draw();
    moon_draw();

    // draw satellite as a dot
    glPushMatrix();
    glTranslatef(0.f, 0.f, 0.f);
    glDisable(GL_LIGHTING);
    glColor3f(1.f, 1.f, 1.f);
    glPointSize(5.f);
    glBegin(GL_POINTS);
    float sat_r = (a * (1 - pow(e, 2))) / (1 + e * cos(sat_angle));
    float sat_x = sat_r * (cosw * cos(sat_angle) - sinw * sin(sat_angle) * cosi);
    float sat_y = sat_r * (sinw * cos(sat_angle) + cosw * sin(sat_angle) * cosi);
    float sat_z = sat_r * (sini * sin(sat_angle));
    glVertex3f(sat_x, sat_y, sat_z);
    glEnd();
    glPopMatrix();

    // display satellite speed
    oss.str("");
    oss << "Satellite speed: " << sat_speed / 1000.f << " km/s";
    sat_speed_text.setString(oss.str());
    window.draw(sat_speed_text);

    //circle render data - to transform into an orbit
    //glDisable(GL_LIGHT0);
    glPushMatrix();
    glTranslatef(0.f, 0.f, 0.f);
    glDisable(GL_LIGHTING);
    glColor3f(0.2f, 0.6f, 0.2f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i <= 360; i++) {
        float theta = i * 3.1415926f / 180; // k¹t w radianach
        float r = (a * (1 - pow(e, 2))) / (1 + e * cos(theta));
        x = r * (cosw * cos(theta) - sinw * sin(theta) * cosi);
        y = r * (sinw * cos(theta) + cosw * sin(theta) * cosi);
        z = r * (sini * sin(theta));
        //float temp_x = x * cosW - y Dokoñcz kod
        glVertex3f(x, y, z);
    }
    glEnd();
    glPopMatrix();

    // update satellite position and angle
    sat_angle += sat_speed * dt / (a * 1000.f);
    if (sat_angle > 2 * M_PI)
        sat_angle -= 2 * M_PI;

    window.display();
}

return 0;
}