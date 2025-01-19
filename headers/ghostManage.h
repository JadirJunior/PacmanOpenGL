//
// Created by jadir on 18/01/2025.
//

#ifndef GHOSTMANAGE_H
#define GHOSTMANAGE_H
#include <gl.h>


typedef struct ghost* Ghost;

struct ghost
{
    int i, j;
    // 1 - esquerda / 2 - direita / 3 - frente / 4 - trás
    int direction;
    int intelligent;
    float color[3];
    float rotateActivation[3];
    GLfloat m[16];

    int moving;
    float speed;

    float fromMovement[2];
    int nextPos[2]; // 0 - i / 1 - j

    float internalRotation;
};



Ghost* initGhosts(char** map);
void printGhosts(const Ghost* ghosts);
void drawGhosts(Ghost* ghosts);
void moveGhost(Ghost ghost, int i, int j, char** map);

#endif //GHOSTMANAGE_H
