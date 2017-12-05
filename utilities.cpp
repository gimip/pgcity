#include "utilities.hpp"

#include "Block.hpp"
#include "Node.hpp"
#include "Road.hpp"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <stdint.h>

//  #include <cstdio>
//  FILE* file = fopen("??.csv","w+");
//  for (int i =0 ; i< 257; i++)
//  {
//    for (int j = 0;j<257; j++)
//    {
//      fprintf(file,"%d,",(int)??[i][j]);
//    }
//    fprintf(file,"\n");
//  }
//  fclose(file);

float euclideanDistance (Cell c, Cell d)
{
  return euclideanDistance( (float)c.getRow(), (float)c.getCol(), (float)d.getRow(), (float)d.getCol());
}

float euclideanDistance (float x1, float y1, float x2, float y2)
{
  return sqrtf(powf(x1-x2, 2.0)+powf(y1-y2, 2.0));
}

float sizeOfVector (float x, float y)
{
  return sqrtf(powf(x, 2.0)+powf(y, 2.0));
}

float sizeOfVectorN (float* const vect, int dimension)
{
  float size = 0.0;
  for(int i=0; i<dimension; i++)
  {
    size += powf(vect[i], 2.0);
  }
  return sqrtf(size);
}

void normalizeVectorN (float* const vect, int dimension)
{
  float size = sizeOfVectorN (vect,dimension);
  for(int i=0; i<dimension; i++)
  {
    vect[i] /= size;
  }
}

float urand ()
{
  float r1 = (float)rand() / (float)RAND_MAX;
  float r2 = (float)rand();
  if (r2 > (((float)RAND_MAX) / 2.0))
  {
    return r1;
  }
  return (-r1);
}

float bilinearInterpolation (float** matrix, float x, float y, int maxX, int maxY) // have to check before interpolation if in matrix size!
{ // x - row, y - col
  // zkratit na int(x)
  maxX--;
  maxY--;
  if (x > maxX && y > maxY)
  {
    return matrix[maxX][maxY];
  }
  if (x > maxX)
  {
    x = maxX;
  }
  else if (x < 0)
  {
    x = 0;
  }
  if (y > maxY)
  {
    y = maxY;
  }
  else if (y < 0)
  {
    y = 0;
  }
  float fx = floorf(x); // f -> floor
  float fy = floorf(y);
  float cx = fx+1;      // c -> ceiling
  float cy = fy+1;
  if (fx > maxX)
  {
    fx = maxX;
  }
  if (fy > maxY)
  {
    fy = maxY;
  }
  if (cx > maxX)
  {
    cx = maxX;
  }
  if (cy > maxY)
  {
    cy = maxY;
  }
  int ifx = (int) fx;
  int ify = (int) fy;
  int icx = (int) cx;
  int icy = (int) cy;
  float f =
  matrix[ifx][ify] * (cx - x) * (cy - y) +
  matrix[ifx][icy] * (cx - x) * (y - fy) +
  matrix[icx][ify] * (x - fx) * (cy - y) +
  matrix[icx][icy] * (x - fx) * (y - fy);
  return f;
}

float* vectorBilinearInterpolation (float*** matrix, float x, float y, int vectorDim, int maxX, int maxY) // have to check before interpolation if in matrix size!
{ // x - row, y - col
  maxX--;
  maxY--;
  float fx,fy,cx,cy;
  int ifx,ify,icx,icy;
  float* f = new float[vectorDim];
  for (int i=0; i<vectorDim; i++)
  {
    fx = floorf(x); // f -> floor
    fy = floorf(y);
    cx = fx+1;      // c -> ceiling
    cy = fy+1;
    if (cx > maxX)
    {
      cx = maxX;
    }
    if (cy > maxY)
    {
      cy = maxY;
    }
    ifx = (int) fx;
    ify = (int) fy;
    icx = (int) cx;
    icy = (int) cy;
    f[i] =
    matrix[ifx][ify][i] * (cx - x) * (cy - y) +
    matrix[icx][ify][i] * (cx - x) * (y - fy) +
    matrix[ifx][icy][i] * (x - fx) * (cy - y) +
    matrix[icx][icy][i] * (x - fx) * (y - fy);
  }
  return f;
}

