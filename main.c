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


#define MENU_PRINCIPAL 0
#define SUBMENU_MAPAS 1

int menuState = MENU_PRINCIPAL; // Define o estado atual do menu
int selectedOption = -1;        // Nenhuma opção está selecionada inicialmente

// Estrutura para definir os "cards" de cada item do menu
typedef struct {
    float x, y, width, height;
    int mapNumber;
    char** map;
    int xSize, ySize;
} Card;

int windowWidth = 500;
int windowHeight = 500;

Card menuPrincipalCards[3] = {
    {0, 0, 400, 60, -1, NULL, 0, 0}, // "Novo Jogo"
    {0, 50, 400, 60, -1, NULL, 0, 0}, // "Selecionar Mapa"
    {0, 100, 400, 60, -1, NULL, 0, 0}  // "Sair"
};

Card* cardsMaps = NULL;

float angleX = 0.0f;
float angleY = 30.0f * (M_PI / 180.0f);
float angleZ = 0.0f;
float radius = 2.5f;
int specialCreeper = 0;

float internalPos = 0.0;
float internalVel = 3.0;

int actualX = 0;
int actualY = 0;

int remainingSlots = -1;

Player player;
Ghost* ghosts;
char** map = NULL;

char* arg = NULL;


//Game infos
int gameStarted = 0;
int restarting = 0;
int paused = 0;
int level = 1;
int score = 0;
int gameOver = 0;
int mapNumber = 0;

void drawRectangle(Card card, int isSelected) {
    if (isSelected) {
        glColor3f(0.2, 0.6, 0.8); // Cor azul claro para a opção selecionada
    }
    else {
        glColor3f(0.3, 0.3, 0.3); // Cor cinza para os outros
    }

    glBegin(GL_QUADS);
    glVertex2f(card.x, card.y);
    glVertex2f(card.x + card.width, card.y);
    glVertex2f(card.x + card.width, card.y + card.height);
    glVertex2f(card.x, card.y + card.height);
    glEnd();
}

void updateCardPositions(int width, int height) {
    float cardWidth = width * 0.5;  // Metade da largura da tela
    float cardHeight = height * 0.1;  // Proporção da altura (10% da tela por card)
    float startX = (width - cardWidth) / 2;  // Centraliza horizontalmente
    float startY = (height - (cardHeight * 3 + 20 * 2)) / 2;  // Centraliza verticalmente com espaçamento

    for (int i = 0; i < 3; i++) {
        menuPrincipalCards[i].x = startX;
        menuPrincipalCards[i].y = startY + i * (cardHeight + 20);  // Espaçamento vertical de 20px entre os cards
        menuPrincipalCards[i].width = cardWidth;
        menuPrincipalCards[i].height = cardHeight;
    }
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
            if (m[i][j] == 1 || m[i][j] == 5)
                colorcube();
            if (m[i][j] == 2 || m[i][j] == 3 || m[i][j] == 4)
                colorForm(0.5, 0.5, 0.5);
            glPopMatrix();
        }
    }

    glPushMatrix();
    glRotatef(90, 1.0, 0.0, 0.0);
    drawPlayer(player);
    glPopMatrix();

    if (player->moving == 0)
    {
        map[player->i][abs(player->j)] = 2;
    }


    glPushMatrix();
    glRotatef(90, 1.0, 0.0, 0.0);
    drawGhosts(ghosts);
    glPopMatrix();

    glPopMatrix();
}

void drawTableImage(Card card) {
    glPushMatrix();

    // Translação inicial para posicionar o tabuleiro
    glTranslatef(card.x, card.y, 0.0);

    // Escala para ajustar o tamanho do tabuleiro
    float scaleX = card.width / (float)card.xSize;
    float scaleY = card.height / (float)card.ySize;
    glScalef(scaleX, scaleY, 1.0);

    // Desenha as células do tabuleiro
    for (int i = 0; i < card.xSize; i++) {
        for (int j = 0; j < card.ySize; j++) {
            glPushMatrix();
            glTranslatef(i, j, 0.0);

            if (card.map[i][j] == 1) {
                glBegin(GL_QUADS);
                glColor3f(0.0, 1.0, 0.0); // Cor verde para células ativas
                glVertex2f(-0.5, -0.5);
                glVertex2f(0.5, -0.5);
                glVertex2f(0.5, 0.5);
                glVertex2f(-0.5, 0.5);
                glEnd();
            }

            glPopMatrix();
        }
    }

    glPopMatrix();
}

