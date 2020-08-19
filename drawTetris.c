#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
#include <stdio.h>
#include <time.h>
#include "drawTetris.h"
void rotateMatrix(int matrixToRotate[4][4], int whichWayToRotate, int result[4][4])
{
    int i,j;
    switch(whichWayToRotate % 4)
    {
        case 0:
            for(i = 0; i < 4; i++)
                for(j = 0; j < 4; j++)
                    result[i][j] = matrixToRotate[i][j];
        break;
        case 1:
            for(i = 0; i < 4; i++)
                for(j = 0; j < 4; j++)
                    result[i][j] = matrixToRotate[3-j][i];
        break;
        case 2:
            for(i = 0; i < 4; i++)
                for(j = 0; j < 4; j++)
                    result[i][j] = matrixToRotate[3-i][3-j];
        break;
        case 3:
            for(i = 0; i < 4; i++)
                for(j = 0; j < 4; j++)
                    result[i][j] = matrixToRotate[j][3-i];
        break;
    }
}

int desetinke()//vracs vreme u desetinkama
{
    return(clock()/(CLOCKS_PER_SEC/10.0));
}
void drawString(float x, float y, float z, char *string)//iscrtava string
{
    glDisable(GL_DEPTH_TEST);
    glRasterPos3f(x, y, z);

    for (char *c = string; *c != '\0'; c++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c); // Updates the position
    }
    glEnable(GL_DEPTH_TEST);
}

GLuint glInitTexture(char* filename) //kopirano sa vezbi, postavlja teksturu
{
    GLuint t = 0;

    glGenTextures( 1, &t );
    glBindTexture(GL_TEXTURE_2D, t);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    unsigned char data[] = { 255, 0, 0, 255 };
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data );
    return t;
}
void drawImage(GLuint file,
    float x,
    float y,
    float w,
    float h,
    float angle)
{
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glPushMatrix();
    glTranslatef(x, y, 0.0);
    glRotatef(angle, 0.0, 0.0, 1.0);

    glBindTexture(GL_TEXTURE_2D, file);
    glEnable(GL_TEXTURE_2D);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(x, y, 0.0f);
    glTexCoord2f(0.0, 2.0); glVertex3f(x, y + h, 0.0f);
    glTexCoord2f(2.0, 2.0); glVertex3f(x + w, y + h, 0.0f);
    glTexCoord2f(2.0, 0.0); glVertex3f(x + w, y, 0.0f);
    glEnd();

    glPopMatrix();
}


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

 void drawTetrisColoredSolidBlock(double size, int color, double blend)
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

