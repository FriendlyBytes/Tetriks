#define _XOPEN_SOURCE 700
#define _BSD_SOURCE
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
#include <stdio.h>
#include <time.h>

#include <unistd.h>
//nije ceo program u sastav komentarima, samo ove globalne promenljive
#include "drawTetris.h" //BUG imam bag (prenos parametara, negde koristim globalne promenljive 
                        // i treba da regulisem one define-ove na pocetku programa, ali rok je za dva sata :(
                        // mislila sam da je lako izdvojiti .c fajl iz .h sa funkcijama

#include "image.h" // preuzeto sa casova vezbi

#define FILENAME0 "slika32.bmp"
static GLuint names[1];// sadrzi teksture
Image *image;// slika koju koristimo kao teksturu

/* Fleg koji odredjuje stanje tajmera. */
static int timer_active;

tetrisPiece currentlyFallingTetromino;// tetromino koji trenutno pada
tetrisPiece switchTetromino; // tetromino koji cuvamo sa strane
tetrisPiece t;
// definicije callback funkcija
static void on_keyboard(unsigned char key, int x, int y);
static void on_reshape(int width, int height);
static void on_timer(int value);
static void on_display(void);
static void SpecialInput(int key, int x, int y);
int fieldMatrix[fieldWidth][fieldHight] = {};             // matrica koja predstavlja stanje trenutne partije tetrisa (sadrzi boje polja)
int fieldMatrixWall[fieldWidth + 2][fieldHight + 1] = {0}; // matrica koja sluzi kao omotac prethodne matrice
                                                           // i koja oznacava gde su granice gde tetromino moze da se pomera
                                                           // tj zid koji tetromino ne moze da prodje
// indikatori za pritisnute dugmice
int upKeyIsPressed = 0;
int downKeyIsPressed = 0;

int score = 0;// rezultat
int lvl = 0;// nivo
int numberOfClearedLines = 0;//  broj popunjenih linija koje su obrisane

int wallHit = 0; // indikator da li trenutno tetromino udara u
time_t wallHitTime;// vreme kada je tetromino udario u zid

clock_t timeOfFalling;//vreme kada se tetromino pomerio za jednu poziciju dole poslednji put


float angleOfRotation = 0;//ugao rotacije tetromina sa leve strane

int indSwitched = 0;// da li je zamenjen tetromino sa tetrominom koji stoji sa leve strane na pocetku igre
int indFellAfterSwitch = 1;// da li je tetromino za pao posle zamene (ako nije, ne moze ponovo da se menja)
int indHelpActivated = 0;//da li je korisnik aktivirao pomocni meni


