//
// Created by jadir on 12/01/2025.
//
#include <gl.h>

#include "headers/characters.h"
#include "headers/baseForm.h"

float animation = 0.0;

void updateAnimation(float newPos)
{
	animation = newPos;
}

void desenhaFaceCreeper() {
	glPushMatrix();
	glColor3f(0.0, 0.0, 0.0); // Cor preta para os olhos e boca

	glTranslatef(0.0,0.0,0.5);
	glScalef(2.5, 2.5, 1.0);

	// Olho esquerdo
	glBegin(GL_QUADS);
	glVertex3f(-0.1, 0.1, 0.51); // Frente do cubo, levemente para fora
	glVertex3f(-0.1, 0.25, 0.51);
	glVertex3f(-0.25, 0.25, 0.51);
	glVertex3f(-0.25, 0.1, 0.51);
	glEnd();

	// Olho direito
	glBegin(GL_QUADS);
	glVertex3f(0.1, 0.1, 0.51);
	glVertex3f(0.1, 0.25, 0.51);
	glVertex3f(0.25, 0.25, 0.51);
	glVertex3f(0.25, 0.1, 0.51);
	glEnd();

	//Boca (bigode)
	glBegin(GL_QUADS);
	glVertex3f(-0.1, 0.0, 0.51);
	glVertex3f(-0.1, 0.1, 0.51);
	glVertex3f(0.1, 0.1, 0.51);
	glVertex3f(0.1, 0.0, 0.51);
	glEnd();

	// Boca (parte superior)
	glBegin(GL_QUADS);
	glVertex3f(-0.1, -0.12, 0.51);
	glVertex3f(-0.1, 0.0, 0.51);
	glVertex3f(0.1, 0.0, 0.51);
	glVertex3f(0.1, -0.12, 0.51);
	glEnd();

	// Boca (parte inferior esquerda)
	glBegin(GL_QUADS);
	glVertex3f(-0.17, -0.2, 0.51);
	glVertex3f(-0.17, 0.0, 0.51);
	glVertex3f(-0.1, 0.0, 0.51);
	glVertex3f(-0.1, -0.2, 0.51);
	glEnd();

	// Boca (parte inferior direita)
	glBegin(GL_QUADS);
	glVertex3f(0.1, -0.2, 0.51);
	glVertex3f(0.1, 0.0, 0.51);
	glVertex3f(0.17, 0.0, 0.51);
	glVertex3f(0.17, -0.2, 0.51);
	glEnd();
	glPopMatrix();
}

void desenhaCreeper()
{

	float bodyWidth = 0.8f;
	float bodyHeight = 1.3f;
	float bodyDepth = 0.8f;

	glTranslatef(0.0, 0.5, 0.0);
	glRotatef(1.5*animation, 0.0, 0.0, 1.0);


	//Head
	glPushMatrix();
	glTranslatef(0.0, 0.8, 0.0);
	glRotatef(30*animation, 0.0, 1.0, 0.0);
	//glRotatef(30, 0.0, 1.0, 0.0);
	desenhaFaceCreeper();
	colorForm(0.0, 1.0, 0.0);
	//colorcube();
	glPopMatrix();


	//Body
	glPushMatrix();
	glTranslatef(0.0, -1.5, 0.0);
	glScaled(bodyWidth, bodyHeight, bodyDepth);
	//colorcube();
	colorForm(0.41, 1.35, 0.34);
	glPopMatrix();


	//Pernas Frente Direita
	glPushMatrix();
	glTranslatef(-bodyWidth*0.5f, -2.8 - (bodyHeight*0.2f), bodyDepth);
	glRotatef(35 * animation, 1.0, 0.0, 0.0);
	glScalef(bodyWidth*0.5f, bodyHeight*0.4f, bodyDepth*0.5f);
	// colorcube();
	colorForm(0.09, 0.51, 0.06);
	glPopMatrix();


	//Perna Frente Esquerda
	glPushMatrix();
	glTranslatef(+bodyWidth*0.5f, -2.8 - (bodyHeight*0.2f), bodyDepth);
	glRotatef(-35 * animation, 1.0, 0.0, 0.0);
	glScalef(bodyWidth*0.5f, bodyHeight*0.4f, bodyDepth*0.5f);
	glRotatef(-180.0, 0.0, 1.0, 0.0);
	// colorcube();
	colorForm(0.29, 0.8, 0.06);
	glPopMatrix();


	//Perna Trás Direita
	glPushMatrix();
	glTranslatef(-bodyWidth*0.5f, -2.8 - (bodyHeight*0.2f), -bodyDepth);
	glRotatef(-35 * animation, 1.0, 0.0, 0.0);
	glScalef(bodyWidth*0.5f, bodyHeight*0.4f, bodyDepth*0.5f);
	// colorcube();
	colorForm(0.29, 0.8, 0.06);
	glPopMatrix();

	//Perna Trás Esquerda
	glPushMatrix();
	glTranslatef(+bodyWidth*0.5f, -2.8 - (bodyHeight*0.2f), -bodyDepth);
	glRotatef(35 * animation, 1.0, 0.0, 0.0);
	glScalef(bodyWidth*0.5f, bodyHeight*0.4f, bodyDepth*0.5f);
	glRotatef(-180.0, 0.0, 1.0, 0.0);
	// colorcube();
	colorForm(0.09, 0.51, 0.06);
	glPopMatrix();

	desenhaFaceCreeper();
}


void drawPlayer(float x, float y, float z)
{
	glTranslatef(x, y, z);
	desenhaCreeper();
}