void loadAllMaps()
{
    printf("Reading map...\n");
    openMapsFile(arg); // Open map file
    int i = 0;

    cardsMaps = (Card*)malloc(sizeof(Card) * numberOfMaps());


    char** m = NULL;

    m = nextMap();

    int gap = windowWidth / (numberOfMaps() + 1);   // Espaçamento entre os cards
    int mapWidth = gap * 0.8;

    if (mapWidth > 400) mapWidth = 400;

    while (m != NULL)
    {
        cardsMaps[i].map = m;
        cardsMaps[i].x = gap *(i + 1) - mapWidth / 2;
        cardsMaps[i].y = windowHeight / 2 - 30;
        cardsMaps[i].width = mapWidth;
        cardsMaps[i].height = 200;
        cardsMaps[i].xSize = xTabSize();
        cardsMaps[i].ySize = yTabSize();
        cardsMaps[i].mapNumber = i;
        i++;

        m = nextMap();
    }

}

void resetMap()
{
    if (map != NULL)
    {
        restarting = 1;
        map = NULL;
        free(map);
        closeMapsFile();
    }

    printf("Reading map...\n");
    openMapsFile(arg); // Open map file
    int i = 0;
    for (int i = 0; i <= mapNumber; i++)
    {
        map = nextMap();
    }

    printMap(); // print the map to the console

    remainingSlots = 0;
    for (int i = 0; i < xTabSize(); i++)
    {
        for (int j = 0; j < yTabSize(); j++)
        {
            if (map[i][j] == 1) remainingSlots++;
        }
    }
}

void killPlayer()
{
    player->isDeath = 1;
    gameOver = 1;
    paused = 1;
}

void resetGame(int resetScore)
{
    if (resetScore)
    {
        score = 0;
        level = 1;
    } else
    {
        level++;
    }

    resetMap();

    if (player != NULL)
    {
        player = NULL;
        free(player);
    }

    if (ghosts != NULL)
    {
        for (int i = 0; i < (numGhosts() + numSmartGhosts()); i++)
        {
            if (ghosts[i] != NULL)
            {
                ghosts[i] = NULL;
                free(ghosts[i]);
            }
        }

        ghosts = NULL;
        free(ghosts);
    }

    internalPos = 0.0;
    internalVel = 3.0;
    angleX = 0.0f;
    angleY = 30.0f * (M_PI / 180.0f);
    angleZ = 0.0f;
    radius = 2.5f;
    actualX = 0;
    actualY = 0;
    paused = 0;
    gameOver = 0;
    gameStarted = 1;


    player = initPlayer(map);
    ghosts = initGhosts(map);

    remainingSlots--;
    restarting = 0;
}

void renderBitmapString(float x, float y, void* font, const char* string) {
    glRasterPos2f(x, y); // Define a posição do texto
    while (*string) {
        glutBitmapCharacter(font, *string); // Exibe cada caractere
        string++;
    }
}

void drawMenuText(float x, float y, void* font, const char* string)
{
    glColor3f(1.0f, 1.0f, 1.0f);
    glDisable(GL_DEPTH_TEST);
    glRasterPos2f(x, y); // Define a posição do texto
    while (*string) {
        glutBitmapCharacter(font, *string); // Exibe cada caractere
        string++;
    }
    glEnable(GL_DEPTH_TEST);
}

int calculateStringWidth(void* font, char* string) {
    int width = 0;
    for (char* c = string; *c != '\0'; c++) {
        width += glutBitmapWidth(font, *c);
    }
    return width;
}

