#define _XOPEN_SOURCE 700
#define _BSD_SOURCE
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
#include <stdio.h>
#include <time.h>
#include "constants.h"
#include <unistd.h>
//TODO odvoj u constants.h
#include "drawTetris.h"
#include "simpleToolFunctions.h"

/* Fleg koji odredjuje stanje tajmera. */
static int timer_active;

/* Deklaracije callback funkcija. */
int delay = 150000;
tetrisPiece trenutnoPadajuci;
tetrisPiece zamena;
tetrisPiece t;
static void on_keyboard(unsigned char key, int x, int y);
static void on_reshape(int width, int height);
static void on_timer(int value);
static void on_display(void);
static void SpecialInput(int key, int x, int y);
int fieldMatrix[fieldWidth][fieldHight] = {};              //matrix which will hold colors of the tetris fields
int fieldMatrixWall[fieldWidth + 2][fieldHight + 1] = {0}; //matrix of 1s and 0s which keeps track of where new pieces can't fall
int upKeyIsPressed = 0;
int downKeyIsPressed = 0;
int moveDown = 0;
int lvl = 0;
int numberOfClearedLines = 0;
int wallHit = 0;
time_t wallHitTime;
int score = 0;
clock_t timeOfFalling;
char scoreString[30];
char linesString[30];
char levelString[30];
/*funkcija koja proverava da li je popunjen neki red i brise popunjene redove*/

//PROBA
void drawString(float x, float y, float z, char *string)
{
    glRasterPos3f(x, y, z);

    for (char *c = string; *c != '\0'; c++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c); // Updates the position
    }
}
//PROBAKRAJ

int checkIfFiled(int matrix[fieldWidth][fieldHight], int matrixWall[fieldWidth + 2][fieldHight + 1])
{
    int i, j, k, ind;
    int numOfFiledRows = 0;
    for (i = 0; i < fieldHight; i++)
    {
        ind = 1;
        for (j = 0; j < fieldWidth; j++)
            if (!matrix[j][i])
                ind = 0;
        if (ind) //Ono sto se desi kad nestane red
        {
            numOfFiledRows++;
            for (k = i; k < fieldHight - 1; k++)
                for (j = 0; j < fieldWidth; j++)
                {
                    matrix[j][k] = matrix[j][k + 1];
                    matrixWall[j + 1][k + 1] = matrixWall[j + 1][k + 2];
                }
            for (j = 0; j < fieldWidth; j++)
                matrix[j][fieldHight - 1] = 0;
            i--;
        }
        switch (numOfFiledRows)
        {
        case 1:
            score += 40 * (lvl + 1);
            break;
        case 2:
            score += 100 * (lvl + 1);
            break;
        case 3:
            score += 300 * (lvl + 1);
            break;
        case 4:
            score += 1200 * (lvl + 1);
            break;
        default:
            break;
        }
        lvl = numOfFiledRows / 10;
        glutPostRedisplay();
    }
    numberOfClearedLines += numOfFiledRows;
    return 0;
}

