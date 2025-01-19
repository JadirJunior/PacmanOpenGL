
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
#include "headers/ghostManage.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Variáveis Globais
#define TEMPO 40


float angleX = 0.0f;
float angleY = 0.0f;
float angleZ = 0.0f;
float radius = 5.0f;
int specialCreeper = 0;

float internalPos = 0.0;
float internalVel = 3.0;

int actualX = 0;
int actualY = 0;


Player player;
Ghost* ghosts;
char** map = NULL;



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
			if (m[i][j] == 1 || m[i][j] == 5)
				colorcube();
			if (m[i][j] == 2 || m[i][j] == 3 || m[i][j] == 4)
				colorForm(0.5, 0.5, 0.5);
			glPopMatrix();
		}
	}

	if (player->isDeath != 1)
	{
		glPushMatrix();
		glRotatef(90, 1.0, 0.0, 0.0);
		drawPlayer(player);
		glPopMatrix();

		if (player->moving == 0)
		{
			map[player->i][abs(player->j)] = 2;
		}
	}

	glPushMatrix();
	glRotatef(90, 1.0, 0.0, 0.0);
	drawGhosts(ghosts);
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
		if (btn == 3) radius = radius <= 2.5 ? 2.5 : radius - 0.5;

		if (btn == 4) radius = radius >= 15.0 ? 15.0 : radius+0.5;
	}
}

void printTeste()
{
	for (int j = yTabSize() - 1; j >= 0; j--) {
		for (int i = 0; i < xTabSize(); i++)
			if (map[i][abs(j)] == 1)
				printf("O ");
			else if (map[i][abs(j)] == 2)
				printf("2 ");
			else
				printf("  ");
		printf("\n");
	}
}

