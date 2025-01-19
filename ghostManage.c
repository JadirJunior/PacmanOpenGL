//
// Created by jadir on 18/01/2025.
//


#include "headers/ghostManage.h"

#include <gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "headers/baseForm.h"
#include "headers/characters.h"
#include "headers/pm_maps.h"


Ghost createGhost(int intelligent, char** map)
{
    Ghost ghost = (Ghost)malloc(sizeof(struct ghost));

    int i, j;
    do
    {
        i = rand() % xTabSize();
        j = -(rand() % yTabSize());
    } while (map[i][abs(j)] != 1);

    ghost->i = i;
    ghost->j = j;
    ghost->direction = (rand() % 4) + 1;
    ghost->intelligent = intelligent;
    ghost->moving = 0;
    ghost->nextPos[0] = 0;
    ghost->nextPos[1] = 0;
    ghost->speed = 2.0f;
    ghost->internalRotation = 0.0f;


    ghost->color[0] = (float)rand() / (float)RAND_MAX;
    ghost->color[1] = (float)rand() / (float)RAND_MAX;
    ghost->color[2] = (float)rand() / (float)RAND_MAX;

    ghost->rotateActivation[0] = 0.0f;
    ghost->rotateActivation[1] = 0.0f;
    ghost->rotateActivation[2] = 0.0f;

    ghost->fromMovement[0] = ghost->i;
    ghost->fromMovement[1] = ghost->j;


    map[ghost->i][abs(ghost->j)] = 5;

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glGetFloatv(GL_MODELVIEW_MATRIX,ghost->m);
    glPopMatrix();

    return ghost;
}

void rotateGhost(Ghost ghost, GLfloat ang, GLfloat x, GLfloat y, GLfloat z)
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glRotatef(ang,x,y,z);
    //glMultMatrixf(ghost->m);
    glGetFloatv(GL_MODELVIEW_MATRIX,ghost->m);
    glPopMatrix();
}

void drawGhosts(Ghost* ghosts)
{
    for (int i = 0; i < (numGhosts() + numSmartGhosts()); i++)
    {
        glPushMatrix();
        glTranslatef(ghosts[i]->fromMovement[0]*2, 2.0, ghosts[i]->fromMovement[1]*2);
        rotateGhost(ghosts[i],
            ghosts[i]->internalRotation,
            ghosts[i]->rotateActivation[0],
            ghosts[i]->rotateActivation[1],
            ghosts[i]->rotateActivation[2]);

        glScalef(0.75f, 0.75f, 0.75f);
        glMultMatrixf(ghosts[i]->m);
        colorForm(ghosts[i]->color[0], ghosts[i]->color[1], ghosts[i]->color[2]);
        //colorcube();
        glPopMatrix();
    }
}

Ghost* initGhosts(char** map)
{
    Ghost* ghosts = (Ghost*)malloc(sizeof(Ghost) * (numGhosts() + numSmartGhosts()));

    for (int i = 0; i < numGhosts(); i++) {ghosts[i] = createGhost(0, map);}

    for (int i = numGhosts(); i < (numGhosts()+numSmartGhosts()); i++) {ghosts[i] = createGhost(1, map);}

    return ghosts;
}


