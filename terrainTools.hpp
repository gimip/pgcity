#ifndef TERRAINUTILITIES_H_INCLUDED
#define TERRAINUTILITIES_H_INCLUDED

#include "Cell.hpp"
#include <queue>

float** skelet(int init, int height, int width);
void nextRidgePoint (float** heM, int coordX, int coordY, int altitude, int orientation, int width, int iter);

void mDBU (float** skelet, int height, int width, float ibu, float dMax); //prozatim specialni hodnoty bude treba jednoducha modifikace
float deltaBU (float e, float d, float ibu, float dMax);
void triangleEdgeAscendants (Cell E, std::queue<Cell>* ascendantsE, int height, int width);
void triangleEdgeAscendantsAtRegion (Cell E, std::queue<Cell>* ascendantsE, int x1, int y1, int x2, int y2);
float sigma (float Ibu);

void midD (float** constraintMap, int height, int width);
void triangleEdge (float** constraintMap, int iter, int maxIter, int x1, int y1, int x2, int y2);
float delta (float rt, float rs, int r, int n, float H);

#endif // TERRAINUTILITIES_H_INCLUDED
