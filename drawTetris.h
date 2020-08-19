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
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
#include <stdio.h>
#include <time.h>
enum typeOfPiece{ none, O, L, Z, T, I, J, S}; //Oblik bloka koji pada, imenovani su po nazivima parcica koji opisuju njihov oblik

typedef struct tetrispiece{
    enum typeOfPiece type; //tip tetromina
    int rotation; //kako je rotiran
    int xPosition; //x koordinata
    int yPosition;//y koordinata
    int blockMatrix[4][4];//tetromino iscrtan na matrici 4x4 
} tetrisPiece;
void rotateMatrix(int matrixToRotate[4][4], int whichWayToRotate, int result[4][4]);

int desetinke();//vracs vreme u desetinkama

void drawString(float x, float y, float z, char *string);//iscrtava string

GLuint glInitTexture(char* filename); //kopirano sa vezbi, postavlja teksturu
void drawImage(GLuint file,
    float x,
    float y,
    float w,
    float h,
    float angle);
/*
    Kako funkcionise iscrtavanje tabele tetris:
    1. postoji matrica field matrix cije su dimenzije fieldWidth i fieldHight
    2. matrica se iscrtava uz pomoc funkcije drawTetrisField
    3. iscrtava samo okvir matrice (siva mreza kockica)
    4. iscrtavaju se polja matrice koja su obojena uz pomoc drawTetrisWireBlock,
    drawTetrisSolidBlock, drawTetrisColoredSolidBlock, drawTetrisColoredWireBlock
    */
void drawTetrisWireBlock(double size, double r, double g, double b, double lineWidth);

void drawTetrisSolidBlock(double size, double r, double g, double b, double blend);

void drawTetrisColoredSolidBlock(double size, int color, double blend);

void drawTetrisColoredWireBlock(double size, int color, double lineWidth);

void drawTetrisField(int size, int matrix[fieldWidth][fieldHight]);

tetrisPiece rotatePiece(tetrisPiece toBeRotated);
tetrisPiece newTetrisPiece(); //pravi random tetromino
void insertPieceIntoField(tetrisPiece piece, int matrix[fieldWidth][fieldHight]);
void insertPieceIntoWall(tetrisPiece piece, int wall[fieldWidth + 2][fieldHight + 1]);
void takeOutTetrisPiece(tetrisPiece piece, int matrix[fieldWidth][fieldHight]);
void drawOnePiece(int color,int matrix[4][4],int size, float angle);//sluzi za iscrtavanje tetromina koji je sa leve strane
int doesThePieceHitTheWall(tetrisPiece piece, int wall[fieldWidth + 2][fieldHight + 1]);
void drawTransparentSqr(float length, float r, float g, float b, float alpha);
void drawLotsOfTransparentSqrs(float length, float r, float g, float b, float alpha, int n, float koordinate [10][2]);
void drawStars();
void drawRedLights();
void drawWindowLight(float x, float y,float r,float g, float b, float t, float s);
void drawLotsOfWindowLights();
void drawKlinickiCentarLight();
void drawSmoke();
void drawBackgroundDetails();
#endif