void moveGhost(Ghost ghost, int i, int j, char** map)
{
    if (ghost->moving == 1) return;

    int aux;
    if (i != 0)
    {
        if (i > 0)
        {
            //Going right
            aux = ghost->i;
            if (aux >= xTabSize()-1) aux = xTabSize()-1;
            else aux++;
            if (map[aux][abs(ghost->j)] != 0)
            {
                if (map[ghost->i][abs(ghost->j)] == 4) map[ghost->i][abs(ghost->j)] = 3;
                if (map[ghost->i][abs(ghost->j)] == 5) map[ghost->i][abs(ghost->j)] = 1;

                ghost->fromMovement[0] = ghost->i;
                ghost->fromMovement[1] = ghost->j;
                ghost->nextPos[0] = aux;
                ghost->moving = 1;

                ghost->internalRotation = 0.0f;
                ghost->rotateActivation[0] = 0.0;
                ghost->rotateActivation[1] = 0.0;
                ghost->rotateActivation[2] = 1.0;
            }
        } else
        {
            //Going Left
            aux = ghost->i;
            if (aux <= 0) aux = 0;
            else aux--;

            if (map[aux][abs(ghost->j)] != 0)
            {
                if (map[ghost->i][abs(ghost->j)] == 4) map[ghost->i][abs(ghost->j)] = 3;
                if (map[ghost->i][abs(ghost->j)] == 5) map[ghost->i][abs(ghost->j)] = 1;


                ghost->fromMovement[0] = ghost->i;
                ghost->fromMovement[1] = ghost->j;
                ghost->nextPos[0] = aux;
                ghost->moving = 1;

                ghost->internalRotation = 0.0f;
                ghost->rotateActivation[0] = 0.0;
                ghost->rotateActivation[1] = 0.0;
                ghost->rotateActivation[2] = 1.0;
            }
        }
    }

    if (j != 0)
    {
        if (j > 0)
        {
            //Going Down
            aux = ghost->j;
            if (aux >= 0) aux = 0;
            else aux++;
            if (map[ghost->i][abs(aux)] != 0)
            {
                if (map[ghost->i][abs(ghost->j)] == 4) map[ghost->i][abs(ghost->j)] = 3;
                if (map[ghost->i][abs(ghost->j)] == 5) map[ghost->i][abs(ghost->j)] = 1;

                ghost->fromMovement[0] = ghost->i;
                ghost->fromMovement[1] = ghost->j;
                ghost->nextPos[1] = aux;
                ghost->moving = 1;

                ghost->internalRotation = 0.0f;
                ghost->rotateActivation[0] = 1.0;
                ghost->rotateActivation[1] = 0.0;
                ghost->rotateActivation[2] = 0.0;
            }
        } else
        {
            //Going up
            aux =  ghost->j;
            if (aux <= -yTabSize()+1) aux = -yTabSize()+1;
            else aux--;
            if (map[ghost->i][abs(aux)] != 0)
            {
                if (map[ghost->i][abs(ghost->j)] == 4) map[ghost->i][abs(ghost->j)] = 3;
                if (map[ghost->i][abs(ghost->j)] == 5) map[ghost->i][abs(ghost->j)] = 1;

                ghost->fromMovement[0] = ghost->i;
                ghost->fromMovement[1] = ghost->j;
                ghost->nextPos[1] = aux;
                ghost->moving = 1;

                ghost->internalRotation = 0.0f;
                ghost->rotateActivation[0] = 1.0;
                ghost->rotateActivation[1] = 0.0;
                ghost->rotateActivation[2] = 0.0;
            }
        }
    }
}

void printGhosts(const Ghost* ghosts)
{
    for (int i = 0; i < (numGhosts() + numSmartGhosts()); i++)
    {
        printf("Ghost %d\n", i);
        printf("i: %d\n", ghosts[i]->i);
        printf("j: %d\n", ghosts[i]->j);
        printf("direction: %d\n", ghosts[i]->direction);
        printf("intelligent: %d\n", ghosts[i]->intelligent);
        printf("moving: %d\n", ghosts[i]->moving);
        printf("nextPos[0]: %d\n", ghosts[i]->nextPos[0]);
        printf("nextPos[1]: %d\n", ghosts[i]->nextPos[1]);
        printf("fromMovement[0]: %f\n", ghosts[i]->fromMovement[0]);
        printf("fromMovement[1]: %f\n", ghosts[i]->fromMovement[1]);
        printf("Speed: %f\n", ghosts[i]->speed);
        printf("Color [R, G, B][%f, %f, %f]\n", ghosts[i]->color[0], ghosts[i]->color[1], ghosts[i]->color[2]);
        printf("\n");
    }
}
