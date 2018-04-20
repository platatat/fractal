#include "application.h"
#include "constants.h"
#include <iostream>
#include <stdio.h>

#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glu.h>

using namespace std;


Application app;


void display()
{
    char* buffer = app.display();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawPixels(Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT, GL_LUMINANCE, GL_BYTE, buffer);
    glutSwapBuffers();
}


int main(int argc, char* argv[])
{
    app = Application();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT);
    glutCreateWindow("Mandelbrot");
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glutDisplayFunc(display);
    glutMainLoop();

    return 0;
}