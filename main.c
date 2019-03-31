#define fieldWidth 12
#define fieldHight 20
#define timeToSlideMax 10
#define speed 10
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
#include <stdio.h>
#include <time.h>
#include "constants.h"
//TODO odvoj u constants.h
#include "drawTetris.h"
#include "simpleToolFunctions.h"
/* Fleg koji odredjuje stanje tajmera. */
static int timer_active;

/* Deklaracije callback funkcija. */

tetrisPiece trenutnoPadajuci;
static void on_keyboard(unsigned char key, int x, int y);
static void on_reshape(int width, int height);
static void on_timer(int value);
static void on_display(void);
static void SpecialInput(int key, int x, int y);
int fieldMatrix[fieldWidth][fieldHight] = {};              //matrix which will hold colors of the tetris fields
int fieldMatrixWall[fieldWidth + 2][fieldHight + 1] = {0}; //matrix of 1s and 0s which keeps track of where new pieces can't fall
int timeToSlideUsed = 0;
int keyIsPressed = 0;
int moveDown = 0;
tetrisPiece rotatePiece(tetrisPiece toBeRotated)
{
    tetrisPiece returnMe = toBeRotated;
    rotateMatrix(toBeRotated.blockMatrix, 1, returnMe.blockMatrix);
    return returnMe;
}
tetrisPiece newTetrisPiece()
{
    tetrisPiece toBeReturned = {(int)rand() % 7 + 1, 0, fieldWidth / 2 - 1, fieldHight};
    int i, j;
    switch (toBeReturned.type)
    {
    case none:
        break;
    case O:
    {
        int nonRotatedMatrix[4][4] = {{0, 0, 0, 0},
                                      {0, 1, 1, 0},
                                      {0, 1, 1, 0},
                                      {0, 0, 0, 0}};
        for (i = 0; i < 4; i++)
            for (j = 0; j < 4; j++)
                toBeReturned.blockMatrix[i][j] = nonRotatedMatrix[i][j];
        break;
    }
    case L:
    {
        int nonRotatedMatrix[4][4] = {{0, 1, 0, 0},
                                      {0, 1, 0, 0},
                                      {0, 1, 1, 0},
                                      {0, 0, 0, 0}};
        for (i = 0; i < 4; i++)
            for (j = 0; j < 4; j++)
                toBeReturned.blockMatrix[i][j] = nonRotatedMatrix[i][j];
        break;
    }
    case Z:
    {
        int nonRotatedMatrix[4][4] = {{0, 0, 1, 0},
                                      {0, 1, 1, 0},
                                      {0, 1, 0, 0},
                                      {0, 0, 0, 0}};
        for (i = 0; i < 4; i++)
            for (j = 0; j < 4; j++)
                toBeReturned.blockMatrix[i][j] = nonRotatedMatrix[i][j];
        break;
    }
    case T:
    {
        int nonRotatedMatrix[4][4] = {{0, 1, 0, 0},
                                      {1, 1, 1, 0},
                                      {0, 0, 0, 0},
                                      {0, 0, 0, 0}};
        for (i = 0; i < 4; i++)
            for (j = 0; j < 4; j++)
                toBeReturned.blockMatrix[i][j] = nonRotatedMatrix[i][j];
        break;
    }
    case I:
    {
        int nonRotatedMatrix[4][4] = {{0, 1, 0, 0},
                                      {0, 1, 0, 0},
                                      {0, 1, 0, 0},
                                      {0, 1, 0, 0}};
        for (i = 0; i < 4; i++)
            for (j = 0; j < 4; j++)
                toBeReturned.blockMatrix[i][j] = nonRotatedMatrix[i][j];
        break;
    }
    case J:
    {
        int nonRotatedMatrix[4][4] = {{0, 1, 0, 0},
                                      {0, 1, 0, 0},
                                      {1, 1, 0, 0},
                                      {0, 0, 0, 0}};
        for (i = 0; i < 4; i++)
            for (j = 0; j < 4; j++)
                toBeReturned.blockMatrix[i][j] = nonRotatedMatrix[i][j];
        break;
    }
    case S:
    {
        int nonRotatedMatrix[4][4] = {{0, 0, 0, 0},
                                      {0, 1, 1, 0},
                                      {1, 1, 0, 0},
                                      {0, 0, 0, 0}};
        for (i = 0; i < 4; i++)
            for (j = 0; j < 4; j++)
                toBeReturned.blockMatrix[i][j] = nonRotatedMatrix[i][j];
        break;
    }
    }
    return toBeReturned;
}
void insertPieceIntoField(tetrisPiece piece, int matrix[fieldWidth][fieldHight])
{
    int i, j;
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            if (piece.blockMatrix[j][i])
                matrix[piece.xPosition + i - 1][piece.yPosition - j - 1] = piece.blockMatrix[j][i] * piece.type;
}
void insertPieceIntoWall(tetrisPiece piece, int wall[fieldWidth + 2][fieldHight + 1])
{
    int i, j;
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            if (piece.blockMatrix[j][i])
                wall[piece.xPosition + i][piece.yPosition - j] = 1; //BUG
}

