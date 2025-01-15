
#include <stdlib.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#include <math.h>
#include "headers/characters.h"
#endif

#include <stdio.h>
#include <time.h>
#include <math.h>
#include "headers/baseForm.h"
#include "headers/pm_maps.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Variáveis Globais
#define TEMPO 40
#define NCubos 2

struct cubo {
	float theta[3];
	float escala;
	float xCubo;
	float yCubo;
	float speedX, speedY, speedZ;
};

float angleX = 0.0f;
float angleY = 0.0f;
float angleZ = 0.0f;
float radius = 5.0f;
struct cubo cubos[NCubos];
float v = 0.5;
int specialCreeper = 0;

float internalPos = 0.0;
float internalVel = 3.0;

int actualX = 0;
int actualY = 0;

static GLfloat theta[] = {0.0,0.0,0.0};

Player player;

char** map = NULL;

int i = 0, j = 0;


void init() {
	cubos[0].theta[0] = 0.0;
	cubos[0].theta[1] = 0.0;
	cubos[0].theta[2] = 0.0;
	cubos[0].escala = 1.0;
	cubos[0].xCubo = -1.5;
	cubos[0].yCubo = 0.0;
	cubos[0].speedX = 0.5;
	cubos[0].speedY = 0.3;
	cubos[0].speedZ = 0.0;

	cubos[1].theta[0] = 0.0;
	cubos[1].theta[1] = 0.0;
	cubos[1].theta[2] = 0.0;
	cubos[1].escala = 1.0;
	cubos[1].xCubo = 1.5;
	cubos[1].yCubo = 0.0;
	cubos[1].speedX = -0.5;
	cubos[1].speedY = -0.4;
	cubos[1].speedZ = 0.0;
}


void drawTable(char** m)
{
	glPushMatrix();
	int maxSize = xTabSize() > yTabSize() ? xTabSize() : yTabSize();
	glScalef(1.0 / maxSize, 1.0 / maxSize, 1.0 / maxSize);
	glTranslatef(-(xTabSize() - 1), -(yTabSize() - 1), 0.0);

	for (int i = 0; i < xTabSize(); i++)
	{
		for (int j = 0; j < yTabSize(); j++)
		{
			glPushMatrix();
			glTranslatef(i * 2.0, j * 2.0, 0.0);
			glScalef(0.8, 0.8, 0.8);
			if (m[i][j] == 1)
				colorcube();
			glPopMatrix();
		}
	}


	glPushMatrix();
	glRotatef(90, 1.0, 0.0, 0.0);
	drawPlayer(player);
	glPopMatrix();

	glPopMatrix();
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float camX = radius * sin(angleX) * cos(angleY);
	float camY = radius * sin(angleY);
	float camZ = radius * cos(angleX) * cos(angleY);

	glLoadIdentity();
	gluLookAt(camX,camY,camZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	glPushMatrix();
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	drawTable(map);
	glPopMatrix();

	glFlush();
	glutSwapBuffers();
}


void mouse(int btn, int state, int x, int y)
{
	int mod = glutGetModifiers();

	if (mod == GLUT_ACTIVE_CTRL)
	{
		if (btn == 3) radius -= 0.5;

		if (btn == 4) radius += 0.5;
	}
}

void teclado(char key, int x, int y) {
	int aux;
	switch (key) {
		case 's':
		case 'S':
			move(player, 0, 1, map);
			break;
		case 'w':
		case 'W':
			move(player, 0, -1, map);
			break;
		case 'd':
		case 'D':
			move(player, 1, 0, map);
			break;
		case 'a':
		case 'A':
			move(player, -1, 0, map);
			break;
		case 'E': specialCreeper = !specialCreeper; break;
		case 'e': specialCreeper = !specialCreeper; break;
	}
}



void myReshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLfloat aspect = (GLfloat)w / (GLfloat)h;
	gluPerspective(45.0, aspect, 1.0, 20.0);
	glMatrixMode(GL_MODELVIEW);
}

void update(int valor)
{
	internalPos += internalVel * (TEMPO/1000.0);

	if (internalPos >= 1.0)
	{
		internalVel *= -1;
		internalPos = 1.0;
	}

	if (internalPos <= -1.0)
	{
		internalVel *= -1;
		internalPos = -1.0;
	}

	updateAnimation(internalPos);

	glutPostRedisplay();
	glutTimerFunc(TEMPO, update, TEMPO);
}

void readMapFile()
{
	openMapsFile("pm_simple.txt");
}

void mouseDrag(int x, int y)
{
	// Calcular a diferença de posição do mouse
	int deltaX = x - actualX;
	int deltaY = y - actualY;

	if (abs(deltaX) < 5 && abs(deltaY) < 5) return;

	if (x < actualX)
	{
		angleX += 0.1;
	} else
	{
		angleX -= 0.1;
	}

	if (y < actualY)
	{
		angleY = angleY <= 0 ? 0.0 : angleY - 0.1;
	} else
	{
		angleY = angleY >= (M_PI/2.5) ? (M_PI/2.5) : angleY + 0.1;
	}

	actualX = x;
	actualY = y;

}

void mouseWheel()
{

}


void main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(500, 500);
	glutCreateWindow("Pac-man");
	glutReshapeFunc(myReshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(teclado);
	glutTimerFunc(TEMPO, update, TEMPO);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseDrag);

	player = malloc(sizeof(Player));
	player->i = 0;
	player->j = 0;
	// PACMAN - MAP
	// if (argc <= 1) {
	// 	printf("Please run as:\n");
	// 	printf("%s <map file>", argv[0]);
	// 	exit(0);
	// }
	printf("Reading map...\n");
	openMapsFile("pm_simple.txt");       // Open map file
	map = nextMap();             // read the first map from file
	printMap();                  // print the map to the console

	glEnable(GL_DEPTH_TEST);
	init();
	glutMainLoop();
}