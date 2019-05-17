#ifndef DRAWTETRIS_H
#define DRAWTETRIS_H
#define fieldHight 24 //The real size +4
#define fieldWidth 12
#define blend_factor 0.7
#define yellow 1
#define orange 2
#define red 3
#define purple 4
#define light_blue 5
#define dark_blue 6
#define green 7
#include "constants.h"
/*
    Kako funkcionise iscrtavanje tabele tetris:
    1. postoji matrica field matrix cije su dimenzije fieldWidth i fieldHight
    2. matrica se iscrtava uz pomoc funkcije drawTetrisField
    3. iscrtava samo okvir matrice (siva mreza kockica)
    4. iscrtavaju se polja matrice koja su obojena uz pomoc drawTetrisWireBlock,
    drawTetrisSolidBlock, drawTetrisColoredSolidBlock, drawTetrisColoredWireBlock
    */
void drawTetrisWireBlock(double size, double r, double g, double b, double lineWidth)
{
    glDisable(GL_DEPTH_TEST);
    glColor4f(r, g, b,0.5);
    glLineWidth(lineWidth);
    glutWireCube(size);
    glEnable(GL_DEPTH_TEST);
}

void drawTetrisSolidBlock(double size, double r, double g, double b, double blend)
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(r, g, b, blend);
    glutSolidCube(size);
}

static void drawTetrisColoredSolidBlock(double size, int color, double blend)
{
    switch (color)
    {
    case yellow:
        drawTetrisSolidBlock(size, 1, 249.0 / 255, 48.0 / 255, blend);
        break;
    case orange:
        drawTetrisSolidBlock(size, 246.0 / 255, 172.0 / 255, 44.0 / 255, blend);
        break;
    case red:
        drawTetrisSolidBlock(size, 1, 57.0 / 255, 57.0 / 255, blend);
        break;
    case purple:
        drawTetrisSolidBlock(size, 223.0 / 255, 26.0 / 255, 245.0 / 255, blend);
        break;
    case light_blue:
        drawTetrisSolidBlock(size, 118.0 / 255, 223.0 / 255, 1, blend);
        break;
    case dark_blue:
        drawTetrisSolidBlock(size, 27.0 / 255, 78.0 / 255, 219.0 / 255, blend);
        break;
    case green:
        drawTetrisSolidBlock(size, 10.0 / 255, 249.0 / 255, 48.0 / 255, blend);
        break;
    }
}

static void drawTetrisColoredWireBlock(double size, int color, double lineWidth)
{
    switch (color)
    {
    case yellow:
        drawTetrisWireBlock(size, 1, 249.0 / 255, 48.0 / 255, lineWidth);
        break;
    case orange:
        drawTetrisWireBlock(size, 246.0 / 255, 172.0 / 255, 44.0 / 255, lineWidth);
        break;
    case red:
        drawTetrisWireBlock(size, 1, 57.0 / 255, 57.0 / 255, lineWidth);
        break;
    case purple:
        drawTetrisWireBlock(size, 223.0 / 255, 26.0 / 255, 245.0 / 255, lineWidth);
        break;
    case light_blue:
        drawTetrisWireBlock(size, 118.0 / 255, 223.0 / 255, 1, lineWidth);
        break;
    case dark_blue:
        drawTetrisWireBlock(size, 27.0 / 255, 78.0 / 255, 219.0 / 255, lineWidth);
        break;
    case green:
        drawTetrisWireBlock(size, 10.0 / 255, 249.0 / 255, 48.0 / 255, lineWidth);
        break;
    }
}

