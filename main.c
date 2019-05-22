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
#include "image.h"

#define FILENAME0 "slika32.bmp"
static GLuint names[1];
Image *image;

/* Fleg koji odredjuje stanje tajmera. */
static int timer_active;

int delay = 150000; //vremenski interval na koji se vrsi pomeranje tetromina nanize
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

float ugaoRotacije = 0;
int indSwitched = 0;
int indFellAfterSwitch = 1;
int indHelpActivated = 0;
tetrisPiece t;

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
        glutPostRedisplay();
    }
    numberOfClearedLines += numOfFiledRows;
    lvl = numberOfClearedLines / 5;
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

    glutInitWindowSize(1480, 950);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);

    glutKeyboardFunc(on_keyboard);
    glutSpecialFunc(SpecialInput);
    glutReshapeFunc(on_reshape);
    glutDisplayFunc(on_display);
    //timer_active = 0; TODO vrati kad budes radila animaciju

    glClearColor(92 / 255.0, 117 / 255.0, 255 / 255.0, 1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //PROBA TEKSTURE
    /* Ukljucuju se teksture. */
    glEnable(GL_TEXTURE_2D);

    glTexEnvf(GL_TEXTURE_ENV,
              GL_TEXTURE_ENV_MODE,
              GL_REPLACE);
    /*
     * Inicijalizuje se objekat koji ce sadrzati teksture ucitane iz
     * fajla.
     */
    image = image_init(0, 0);

    /* Kreira se prva tekstura. */
    image_read(image, FILENAME0);

    /* Generisu se identifikatori tekstura. */
    glGenTextures(1, names);

    glBindTexture(GL_TEXTURE_2D, names[0]);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 image->width, image->height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image->pixels);
    /* Iskljucujemo aktivnu teksturu */
    glBindTexture(GL_TEXTURE_2D, 0);

    /* Unistava se objekat za citanje tekstura iz fajla. */
    image_done(image);

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
    case 's':
    case 'S':
        /* Pokrece se simulacija. */
        if (!timer_active && !indHelpActivated)
        {
            glutTimerFunc(0, on_timer, 0);
            timer_active = 1;
        }
        break;

    case 'p':
    case 'P':
    {
        /* Zaustavlja se. */
        if (timer_active && !indHelpActivated)
            timer_active = 0;
        break;
    }
    case 'c':
    case 'C':
    {
        if (timer_active)
        {
            indSwitched = 1;

            if (indFellAfterSwitch)
            {
                t = trenutnoPadajuci;
                takeOutTetrisPiece(trenutnoPadajuci, fieldMatrix);
                trenutnoPadajuci = zamena;
                insertPieceIntoField(trenutnoPadajuci, fieldMatrix);
                zamena = t;
                zamena.xPosition = fieldWidth / 2 - 1;
                zamena.yPosition = fieldHight;
                indFellAfterSwitch = 0;
            }
        }
        break;
    case 'h':
    case 'H':
    {
        if (indHelpActivated)
        {
            indHelpActivated = 0;
            glutTimerFunc(0, on_timer, 0);
            timer_active = 1;
        }
        else
        {
            indHelpActivated = 1;
            timer_active = 0;
        }
        glutPostRedisplay();
        break;
    }
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
        if (!upKeyIsPressed && timer_active)
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
        if (!downKeyIsPressed && timer_active)
        {
            downKeyIsPressed = 1;
            score += 20;
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
                    indFellAfterSwitch = 1;
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
        if (!upKeyIsPressed && timer_active)
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
        if (!upKeyIsPressed && timer_active)
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
    if (((clock() - timeOfFalling) * 1.0) / CLOCKS_PER_SEC > 0.2 - log(lvl + 1) / 10.0)
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
                indFellAfterSwitch = 1;
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
    gluLookAt(0, 0, 780, 0, 0, 0, 0, 1, 0);

    glBindTexture(GL_TEXTURE_2D, names[0]); //postavljanje izabrane pozadine za teksturu
    //iscrtavanje pozadinske teksture u formi quad-a
    glPushMatrix();
    glTranslatef(-720, -480, 0);
    glDisable(GL_DEPTH_TEST);
    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1);

    glTexCoord2f(0, 0);
    glVertex2f(0, 0);

    glTexCoord2f(1, 0);
    glVertex2f(1439, 0);

    glTexCoord2f(1, 1);
    glVertex2f(1439, 960);

    glTexCoord2f(0, 1);
    glVertex2f(0, 960);
    glEnd();

    glPopMatrix();
    //kraj iscrtavanja pozadine
    //vraca se originalna tekstura
    glBindTexture(GL_TEXTURE_2D, 0);

    //beli kvadrat koji sadrzi podateke o poenima
    glPushMatrix();
    glTranslatef(-390, 180, -50);
    drawTransparentSqr(150, 1, 1, 1, 0.2);
    glPopMatrix();

    drawBackgroundDetails();
    //iscrtava se matrica sa tetriminima
    drawTetrisField(40, fieldMatrix);

    //provera da li treba da se prikaze sacuvani tetromino i njegov prikaz
    if (indSwitched)
    {
        glPushMatrix();
        glTranslatef(-400, 0, 0);
        drawOnePiece(zamena.type, zamena.blockMatrix, 50, ugaoRotacije);
        ugaoRotacije += 0.5;
        glPopMatrix();
    }

    glColor3f(0, 0, 0);

    //tekst: bodovi, nivo, unistene linije
    char scoreString[30];
    char linesString[30];
    char levelString[30];
    char byString[30] = "Kristina Popovic, 2019";
    char helpString[30] = "For help, press h";
    sprintf(scoreString, "Score: %d", score);
    sprintf(levelString, "Level: %d", lvl);
    sprintf(linesString, "Lines cleared: %d", numberOfClearedLines);
    drawString(-430, 150, 0, scoreString);
    drawString(-430, 180, 0, levelString);
    drawString(-430, 210, 0, linesString);
    glColor3f(1, 1, 1);
    drawString(250, -390, 0, helpString);
    drawString(250, -420, 0, byString);
    if (indHelpActivated)
    {
        glPushMatrix();
        glDisable(GL_DEPTH_TEST);
        glScalef(1, 0.8, 1);
        drawTransparentSqr(400, 1, 1, 1, 0.3);
        glEnable(GL_DEPTH_TEST);
        glPopMatrix();
        glColor3f(0, 0, 0);

        char red1[30] = "HELP";
        char red2[30] = "S - start game";
        char red3[30] = "P - pause game";
        char red4[30] = "C - change tetrimino";
        char red5[50] = "Left and right keys - move left and right";
        char red6[30] = "Up key - rotate";
        char red7[30] = "Down key - go down faster";
        char red8[30] = "H - enough help";
        char red9[30] = "Esc - exit";
        drawString(-20, 130, 0, red1);
        drawString(-180, 70, 0, red2);
        drawString(-180, 40, 0, red3);
        drawString(-180, 10, 0, red4);
        drawString(-180, -20, 0, red5);
        drawString(-180, -50, 0, red6);
        drawString(-180, -80, 0, red7);
        drawString(-180, -110, 0, red8);
        drawString(-180, -140, 0, red9);
    }
    /* Postavlja se nova slika u prozor. */
    glutSwapBuffers();
}
