//
// Created by jadir on 12/01/2025.
//

#ifndef CHARACTERS_H
#define CHARACTERS_H

typedef struct player* Player;

#define LEFT 1
#define RIGHT 2
#define UP 3
#define DOWN 4

struct player
{
    int i, j;
    // 1 - esquerda / 2 - direita / 3 - frente / 4 - trás
    //enum direction dir;
    int direction;
    GLfloat m[16];
    int moving;
    float speed;
    int isDeath;

    float fromMovement[2];
    int nextPos[2]; // 0 - i / 1 - j
};

void drawPlayer(Player player);
void updateAnimation(float newPos);
void move(Player player, int i, int j, char** map);
Player initPlayer(char** map);

#endif //CHARACTERS_H