float*** gradientField (float** heightMap, int rows, int cols) // forward/backward difference on edges + central in center and !evenly spaced = 1!;
{
//  [0] - gradCol
//  [1] - gradRow
  float*** gradF = new float**[rows];
  for(int i = 0; i < rows; i++)
  {
    gradF[i] = new float*[cols];
  }
  for (int i=0; i<rows; i++)
  {
    for (int j=0; j<cols; j++)
    {
      gradF[i][j] = new float[2];
    }
  }

  rows--;
  cols--;

  //centre
  for (int i=1; i<rows; i++)
  {
    for (int j=1; j<cols; j++)
    {
      gradF[i][j][0] = (heightMap[i][j+1] - heightMap[i][j-1]) / 2.0;
      gradF[i][j][1] = (heightMap[i+1][j] - heightMap[i-1][j]) / 2.0;
    }
  }
  //edges
  for (int i=1; i<rows; i++)
  {
    gradF[i][0][0] = heightMap[i][1] - heightMap[i][0];
    gradF[i][0][1] = (heightMap[i+1][0] - heightMap[i-1][0]) / 2.0;
  }
  for (int i=1; i<rows; i++)
  {
    gradF[i][cols][0] = heightMap[i][cols] - heightMap[i][cols-1];
    gradF[i][cols][1] = (heightMap[i+1][0] - heightMap[i-1][0]) / 2.0;
  }
  for (int j=1; j<cols; j++)
  {
    gradF[0][j][0] = (heightMap[0][j+1] - heightMap[0][j-1]) / 2.0;
    gradF[0][j][1] = heightMap[0][j] - heightMap[1][j];
  }
  for (int j=1; j<cols; j++)
  {
    gradF[rows][j][0] = (heightMap[0][j+1] - heightMap[0][j-1]) / 2.0;
    gradF[rows][j][1] = heightMap[rows-1][j] - heightMap[rows][j];
  }
  //corners
  gradF[0][0][0] = heightMap[0][1] - heightMap[0][0];
  gradF[rows][0][0] = heightMap[rows][1] - heightMap[rows][0];
  gradF[0][cols][0] = heightMap[0][cols] - heightMap[0][cols-1];
  gradF[rows][cols][0] = heightMap[0][cols] - heightMap[0][cols-1];

  gradF[0][0][1] = heightMap[0][0] - heightMap[1][0];
  gradF[0][cols][1] = heightMap[0][cols] - heightMap[1][cols];
  gradF[rows][0][1] = heightMap[rows-1][0] - heightMap[rows][0];
  gradF[rows][cols][1] = heightMap[rows-1][cols] - heightMap[rows][cols];
//  return gradF;

  float*** agradF = new float**[rows+1];
  for(int i = 0; i < rows+1; i++)
  {
    agradF[i] = new float*[cols+1];
  }
  for (int i=0; i<rows+1; i++)
  {
    for (int j=0; j<cols+1; j++)
    {
      agradF[i][j] = new float[2];
    }
  }
  for (int i=0; i<rows+1; i++)
  {
  for (int j=0; j<cols+1; j++)
    {
      agradF[i][j][0] = gradF[i][j][1];
      agradF[i][j][1] = gradF[i][j][0];
    }
  }
    return agradF;

}

void repairXY (float firstX, float firstY, float &x, float &y, int maxX, int maxY) //get x and y -> matrix size
{
  maxX--;
  maxY--;
  float helpx = x;
  float helpy = y;
  if (x > maxX)
  {
    float k = ((float)maxX-firstX) / (x-firstX);
    x = maxX;
    y += -k * (y-firstY);
  }
  if (x < 0.0)
  {
    float k = (firstX) / (x-firstX);
    x = 0.0;
    y += k * (y-firstY);
  }
  if (y > maxY)
  {
    float k = (firstY-(float)maxY) / (y-firstY);
    x += -k * (x-firstX);
    y = maxY;
  }
  if (y < 0.0)
  {
    float k = (firstY) / (y-firstY);
    x += k * (x-firstX);
    y = 0.0;
  }
}