static void drawTetrisField(int size, int matrix[fieldWidth][fieldHight])
{
    int i, j;
    glPushMatrix();

    glTranslatef(-size * fieldWidth / 2, -size * (fieldHight -4) / 2, 0);
    glPushMatrix();
    //crta sivu mrezu u pozadini
    for (i = 0; i < fieldWidth; i++)
    {
        glPushMatrix();
        for (j = 0; j < fieldHight-4; j++)//for (j = 0; j < fieldHight; j++)
        {
            drawTetrisWireBlock(size, 0.3, 0.3, 0.3, 1);
            glTranslatef(0, size, 0);
        }
        glPopMatrix();
        glTranslatef(size, 0, 0);
    }
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-size * fieldWidth / 2, -size * (fieldHight-4) / 2, 0);
    
    glPushMatrix();
    //crta providne obojene kocke
    for (i = 0; i < fieldWidth; i++)
    {
        glPushMatrix();
        for (j = 0; j < fieldHight-4; j++)//for (j = 0; j < fieldHight; j++)
        {
            if (matrix[i][j])
                drawTetrisColoredSolidBlock(size, matrix[i][j], 0.5);
            glTranslatef(0, size, 0);
        }
        glPopMatrix();
        glTranslatef(size, 0, 0);
    }
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-size * fieldWidth / 2, -size * (fieldHight-4) / 2, 0);
    
    //DONJA POLOVINA
    glPushMatrix();
    //crta donji levi ugao obojene zice
    for (i = 0; i < fieldWidth/2; i++)
    {
        glPushMatrix();
        for (j = 0; j < (fieldHight-4)/2; j++)//for (j = 0; j < fieldHight; j++)
        {
            if (matrix[i][j])
                drawTetrisColoredWireBlock(size, matrix[i][j], 2.5);
            glTranslatef(0, size, 0);
        }
        glPopMatrix();
        glTranslatef(size, 0, 0);
    }
    glPopMatrix();

    glPushMatrix();
    glTranslatef(size*(fieldWidth-1),0,0);
    //crta donji desni ugao obojene zice
    for (i = fieldWidth-1; i >= fieldWidth/2; i--)
    {
        glPushMatrix();
        for (j = 0; j < (fieldHight-4)/2; j++)//for (j = 0; j < fieldHight; j++)
        {
            if (matrix[i][j])
                drawTetrisColoredWireBlock(size, matrix[i][j], 2.5);
            glTranslatef(0, size, 0);
        }
        glPopMatrix();
        glTranslatef(-size, 0, 0);
    }
    glPopMatrix();

    //GORNJA POLOVINA
    glPushMatrix();
    //crta gornji levi ugao obojene zice
    glTranslatef(0,size*(fieldHight-4-1),0);
    for (i = 0; i < fieldWidth/2; i++)
    {
        glPushMatrix();
        for (j = fieldHight-4-1; j >= (fieldHight-4)/2; j--)//for (j = 0; j < fieldHight; j++)
        {
            if (matrix[i][j])
                drawTetrisColoredWireBlock(size, matrix[i][j], 2.5);
            glTranslatef(0, -size, 0);
        }
        glPopMatrix();
        glTranslatef(size, 0, 0);
    }
    glPopMatrix();

    glPushMatrix();
    glTranslatef(size*(fieldWidth-1),0,0);

    glTranslatef(0,size*(fieldHight-4-1),0);
    //crta gornji desni ugao obojene zice
    for (i = fieldWidth-1; i >= (fieldWidth-4)/2; i--)
    {
        glPushMatrix();
        for (j = fieldHight-4-1; j >= (fieldHight-4)/2; j--)
        {
            if (matrix[i][j])
                drawTetrisColoredWireBlock(size, matrix[i][j], 2.5);
            glTranslatef(0, -size, 0);
        }
        glPopMatrix();
        glTranslatef(-size, 0, 0);
    }
    glPopMatrix();
    glPopMatrix();
}
tetrisPiece rotatePiece(tetrisPiece toBeRotated) //TODO napravi rotaciju u kontra smeru
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
                if(piece.xPosition+i-1<fieldWidth && piece.yPosition-j-1<fieldHight)
                    matrix[piece.xPosition + i - 1][piece.yPosition - j - 1] = piece.blockMatrix[j][i] * piece.type;
}
void insertPieceIntoWall(tetrisPiece piece, int wall[fieldWidth + 2][fieldHight + 1])
{
    int i, j;
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            if (piece.blockMatrix[j][i])
                if(!(piece.xPosition + 1 >= fieldHight + 1))
                    wall[piece.xPosition + i][piece.yPosition - j] = 1; //BUG NA KRAJU IGRE!!!
}

void takeOutTetrisPiece(tetrisPiece piece, int matrix[fieldWidth][fieldHight])
{
    int i, j;
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            if (piece.blockMatrix[j][i])
                if(piece.xPosition+i-1<fieldWidth && piece.yPosition-j-1<fieldHight)
                matrix[piece.xPosition + i - 1][piece.yPosition - j - 1] = 0;
}
void drawOnePiece(int color,int matrix[4][4],int size, float angle)
{
    glPushMatrix();
    int i,j;
    size *= 0.75;
    glPushMatrix();
    glRotatef(angle,0,1,0);
    glTranslatef(-2*size,0,0);
    glRotatef(90,0,0,1);
    for(j = 0; j < 4; j++)
        {
            for(i = 0; i < 4; i++)
            {
                if(matrix[i][j])
                {
                drawTetrisColoredSolidBlock(size, color, 0.3);
                drawTetrisColoredWireBlock(size, color,3);
                }
            glTranslatef(-size,0,0);
            }
            glTranslatef(size*4,0,0);
            glTranslatef(0,-size,0);
        }
    glPopMatrix();
    glPopMatrix();
}
int doesThePieceHitTheWall(tetrisPiece piece, int wall[fieldWidth + 2][fieldHight + 1])
{
    int i, j;
    for (j = 0; j < 4; j++)
        for (i = 0; i < 4; i++)
        {
            if (piece.yPosition - j <= fieldHight)
                if (piece.blockMatrix[j][i] && wall[piece.xPosition + i][piece.yPosition - j]) //BUG
                    return 1;
        }
    return 0;
    
}
#endif