#ifndef UTILITIES_H_INCLUDED
#define UTILITIES_H_INCLUDED

#include "Cell.hpp"
#include <list>

class Block;
class Node;
class Road;

float euclideanDistance (float x1, float y1, float x2, float y2);
float euclideanDistance (Cell c, Cell d);
float sizeOfVector (float x, float y);
float sizeOfVectorN (float* const vect, int dimension);
void normalizeVectorN (float* const vect, int dimension);
float urand ();

float bilinearInterpolation (float** Matrix, float x, float y, int maxX, int maxY); // have to check before interpolation if in matrix size!
float* vectorBilinearInterpolation (float*** matrix, float x, float y, int vectorDim,int maxX, int maxY); // have to check before interpolation if in matrix size!
float*** gradientField (float** heightMap, int rows, int cols); // forward/backward difference on edges + central in center and !evenly spaced = 1!;
float* rk4(float** heightMap,float*** gradField, float x, float y, float h, int rows, int cols); //Runge-Kutta 4, h-step
float* ark4(float** heightMap,float*** gradField, float x, float y, int rows, int cols); //Adaptive Runge-Kutta 4
float* lineIntersection(float* startLine1, float* endLine1, float* startLine2, float* endLine2);
float* lineIntersection(float p0_x, float p0_y, float p1_x, float p1_y, float p2_x, float p2_y, float p3_x, float p3_y);
bool isPointOnSegment(float* point, float* pointLine1, float* pointLine2);

float degreesToRadians(float degrees);
float radiansToDegrees(float radians);

void writeToBMP (float** Matrix, int Matrix_dimension,const char nameBMP []);
void writeBMP (unsigned char** Matrix, int Matrix_dimension,const char nameBMP []);

Node* getOtherEndOfRoad(Node* here, Road* road);
float lengthOfRoad(Road* road);
float leftAngleBetweenRoads(Node* here, Road* previousRoad, Road* nextRoad);
Road* leftRoad2 (Node* here,std::list<Road*>* previousRoads,float* angle);//zobecnit a vylepsit (vyuzit curvature)
Road* rightRoad2 (Node* here,std::list<Road*>* previousRoads,float angle); // nefunguje myslim!
float computeBlockArea(Block* block); // without curvature

std::list<float*> twoFurthestLeftPointsFromLine(float* first,std::list<float*> pointsWithLastSecond);
float* furthestLeftPointFromLine(float* first,std::list<float*> pointsWithLastSecond);
float* furthestLeftPointFromLine(float* from,std::list<float*>* points, float* to);
float* getLeftNormalVector(float* from, float* to); // simplify -- only for two dimensional
//create function getNormovanyvector

float** createGaussianFilter(int size, float deviation);
float** createOnesFilter(int size);
float** convolution(float** filter, int filterSize, float** matrix, int matrixSize);
float** convolutionRestrictedEmpty(float** filter, int filterSize, float** matrix, int matrixSize, float lowerBound, float upperBound);
float** convolutionRestrictedFilled(float** filter, int filterSize, float** matrix, int matrixSize, float lowerBound, float upperBound);
#endif // UTILITIES_H_INCLUDED
