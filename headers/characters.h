//
// Created by jadir on 12/01/2025.
//

#ifndef CHARACTERS_H
#define CHARACTERS_H

typedef struct player* Player;
typedef struct ghost* Ghost;

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
    int* m[16];
};

struct ghost
{
    int i, j;
    // 1 - esquerda / 2 - direita / 3 - frente / 4 - trás
    int direction;
    int intelligent;
    int color[3];
    int* m[16];
};

void drawPlayer(Player player);
void updateAnimation(float newPos);
void move(Player player, int i, int j, char** map);

#endif //CHARACTERS_H
