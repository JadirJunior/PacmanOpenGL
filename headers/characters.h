//
// Created by jadir on 12/01/2025.
//

#ifndef CHARACTERS_H
#define CHARACTERS_H

typedef struct player* Player;
typedef struct ghost* Ghost;

struct player
{
    int i, j;
    int* m[16];
};

struct ghost
{
    int i, j;
    int intelligent;
    int color[3];
    int* m[16];
};

void drawPlayer(Player player);
void updateAnimation(float newPos);
void move(Player player, int i, int j, char** map);

#endif //CHARACTERS_H