void showData()
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    // Configura projeção ortogonal com base no tamanho da janela
    glOrtho(0, windowWidth, 0, windowHeight, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Cor do texto
    glColor3f(1.0, 1.0, 1.0);

    // Desenha texto em pixels (canto superior esquerdo)
    char playerScore[15];
    sprintf(playerScore, "Score: %d", score);

    char levelText[15];
    sprintf(levelText, "Level: %d", level);

    renderBitmapString(10, windowHeight - 20, GLUT_BITMAP_HELVETICA_18, playerScore);
    renderBitmapString(10, windowHeight - 40, GLUT_BITMAP_HELVETICA_18, levelText);
    if (gameOver == 1)
    {
        char message[] = "Voce perdeu o jogo! Pressione 'r' para reiniciar ou 'q' para sair.";
        int offSet = calculateStringWidth(GLUT_BITMAP_HELVETICA_18, message);

        renderBitmapString((windowWidth - offSet)/2, windowHeight - 20, GLUT_BITMAP_HELVETICA_18, message);
    } else if (gameOver == 2)
    {
        char message[] = "Voce venceu a rodada! Pressione 'r' para reiniciar, 'c' para a próxima fase ou 'q' para sair";
        int offSet = calculateStringWidth(GLUT_BITMAP_HELVETICA_18, message);

        renderBitmapString((windowWidth - offSet)/2, windowHeight - 20, GLUT_BITMAP_HELVETICA_18, message);
    } else if (paused == 1)
    {
        char message[] = "Jogo Pausado! Pressione ESC para continuar ou 'q' para sair";

        int offSet = calculateStringWidth(GLUT_BITMAP_HELVETICA_18, message);

        renderBitmapString((windowWidth - offSet)/2, windowHeight - 20, GLUT_BITMAP_HELVETICA_18, message);
    }


    // Restaura o estado anterior
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    if (gameStarted == 0)
    {
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();


        glOrtho(0, windowWidth, 0, windowHeight, -1, 1);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        Card* cards = (menuState == MENU_PRINCIPAL) ? menuPrincipalCards : cardsMaps;

        const char* texts[3] = { 0 };

        if (menuState == MENU_PRINCIPAL) {
            texts[2] = "Novo Jogo";
            texts[1] = "Selecionar Mapa";
            texts[0] = "Sair";
        }
        else if (cardsMaps == NULL) loadAllMaps();

        updateCardPositions(windowWidth, windowHeight);

        if (menuState == MENU_PRINCIPAL)
        {
            // Renderiza os cards e os textos
            for (int i = 0; i < 3; i++) {
                drawRectangle(cards[i], selectedOption == i);
                drawMenuText(cards[i].x + 20, cards[i].y + 35,  GLUT_BITMAP_HELVETICA_18, texts[i]);
            }
        } else
        {
            for (int i = 0; i < numberOfMaps(); i++) {
                //drawRectangle(cardsMaps[i], 1);
                drawTableImage(cardsMaps[i]);
            }
        }


        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
    } else
    {
        float camX = radius * sin(angleX) * cos(angleY);
        float camY = radius * sin(angleY);
        float camZ = radius * cos(angleX) * cos(angleY);

        glLoadIdentity();
        gluLookAt(camX, camY, camZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

        glPushMatrix();
        glRotatef(-90.0, 1.0, 0.0, 0.0);
        drawTable(map);
        glPopMatrix();


        showData();
    }

    glFlush();
    glutSwapBuffers();
}


void mouse(int btn, int state, int x, int y)
{
    if (gameStarted == 0)
    {
        if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        {
            y = windowHeight - y;

            Card* cards = (menuState == MENU_PRINCIPAL) ? menuPrincipalCards : cardsMaps;

            if (menuState == MENU_PRINCIPAL)
            {
                for (int i = 0; i < 3; i++) {

                    if (x >= cards[i].x && x <= cards[i].x + cards[i].width &&
                        y >= cards[i].y && y <= cards[i].y + cards[i].height) {

                        // Lógica para cada botão
                        if (i == 0) {
                            exit(0);
                        }
                        if (i == 1) {
                            menuState = SUBMENU_MAPAS;
                            selectedOption = -1;
                        }
                        else if (i == 2) {
                            if (map != NULL)
                            {

                                glViewport(0, 0, windowWidth, windowHeight);
                                glMatrixMode(GL_PROJECTION);
                                glLoadIdentity();
                                GLfloat aspect = (GLfloat)windowWidth / (GLfloat)windowHeight;
                                gluPerspective(45.0, aspect, 1.0, 20.0);
                                glMatrixMode(GL_MODELVIEW);

                                closeMapsFile();
                                resetGame(1);
                                printf("Game started!\n");
                                break;
                            }
                        }

                        glutPostRedisplay();
                    }
                }
            } else
            {
                for (int i = 0; i < numberOfMaps(); i++)
                {
                    if (x >= cardsMaps[i].x && x <= cardsMaps[i].x + cardsMaps[i].width &&
                        y >= cardsMaps[i].y && y <= cardsMaps[i].y + cardsMaps[i].height)
                    {
                        map = cardsMaps[i].map;
                        mapNumber = cardsMaps[i].mapNumber;
                        menuState = MENU_PRINCIPAL;
                        break;
                    }
                }
            }
        }
    } else
    {
        int mod = glutGetModifiers();

        if (mod == GLUT_ACTIVE_CTRL)
        {
            if (btn == 3) radius = radius <= 2.5 ? 2.5 : radius - 0.5;

            if (btn == 4) radius = radius >= 15.0 ? 15.0 : radius + 0.5;
        }
    }
}

void camDirection(int* wDir, int* sDir, int* dDir, int* aDir)
{
    int angleDegrees = (int)(angleX * 180.0 / M_PI);
    angleDegrees = (angleDegrees % 360 + 360) % 360;

    if ((angleDegrees >= 0 && angleDegrees <= 45))
    {
        wDir[0] = 0;
        wDir[1] = -1;

        sDir[0] = 0;
        sDir[1] = 1;

        dDir[0] = 1;
        dDir[1] = 0;

        aDir[0] = -1;
        aDir[1] = 0;
    } else if (angleDegrees >= 45 && angleDegrees <= 135)
    {
        wDir[0] = -1;
        wDir[1] = 0;

        sDir[0] = 1;
        sDir[1] = 0;

        dDir[0] = 0;
        dDir[1] = -1;

        aDir[0] = 0;
        aDir[1] = 1;
    } else if (angleDegrees >= 135 && angleDegrees <= 225)
    {
        wDir[0] = 0;
        wDir[1] = 1;

        sDir[0] = 0;
        sDir[1] = -1;

        dDir[0] = -1;
        dDir[1] = 0;

        aDir[0] = 1;
        aDir[1] = 0;
    } else if (angleDegrees >= 225 && angleDegrees <= 315)
    {
        wDir[0] = 1;
        wDir[1] = 0;

        sDir[0] = -1;
        sDir[1] = 0;

        dDir[0] = 0;
        dDir[1] = 1;

        aDir[0] = 0;
        aDir[1] = -1;
    } else
    {
        wDir[0] = 0;
        wDir[1] = -1;

        sDir[0] = 0;
        sDir[1] = 1;

        dDir[0] = 1;
        dDir[1] = 0;

        aDir[0] = -1;
        aDir[1] = 0;
    }
}

void teclado(char key, int x, int y)
{

    if (gameStarted == 0) return;

    int wDir[2] = {0, -1};
    int sDir[2] = {0, 1};
    int dDir[2] = {1, 0};
    int aDir[2] = {-1, 0};

    camDirection(wDir, sDir, dDir, aDir);

    switch (key)
    {
    case 's':
    case 'S':
        if (paused == 1) return;
        //rotate player to the back
        move(player, sDir[0], sDir[1], map);
        break;
    case 'w':
    case 'W':
        if (paused == 1) return;
        //rotate player to front
        move(player, wDir[0], wDir[1], map);
        break;
    case 'd':
    case 'D':
        if (paused == 1) return;
        //rotate player to the right
        move(player, dDir[0], dDir[1], map);
        break;
    case 'a':
    case 'A':
        if (paused == 1) return;
        //rotate player to the left
        move(player, aDir[0], aDir[1], map);
        break;
    case 'r': if (gameOver == 1 || gameOver == 2) resetGame(1);
        break;
    case 'c': if (gameOver == 2) resetGame(0);
        break;
    case 27: if (gameOver == 0) paused = paused == 0 ? 1 : 0;
        break;
    case 'q':
        if (gameStarted == 1)
        {
            glViewport(0, 0, windowWidth, windowHeight);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluOrtho2D(0.0, windowWidth, 0.0, windowHeight);
            glMatrixMode(GL_MODELVIEW);
            gameStarted = 0;



            free(cardsMaps);
            cardsMaps = NULL;

            closeMapsFile();
            mapNumber = -1;


        }
        break;
    }
}

void myReshape(int w, int h)
{
    if (gameStarted == 0)
    {
        glViewport(0, 0, w, h);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0.0, w, 0.0, h);
        glMatrixMode(GL_MODELVIEW);

        windowWidth = w;
        windowHeight = h;
    } else
    {
        glViewport(0, 0, w, h);
        windowWidth = w;
        windowWidth = h;
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        GLfloat aspect = (GLfloat)w / (GLfloat)h;
        gluPerspective(45.0, aspect, 1.0, 20.0);
        glMatrixMode(GL_MODELVIEW);
    }
}