void teclado(char key, int x, int y) {
	int aux;
	switch (key) {
		case 's':
		case 'S':
			//rotate player to the back
			move(player, 0, 1, map);
			break;
		case 'w':
		case 'W':
			//rotate player to front
			move(player, 0, -1, map);
			break;
		case 'd':
		case 'D':
			//rotate player to the right
			move(player, 1, 0, map);
			break;
		case 'a':
		case 'A':
			//rotate player to the left
			move(player, -1, 0, map);
			break;
		case 'E': printTeste(); break;
		case 'e': printTeste(); break;
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

	//Player movement
	if (player->moving)
	{

		for (int i = 0; i < (numGhosts() + numSmartGhosts()); i++)
		{
			if ((player->fromMovement[0] == ghosts[i]->nextPos[0] && player->fromMovement[1] == ghosts[i]->nextPos[1]) ||
				player->nextPos[0] == ghosts[i]->fromMovement[0] && ghosts[i]->fromMovement[1] == player->nextPos[1])
			{
				player->isDeath = 1;
				break;
			}
		}

		if (player->isDeath != 1)
		{
			switch (player->direction)
			{
			case UP:
				if (player->fromMovement[1] <= player->nextPos[1])
				{
					player->j = player->nextPos[1];
					if (map[player->i][abs(player->j)] == 4 || map[player->i][abs(player->j)] == 5) player->isDeath = 1; // Pacman morreu
					player->moving = 0;
				} else player->fromMovement[1] -= player->speed * (TEMPO/1000.0);
				break;
			case DOWN:
				if (player->fromMovement[1] >= player->nextPos[1])
				{
					player->j = player->nextPos[1];
					if (map[player->i][abs(player->j)] == 4 || map[player->i][abs(player->j)] == 5) player->isDeath = 1; // Pacman morreu
					player->moving = 0;
				} else player->fromMovement[1] += player->speed * (TEMPO/1000.0);
				break;
			case LEFT:
				if (player->fromMovement[0] <= player->nextPos[0])
				{
					player->i = player->nextPos[0];
					if (map[player->i][abs(player->j)] == 4 || map[player->i][abs(player->j)] == 5) player->isDeath = 1; // Pacman morreu
					player->moving = 0;
				} else player->fromMovement[0] -= player->speed * (TEMPO/1000.0);
				break;
			case RIGHT:
				if (player->fromMovement[0] >= player->nextPos[0])
				{
					player->i = player->nextPos[0];
					if (map[player->i][abs(player->j)] == 4 || map[player->i][abs(player->j)] == 5) player->isDeath = 1; // Pacman morreu
					player->moving = 0;
				} else player->fromMovement[0] += player->speed * (TEMPO/1000.0);
				break;
			}
		}
	}


	//Ghost movement
	for (int i = 0; i < (numGhosts() + numSmartGhosts()); i++)
	{
		if (ghosts[i]->moving == 0)
		{
			ghosts[i]->direction = (rand()%4) + 1;

			if (ghosts[i]->direction == UP) moveGhost(ghosts[i], 0, -1, map);
			if (ghosts[i]->direction == DOWN) moveGhost(ghosts[i], 0, 1, map);
			if (ghosts[i]->direction == LEFT) moveGhost(ghosts[i], -1, 0, map);
			if (ghosts[i]->direction == RIGHT) moveGhost(ghosts[i], 1, 0, map);
			continue;
		}

		switch (ghosts[i]->direction)
		{
		case UP:
			if (ghosts[i]->fromMovement[1] <= ghosts[i]->nextPos[1])
			{
				ghosts[i]->j = ghosts[i]->nextPos[1];
				if (map[ghosts[i]->i][abs(ghosts[i]->j)] == 2) player->isDeath = 1; // Pacman morreu
				if (map[ghosts[i]->i][abs(ghosts[i]->j)] == 3) map[ghosts[i]->i][abs(ghosts[i]->j)] = 4;
				if (map[ghosts[i]->i][abs(ghosts[i]->j)] == 1) map[ghosts[i]->i][abs(ghosts[i]->j)] = 5;

				ghosts[i]->moving = 0;
				ghosts[i]->internalRotation = 0.0f;
			} else {ghosts[i]->fromMovement[1] -= ghosts[i]->speed * (TEMPO/1000.0); ghosts[i]->internalRotation -= 15.0; }
			break;
		case DOWN:
			if (ghosts[i]->fromMovement[1] >= ghosts[i]->nextPos[1])
			{
				ghosts[i]->j = ghosts[i]->nextPos[1];
				if (map[ghosts[i]->i][abs(ghosts[i]->j)] == 2) player->isDeath = 1; // Pacman morreu
				if (map[ghosts[i]->i][abs(ghosts[i]->j)] == 3) map[ghosts[i]->i][abs(ghosts[i]->j)] = 4;
				if (map[ghosts[i]->i][abs(ghosts[i]->j)] == 1) map[ghosts[i]->i][abs(ghosts[i]->j)] = 5;

				ghosts[i]->moving = 0;
				ghosts[i]->internalRotation = 0.0f;
			} else {ghosts[i]->fromMovement[1] += ghosts[i]->speed * (TEMPO/1000.0); ghosts[i]->internalRotation += 15.0;}
			break;
		case LEFT:
			if (ghosts[i]->fromMovement[0] <= ghosts[i]->nextPos[0])
			{
				ghosts[i]->i = ghosts[i]->nextPos[0];

				if (map[ghosts[i]->i][abs(ghosts[i]->j)] == 2) player->isDeath = 1; // Pacman morreu
				if (map[ghosts[i]->i][abs(ghosts[i]->j)] == 3) map[ghosts[i]->i][abs(ghosts[i]->j)] = 4;
				if (map[ghosts[i]->i][abs(ghosts[i]->j)] == 1) map[ghosts[i]->i][abs(ghosts[i]->j)] = 5;


				ghosts[i]->moving = 0;
				ghosts[i]->internalRotation = 0.0f;
			} else {ghosts[i]->fromMovement[0] -= ghosts[i]->speed * (TEMPO/1000.0); ghosts[i]->internalRotation += 15.0;}
			break;
		case RIGHT:
			if (ghosts[i]->fromMovement[0] >= ghosts[i]->nextPos[0])
			{
				ghosts[i]->i = ghosts[i]->nextPos[0];

				if (map[ghosts[i]->i][abs(ghosts[i]->j)] == 2) player->isDeath = 1; // Pacman morreu
				if (map[ghosts[i]->i][abs(ghosts[i]->j)] == 3) map[ghosts[i]->i][abs(ghosts[i]->j)] = 4;
				if (map[ghosts[i]->i][abs(ghosts[i]->j)] == 1) map[ghosts[i]->i][abs(ghosts[i]->j)] = 5;


				ghosts[i]->moving = 0;
				ghosts[i]->internalRotation = 0.0f;
			} else {ghosts[i]->fromMovement[0] += ghosts[i]->speed * (TEMPO/1000.0); ghosts[i]->internalRotation -= 15.0;}
			break;
		}
	}


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
	srand(time(NULL));
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
	//player->dir = FRONT;
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


	player = initPlayer(map);
	ghosts = initGhosts(map);
	printGhosts(ghosts);


	glEnable(GL_DEPTH_TEST);
	glutMainLoop();
}