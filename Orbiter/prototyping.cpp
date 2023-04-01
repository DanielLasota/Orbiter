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