void takeOutTetrisPiece(tetrisPiece piece, int matrix[fieldWidth][fieldHight])
{
    int i, j;
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            if (piece.blockMatrix[j][i])
                matrix[piece.xPosition + i - 1][piece.yPosition - j - 1] = 0;
}
int doesThePieceHitTheWall(tetrisPiece piece, int wall[fieldWidth + 2][fieldHight + 1])
{
    int i, j;
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            if (piece.blockMatrix[j][i] && wall[piece.xPosition + i][piece.yPosition - j]) //BUG
                return 1;
    return 0;
}
int main(int argc, char **argv)
{
    srand(time(NULL));

    trenutnoPadajuci = newTetrisPiece();
    //inicijalizacija matrice zida, TODO odvojiti u posebnu funkciju
    int i;
    for (i = 0; i < fieldWidth + 2; i++)
        fieldMatrixWall[i][0] = 1;
    for (i = 0; i < fieldHight + 1; i++)
    {
        fieldMatrixWall[0][i] = 1;
        fieldMatrixWall[fieldWidth + 1][i] = 1;
    }
    //Kraj TODO-a
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

    glutInitWindowSize(1000, 1000);
    glutInitWindowPosition(200, 100);
    glutCreateWindow(argv[0]);

    glutKeyboardFunc(on_keyboard);
    glutSpecialFunc(SpecialInput);
    glutReshapeFunc(on_reshape);
    glutDisplayFunc(on_display);
    //timer_active = 0; TODO vrati kad budes radila animaciju

    glClearColor(0, 0, 0, 0);
    glEnable(GL_DEPTH_TEST);

    glutMainLoop();

    return 0;
}

static void on_keyboard(unsigned char key, int x, int y)
{

    switch (key)
    {
    case 27:
        /* Zavrsava se program. */
        exit(0);
        break;
    //TODO obrisi ovaj red kad budes animirala
    case 'g':
    case 'G':
        /* Pokrece se simulacija. */
        if (!timer_active)
        {
            glutTimerFunc(0, on_timer, 0);
            timer_active = 1;
        }
        break;

    case 's':
    case 'S':
    {
        /* Zaustavlja se simulacija. */
        if (timer_active)
            timer_active = 0;
        break;
    }
    }
}
void SpecialInput(int key, int x, int y)
{
    tetrisPiece checker;
    switch (key)
    {
    case GLUT_KEY_UP:
    {
        if (!keyIsPressed)
        {
            keyIsPressed = 1;
            checker = rotatePiece(trenutnoPadajuci);
            if (!doesThePieceHitTheWall(checker, fieldMatrixWall))
            {
                takeOutTetrisPiece(trenutnoPadajuci, fieldMatrix);
                trenutnoPadajuci = checker;
                insertPieceIntoField(trenutnoPadajuci,fieldMatrix);
            }
            keyIsPressed = 0;
        }
        break;
    }
    case GLUT_KEY_DOWN:
        break;
    case GLUT_KEY_LEFT:
    {
        if (!keyIsPressed)
        {
            keyIsPressed = 1;
        checker = trenutnoPadajuci;
        checker.xPosition = checker.xPosition - 1;
        if (!doesThePieceHitTheWall(checker, fieldMatrixWall))
        {
            takeOutTetrisPiece(trenutnoPadajuci, fieldMatrix);
            trenutnoPadajuci = checker;
            insertPieceIntoField(trenutnoPadajuci,fieldMatrix);
        }
        keyIsPressed = 0;
        }
        break;
    }
    case GLUT_KEY_RIGHT:
    {
        if(!keyIsPressed){
            keyIsPressed=1;
        checker = trenutnoPadajuci;
        checker.xPosition = checker.xPosition + 1;
        if (!doesThePieceHitTheWall(checker, fieldMatrixWall))
        {
            takeOutTetrisPiece(trenutnoPadajuci, fieldMatrix);
            trenutnoPadajuci = checker;
            insertPieceIntoField(trenutnoPadajuci,fieldMatrix);
        }
        keyIsPressed=0;
        }
        break;
    }
    }

    glutPostRedisplay();
}
static void on_timer(int value)
{
    /* Proverava se da li callback dolazi od odgovarajuceg tajmera. */
    if (value != 0)
        return;
    //takeOutTetrisPiece(trenutnoPadajuci, fieldMatrix);
    moveDown++;
    if(moveDown == speed)
    {
    moveDown = 0;
    trenutnoPadajuci.yPosition = trenutnoPadajuci.yPosition - 1;
    if (doesThePieceHitTheWall(trenutnoPadajuci, fieldMatrixWall))
    {
        timeToSlideUsed++;
        trenutnoPadajuci.yPosition = trenutnoPadajuci.yPosition + 1;
        if (timeToSlideUsed == timeToSlideMax)
        {
            takeOutTetrisPiece(trenutnoPadajuci, fieldMatrix);
            insertPieceIntoField(trenutnoPadajuci, fieldMatrix);
            insertPieceIntoWall(trenutnoPadajuci, fieldMatrixWall);
            trenutnoPadajuci = newTetrisPiece();
        }
    }
    else
    {
        trenutnoPadajuci.yPosition = trenutnoPadajuci.yPosition + 1;
        takeOutTetrisPiece(trenutnoPadajuci, fieldMatrix);
        timeToSlideUsed = 0;
        trenutnoPadajuci.yPosition = trenutnoPadajuci.yPosition - 1;
        insertPieceIntoField(trenutnoPadajuci, fieldMatrix);
    }
    }
    /* Forsira se ponovno iscrtavanje prozora. */
    glutPostRedisplay();

    /* Po potrebi se ponovo postavlja tajmer. */
    if (timer_active)
        glutTimerFunc(1, on_timer, 0);
}

static void on_reshape(int width, int height)
{
    /* Postavlja se viewport. */
    glViewport(0, 0, width, height);

    /* Postavljaju se parametri projekcije. */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (float)width / height, 1, 1500);
}

static void on_display(void)
{
    /* Postavlja se boja svih piksela na zadatu boju pozadine. */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Set viewpoint */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, 1000, 0, 0, 0, 0, 1, 0);
    drawTetrisField(50, fieldMatrix);

    /* Postavlja se nova slika u prozor. */
    glutSwapBuffers();
}
