
#ifndef DRAWTETRIS_H
#define DRAWTETRIS_H
#define fieldHight 20
#define blend_factor 0.7
#define yellow 1
#define orange 2
#define red 3
#define purple 4
#define light_blue 5
#define dark_blue 6
#define green 7
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
                        if(matrix[i][j])
                            drawTetrisColoredSolidBlock(size,matrix[i][j],0.5);
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
                        if(matrix[i][j])
                            drawTetrisColoredWireBlock(size,matrix[i][j],2.5);
                        glTranslatef(0,size,0);
                    }
                    glPopMatrix();
                    glTranslatef(size,0,0);
                }
        glPopMatrix();
    glPopMatrix();
}

#endif