#include "constants.h"
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
#include "simpleToolFunctions.h"
#include <stdio.h>
//TODO odvoj u constants.h
#define fieldWidth 12
#define fieldHight 20
#define blend_factor 0.7
#define yellow 1
#define orange 2
#define red 3
#define purple 4
#define light_blue 5
#define dark_blue 6
#define green 7



/* Fleg koji odredjuje stanje tajmera. */
//static int timer_active; TODO vrati ga nazad kad budes radila animaciju
 
/* Deklaracije callback funkcija. */
static void on_keyboard(unsigned char key, int x, int y);
static void on_reshape(int width, int height);
static void on_timer(int value);
static void on_display(void);

int fieldMatrix[fieldWidth][fieldHight] = {{0,1,2,3,4,5,6,7}}; //matrix which will hold colors of the tetris fields

void drawTetrisWireBlock(double size,double r, double g, double b, double lineWidth)
{
    glDisable(GL_DEPTH_TEST);
    glColor3f(r,g,b);
    glLineWidth(lineWidth);
    glutWireCube(size);
    glEnable(GL_DEPTH_TEST);
}

void drawTetrisSolidBlock(double size,double r, double g, double b, double blend)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(r,g,b,blend);
    glutSolidCube(size);
    glDisable(GL_BLEND);
}
static void drawTetrisColoredSolidBlock(double size,int color, double blend)
{
    switch(color)
    {
        case yellow:
            drawTetrisSolidBlock(size,1, 249.0/255, 48.0/255, blend);
            break;
        case orange:
            drawTetrisSolidBlock(size,246.0/255, 172.0/255, 44.0/255, blend);
            break;
        case red:
            drawTetrisSolidBlock(size,1, 57.0/255, 57.0/255, blend);
            break;
        case purple:
            drawTetrisSolidBlock(size,223.0/255, 26.0/255, 245.0/255, blend);
            break;
        case light_blue:
            drawTetrisSolidBlock(size,118.0/255, 223.0/255, 1, blend);
            break;
        case dark_blue:
            drawTetrisSolidBlock(size,27.0/255, 78.0/255, 219.0/255, blend);
            break;
        case green:
            drawTetrisSolidBlock(size,10.0/255, 249.0/255, 48.0/255, blend);
            break;
    }
}

static void drawTetrisColoredWireBlock(double size,int color, double lineWidth)
{
    switch(color)
    {
        case yellow:
            drawTetrisWireBlock(size,1, 249.0/255, 48.0/255, lineWidth);
            break;
        case orange:
            drawTetrisWireBlock(size,246.0/255, 172.0/255, 44.0/255, lineWidth);
            break;
        case red:
            drawTetrisWireBlock(size,1, 57.0/255, 57.0/255, lineWidth);
            break;
        case purple:
            drawTetrisWireBlock(size,223.0/255, 26.0/255, 245.0/255, lineWidth);
            break;
        case light_blue:
            drawTetrisWireBlock(size,118.0/255, 223.0/255, 1, lineWidth);
            break;
        case dark_blue:
            drawTetrisWireBlock(size,27.0/255, 78.0/255, 219.0/255, lineWidth);
            break;
        case green:
            drawTetrisWireBlock(size,10.0/255, 249.0/255, 48.0/255, lineWidth);
            break;
    }
}


int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

    glutInitWindowSize(1000, 1000);
    glutInitWindowPosition(200, 100);
    glutCreateWindow(argv[0]);

    glutKeyboardFunc(on_keyboard);
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
    switch (key) {
    case 27:
        /* Zavrsava se program. */
        exit(0);
        break;
        on_timer(4);//TODO obrisi ovaj red kad budes animirala
//     case 'g':
//     case 'G':
//         /* Pokrece se simulacija. */
//         if (!timer_active) {
//             glutTimerFunc(50, on_timer, 0);
//             timer_active = 1;
//         }
//         break;
// 
//     case 's':
//     case 'S':
//         /* Zaustavlja se simulacija. */
//         if(timer_active)
//             timer_active = 0;
//         break;
    }
}

static void on_timer(int value)
{
    /* Proverava se da li callback dolazi od odgovarajuceg tajmera. */
    if (value != 0)
        return;


    /* Forsira se ponovno iscrtavanje prozora. */
    glutPostRedisplay();

    /* Po potrebi se ponovo postavlja tajmer. */
    //if (timer_active)
        //glutTimerFunc(50, on_timer, 0); TODO vrati kad budes animirala
}

static void on_reshape(int width, int height)
{
    /* Postavlja se viewport. */
    glViewport(0, 0, width, height);

    /* Postavljaju se parametri projekcije. */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (float) width / height, 1, 1500);
}

static void drawTetrisField(int size, int matrix[fieldWidth][fieldHight])
{
    int i,j;
    glPushMatrix();
        glTranslatef(-size*fieldWidth/2,-size*fieldHight/2,0);
        glPushMatrix(); 
            for(i = 0; i<fieldWidth; i++)
                {
                    glPushMatrix();
                    for(j = 0; j<fieldHight; j++)
                    {
                        drawTetrisWireBlock(size,0.3,0.3,0.3,1);
                        glTranslatef(0,size,0);
                    }
                    glPopMatrix();
                    glTranslatef(size,0,0);
                }
        glPopMatrix();
    glPopMatrix();
    
    glPushMatrix();
        glTranslatef(-size*fieldWidth/2,-size*fieldHight/2,0);
        glPushMatrix(); 
            for(i = 0; i<fieldWidth; i++)
                {
                    glPushMatrix();
                    for(j = 0; j<fieldHight; j++)
                    {
                        if(fieldMatrix[i][j])
                            drawTetrisColoredSolidBlock(size,fieldMatrix[i][j],0.5);
                        glTranslatef(0,size,0);
                    }
                    glPopMatrix();
                    glTranslatef(size,0,0);
                }
        glPopMatrix();
    glPopMatrix();
    
    glPushMatrix();
        glTranslatef(-size*fieldWidth/2,-size*fieldHight/2,0);
        glPushMatrix(); 
            for(i = 0; i<fieldWidth; i++)
                {
                    glPushMatrix();
                    for(j = 0; j<fieldHight; j++)
                    {
                        if(fieldMatrix[i][j])
                            drawTetrisColoredWireBlock(size,fieldMatrix[i][j],2.5);
                        glTranslatef(0,size,0);
                    }
                    glPopMatrix();
                    glTranslatef(size,0,0);
                }
        glPopMatrix();
    glPopMatrix();
}


static void on_display(void)
{
    /* Postavlja se boja svih piksela na zadatu boju pozadine. */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Postavlja se vidna tacka. */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, 1000, 0, 0, 0, 0, 1, 0);
    
    drawTetrisField(50,fieldMatrix);

    /* Odredjuju se uglovi rotacije Zemlje i generise se ovo telo. */

    /* Odredjuju se uglovi rotacije Meseca i
     * generise se ovo telo. */

    /* Postavlja se nova slika u prozor. */
    glutSwapBuffers();
}