void update(int valor)
{


    if (restarting == 0 && paused == 0 && gameOver == 0 && gameStarted == 1)
    {
        internalPos += internalVel * (TEMPO / 1000.0);

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
                if ((player->fromMovement[0] == ghosts[i]->nextPos[0] && player->fromMovement[1] == ghosts[i]->nextPos[
                        1]) ||
                    player->nextPos[0] == ghosts[i]->fromMovement[0] && ghosts[i]->fromMovement[1] == player->nextPos[
                        1])
                {
                    killPlayer();
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
                        if (map[player->i][abs(player->j)] == 4 || map[player->i][abs(player->j)] == 5) {
                            killPlayer();
                        } else if (map[player->i][abs(player->j)] == 1)
                        {
                            score++;
                            map[player->i][abs(player->j)] = 2;
                            remainingSlots--;
                        }

                        if (remainingSlots == 0)
                        {
                            gameOver = 2;
                        }
                        player->moving = 0;
                    }
                    else player->fromMovement[1] -= player->speed * (TEMPO / 1000.0);
                    break;
                case DOWN:
                    if (player->fromMovement[1] >= player->nextPos[1])
                    {
                        player->j = player->nextPos[1];
                        if (map[player->i][abs(player->j)] == 4 || map[player->i][abs(player->j)] == 5)
                        {
                            killPlayer();
                        }
                        else if (map[player->i][abs(player->j)] == 1)
                        {
                            score++;
                            map[player->i][abs(player->j)] = 2;
                            remainingSlots--;
                        }

                        if (remainingSlots == 0)
                        {
                            gameOver = 2;
                        }

                        player->moving = 0;
                    }
                    else player->fromMovement[1] += player->speed * (TEMPO / 1000.0);
                    break;
                case LEFT:
                    if (player->fromMovement[0] <= player->nextPos[0])
                    {
                        player->i = player->nextPos[0];
                        if (map[player->i][abs(player->j)] == 4 || map[player->i][abs(player->j)] == 5)
                        {
                            killPlayer();
                        }
                        else if (map[player->i][abs(player->j)] == 1)
                        {
                            score++;
                            map[player->i][abs(player->j)] = 2;
                            remainingSlots--;
                        }

                        if (remainingSlots == 0)
                        {
                            gameOver = 2;
                        }
                        player->moving = 0;
                    }
                    else player->fromMovement[0] -= player->speed * (TEMPO / 1000.0);
                    break;
                case RIGHT:
                    if (player->fromMovement[0] >= player->nextPos[0])
                    {
                        player->i = player->nextPos[0];
                        if (map[player->i][abs(player->j)] == 4 || map[player->i][abs(player->j)] == 5)
                        {
                            killPlayer();
                        }
                        else if (map[player->i][abs(player->j)] == 1)
                        {
                            score++;
                            map[player->i][abs(player->j)] = 2;
                            remainingSlots--;
                        }

                        if (remainingSlots == 0)
                        {
                            gameOver = 2;
                        }
                        player->moving = 0;
                    }
                    else player->fromMovement[0] += player->speed * (TEMPO / 1000.0);
                    break;
                }
            }
        }


        //Ghost movement
        for (int i = 0; i < (numGhosts() + numSmartGhosts()); i++)
        {
            if (ghosts[i]->moving == 0)
            {
                ghosts[i]->direction = (rand() % 4) + 1;

                if (ghosts[i]->intelligent == 1)
                {
                    if (player->i < ghosts[i]->i && map[ghosts[i]->i - 1][abs(ghosts[i]->j)] != 0)
                    {
                        ghosts[i]->direction = LEFT;
                    } else if (player->i > ghosts[i]->i && map[ghosts[i]->i + 1][abs(ghosts[i]->j)] != 0)
                    {
                        ghosts[i]->direction = RIGHT;
                    } else if (player->j < ghosts[i]->j && map[ghosts[i]->i][abs(ghosts[i]->j) - 1] != 0)
                    {
                        ghosts[i]->direction = UP;
                    } else if (player->j > ghosts[i]->j && map[ghosts[i]->i][abs(ghosts[i]->j) + 1] != 0)
                    {
                        ghosts[i]->direction = DOWN;
                    } else
                    {
                        ghosts[i]->direction = (rand() % 4) + 1;
                    }
                }

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
                    if (map[ghosts[i]->i][abs(ghosts[i]->j)] == 2)
                    {
                        killPlayer();
                    } // Pacman morreu
                    if (map[ghosts[i]->i][abs(ghosts[i]->j)] == 3) map[ghosts[i]->i][abs(ghosts[i]->j)] = 4;
                    if (map[ghosts[i]->i][abs(ghosts[i]->j)] == 1) map[ghosts[i]->i][abs(ghosts[i]->j)] = 5;

                    ghosts[i]->moving = 0;
                    ghosts[i]->internalRotation = 0.0f;
                }
                else
                {
                    ghosts[i]->fromMovement[1] -= ghosts[i]->speed * (TEMPO / 1000.0);
                    if (ghosts[i]->intelligent == 1) ghosts[i]->internalRotation -= 15.0;
                }
                break;
            case DOWN:
                if (ghosts[i]->fromMovement[1] >= ghosts[i]->nextPos[1])
                {
                    ghosts[i]->j = ghosts[i]->nextPos[1];
                    if (map[ghosts[i]->i][abs(ghosts[i]->j)] == 2)
                    {
                        killPlayer();
                    }
                    if (map[ghosts[i]->i][abs(ghosts[i]->j)] == 3) map[ghosts[i]->i][abs(ghosts[i]->j)] = 4;
                    if (map[ghosts[i]->i][abs(ghosts[i]->j)] == 1) map[ghosts[i]->i][abs(ghosts[i]->j)] = 5;

                    ghosts[i]->moving = 0;
                    ghosts[i]->internalRotation = 0.0f;
                }
                else
                {
                    ghosts[i]->fromMovement[1] += ghosts[i]->speed * (TEMPO / 1000.0);
                    if (ghosts[i]->intelligent == 1) ghosts[i]->internalRotation += 15.0;
                }
                break;
            case LEFT:
                if (ghosts[i]->fromMovement[0] <= ghosts[i]->nextPos[0])
                {
                    ghosts[i]->i = ghosts[i]->nextPos[0];

                    if (map[ghosts[i]->i][abs(ghosts[i]->j)] == 2)
                    {
                        killPlayer();
                    }// Pacman morreu
                    if (map[ghosts[i]->i][abs(ghosts[i]->j)] == 3) map[ghosts[i]->i][abs(ghosts[i]->j)] = 4;
                    if (map[ghosts[i]->i][abs(ghosts[i]->j)] == 1) map[ghosts[i]->i][abs(ghosts[i]->j)] = 5;


                    ghosts[i]->moving = 0;
                    ghosts[i]->internalRotation = 0.0f;
                }
                else
                {
                    ghosts[i]->fromMovement[0] -= ghosts[i]->speed * (TEMPO / 1000.0);
                    if (ghosts[i]->intelligent == 1) ghosts[i]->internalRotation += 15.0;
                }
                break;
            case RIGHT:
                if (ghosts[i]->fromMovement[0] >= ghosts[i]->nextPos[0])
                {
                    ghosts[i]->i = ghosts[i]->nextPos[0];

                    if (map[ghosts[i]->i][abs(ghosts[i]->j)] == 2)
                    {
                        killPlayer();
                    }// Pacman morreu
                    if (map[ghosts[i]->i][abs(ghosts[i]->j)] == 3) map[ghosts[i]->i][abs(ghosts[i]->j)] = 4;
                    if (map[ghosts[i]->i][abs(ghosts[i]->j)] == 1) map[ghosts[i]->i][abs(ghosts[i]->j)] = 5;


                    ghosts[i]->moving = 0;
                    ghosts[i]->internalRotation = 0.0f;
                }
                else
                {
                    ghosts[i]->fromMovement[0] += ghosts[i]->speed * (TEMPO / 1000.0);
                    if (ghosts[i]->intelligent == 1) ghosts[i]->internalRotation -= 15.0;
                }
                break;
            }
        }
    }


    glutPostRedisplay();
    glutTimerFunc(TEMPO, update, TEMPO);
}