int checkIfEndOfGame(int matrixWall[fieldWidth + 2][fieldHight + 1])
{
    int i;
    for (i = 1; i < fieldWidth + 1; i++)
        if (matrixWall[i][fieldHight - 4])
            return 1;
    return 0;
}
/*TODO:
rotacija u kontra smeru
bodovi
zvuk */
int main(int argc, char **argv)
{
    srand(time(NULL));
    timeOfFalling = clock();
    trenutnoPadajuci = newTetrisPiece();
    zamena = newTetrisPiece();
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
    glEnable(GL_BLEND);

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
    case 'c':
    case 'C':
    {
        t = trenutnoPadajuci;
        takeOutTetrisPiece(trenutnoPadajuci, fieldMatrix);
        trenutnoPadajuci = zamena;
        insertPieceIntoField(trenutnoPadajuci, fieldMatrix);
        zamena = t;
        zamena.xPosition = fieldWidth / 2 - 1;
        zamena.yPosition = fieldHight;
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
        if (!upKeyIsPressed)
        {
            upKeyIsPressed = 1;
            tetrisPiece checker1 = rotatePiece(trenutnoPadajuci);
            if (!doesThePieceHitTheWall(checker1, fieldMatrixWall))
            {
                takeOutTetrisPiece(trenutnoPadajuci, fieldMatrix);
                trenutnoPadajuci = checker1;
                insertPieceIntoField(trenutnoPadajuci, fieldMatrix);
            }
            else
            {
                tetrisPiece checker2 = rotatePiece(trenutnoPadajuci);
                checker2.yPosition = checker2.yPosition + 1;
                if (!doesThePieceHitTheWall(checker2, fieldMatrixWall))
                {
                    takeOutTetrisPiece(trenutnoPadajuci, fieldMatrix);
                    trenutnoPadajuci = checker2;
                    insertPieceIntoField(trenutnoPadajuci, fieldMatrix);
                }
                else
                {
                    tetrisPiece checker3 = rotatePiece(trenutnoPadajuci);
                    checker3.yPosition = checker3.yPosition + 2;
                    if (!doesThePieceHitTheWall(checker3, fieldMatrixWall))
                    {
                        takeOutTetrisPiece(trenutnoPadajuci, fieldMatrix);
                        trenutnoPadajuci = checker3;
                        insertPieceIntoField(trenutnoPadajuci, fieldMatrix);
                    }
                    else
                    {
                        tetrisPiece checker4 = rotatePiece(trenutnoPadajuci);

                        checker4.xPosition = checker4.xPosition - 1;
                        if (!doesThePieceHitTheWall(checker4, fieldMatrixWall))
                        {
                            takeOutTetrisPiece(trenutnoPadajuci, fieldMatrix);
                            trenutnoPadajuci = checker4;
                            insertPieceIntoField(trenutnoPadajuci, fieldMatrix);
                        }
                        else
                        {
                            tetrisPiece checker5 = rotatePiece(trenutnoPadajuci);
                            checker4.xPosition = checker5.xPosition + 1;
                            if (!doesThePieceHitTheWall(checker5, fieldMatrixWall))
                            {
                                takeOutTetrisPiece(trenutnoPadajuci, fieldMatrix);
                                trenutnoPadajuci = checker5;
                                insertPieceIntoField(trenutnoPadajuci, fieldMatrix);
                            }
                        }
                    }
                }
            }
        }
        upKeyIsPressed = 0;

        break;
    }
    case GLUT_KEY_DOWN:
    {
        if (!downKeyIsPressed)
        {
            downKeyIsPressed = 1;
            trenutnoPadajuci.yPosition = trenutnoPadajuci.yPosition - 1;
            if (doesThePieceHitTheWall(trenutnoPadajuci, fieldMatrixWall))
            {
                if (!wallHit)
                {
                    wallHit = 1;
                    wallHitTime = time(NULL);
                }
                trenutnoPadajuci.yPosition = trenutnoPadajuci.yPosition + 1;
                if (difftime(time(NULL), wallHitTime) > 1)
                {
                    takeOutTetrisPiece(trenutnoPadajuci, fieldMatrix);
                    insertPieceIntoField(trenutnoPadajuci, fieldMatrix);
                    insertPieceIntoWall(trenutnoPadajuci, fieldMatrixWall);
                    checkIfFiled(fieldMatrix, fieldMatrixWall);
                    trenutnoPadajuci = newTetrisPiece();
                    wallHit = 0;
                }
            }
            else
            {
                trenutnoPadajuci.yPosition = trenutnoPadajuci.yPosition + 1;
                takeOutTetrisPiece(trenutnoPadajuci, fieldMatrix);
                wallHit = 0;
                trenutnoPadajuci.yPosition = trenutnoPadajuci.yPosition - 1;
                insertPieceIntoField(trenutnoPadajuci, fieldMatrix);
            }
            glutPostRedisplay();
            downKeyIsPressed = 0;
        }
    }
    break;
    case GLUT_KEY_LEFT:
    {
        if (!upKeyIsPressed)
        {
            upKeyIsPressed = 1;
            checker = trenutnoPadajuci;
            checker.xPosition = checker.xPosition - 1;
            if (!doesThePieceHitTheWall(checker, fieldMatrixWall))
            {
                takeOutTetrisPiece(trenutnoPadajuci, fieldMatrix);
                trenutnoPadajuci = checker;
                insertPieceIntoField(trenutnoPadajuci, fieldMatrix);
            }
            upKeyIsPressed = 0;
        }
        break;
    }

    case GLUT_KEY_RIGHT:
    {
        if (!upKeyIsPressed)
        {
            upKeyIsPressed = 1;
            checker = trenutnoPadajuci;
            checker.xPosition = checker.xPosition + 1;
            if (!doesThePieceHitTheWall(checker, fieldMatrixWall))
            {
                takeOutTetrisPiece(trenutnoPadajuci, fieldMatrix);
                trenutnoPadajuci = checker;
                insertPieceIntoField(trenutnoPadajuci, fieldMatrix);
            }
            upKeyIsPressed = 0;
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
    if (checkIfEndOfGame(fieldMatrixWall))
        timer_active = 0;
    if (((clock() - timeOfFalling) * 1.0) / CLOCKS_PER_SEC > 0.3)
    {

        trenutnoPadajuci.yPosition = trenutnoPadajuci.yPosition - 1;
        if (doesThePieceHitTheWall(trenutnoPadajuci, fieldMatrixWall))
        {
            if (!wallHit)
            {
                wallHit = 1;
                wallHitTime = time(NULL);
            }
            trenutnoPadajuci.yPosition = trenutnoPadajuci.yPosition + 1;
            if (difftime(time(NULL), wallHitTime) > 1)
            {
                takeOutTetrisPiece(trenutnoPadajuci, fieldMatrix);
                insertPieceIntoField(trenutnoPadajuci, fieldMatrix);
                insertPieceIntoWall(trenutnoPadajuci, fieldMatrixWall);
                checkIfFiled(fieldMatrix, fieldMatrixWall);
                trenutnoPadajuci = newTetrisPiece();
                wallHit = 0;
            }
        }
        else
        {
            trenutnoPadajuci.yPosition = trenutnoPadajuci.yPosition + 1;
            takeOutTetrisPiece(trenutnoPadajuci, fieldMatrix);
            wallHit = 0;
            trenutnoPadajuci.yPosition = trenutnoPadajuci.yPosition - 1;
            insertPieceIntoField(trenutnoPadajuci, fieldMatrix);
        }
        timeOfFalling = clock();
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
    glColor3f(0.5, 0.5, 0.5);
    sprintf(scoreString, "Score: %d", score);
    sprintf(levelString, "Level: %d", lvl);
    sprintf(linesString, "Lines cleared: %d", numberOfClearedLines);
    drawString(350, 0, 0, scoreString); //Koristi sprintf
    drawString(350, -30, 0, levelString);
    drawString(350, -60, 0, linesString);

    /* Postavlja se nova slika u prozor. */
    glutSwapBuffers();
}