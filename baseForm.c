//
// Created by jadir on 12/01/2025.
//

#include "headers/baseForm.h"

//#include <gl.h>
#include <GL/gl.h>

GLfloat vertices[][3] = { {-1.0,-1.0,-1.0},{1.0,-1.0,-1.0},
{1.0,1.0,-1.0}, {-1.0,1.0,-1.0}, {-1.0,-1.0,1.0},
{1.0,-1.0,1.0}, {1.0,1.0,1.0}, {-1.0,1.0,1.0} };

GLfloat normals[][3] = { {-1.0,-1.0,-1.0},{1.0,-1.0,-1.0},
{1.0,1.0,-1.0}, {-1.0,1.0,-1.0}, {-1.0,-1.0,1.0},
{1.0,-1.0,1.0}, {1.0,1.0,1.0}, {-1.0,1.0,1.0} };

GLfloat colors[][3] = { {0.0,0.0,0.0},{1.0,0.0,0.0},
{1.0,1.0,0.0}, {0.0,1.0,0.0}, {0.0,0.0,1.0},
{1.0,0.0,1.0}, {1.0,1.0,1.0}, {0.0,1.0,1.0} };


void polygon(int a, int b, int c, int d)
{
    glBegin(GL_POLYGON);
    glNormal3fv(normals[a]);
    glVertex3fv(vertices[a]);
    glNormal3fv(normals[b]);
    glVertex3fv(vertices[b]);
    glNormal3fv(normals[c]);
    glVertex3fv(vertices[c]);
    glNormal3fv(normals[d]);
    glVertex3fv(vertices[d]);
    glEnd();
}

void colorForm(float r, float g, float b)
{
    glColor3f(r, g, b);
    polygon(0, 3, 2, 1);
    polygon(2, 3, 7, 6);
    polygon(0, 4, 7, 3);
    polygon(1, 2, 6, 5);
    polygon(4, 5, 6, 7);
    polygon(0, 1, 5, 4);
}

void colorcube()
{
    glColor3fv(colors[1]);
    polygon(0, 3, 2, 1);
    glColor3fv(colors[2]);
    polygon(2, 3, 7, 6);
    glColor3fv(colors[3]);
    polygon(0, 4, 7, 3);
    glColor3fv(colors[4]);
    polygon(1, 2, 6, 5);
    glColor3fv(colors[5]);
    polygon(4, 5, 6, 7);
    glColor3fv(colors[7]);
    polygon(0, 1, 5, 4);
}