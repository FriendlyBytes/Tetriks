#ifndef CONSTANTS_H
#define CONSTANTS_H


enum typeOfPiece{ none, O, L, Z, T, I, J, S}; //Oblik bloka koji pada, imenovani su po nazivima parcica koji opisuju njihov oblik

typedef struct tetrispiece{
    enum typeOfPiece type; //what piece
    int rotation; //the way it's rotated
    int xPosition; //its location on the x axis
    int yPosition;//its location on the y axis
    int blockMatrix[4][4];
} tetrisPiece;
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

#endif