void mouseDrag(int x, int y)
{

    if (gameStarted == 0) return;

    // Calcular a diferença de posição do mouse
    int deltaX = x - actualX;
    int deltaY = y - actualY;

    if (abs(deltaX) < 5 && abs(deltaY) < 5) return;

    if (x < actualX)
    {
        angleX = angleX + 0.1;
    }
    else
    {
        angleX = angleX - 0.1;
    }

    if (y < actualY)
    {
        angleY = angleY <= 0 ? 0.0 : angleY - 0.1;
    }
    else
    {
        angleY = angleY >= (M_PI / 2.5) ? (M_PI / 2.5) : angleY + 0.1;
    }

    actualX = x;
    actualY = y;
}

void motion(int x, int y) {
    // Inverter a coordenada Y para alinhar com o sistema gráfico
    y = windowHeight - y;

    // Cards para o menu principal e submenu

    if (menuState == MENU_PRINCIPAL)
    {
        Card* cards = menuPrincipalCards;
        selectedOption = -1; // Resetar seleção

        for (int i = 0; i < 3; i++) {
            if (x >= cards[i].x && x <= cards[i].x + cards[i].width &&
                y >= cards[i].y && y <= cards[i].y + cards[i].height) {
                selectedOption = i;
                break;
            }
        }

        glutPostRedisplay();
    }


}

void main(int argc, char** argv)
{
    srand(time(NULL));
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Pac-man");
    glutFullScreen();
    glutReshapeFunc(myReshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(teclado);
    glutTimerFunc(TEMPO, update, TEMPO);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseDrag);
    glutPassiveMotionFunc(motion);


    // PACMAN - MAP
    if (argc <= 1) {
    	printf("Please run as:\n");
    	printf("%s <map file>", argv[0]);
    	exit(0);
    }

    arg = (char*) malloc( sizeof(strlen(argv[1]) + 1) );
    arg = argv[1];

    //resetGame(1);

    glEnable(GL_DEPTH_TEST);
    glutMainLoop();
}