float* rk4(float** heightMap,float*** gradField, float x, float y, float h, int rows, int cols) //Runge-Kutta 4
{ // x - row, y - col
  float* newPoint = new float[2]; //x,y
  float* v1,*v2,*v3,*v4;
  float tmpx,tmpy;

  v1 = vectorBilinearInterpolation(gradField,x,y,2,rows,cols);
  tmpx = x + (0.5 * h) * v1[0];
  tmpy = y + (0.5 * h) * v1[1];
  repairXY(x,y,tmpx,tmpy,rows,cols);

  v2 = vectorBilinearInterpolation(gradField,tmpx,tmpy,2,rows,cols);
  tmpx = x + (0.5 * h) * v2[0];
  tmpy = y + (0.5 * h) * v2[1];
  repairXY(x,y,tmpx,tmpy,rows,cols);

  v3 = vectorBilinearInterpolation(gradField,tmpx,tmpy,2,rows,cols);
  tmpx = x + h * v3[0];
  tmpy = y + h * v3[1];
  repairXY(x,y,tmpx,tmpy,rows,cols);

  v4 = vectorBilinearInterpolation(gradField,tmpx,tmpy,2,rows,cols);

  newPoint [0] = x + (1.0/6.0) * h * (v1[0] + 2*v2[0] + 2*v3[0] + v4[0]);
  newPoint [1] = y + (1.0/6.0) * h * (v1[1] + 2*v2[1] + 2*v3[1] + v4[1]);
  repairXY(x,y,newPoint [0],newPoint [1],rows,cols);

  delete[] v1;
  delete[] v2;
  delete[] v3;
  delete[] v4;
  return newPoint;
}

float maxARK4(float* v1,float* v3,float* v4,float* v5,float h)
{
  float epsilon1 = (h/3.0) * fabsf(0.2 * v1[0] - 0.9 * v3[0] + 0.8 * v4[0] - 0.1 * v5[0]);
  float epsilon2 = (h/3.0) * fabsf(0.2 * v1[1] - 0.9 * v3[1] + 0.8 * v4[1] - 0.1 * v5[1]);
  if (epsilon2 > epsilon1)
  {
    return epsilon2;
  }
  return epsilon1;
}

float* ark4(float** heightMap,float*** gradField, float x, float y, int rows, int cols) //Adaptive Runge-Kutta 4
{ // x - row, y - col
  float* newPoint = new float[2]; //x,y
  float *v1,*v2,*v3,*v4,*v5;
  float tmpx,tmpy;
  float epsilon, delta = 0.05;
  float h = 1.0;

  while (true)
  {
    v1 = vectorBilinearInterpolation(gradField,x,y,2,rows,cols);
    tmpx = x + ((1.0/3.0) * h) * v1[0];
    tmpy = y + ((1.0/3.0) * h) * v1[1];
    repairXY(x,y,tmpx,tmpy,rows,cols);

    v2 = vectorBilinearInterpolation(gradField,tmpx,tmpy,2,rows,cols);
    tmpx = x + ((1.0/6.0) * h) * (v1[0] + v2[0]);
    tmpy = y + ((1.0/6.0) * h) * (v1[1] + v2[1]);
    repairXY(x,y,tmpx,tmpy,rows,cols);

    v3 = vectorBilinearInterpolation(gradField,tmpx,tmpy,2,rows,cols);
    tmpx = x + ((1.0/8.0) * h) * (v1[0] + 3.0 * v3[0]);
    tmpy = y + ((1.0/8.0) * h) * (v1[1] + 3.0 * v3[1]);
    repairXY(x,y,tmpx,tmpy,rows,cols);

    v4 = vectorBilinearInterpolation(gradField,tmpx,tmpy,2,rows,cols);
    tmpx = x + h * ((1.0/2.0) * v1[0] - (3.0/2.0) * v3[0] + 2.0 * v4[0]);
    tmpy = y + h * ((1.0/2.0) * v1[1] - (3.0/2.0) * v3[1] + 2.0 * v4[1]);
    repairXY(x,y,tmpx,tmpy,rows,cols);

    v5 = vectorBilinearInterpolation(gradField,tmpx,tmpy,2,rows,cols);
    epsilon = maxARK4(v1,v3,v4,v5,h);
    if (epsilon < delta)
    {
      newPoint [0] = x + (1.0/6.0) * h * (v1[0] + 4*v4[0] + v5[0]);
      newPoint [1] = y + (1.0/6.0) * h * (v1[1] + 4*v4[1] + v5[1]);
      repairXY(x,y,newPoint [0],newPoint [1],rows,cols);
      break;
    }
    h *= pow(delta/epsilon,0.2) * 0.8;
  }

  delete[] v1;
  delete[] v2;
  delete[] v3;
  delete[] v4;
  delete[] v5;
  return newPoint;
}