void drawTetrisColoredWireBlock(double size, int color, double lineWidth)
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

 void drawTetrisField(int size, int matrix[fieldWidth][fieldHight])
{
    int i, j;
    glPushMatrix();

    glTranslatef(-size * fieldWidth / 2, -size * (fieldHight -4) / 2, 0);
    glPushMatrix();
    //crta sivu mrezu u pozadini
    for (i = 0; i < fieldWidth; i++)
    {
        glPushMatrix();
        for (j = 0; j < fieldHight-4; j++)
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
void drawOnePiece(int color,int matrix[4][4],int size, float angle)//sluzi za iscrtavanje tetromina koji je sa leve strane
{
    glPushMatrix();
    int i,j;
    size *= 0.75;
    glPushMatrix();
    glTranslatef(-2*size,0,0);
    glRotatef(angle,0,1,0);
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
tetrisPiece rotatePiece(tetrisPiece toBeRotated) 
{
    tetrisPiece returnMe = toBeRotated;
    rotateMatrix(toBeRotated.blockMatrix, 1, returnMe.blockMatrix);
    return returnMe;
}
tetrisPiece newTetrisPiece() //pravi random tetromino
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
void drawOnePiece(int color,int matrix[4][4],int size, float angle);//sluzi za iscrtavanje tetromina koji je sa leve strane
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
void drawTransparentSqr(float length, float r, float g, float b, float alpha)
{
    glColor4f(r, g, b, alpha);
   // glEnable(GL_DEPTH_TEST);
    glBegin(GL_QUADS);
        glVertex2f(-length/2.0, length/2.0); // top left
        glVertex2f(length/2.0, length/2.0); // top right 
        glVertex2f(length/2.0, -length/2.0); // bottom right
        glVertex2f(-length/2.0, -length/2.0); // bottom left
    glEnd();
    glColor4f(1,1,1,1);
}
void drawLotsOfTransparentSqrs(float length, float r, float g, float b, float alpha, int n, float koordinate [10][2])
{
    int i;
    for(i = 0; i < n; i++)
    {
        glPushMatrix();

        glTranslatef(koordinate[i][0],koordinate[i][1],0);
        drawTransparentSqr(length,r,g,b,alpha);
        glPopMatrix();
    }
}

//Ovde ima dosta formula i brojeva koji su hardcode-ovani da bi se lepo uklopili u pozadinu
void drawStars()
{
    float koordinate1[10][2] ={{-700,1},{-650,400},{-300,300},{-211,221},{431,150},{510,403},{300,542},{400,200},{36,396},{110,334}}; 
    float koordinate2[10][2] ={{-700,1},{-593,234},{-314,17},{-292,141},{390,320},{403,302},{531,321},{112,200},{2,432},{704,20}};
    drawLotsOfTransparentSqrs(3,1,1,1,fabs(0.5 - 16.0/((desetinke()+8)%16+1))/3,10,koordinate1);
    drawLotsOfTransparentSqrs(3,1,1,0,fabs(0.5 - 16.0/(desetinke()%16+1))/3,10,koordinate2);
}

void drawRedLights()
{
    glDisable(GL_DEPTH_TEST);
    float koordinate[10][2] = {{-515,-9},{-263,18},{-263,20},{-528,-79},{-531,-10}};
    drawLotsOfTransparentSqrs(6,1,0,0,(1.0/((time(NULL)+1)%4+2)),5,koordinate); //koristim ostatke koji se dobijaju pri deljenju vremena sa nekim brojem
                                                                                    //za koeficijent transparentnosti da bi se dobio efekat postepenog 
                                                                                    //povecavanja inteziteta svetlosti i naglo gasenje
    glEnable(GL_DEPTH_TEST);
}

void drawWindowLight(float x, float y,float r,float g, float b, float t, float s)
{
    glPushMatrix();
    glDisable(GL_DEPTH_TEST);
    glTranslatef(x, y, 0);
    drawTransparentSqr(s,r,g,b,t);
    glEnable(GL_DEPTH_TEST);
    glPopMatrix();
}

void drawLotsOfWindowLights()
{
    int i;
    int numZute =4;
    int numBele = 4;
    int numMaleZute = 6;
    float koordinateZute[10][2] = {{-412,-238},{192,-240},{221,-345},{265,-345}};
    float koordinateBele[10][2] = {{-60,-156},{-121, -213},{-630,-130},{-500,-410}};
    float koordinateMaleZute[10][2] = {{35,-9},{69,-9},{135,-4},{571,-4},{589,-4},{606,-4}};
    for(i = 0; i < numZute; i++)
    {
        drawWindowLight(koordinateZute[i][0],koordinateZute[i][1], 1,0.8,0,1.0/((time(NULL)%2+1)*5),30);
    }
    for(i = 0; i < numBele; i++)
    {
        drawWindowLight(koordinateBele[i][0],koordinateBele[i][1], 1,1,1,1.0/(((time(NULL)+1)%2+1)*5),20);
    }
    for(i = 0; i < numMaleZute; i++)
    {
        drawWindowLight(koordinateMaleZute[i][0],koordinateMaleZute[i][1], 1,0.8,0,1.0/(((time(NULL)+1)%5+1)*3),15);
    }
}
void drawKlinickiCentarLight()
{
    glPushMatrix();
    glDisable(GL_DEPTH_TEST);
    glTranslatef(-380, -74, 0);
    glScalef(6,1.5,1);
    drawTransparentSqr(20,1,1,1,1.0/(time(NULL)%7+3));
    glEnable(GL_DEPTH_TEST);
    glPopMatrix();
}
void drawSmoke()
{
    glDisable(GL_DEPTH_TEST);
    glPushMatrix();
    glTranslatef(-516,65 + (desetinke()%15)*10,0);
    glColor4f(0.7,0.6,1,(1-desetinke()%15/15.0)/2);
    glutSolidSphere((desetinke()%15+1)*1.5,10,10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-530,70 + (desetinke()+3)%11*10,0);
    glColor4f(0.75,0.6,1,(1-(desetinke()+3)%11/11.0)/2);
    glutSolidSphere((((desetinke()+3)%11)+1)*1.5,10,10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-530,70 + (desetinke()+8)%11*10,0);
    glColor4f(0.65,0.65,1,(1-(desetinke()+8)%11/11.0)/2);
    glutSolidSphere((((desetinke()+8)%11)+1)*1.5,10,10);
    glPopMatrix();

    glEnable(GL_DEPTH_TEST);

}
void drawBackgroundDetails()
{
    glPushMatrix();
    drawStars();
    drawRedLights();
    drawLotsOfWindowLights();
    drawKlinickiCentarLight();
    drawSmoke();
    glPopMatrix();
}