int checkIfFiled(int matrix[fieldWidth][fieldHight], int matrixWall[fieldWidth + 2][fieldHight + 1])//provera da li je red popunjen
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
        switch (numOfFiledRows)//formula za bodovanje sa tetrisa sa nintendo entertainment system-a
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
    currentlyFallingTetromino
 = newTetrisPiece();
    switchTetromino = newTetrisPiece();

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
    
    glClearColor(92 / 255.0, 117 / 255.0, 255 / 255.0, 1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

     //teksture, preuzeto sa vezbi
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
        // zapocinje/nastavlja se igra
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
        //menja se tetromino koji pada sa onim sa strana ako je to moguce
        if (timer_active)
        {
            indSwitched = 1;

            if (indFellAfterSwitch)// moguce je zameniti tetromino
            {
                t = currentlyFallingTetromino;
                takeOutTetrisPiece(currentlyFallingTetromino
            , fieldMatrix);//vadi se tetromino koji pada iz matrice
                currentlyFallingTetromino
             = switchTetromino;
                insertPieceIntoField(currentlyFallingTetromino
            , fieldMatrix);
                switchTetromino = t;
                switchTetromino.xPosition = fieldWidth / 2 - 1;//postavljaju se koordinate switcha na vrh table
                switchTetromino.yPosition = fieldHight;
                indFellAfterSwitch = 0;
            }
        }
        break;
    case 'h':
    case 'H':
    {
        // ukljucuje se help, zaustavlja se igra
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
void SpecialInput(int key, int x, int y) // funkcija za specijalne dugmice, poput strelica
{
    tetrisPiece checker;
    switch (key)
    {
    case GLUT_KEY_UP: // kada se pritisne dugme na gore, treba da se izvrsi rotacija tetromina
                        //nije moguce, treba da se proveri da li je moguce izvrsiti rotaciju
                        // ako se tetromino pomeri za mesto levo, desno, gore ili za dva mesta gore
                        // pa zato sluzi ovaj malo veci if-else
    {
        if (!upKeyIsPressed && timer_active)// upKeyIsPressed i drugi indikatori
                                            // za dugmice sluze samo da se cela funkcija izvrsi pre nego sto se ponovo pozove
        {
            upKeyIsPressed = 1;
            tetrisPiece checker1 = rotatePiece(currentlyFallingTetromino
        );
            if (!doesThePieceHitTheWall(checker1, fieldMatrixWall))
            {
                takeOutTetrisPiece(currentlyFallingTetromino
            , fieldMatrix);
                currentlyFallingTetromino
             = checker1;
                insertPieceIntoField(currentlyFallingTetromino
            , fieldMatrix);
            }
            else
            {
                tetrisPiece checker2 = rotatePiece(currentlyFallingTetromino
            );
                checker2.yPosition = checker2.yPosition + 1;
                if (!doesThePieceHitTheWall(checker2, fieldMatrixWall))
                {
                    takeOutTetrisPiece(currentlyFallingTetromino
                , fieldMatrix);
                    currentlyFallingTetromino
                 = checker2;
                    insertPieceIntoField(currentlyFallingTetromino
                , fieldMatrix);
                }
                else
                {
                    tetrisPiece checker3 = rotatePiece(currentlyFallingTetromino
                );
                    checker3.yPosition = checker3.yPosition + 2;
                    if (!doesThePieceHitTheWall(checker3, fieldMatrixWall))
                    {
                        takeOutTetrisPiece(currentlyFallingTetromino
                    , fieldMatrix);
                        currentlyFallingTetromino
                     = checker3;
                        insertPieceIntoField(currentlyFallingTetromino
                    , fieldMatrix);
                    }
                    else
                    {
                        tetrisPiece checker4 = rotatePiece(currentlyFallingTetromino
                    );

                        checker4.xPosition = checker4.xPosition - 1;
                        if (!doesThePieceHitTheWall(checker4, fieldMatrixWall))
                        {
                            takeOutTetrisPiece(currentlyFallingTetromino
                        , fieldMatrix);
                            currentlyFallingTetromino
                         = checker4;
                            insertPieceIntoField(currentlyFallingTetromino
                        , fieldMatrix);
                        }
                        else
                        {
                            tetrisPiece checker5 = rotatePiece(currentlyFallingTetromino
                        );
                            checker4.xPosition = checker5.xPosition + 1;
                            if (!doesThePieceHitTheWall(checker5, fieldMatrixWall))
                            {
                                takeOutTetrisPiece(currentlyFallingTetromino
                            , fieldMatrix);
                                currentlyFallingTetromino
                             = checker5;
                                insertPieceIntoField(currentlyFallingTetromino
                            , fieldMatrix);
                            }
                        }
                    }
                }
            }
        }
        upKeyIsPressed = 0;

        break;
    }
    case GLUT_KEY_DOWN: // treba da se proveri da li je moguce da se tetromino pomeri brze dole
                      // i da se inkrementira broj poena kad se to desi
    {
        if (!downKeyIsPressed && timer_active)
        {
            downKeyIsPressed = 1;
            currentlyFallingTetromino
        .yPosition = currentlyFallingTetromino
        .yPosition - 1;//tetromino se pomera dole
            if (doesThePieceHitTheWall(currentlyFallingTetromino
        , fieldMatrixWall))
            {
                if (!wallHit)
                {
                    wallHit = 1;
                    wallHitTime = time(NULL);
                }
                currentlyFallingTetromino
            .yPosition = currentlyFallingTetromino
            .yPosition + 1;//vracamo ga za korak gore
                if (difftime(time(NULL), wallHitTime) > 1)
                {
                    indFellAfterSwitch = 1;
                    takeOutTetrisPiece(currentlyFallingTetromino
                , fieldMatrix);
                    insertPieceIntoField(currentlyFallingTetromino
                , fieldMatrix);
                    insertPieceIntoWall(currentlyFallingTetromino
                , fieldMatrixWall);
                    checkIfFiled(fieldMatrix, fieldMatrixWall);
                    currentlyFallingTetromino
                 = newTetrisPiece();
                    wallHit = 0;
                }
            }
            else
            {
                currentlyFallingTetromino
            .yPosition = currentlyFallingTetromino
            .yPosition + 1;
                takeOutTetrisPiece(currentlyFallingTetromino
            , fieldMatrix);
                wallHit = 0;
                currentlyFallingTetromino
            .yPosition = currentlyFallingTetromino
            .yPosition - 1;
                insertPieceIntoField(currentlyFallingTetromino
            , fieldMatrix);
            score += 20;
            }
            glutPostRedisplay();
            downKeyIsPressed = 0;
        }
    }
    break;
    case GLUT_KEY_LEFT:// pomeranje tetromina levo, slicno glut key down spustanju, samo nema onih provera
    {
        if (!upKeyIsPressed && timer_active)
        {
            upKeyIsPressed = 1;
            checker = currentlyFallingTetromino
        ;
            checker.xPosition = checker.xPosition - 1;
            if (!doesThePieceHitTheWall(checker, fieldMatrixWall))
            {
                takeOutTetrisPiece(currentlyFallingTetromino
            , fieldMatrix);
                currentlyFallingTetromino
             = checker;
                insertPieceIntoField(currentlyFallingTetromino
            , fieldMatrix);
            }
            upKeyIsPressed = 0;
        }
        break;
    }

    case GLUT_KEY_RIGHT:// analogno levom
    {
        if (!upKeyIsPressed && timer_active)
        {
            upKeyIsPressed = 1;
            checker = currentlyFallingTetromino
        ;
            checker.xPosition = checker.xPosition + 1;
            if (!doesThePieceHitTheWall(checker, fieldMatrixWall))
            {
                takeOutTetrisPiece(currentlyFallingTetromino
            , fieldMatrix);
                currentlyFallingTetromino
             = checker;
                insertPieceIntoField(currentlyFallingTetromino
            , fieldMatrix);
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
    if (((clock() - timeOfFalling) * 1.0) / CLOCKS_PER_SEC > 0.2 - log(lvl + 1) / 10.0)//povecavanje brzine padanja sa povecanjem nivoa
    {

        currentlyFallingTetromino
    .yPosition = currentlyFallingTetromino
    .yPosition - 1;
        if (doesThePieceHitTheWall(currentlyFallingTetromino
    , fieldMatrixWall)) //posmatramo slucaj gde se tetromino spusta nanize i gde udara u zid
        {
            if (!wallHit)
            {
                wallHit = 1;
                wallHitTime = time(NULL);
            }
            currentlyFallingTetromino
        .yPosition = currentlyFallingTetromino
        .yPosition + 1;
            if (difftime(time(NULL), wallHitTime) > 1) //ne zelimo da se tetromino odmah stopi sa zidom,
                                                //da bi igrac bio u stanju da se pomera levo-desno i rotira 1 sekund
            {
                indFellAfterSwitch = 1;
                takeOutTetrisPiece(currentlyFallingTetromino
            , fieldMatrix);
                insertPieceIntoField(currentlyFallingTetromino
            , fieldMatrix);
                insertPieceIntoWall(currentlyFallingTetromino
            , fieldMatrixWall);
                checkIfFiled(fieldMatrix, fieldMatrixWall);
                currentlyFallingTetromino
             = newTetrisPiece();
                wallHit = 0;
            }
        }
        else
        {
            currentlyFallingTetromino
        .yPosition = currentlyFallingTetromino
        .yPosition + 1;
            takeOutTetrisPiece(currentlyFallingTetromino
        , fieldMatrix);
            wallHit = 0;
            currentlyFallingTetromino
        .yPosition = currentlyFallingTetromino
        .yPosition - 1;
            insertPieceIntoField(currentlyFallingTetromino
        , fieldMatrix);
        }
        timeOfFalling = clock();
    }
    /* Forsira se ponovno iscrtavanje prozora. */
    glutPostRedisplay();

    /* Po potrebi se ponovo postavlja tajmer. */
    if (timer_active)
        glutTimerFunc(1, on_timer, 0);
}

static void on_reshape(int width, int height) //sa vezbi
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
    //iscrtavanje pozadinske slike
    glBindTexture(GL_TEXTURE_2D, names[0]); //postavljanje izabrane pozadine za teksturu
    //iscrtavanje pozadinske teksture u formi quad-a
    glPushMatrix();
    glTranslatef(-720, -480, 0);
    glDisable(GL_DEPTH_TEST);
    glBegin(GL_QUADS);//todo odvojiti obaj quad u zasebnu funkciju
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
        drawOnePiece(switchTetromino.type, switchTetromino.blockMatrix, 50, angleOfRotation);
        angleOfRotation += 0.5;
        glPopMatrix();
    }

    glColor3f(0, 0, 0);

    //tekst: bodovi, nivo, unistene linije, pomoc, potpis
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
    //meni za pomoc
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