float* lineIntersection(float* startLine1, float* endLine1, float* startLine2, float* endLine2)
{
  return lineIntersection(startLine1[0],startLine1[1],endLine1[0],endLine1[1],startLine2[0],startLine2[1],endLine2[0],endLine2[1]);
}

float* lineIntersection(float p0_x, float p0_y, float p1_x, float p1_y, float p2_x, float p2_y, float p3_x, float p3_y)
{ // NULL - no intersection, otherwise - x,y of intersection
  float s1_x, s1_y, s2_x, s2_y;
  s1_x = p1_x - p0_x;
  s1_y = p1_y - p0_y;
  s2_x = p3_x - p2_x;
  s2_y = p3_y - p2_y;

  float s, t;
  s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
  t = ( s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

  float eps = 0.001;
  if (s >= -eps && s <= 1+eps && t >= -eps && t <= 1+eps)
  {
    float* i = new float[2];
    i[0] = p0_x + (t * s1_x);
    i[1] = p0_y + (t * s1_y);
    return i;
  }
  return NULL;
}

bool isPointOnSegment(float* point, float* pointLine1, float* pointLine2) // pridat epsilon
{
  float x = pointLine2[0] - pointLine1[0];
  float y = pointLine2[1] - pointLine1[1];
  float t2 = (point [0] - pointLine1[0]) / x;
  float t1 = (point [1] - pointLine1[1]) / y;
  float eps = 0.005;
  if ((t1 >= (t2 - eps) && t1 <= (t2 + eps) && t1 >= -eps && t1 <= 1+eps) ||
      (fabsf(x) <= eps && fabsf(point[0] - pointLine1[0]) <= eps && t1 >= -eps && t1 <= 1+eps) ||
      (fabsf(y) <= eps && fabsf(point[1] - pointLine1[1]) <= eps && t2 >= -eps && t2 <= 1+eps))
  {
    return true;
  }
  return false;
}

float degreesToRadians(float degrees)
{
  return degrees * (M_PI / 180);
}

float radiansToDegrees(float radians)
{
  return radians * (180 / M_PI);
}

void writeToBMP(float **matrix, int matrix_dimension,const char nameBMP [])
{
  unsigned char **a;
  a = new unsigned char*[matrix_dimension];
  for(int i = 0; i < matrix_dimension; i++)
  {
    a[i] = new unsigned char[matrix_dimension];
  }

  for (int i=0; i<matrix_dimension; i++)
  {
    for (int j=0; j<matrix_dimension; j++)
    {
      a[i][j] = 0;
    }
  }

  for (int i=0; i<matrix_dimension; i++)
  {
    for (int j=0; j<matrix_dimension; j++)
    {
      if (matrix[matrix_dimension-i-1][j] < 0.0) // zmenit cyklus for i na odcitani
      {
        a[i][j] = 0;
      }
      else if (matrix[matrix_dimension-i-1][j] > 255.0)
      {
        a[i][j] = 255;
      }
      else
      {
        a[i][j] =  (unsigned char) matrix[matrix_dimension-i-1][j];
      }
    }
  }

  writeBMP(a,matrix_dimension,nameBMP);
  for(int i = 0; i < matrix_dimension; i++)
    {
      delete [] a[i];
    }
  delete [] a;
  //temp/
}

void writeBMP(unsigned char **Matrix, int Matrix_dimension,const char nameBMP [])
{
    FILE *out;
    int ii,jj;
    long pos = 1077;

    out = fopen(nameBMP,"wb");

    // SEEK_SET - Beginning of file

    // Image Signature
    unsigned char signature[2] = {'B','M'};
    fseek(out,0,SEEK_SET);
    fwrite(&signature,2,1,out);


    // Image file size
    uint32_t filesize = 54 + 4*256 + Matrix_dimension*Matrix_dimension;
    fseek(out,2,SEEK_SET);
    fwrite(&filesize,4,1,out);


    // Reserved
    uint32_t reserved = 0;
    fseek(out,6,SEEK_SET);
    fwrite(&reserved,4,1,out);


    // Offset
    uint32_t offset = 1078;
    fseek(out,10,SEEK_SET);
    fwrite(&offset,4,1,out);


   // Info header size
    uint32_t ihsize = 40;
    fseek(out,14,SEEK_SET);
    fwrite(&ihsize,4,1,out);


    // Image Width in pixels
    uint32_t width = (uint32_t) Matrix_dimension;
    fseek(out,18,SEEK_SET);
    fwrite(&width,4,1,out);


    // Image Height in pixels
    uint32_t height = (uint32_t) Matrix_dimension;
    fseek(out,22,SEEK_SET);
    fwrite(&height,4,1,out);


    // Number of planes
    uint16_t planes = 1;
    fseek(out,26,SEEK_SET);
    fwrite(&planes,2,1,out);


    // Color depth, BPP (bits per pixel)
    uint16_t bpp = 8;
    fseek(out,28,SEEK_SET);
    fwrite(&bpp,2,1,out);


    // Compression type
    uint32_t compression = 0;
    fseek(out,30,SEEK_SET);
    fwrite(&compression,4,1,out);


    // Image size in bytes
    uint32_t imagesize = (uint32_t) Matrix_dimension*Matrix_dimension;
    fseek(out,34,SEEK_SET);
    fwrite(&imagesize,4,1,out);


    // Xppm
    uint32_t xppm = 0; // 2835
    fseek(out,38,SEEK_SET);
    fwrite(&xppm,4,1,out);


    // Yppm
    uint32_t yppm = 0; // 2835
    fseek(out,42,SEEK_SET);
    fwrite(&yppm,4,1,out);


    // Number of color used (NCL)
    uint32_t colours = 256;
    fseek(out,46,SEEK_SET);
    fwrite(&colours,4,1,out);


    // Number of important color (NIC)
    // value = 0 means all collors important
    uint32_t impcolours = 0;
    fseek(out,50,SEEK_SET);
    fwrite(&impcolours,4,1,out);


    // Colour table
    unsigned char bmpcolourtable[1024];
    for(ii=0; ii < 1024; ii++){
        bmpcolourtable[ii] =  0;
    }
    jj=3;
    for(ii=0; ii < 255; ii++){
        bmpcolourtable[jj+1] =  ii+1;
        bmpcolourtable[jj+2] =  ii+1;
        bmpcolourtable[jj+3] =  ii+1;
        jj=jj+4;
    }

    fseek(out,54,SEEK_SET);
    fwrite(&bmpcolourtable,256,4,out);

    // bmp writing
    unsigned char padding = 0,pitch;
    for(ii=0;ii<Matrix_dimension;ii++)
    {
        for(jj=0;jj<Matrix_dimension;jj++)
        {
            pos += 1;
            fseek(out,pos,SEEK_SET);
            fwrite(&Matrix[ii][jj],(sizeof(unsigned char)),1,out);
        }

        // padding
        pitch = (((Matrix_dimension * 24) + 31) / 32) * 4;
        for (int k = 1; k<pitch; k++)
        {
          pos += 1;
          fseek(out,pos,SEEK_SET);
          fwrite(&padding,2,1,out);
        }
    }

    fflush(out);
    fclose(out);
}

Node* getOtherEndOfRoad(Node* here, Road* road)
{
  if (road->from == here)
  {
    return road->to;
  }
  if (road->to == here)
  {
    return road->from;
  }
  return NULL;
}

float lengthOfRoad(Road* road)
{
  float* firstNode = new float [2];
  firstNode[0] = road->from->x;
  firstNode[1] = road->from->y;
  std::list<float*>* curvature = road->curvature;
  float* secondNode;
  float length = 0.0;
  if (!curvature->empty())
  {
    for (std::list<float*>::iterator nodeIt = curvature->begin(); nodeIt != curvature->end(); nodeIt++)
    {
      secondNode = *nodeIt;
      length += euclideanDistance(firstNode[0],firstNode[1],secondNode[0],secondNode[1]);
      firstNode = secondNode;
    }
  }
  secondNode = new float [2];
  secondNode[0] = road->to->x;
  secondNode[1] = road->to->y;
  length += euclideanDistance(firstNode[0],firstNode[1],secondNode[0],secondNode[1]);
  delete firstNode;
  delete secondNode;
  return length;
}

// docasne
Road* leftRoad2 (Node* here,std::list<Road*>* previousRoads,float* angle)//zobecnit a vylepsit (vyuzit curvature)
{
//  atan2(y,x);
  std::list<Road*>* possibleRoads = here->roads;
  Road* previousRoad = *(previousRoads->begin()); // WARNING here using push_front!
  Node* previousNode;

  previousNode = getOtherEndOfRoad(here,previousRoad);

  float previousAngle = /*360.0 -*/ ((atan2(here->x - previousNode->x,here->y - previousNode->y) * 180.0 / M_PI) + 180.0);
  Road* leftRoad = NULL;

  for (std::list<Road*>::iterator possibleRoadIt = possibleRoads->begin(); possibleRoadIt != possibleRoads->end(); ++possibleRoadIt)
  {
    Road* newRoad = *possibleRoadIt;
    Node* newNode = getOtherEndOfRoad(here,newRoad);
    bool alreadyVisited = false;

    for (std::list<Road*>::iterator previousRoadIt = previousRoads->begin(); previousRoadIt != previousRoads->end(); ++previousRoadIt)
    {
      if (newRoad == *previousRoadIt)
      {
        alreadyVisited = true;
        break;
      }
    }

    if (!alreadyVisited)
    {
      float newAngle = (atan2(here->x - newNode->x,here->y - newNode->y) * 180.0 / M_PI) + 180.0;
      if (newAngle < previousAngle)
      {
        newAngle += 360.0;
      }
      if (newAngle - previousAngle <= *angle)
      {
        *angle = newAngle - previousAngle;
        float kravina = *angle;
        leftRoad = newRoad;
      }
    }
  }
  return leftRoad; //NULL - blind alley
}
// docasne
float leftAngleBetweenRoads(Node* here, Road* previousRoad, Road* nextRoad)
{
  std::list<Road*> previousRoads;
  previousRoads.push_back(previousRoad);
  float angle = 360.0;
  leftRoad2(here,&previousRoads,&angle);
  return angle;
}

// docasne
Road* rightRoad2 (Node* here,std::list<Road*>* previousRoads,float angle)//zobecnit a vylepsit (vyuzit curvature)
{
//  atan2(y,x);
  std::list<Road*>* possibleRoads = here->roads;
  Road* previousRoad = *(previousRoads->begin()); // WARNING here using push_front!
  Node* previousNode;

  previousNode = getOtherEndOfRoad(here,previousRoad);

  float previousAngle = /*360.0 -*/ ((atan2(here->x - previousNode->x,here->y - previousNode->y) * 180.0 / M_PI) + 180.0);
  Road* leftRoad = NULL;

  for (std::list<Road*>::iterator possibleRoadIt = possibleRoads->begin(); possibleRoadIt != possibleRoads->end(); ++possibleRoadIt)
  {
    Road* newRoad = *possibleRoadIt;
    Node* newNode = getOtherEndOfRoad(here,newRoad);
    bool alreadyVisited = false;

    for (std::list<Road*>::iterator previousRoadIt = previousRoads->begin(); previousRoadIt != previousRoads->end(); ++previousRoadIt)
    {
      if (newRoad == *previousRoadIt)
      {
        alreadyVisited = true;
        break;
      }
    }

    if (!alreadyVisited)
    {
      float newAngle = (atan2(here->x - newNode->x,here->y - newNode->y) * 180.0 / M_PI) + 180.0;
      if (newAngle < previousAngle)
      {
        newAngle += 360.0;
      }
      if (newAngle - previousAngle >= angle)
      {
        angle = newAngle - previousAngle;
        float kravina = angle;
        leftRoad = newRoad;
      }
    }
  }
  return leftRoad; //NULL - blind alley
}
// docasne

float computeBlockArea(Block* block) // without curvature
{
  std::list<Road*>::iterator bordersIt = block->borders->begin();
  Road* firstBorder = *bordersIt;
  bordersIt++;
  Road* secondBorder = *bordersIt;
  Node* firstNode = firstBorder->from;
  Node* secondNode = firstBorder->to;
  if (secondBorder->from == firstNode || secondBorder->to == firstNode)
  {
    firstNode = firstBorder->to;
    secondNode = firstBorder->from;
  }
  float area = firstNode->x*secondNode->y - firstNode->y*secondNode->x;

  while(bordersIt != block->borders->end())
  {
    secondBorder = *bordersIt;
    firstNode = secondNode;
    secondNode = getOtherEndOfRoad(firstNode,secondBorder);
    area += firstNode->x*secondNode->y - firstNode->y*secondNode->x;
    bordersIt++;
  }
  return fabs(area/2.0);
}

std::list<float*> twoFurthestLeftPointsFromLine(float* first,std::list<float*> pointsWithLastSecond)
{
  std::list<float*> twoFurthestPoints;
  float* one = furthestLeftPointFromLine(first,pointsWithLastSecond);
  for (std::list<float*>::iterator pointsWithLastSecondIt = pointsWithLastSecond.begin(); pointsWithLastSecondIt != pointsWithLastSecond.end(); pointsWithLastSecondIt++)
  {
    float* point = *pointsWithLastSecondIt;
    if (one == point)
    {
      pointsWithLastSecond.erase(pointsWithLastSecondIt);
      break;
    }
  }
  float* two = furthestLeftPointFromLine(first,pointsWithLastSecond);
  if (one == NULL || two == NULL)
  {
    twoFurthestPoints.push_back(one);
    twoFurthestPoints.push_back(two);
  }
  else
  {
    for (std::list<float*>::iterator pointsWithLastSecondIt = pointsWithLastSecond.begin(); pointsWithLastSecondIt != pointsWithLastSecond.end(); pointsWithLastSecondIt++)
    {
      float* point = *pointsWithLastSecondIt;
      if (one == point)
      {
        twoFurthestPoints.push_back(one);
      }
      if (two == point)
      {
        twoFurthestPoints.push_back(two);
      }
    }
  }
  return twoFurthestPoints;
}

float* furthestLeftPointFromLine(float* first,std::list<float*> pointsWithLastSecond)
{
  std::list<float*>::iterator pointsWithLastSecondIt = pointsWithLastSecond.end();
  pointsWithLastSecondIt--;
  float* second = *pointsWithLastSecondIt;
  pointsWithLastSecond.erase(pointsWithLastSecondIt);
  return furthestLeftPointFromLine(first, &pointsWithLastSecond, second);
}

float* furthestLeftPointFromLine(float* from,std::list<float*>* points, float* to)
{
  float* normalVector = getLeftNormalVector(from,to);
  float c = from[0]*to[1] - from[1]*to[0];
  float maxDistance = 0.0,distance;
  float* mostDistantPoint = NULL;
  for (std::list<float*>::iterator pointsIt = points->begin(); pointsIt != points->end(); pointsIt++)
  {
    float* point = *pointsIt;
    distance = normalVector[0]*point[0] + normalVector[1]*point[1]+c;
    if (maxDistance < distance)
    {
      maxDistance = distance;
      mostDistantPoint = point;
    }
  }
  return mostDistantPoint;
}

float* getLeftNormalVector(float* from, float* to) // simplify
{
  float* vect = new float [2];
  float x = to[0] - from[0];
  float y = to[1] - from[1];
  if (x < 0.0 && y > 0.0) //add epsilon
  {
    vect[0] = y;
    vect[1] = -x;
  }
  else if (x > 0.0 && y < 0.0) //add epsilon
  {
    vect[0] = y;
    vect[1] = -x;
  }
  else
  {
    vect[0] = y;
    vect[1] = -x;
  }
  return vect;
}

float** createGaussianFilter(int size, float deviation)
{
  int centre = (size-1)/2;
  float** gauss = new float* [size];
    for (int i = 0; i < size; i++)
  {
    gauss[i] = new float [size];
  }

  for (int i = 0; i < size; i++)
  {
    for (int j = 0; j < size; j++)
    {
      gauss[i][j] = (1.0 / (2.0 * M_PI * pow(deviation, 2.0)))
      * exp(-(pow((float)(i-centre), 2.0) + pow((float)(j-centre), 2.0)) / (2.0 * deviation));
    }
  }
  return gauss;
}

float** createOnesFilter(int size)
{
  float** ones = new float* [size];
  for (int i = 0; i < size; i++)
  {
    ones[i] = new float [size];
  }

  for (int i = 0; i < size; i++)
  {
    for (int j = 0; j < size; j++)
    {
      ones[i][j] = 1.0;
    }
  }
  return ones;
}

float** convolution(float** filter, int filterSize, float** matrix, int matrixSize)
{
  int centre = (filterSize-1)/2;
  float** returnMatrix = new float* [matrixSize];
  for (int i = 0; i < matrixSize; i++)
  {
    returnMatrix[i] = new float [matrixSize];
  }

  for (int i = 0; i < matrixSize; i++)
  {
    for (int j = 0; j < matrixSize; j++)
    {
      float tmp = 0.0;
      float counter = 0;
      for (int k = 0; k < filterSize; k++)
      {
        for (int l = 0; l < filterSize; l++)
        {
          int newI = i-filterSize+centre+1+k;
          int newJ = j-filterSize+centre+1+l;
          if (newI >= 0 && newI < matrixSize && newJ >= 0 && newJ < matrixSize)
          {
            tmp += matrix[newI][newJ] * filter[k][l];
            counter += filter[k][l];
          }
        }
      }
      returnMatrix[i][j] = tmp / counter;
    }
  }
  return returnMatrix;
}

float** convolutionRestrictedEmpty(float** filter, int filterSize, float** matrix, int matrixSize, float lowerBound, float upperBound)
{
  int centre = (filterSize-1)/2;
  float** returnMatrix = new float* [matrixSize];
  for (int i = 0; i < matrixSize; i++)
  {
    returnMatrix[i] = new float [matrixSize];
  }

  for (int i = 0; i < matrixSize; i++)
  {
    for (int j = 0; j < matrixSize; j++)
    {
      if (!( upperBound > matrix[i][j] && matrix[i][j] >= lowerBound))
      {
        returnMatrix[i][j] = 0.0;
        continue;
      }
      float tmp = 0.0;
      float counter = 0;
      for (int k = 0; k < filterSize; k++)
      {
        for (int l = 0; l < filterSize; l++)
        {
          int newI = i-filterSize+centre+1+k;
          int newJ = j-filterSize+centre+1+l;
          if (newI >= 0 && newI < matrixSize && newJ >= 0 && newJ < matrixSize)
          {
            tmp += matrix[newI][newJ] * filter[k][l];
            counter += filter[k][l];
          }
        }
      }
      returnMatrix[i][j] = tmp / counter;
    }
  }
  return returnMatrix;
}

float** convolutionRestrictedFilled(float** filter, int filterSize, float** matrix, int matrixSize, float lowerBound, float upperBound)
{
  int centre = (filterSize-1)/2;
  float** returnMatrix = new float* [matrixSize];
  for (int i = 0; i < matrixSize; i++)
  {
    returnMatrix[i] = new float [matrixSize];
  }

  for (int i = 0; i < matrixSize; i++)
  {
    for (int j = 0; j < matrixSize; j++)
    {
      if (!( upperBound > matrix[i][j] && matrix[i][j] >= lowerBound))
      {
        returnMatrix[i][j] = matrix[i][j];
        continue;
      }
      float tmp = 0.0;
      float counter = 0;
      for (int k = 0; k < filterSize; k++)
      {
        for (int l = 0; l < filterSize; l++)
        {
          int newI = i-filterSize+centre+1+k;
          int newJ = j-filterSize+centre+1+l;
          if (newI >= 0 && newI < matrixSize && newJ >= 0 && newJ < matrixSize)
          {
            tmp += matrix[newI][newJ] * filter[k][l];
            counter += filter[k][l];
          }
        }
      }
      returnMatrix[i][j] = tmp / counter;
    }
  }
  return returnMatrix;
}
