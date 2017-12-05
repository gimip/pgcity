#include "terrainTools.hpp"

#include "utilities.hpp"
#include "constants.hpp"

#include <map>
#include <math.h>
#include <stdlib.h>
//#include <time.h>

typedef std::map<Cell, std::queue<Cell> >::iterator iteratorHashtable;

#include <stdio.h>
void draw(int magnify,int width,int height,std::list<std::list<float*>* >* hslList)
{
  FILE* file = fopen("debug/ridges.svg","w+");
  float multiply = (float) magnify;
  fprintf(file,"<svg width=\"%dpx\" height=\"%dpx\" xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" >\n", (width+1)*magnify, (height+1)*magnify);
  for (std::list<std::list<float*>* >::iterator it = hslList->begin(); it != hslList->end(); ++it)
  {
    std::list<float*>* hsl1 = *it;
    for (std::list<float*>::iterator iter = hsl1->begin(); iter != hsl1->end(); iter)
    {
      float* bit1 = *iter;
      if ((++iter) == hsl1->end())
      {
        break; // vylepsit na dva iteratory
      }
      float* bit2 = *iter;
      fprintf(file,"\t<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" style=\"stroke:rgb(0,0,0);stroke-width:1\" />\n",bit1[1]*multiply,bit1[0]*multiply,bit2[1]*multiply,bit2[0]*multiply);
    }
    fprintf(file,"\n");
  }
  fprintf(file,"</svg>");
  fclose(file);
}

float* makeAStep(float* startPoint, float* angle, float* step, int counter)
{
  float* newPoint = new float [3];

//  float newX = x * cos - y * sin
//  float newY = x * sin + y * cos

  newPoint[0] = startPoint[0] - (*step) * sin(degreesToRadians(*angle));
  newPoint[1] = startPoint[1] + (*step) * cos(degreesToRadians(*angle));
  float sigma = 128.0;
  float x = (float) counter * (*step);
  newPoint[2] = startPoint[2] - (1.0/(sigma*sqrt(2.0*M_PI)))* exp(-((x*x)/ (2.0*sigma*sigma)));
  FILE* file = fopen("ridgesHeights.txt","a+");
  fprintf(file,"%f\t%f\t%f\n",startPoint[2],(1.0/(sigma*sqrt(2.0*M_PI)))* exp(-((x*x)/ (2.0*sigma*sigma))),newPoint[2]);
  fclose(file);
  return newPoint;
}

float* checkAndFindCollision(std::list<std::list<float*>* >* ridges, std::list<float*>* currentRidge, float* nextPoint) //prepsat tuhle fci kvuli poslednim bodu a pridavani kolmych bodu
{
  float* lastRidgePoint = *(currentRidge->rbegin());
  float* intersection = NULL;
  for (std::list<std::list<float*>* >::iterator ridgesIt = ridges->begin(); ridgesIt != ridges->end(); ridgesIt++)
  {
    std::list<float*>* ridge = *ridgesIt;
    std::list<float*>::iterator ridgeIt1 = ridge->begin();
    std::list<float*>::iterator ridgeIt2 = ++ridge->begin();
    for (; ridgeIt2 != ridge->end(); ridgeIt1++,ridgeIt2++)
    {
      float* point1 = *ridgeIt1;
      if (point1 == lastRidgePoint)
      {
        return nextPoint;
      }
      float* point2 = *ridgeIt2;
      intersection = lineIntersection(point1,point2,lastRidgePoint,nextPoint);
      if (intersection != NULL)
      {
        nextPoint[0] = intersection[0];
        nextPoint[1] = intersection[1];
        nextPoint[2] = (nextPoint[2] + (point1[2]/(point1[2]+point2[2]))*point1[2] + (point2[2]/(point1[2]+point2[2]))*point2[2]) / 2.0;
        ridge->insert(ridgeIt2,nextPoint);
        currentRidge->push_back(nextPoint);
        delete[] intersection;
        return NULL;
      }
    }
  }
  return nextPoint;
}

void changeAngle(float* angle)
{
  *angle = *angle - 15.0 + (float) (rand() % 30);
//  *angle = *angle - 10.0 + (float) (rand() % 20);
//  *angle = *angle - 5.0 + (float) (rand() % 10);
}

void changeStep(float* step)
{
  *step = 10.0 + 5.0*(((float) rand()) / ((float)RAND_MAX));
}

void createHeightMapFromRidges(float** heightMap, std::list<std::list<float*>* >* ridges, int height, int width)
{
  for (std::list<std::list<float*>* >::iterator ridgesIt = ridges->begin(); ridgesIt != ridges->end(); ridgesIt++)
  {
    std::list<float*>* ridge = *ridgesIt;
    for (std::list<float*>::iterator ridgeIt = ridge->begin(); ridgeIt != ridge->end(); ridgeIt++)
    {
      float* point = *ridgeIt;
      if (point[0] > 0.0 && point[0] < (float)height && point[1] > 0.0 && point[1] < (float)height)
      {
        heightMap[(int)floor(point[0])][(int)floor(point[1])] = point[2];
      }
    }
//    while (ridge->size() < 3)
//    {
//      ridgesIt++;
//      ridge = *ridgesIt;
//    }
    if (ridge->size() < 3)
    {
      continue;
    }
    std::list<float*>::iterator ridgeIt1 = ridge->begin();
    std::list<float*>::iterator ridgeIt2 = ++ridge->begin();
    std::list<float*>::iterator ridgeIt3 = ++(++ridge->begin());
    for (;;)
    {
      float* point1 = *ridgeIt1;
      float* point2 = *ridgeIt2;
      float* point3 = *ridgeIt3;
      float* leftNormalVector = getLeftNormalVector(point1,point2);
      normalizeVectorN(leftNormalVector,2);
      float* leftNormalVector2 = getLeftNormalVector(point2,point3);
      normalizeVectorN(leftNormalVector2,2);
      leftNormalVector[0] = (leftNormalVector[0]+leftNormalVector2[0])/2.0;
      leftNormalVector[1] = (leftNormalVector[1]+leftNormalVector2[1])/2.0;

      float newX = point2[0];
//      newX += 64.0 * leftNormalVector[0];
      float newY = point2[1];
//      newY += 64.0 * leftNormalVector[1];
//      if (newX > 0.0 && newX < 512.0 && newY > 0.0 && newY < 512.0)
//      {
//        heightMap[(int)floor(newX)][(int)floor(newY)] = point2[2] * exp(-8.0*(32.0*8.0)/(128.0*128.0));
//      }

      newX = point2[0];
      newX += 36.0 * leftNormalVector[0];
      newY = point2[1];
      newY += 36.0 * leftNormalVector[1];
      if (newX > 0.0 && newX < (float)height && newY > 0.0 && newY < (float)height)
      {
        heightMap[(int)floor(newX)][(int)floor(newY)] = point2[2] * exp(-8.0*(20.0*12.0)/(128.0*128.0));
      }

      newX = point2[0];
      newX += 28.0 * leftNormalVector[0];
      newY = point2[1];
      newY += 28.0 * leftNormalVector[1];
      if (newX > 0.0 && newX < (float)height && newY > 0.0 && newY < (float)height)
      {
        heightMap[(int)floor(newX)][(int)floor(newY)] = point2[2] * exp(-8.0*(16.0*8.0)/(128.0*128.0));
      }

      newX = point2[0];
      newX += 16.0 * leftNormalVector[0];
      newY = point2[1];
      newY += 16.0 * leftNormalVector[1];
      if (newX > 0.0 && newX < (float)height && newY > 0.0 && newY < (float)height)
      {
        heightMap[(int)floor(newX)][(int)floor(newY)] = point2[2] * exp(-8.0*(8.0*4.0)/(128.0*128.0));
      }

      newX = point2[0];
      newX += 8.0 * leftNormalVector[0];
      newY = point2[1];
      newY += 8.0 * leftNormalVector[1];
      if (newX > 0.0 && newX < (float)height && newY > 0.0 && newY < (float)height)
      {
        heightMap[(int)floor(newX)][(int)floor(newY)] = point2[2] * exp(-8.0*(4.0*2.0)/(128.0*128.0));
      }

      newX = point2[0];
      newX += 6.0 * leftNormalVector[0];
      newY = point2[1];
      newY += 6.0 * leftNormalVector[1];
      if (newX > 0.0 && newX < (float)height && newY > 0.0 && newY < (float)height)
      {
        heightMap[(int)floor(newX)][(int)floor(newY)] = point2[2] * exp(-8.0*(3.0*1.5)/(128.0*128.0));
      }

      newX = point2[0];
      newX += 4.0 * leftNormalVector[0];
      newY = point2[1];
      newY += 4.0 * leftNormalVector[1];
      if (newX > 0.0 && newX < (float)height && newY > 0.0 && newY < (float)height)
      {
        heightMap[(int)floor(newX)][(int)floor(newY)] = point2[2] * exp(-8.0*(2.0*1.0)/(128.0*128.0));
      }

      newX = point2[0];
      newX += 2.0 * leftNormalVector[0];
      newY = point2[1];
      newY += 2.0 * leftNormalVector[1];
      if (newX > 0.0 && newX < (float)height && newY > 0.0 && newY < (float)height)
      {
        heightMap[(int)floor(newX)][(int)floor(newY)] = point2[2] * exp(-8.0*(1.0*1.0)/(128.0*128.0));
      }

//      newX = point2[0];
//      newX -= 64.0 * leftNormalVector[0];
//      newY = point2[1];
//      newY -= 64.0 * leftNormalVector[1];
//      if (newX > 0.0 && newX < 512.0 && newY > 0.0 && newY < 512.0)
//      {
//        heightMap[(int)floor(newX)][(int)floor(newY)] = point2[2] * exp(-8.0*(32.0*8.0)/(128.0*128.0));
//      }

      newX = point2[0];
      newX -= 36.0 * leftNormalVector[0];
      newY = point2[1];
      newY -= 36.0 * leftNormalVector[1];
      if (newX > 0.0 && newX < (float)height && newY > 0.0 && newY < (float)height)
      {
        heightMap[(int)floor(newX)][(int)floor(newY)] = point2[2] * exp(-8.0*(20.0*12.0)/(128.0*128.0));
      }

      newX = point2[0];
      newX -= 28.0 * leftNormalVector[0];
      newY = point2[1];
      newY -= 28.0 * leftNormalVector[1];
      if (newX > 0.0 && newX < (float)height && newY > 0.0 && newY < (float)height)
      {
        heightMap[(int)floor(newX)][(int)floor(newY)] = point2[2] * exp(-8.0*(16.0*8.0)/(128.0*128.0));
      }

      newX = point2[0];
      newX -= 16.0 * leftNormalVector[0];
      newY = point2[1];
      newY -= 16.0 * leftNormalVector[1];
      if (newX > 0.0 && newX < (float)height && newY > 0.0 && newY < (float)height)
      {
        heightMap[(int)floor(newX)][(int)floor(newY)] = point2[2] * exp(-8.0*(8.0*4.0)/(128.0*128.0));
      }

      newX = point2[0];
      newX -= 8.0 * leftNormalVector[0];
      newY = point2[1];
      newY -= 8.0 * leftNormalVector[1];
      if (newX > 0.0 && newX < (float)height && newY > 0.0 && newY < (float)height)
      {
        heightMap[(int)floor(newX)][(int)floor(newY)] = point2[2] * exp(-8.0*(4.0*2.0)/(128.0*128.0));
      }

      newX = point2[0];
      newX -= 6.0 * leftNormalVector[0];
      newY = point2[1];
      newY -= 6.0 * leftNormalVector[1];
      if (newX > 0.0 && newX < (float)height && newY > 0.0 && newY < (float)height)
      {
        heightMap[(int)floor(newX)][(int)floor(newY)] = point2[2] * exp(-8.0*(3.0*1.5)/(128.0*128.0));
      }

      newX = point2[0];
      newX -= 4.0 * leftNormalVector[0];
      newY = point2[1];
      newY -= 4.0 * leftNormalVector[1];
      if (newX > 0.0 && newX < (float)height && newY > 0.0 && newY < (float)height)
      {
        heightMap[(int)floor(newX)][(int)floor(newY)] = point2[2] * exp(-8.0*(2.0*1.0)/(128.0*128.0));
      }

      newX = point2[0];
      newX -= 2.0 * leftNormalVector[0];
      newY = point2[1];
      newY -= 2.0 * leftNormalVector[1];
      if (newX > 0.0 && newX < (float)height && newY > 0.0 && newY < (float)height)
      {
        heightMap[(int)floor(newX)][(int)floor(newY)] = point2[2] * exp(-8.0*(1.0*1.0)/(128.0*128.0));
      }
//      float aaa = exp(-8.0*(16.0*16.0)/(128.0*128.0));
//      float bbb = point2[2] * exp(-8.0*(16.0*16.0)/(128.0*128.0));
//      newX -= 32.0 * leftNormalVector[0];
//      newY -= 32.0 * leftNormalVector[1];
//      if (newX > 0.0 && newX < 512.0 && newY > 0.0 && newY < 512.0)
//      {
//        heightMap[(int)newX][(int)newY] = point2[2] * exp(-8.0*(4*4)*(128.0*128.0));
//      }
ridgeIt1++;
ridgeIt2++;
ridgeIt3++;

ridgeIt1++;
ridgeIt2++;
ridgeIt3++;

ridgeIt1++;
ridgeIt2++;
ridgeIt3++;
if (ridgeIt1 == ridge->end() || ridgeIt2 == ridge->end() || ridgeIt3 == ridge->end())
{
  break;
}
    }
  }
}

std::list<float*>* createRidge (std::list<std::list<float*>* >* ridges, float* startPoint, float startStep, float startAngle, int heightOfMap, int widthOfMap)
{
  std::list<float*>* ridge = new std::list<float*>();

  float* step = new float (startStep);
  float* angle = new float (startAngle);
  float* nextPoint = startPoint;
  int ridgeLength = 1;
  while (nextPoint != NULL && nextPoint[0] > 0.0 && nextPoint[0] < (float)heightOfMap && nextPoint[1] > 0.0 && nextPoint[1] < (float)widthOfMap && nextPoint[2] > 60.0)
  {
    ridge->push_back(nextPoint);
    nextPoint = makeAStep(nextPoint, angle, step, ridgeLength);
    nextPoint = checkAndFindCollision(ridges, ridge, nextPoint);
    changeAngle(angle);
    changeStep(step);
  }
  return ridge;
}

void createRandomRidge (std::list<std::list<float*>* >* ridges, int heightOfMap, int widthOfMap)
{
  float* startPoint = new float [3];
  /* old
  startPoint[0] = (float) (rand() % heightOfMap);
  startPoint[1] = (float) (rand() % widthOfMap);
  */
  startPoint[0] = (float) (rand() % heightOfMap);
  while (startPoint[0] > (float)heightOfMap*0.2 && startPoint[0] < (float)heightOfMap*0.8)
  {
    startPoint[0] = (float) (rand() % 256) + ((float)rand())/((float)RAND_MAX);
  }
  startPoint[1] = (float) (rand() % widthOfMap);
  while (startPoint[1] > (float)heightOfMap*0.2 && startPoint[1] < (float)heightOfMap*0.8)
  {
    startPoint[1] = (float) (rand() % 256) + ((float)rand())/((float)RAND_MAX);
  }
  startPoint[2] = (float) (rand() % 256);
  startPoint[2] = 250.0; //old
  while (startPoint[2] < 150)
  {
    startPoint[2] = (float) (rand() % 256);
  }

  float startAngle = (float) (rand() % 360);
  while ((startAngle > 15.0 && startAngle < 75.0) || (startAngle > 105.0 && startAngle < 315.0))
  {
    startAngle = (float) (rand() % 360);
  }

  float startStep = 2.0 + 3.0*(((float) rand()) / ((float)RAND_MAX));
  changeAngle(&startAngle);
  changeStep(&startStep);

  std::list<float*>* ridge = createRidge(ridges, startPoint, startStep, startAngle, heightOfMap, widthOfMap);
  int a = ridge->size();
  ridges->push_back(ridge);
  if (startAngle > 180.0)
  {
    startAngle -= 180.0;
  }
  else
  {
    startAngle += 180.0;
  }
  std::list<float*>* ridgeb = createRidge(ridges, startPoint, startStep, startAngle, heightOfMap, widthOfMap);
  int b = ridgeb->size();
  ridges->push_back(ridgeb);
}

float** skelet (int ridgeCounter, int height, int width)
{
//  int init_num;
//  if (TIME_NULL == 0)
//  {
//    init_num = time(NULL);
//  }
//  else
//  {
//    init_num = TIME_NULL;
//  }
//  FILE* file = fopen("srand.txt","a+");
//  fprintf(file,"%d\n",init_num);
//  fclose(file);
//  srand (init_num);
//  srand(1486641469);
//  srand(1486642585);
//  srand(10);
//  srand(36);
//  srand(1493894627);
//  srand(1493894627);
//  srand(1493960735);

  float** heightMap = new float* [height];
  for(int i = 0; i < height; i++)
  {
    heightMap[i] = new float[width];
  }

  for (int i=0; i<height; i++)
  {
    for (int j=0; j<width; j++)
    {
        heightMap [i][j] = -128.0; // inicializovat na jinou hodnotu? (pridat mapu bool voda/nevoda?)
    }
  }
  std::list<std::list<float*>* >* ridges = new std::list<std::list<float*>* >();
  for (int i = 0; i < ridgeCounter; i++)
  {
    createRandomRidge(ridges, height, width);
  }

  createHeightMapFromRidges(heightMap, ridges, height, width); //momentalne ignorovat posledni dva a ?,u kazdeho druheho prvni?,
  draw(3,width,height,ridges);
  delete ridges;

/*//old->

//  for (int i=0; i<init; i++)
  for (int i=0; i<3; i++)
  {
    heM [rand() % height][rand() % width] = (float) (rand() % 256);
//    heM [11][5] = (float) (200);
//    heM [13+i][6+i] = (float) (200);
  }
//
//  pridat moznost vice pohori a pridat podminku na stretnuti pohori
//   for (int i=0; i<init; i++)
//  zlepsit na vice smeru(i sikmo)

  int altitude = rand() % 256;
  while (altitude < 200)
  {
    altitude = rand() % 256;
  }
  int coordX = rand() % height;
  int coordY = rand() % width;
  heM [coordX][coordY] = altitude;
  int orientation = rand() % 4;

  nextRidgePoint(heM, coordX, coordY, altitude, orientation, width, 1);
  nextRidgePoint(heM, coordX, coordY, altitude, (orientation + 2) % 4, width, 1);

  for (int i=0; i<init - 1; i++)
  for (int i=0; i<3; i++)
  {
    altitude = rand() % 256;
    while (altitude < 230)
    {
      altitude = rand() % 256;
    }
    coordX = rand() % height;
    coordY = rand() % width;
    heM [coordX][coordY] = altitude;
    orientation = rand() % 4;
    nextRidgePoint(heM, coordX, coordY, altitude, orientation, width, 1);
    nextRidgePoint(heM, coordX, coordY, altitude, (orientation + 2) % 4, width, 1);
  }

  return heM;
//*/ //<-old


  return heightMap;
}

void nextRidgePoint (float** heM, int coordX, int coordY, int altitude, int orientation, int width, int iter)
{
  int orX,orY;
  int randDirection = rand() % 3;
  switch (orientation)
  {
  case 0:
    switch (randDirection)
    {
    case 0:
      orX = -1;
      orY = 1;
      break;
    case 1:
      orX = 0;
      orY = 1;
      break;
    case 2:
      orX = 1;
      orY = 1;
      break;
    }
  break;

  case 1:
    switch (randDirection)
    {
    case 0:
      orX = 1;
      orY = 1;
      break;
    case 1:
      orX = 1;
      orY = 0;
      break;
    case 2:
      orX = 1;
      orY = -1;
      break;
    }
  break;

  case 2:
    switch (randDirection)
    {
    case 0:
      orX = 1;
      orY = -1;
      break;
    case 1:
      orX = 0;
      orY = -1;
      break;
    case 2:
      orX = -1;
      orY = -1;
      break;
    }
  break;

  case 3:
    switch (randDirection)
    {
    case 0:
      orX = -1;
      orY = -1;
      break;
    case 1:
      orX = -1;
      orY = 0;
      break;
    case 2:
      orX = -1;
      orY = 1;
      break;
    }
  break;
  }

  int scaleFactor = rand() % 3 + 1;
  int corX = coordX + orX * scaleFactor;
  int corY = coordY + orY * scaleFactor;

  float eu = euclideanDistance(coordX, coordY, corX, corY);

  float newAlt = (float)altitude * 0.995; //chce vylepsit klesani a taky pro vetsi (500+) mapy vytvorit nejakej limit kdy se to zastavi jinak to jde moc do hloubky

  if (newAlt > 30.0 && corX < width && corX > 0 && corY < width && corY > 0) // experimentalne 30
  {
    if (heM[corX][corY] != -128.0)
    {
      return;
    }
    ++iter;
    heM[corX][corY] = newAlt;
    nextRidgePoint(heM, corX, corY, newAlt, orientation, width, iter);
  }
}

void mDBU (float** skelet, int height, int width, float ibu, float dMax)
{
  std::queue<Cell>* fifo = new std::queue<Cell> ();
  std::queue<Cell>* ascendantsE = new std::queue<Cell> ();
  std::map<Cell, std::queue<Cell>, Cell> *hashtable = new std::map<Cell, std::queue<Cell>, Cell> ();
  bool** usedCells = new bool* [height];
  for (int i=0; i<height; i++)
  {
    usedCells[i] = new bool [width];
  }

  for(int i=0; i<height; i++)
  {
    for(int j=0; j<width;j++)
    {
      usedCells [i][j] = false;
    }
  }
  int n;
  float e;

  for(int i=0; i<height; i++)
  {
    for(int j=0; j<width;j++)
    {
      if(skelet [i][j] != -128.0) //zmenit tuhle podminku na neco jinyho nez nulu, nuly muze hodnota nadmorske vysky nabyvat
      {
        Cell cell(i,j);
        usedCells[i][j] = true;
        fifo->push(cell);
      }
    }
  }

  while (!fifo->empty())
  {
    while (!fifo->empty())
    {
      Cell E = fifo->front();
      fifo->pop();
      triangleEdgeAscendants(E, ascendantsE, height, width); //vytvorit moznost zavolat jinej typ(diamondSquare)?

      while (!ascendantsE->empty())
      {
        Cell A = ascendantsE->front();
        ascendantsE->pop();
        if (!usedCells [A.getRow()] [A.getCol()])
        {
          if (hashtable->find(A) == hashtable->end())
          {
            std::queue<Cell> AChilds;
            AChilds.push(E);
            hashtable->insert( std::pair<Cell, std::queue<Cell> > ( A,AChilds)); // pro zrychleni predelat na ukazatele?(mene pameti?)
          }
          else
          {
            (*hashtable)[A].push(E);
          }
        }
      }
    }

    for (iteratorHashtable it = hashtable->begin(); it != hashtable->end(); ++it)
    {
      Cell A = it->first;
      n = 0;
      e = n;
      std::queue<Cell>* queueC = &(it->second);
      while (!queueC->empty())
      {
        Cell C = queueC->front();
        queueC->pop();
        e += deltaBU(skelet[C.getRow()][C.getCol()], euclideanDistance(A,C), ibu, dMax);
        ++n;
      }
      skelet [A.getRow()] [A.getCol()] = (e / (float)n);
      usedCells [A.getRow()] [A.getCol()] = true;
      fifo->push(A);
    }
    hashtable->clear();
  }

  delete fifo;
  delete [] usedCells;
  delete ascendantsE;
  delete hashtable;
}

float deltaBU (float e, float d, float ibu, float dMax)
{
  return e * (1.0 - sigma(ibu) * (1.0 - powf(1.0 - d / dMax, fabsf(ibu))));
}

float sigma (float ibu)
{
  if (ibu >= 0.0)
  {
    return 1.0;
  }
  return -1.0;
}

void triangleEdgeAscendants (Cell E, std::queue<Cell>* ascendantsE, int height, int width)
{
  if (!((E.getRow() == 0 && E.getCol() == 0)
        || (E.getRow() == 0 && E.getCol() == height-1)
        || (E.getRow() == width-1 && E.getCol() == 0)
        || (E.getRow() == width-1 && E.getCol() == height-1 )))
  {
  triangleEdgeAscendantsAtRegion(E, ascendantsE, 0, 0, height-1, width-1);
  }
}

void triangleEdgeAscendantsAtRegion (Cell E, std::queue<Cell>* ascendantsE, int x1, int y1, int x2, int y2)
{
  int x = E.getRow();
  int y = E.getCol();

  if ( x1 <= x && x <= x2 && y1 <= y && y <= y2)
  {
    bool found = false;
    if ( x1 == x && y == ((y1+y2) / 2))
    {
      Cell C(x1,y1);
      Cell D(x1,y2);
      ascendantsE->push(C);
      ascendantsE->push(D);
      found = true;
    }
    if ( x2 == x && y == ((y1+y2) / 2))
    {
      Cell C(x2,y1);
      Cell D(x2,y2);
      ascendantsE->push(C);
      ascendantsE->push(D);
      found = true;
    }
    if ( ((x1+x2) / 2) == x && y == y1)
    {
      Cell C(x1,y1);
      Cell D(x2,y1);
      ascendantsE->push(C);
      ascendantsE->push(D);
      found = true;
    }
    if ( ((x1+x2) / 2) == x && y == y2)
    {
      Cell C(x1,y2);
      Cell D(x2,y2);
      ascendantsE->push(C);
      ascendantsE->push(D);
      found = true;
    }
    if ( ((x1+x2) / 2) == x && y == ((y1+y2) / 2))
    {
      Cell C(x1,y1);
      Cell D(x1,y2);
      Cell E(x2,y1);
      Cell F(x2,y2);
      ascendantsE->push(C);
      ascendantsE->push(D);
      ascendantsE->push(E);
      ascendantsE->push(F);
      found = true;
    }

    if (!found)
    {
      triangleEdgeAscendantsAtRegion(E,ascendantsE,x1,y1,(x1+x2)/2,(y1+y2)/2);
      triangleEdgeAscendantsAtRegion(E,ascendantsE,(x1+x2)/2,y1,x2,(y1+y2)/2);
      triangleEdgeAscendantsAtRegion(E,ascendantsE,x1,(y1+y2)/2,(x1+x2)/2,y2);
      triangleEdgeAscendantsAtRegion(E,ascendantsE,(x1+x2)/2,(y1+y2)/2,x2,y2);
    }
  }
}

void midD (float** constraintMap, int height, int width) // pridat nekam kdyby nebyli nahodou inicializovany vsechny vrcholy/init!(muze se to vubec stat po MDBU?)
{
  int maxIter = log(width) / log (2); // dimension instead of width
  triangleEdge(constraintMap, 1, maxIter, 0, 0, width-1, height-1); // vylepsit nuly?
}


float deltaX (int iter);
void triangleEdge (float** constraintMap, int iter, int maxIter, int x1, int y1, int x2, int y2) // upgrade na libovolně velkou čtvrcovou! bud nasledujicim anebo pridat dodatecne radky do 2^n+1? a pote je odstranit
{
//  int x_2 = (x1+x2) % 2;
//  if (x_2 == 1)
//  {
//    if (constraintMap[(x1+x2-1)/2][y1] == -128.0 )
//    {
//      constraintMap[(x1+x2-1)/2][y1] = (constraintMap[x1][y1] + constraintMap[x2][y1]) / 2.0 + delta(0.7, 25.0, iter, 2, 0.4); // vytvorit delta paramtery dostupne z venku
//    }
//    if (constraintMap[(x1+x2+1)/2][y1] == -128.0 )
//    {
//      constraintMap[(x1+x2+1)/2][y1] = (constraintMap[x1][y1] + constraintMap[x2][y1]) / 2.0 + delta(0.7, 25.0, iter, 2, 0.4); // vytvorit delta paramtery dostupne z venku
//    }
//    if (constraintMap[(x1+x2-1)/2][y2] == -128.0 )
//    {
//      constraintMap[(x1+x2-1)/2][y2] = (constraintMap[x1][y2] + constraintMap[x2][y2]) / 2.0 + delta(0.7, 25.0, iter, 2, 0.4);
//    }
//    if (constraintMap[(x1+x2+1)/2][y2] == -128.0 )
//    {
//      constraintMap[(x1+x2+1)/2][y2] = (constraintMap[x1][y2] + constraintMap[x2][y2]) / 2.0 + delta(0.7, 25.0, iter, 2, 0.4);
//    }
//    if (constraintMap[x1][(y1+y2-1)/2] == -128.0 )
//    {
//      constraintMap[x1][(y1+y2-1)/2] = (constraintMap[x1][y1] + constraintMap[x1][y2]) / 2.0 + delta(0.7, 25.0, iter, 2, 0.4);
//    }
//    if (constraintMap[x1][(y1+y2+1)/2] == -128.0 )
//    {
//      constraintMap[x1][(y1+y2+1)/2] = (constraintMap[x1][y1] + constraintMap[x1][y2]) / 2.0 + delta(0.7, 25.0, iter, 2, 0.4);
//    }
//    if (constraintMap[x2][(y1+y2-1)/2] == -128.0 )
//    {
//      constraintMap[x2][(y1+y2-1)/2] = (constraintMap[x2][y1] + constraintMap[x2][y2]) / 2.0 + delta(0.7, 25.0, iter, 2, 0.4);
//    }
//    if (constraintMap[x2][(y1+y2+1)/2] == -128.0 )
//    {
//      constraintMap[x2][(y1+y2+1)/2] = (constraintMap[x2][y1] + constraintMap[x2][y2]) / 2.0 + delta(0.7, 25.0, iter, 2, 0.4);
//    }
//    if (constraintMap[(x1+x2-1)/2][(y1+y2-1)/2] == -128.0 )
//    {
//      constraintMap[(x1+x2-1)/2][(y1+y2-1)/2] = (constraintMap[x1][y1] + constraintMap[x1][y2] + constraintMap[x2][y1] + constraintMap[x2][y2]) / 4.0 + delta(0.7, 25.0, iter, 2, 0.4);
//    }
//    if (constraintMap[(x1+x2-1)/2][(y1+y2+1)/2] == -128.0 )
//    {
//      constraintMap[(x1+x2-1)/2][(y1+y2+1)/2] = (constraintMap[x1][y1] + constraintMap[x1][y2] + constraintMap[x2][y1] + constraintMap[x2][y2]) / 4.0 + delta(0.7, 25.0, iter, 2, 0.4);
//    }
//    if (constraintMap[(x1+x2+1)/2][(y1+y2-1)/2] == -128.0 )
//    {
//      constraintMap[(x1+x2+1)/2][(y1+y2-1)/2] = (constraintMap[x1][y1] + constraintMap[x1][y2] + constraintMap[x2][y1] + constraintMap[x2][y2]) / 4.0 + delta(0.7, 25.0, iter, 2, 0.4);
//    }
//    if (constraintMap[(x1+x2+1)/2][(y1+y2+1)/2] == -128.0 )
//    {
//      constraintMap[(x1+x2+1)/2][(y1+y2+1)/2] = (constraintMap[x1][y1] + constraintMap[x1][y2] + constraintMap[x2][y1] + constraintMap[x2][y2]) / 4.0 + delta(0.7, 25.0, iter, 2, 0.4);
//    }
//    if (iter != maxIter)
//    {
//      ++iter;
//      triangleEdge(constraintMap, iter, maxIter,x1,y1,(x1+x2-1)/2,(y1+y2-1)/2); // |1,3|
//      triangleEdge(constraintMap, iter, maxIter,(x1+x2+1)/2,y1,x2,(y1+y2-1)/2); // |2,4|
//      triangleEdge(constraintMap, iter, maxIter,x1,(y1+y2+1)/2,(x1+x2-1)/2,y2);
//      triangleEdge(constraintMap, iter, maxIter,(x1+x2+1)/2,(y1+y2+1)/2,x2,y2);
//    }
//  }
//  else
//  {
//    if (constraintMap[(x1+x2)/2][y1] == -128.0 )
//    {
//      constraintMap[(x1+x2)/2][y1] = (constraintMap[x1][y1] + constraintMap[x2][y1]) / 2.0 + delta(0.7, 25.0, iter, 2, 0.4); // treti
//    }
//    if (constraintMap[(x1+x2)/2][y2] == -128.0 )
//    {
//      constraintMap[(x1+x2)/2][y2] = (constraintMap[x1][y2] + constraintMap[x2][y2]) / 2.0 + delta(0.7, 25.0, iter, 2, 0.4);// treti
//    }
//    if (constraintMap[x1][(y1+y2)/2] == -128.0 )
//    {
//      constraintMap[x1][(y1+y2)/2] = (constraintMap[x1][y1] + constraintMap[x1][y2]) / 2.0 + delta(0.7, 25.0, iter, 2, 0.4);// treti
//    }
//    if (constraintMap[x2][(y1+y2)/2] == -128.0 )
//    {
//      constraintMap[x2][(y1+y2)/2] = (constraintMap[x2][y1] + constraintMap[x2][y2]) / 2.0 + delta(0.7, 25.0, iter, 2, 0.4);// treti
//    }
//    if (constraintMap[(x1+x2)/2][(y1+y2)/2] == -128.0 )
//    {
//      constraintMap[(x1+x2)/2][(y1+y2)/2] = (constraintMap[x1][y1] + constraintMap[x1][y2] + constraintMap[x2][y1] + constraintMap[x2][y2]) / 4.0 + delta(0.7, 25.0, iter, 2, 0.4);
//    }


//    if (constraintMap[(x1+x2)/2][y1] == -128.0 )
//    {
//      constraintMap[(x1+x2)/2][y1] = (constraintMap[x1][y1] + constraintMap[x2][y1]) / 2.0 + delta(0.4, 20.0, iter, 2, 0.33); // ctvrt
//    }
//    if (constraintMap[(x1+x2)/2][y2] == -128.0 )
//    {
//      constraintMap[(x1+x2)/2][y2] = (constraintMap[x1][y2] + constraintMap[x2][y2]) / 2.0 + delta(0.4, 20.0, iter, 2, 0.33);// ctvrt
//    }
//    if (constraintMap[x1][(y1+y2)/2] == -128.0 )
//    {
//      constraintMap[x1][(y1+y2)/2] = (constraintMap[x1][y1] + constraintMap[x1][y2]) / 2.0 + delta(0.4, 20.0, iter, 2, 0.33);// ctvrt
//    }
//    if (constraintMap[x2][(y1+y2)/2] == -128.0 )
//    {
//      constraintMap[x2][(y1+y2)/2] = (constraintMap[x2][y1] + constraintMap[x2][y2]) / 2.0 + delta(0.4, 20.0, iter, 2, 0.33);// ctvrt
//    }
//    if (constraintMap[(x1+x2)/2][(y1+y2)/2] == -128.0 )
//    {
//      constraintMap[(x1+x2)/2][(y1+y2)/2] = (constraintMap[x1][y1] + constraintMap[x1][y2] + constraintMap[x2][y1] + constraintMap[x2][y2]) / 4.0 + delta(0.4, 20.0, iter, 2, 0.33);
//    }


    if (constraintMap[(x1+x2)/2][y1] == -128.0 )
    {
      constraintMap[(x1+x2)/2][y1] = (constraintMap[x1][y1] + constraintMap[x2][y1]) / 2.0 + deltaX(iter); // ctvrt
    }
    if (constraintMap[(x1+x2)/2][y2] == -128.0 )
    {
      constraintMap[(x1+x2)/2][y2] = (constraintMap[x1][y2] + constraintMap[x2][y2]) / 2.0 + deltaX(iter);// ctvrt
    }
    if (constraintMap[x1][(y1+y2)/2] == -128.0 )
    {
      constraintMap[x1][(y1+y2)/2] = (constraintMap[x1][y1] + constraintMap[x1][y2]) / 2.0 + deltaX(iter);// ctvrt
    }
    if (constraintMap[x2][(y1+y2)/2] == -128.0 )
    {
      constraintMap[x2][(y1+y2)/2] = (constraintMap[x2][y1] + constraintMap[x2][y2]) / 2.0 + deltaX(iter);// ctvrt
    }
    if (constraintMap[(x1+x2)/2][(y1+y2)/2] == -128.0 )
    {
      constraintMap[(x1+x2)/2][(y1+y2)/2] = (constraintMap[x1][y1] + constraintMap[x1][y2] + constraintMap[x2][y1] + constraintMap[x2][y2]) / 4.0 + deltaX(iter);
    }


    if (iter != maxIter)
    {
      ++iter;
      triangleEdge(constraintMap, iter, maxIter,x1,y1,(x1+x2)/2,(y1+y2)/2);
      triangleEdge(constraintMap, iter, maxIter,(x1+x2)/2,y1,x2,(y1+y2)/2);
      triangleEdge(constraintMap, iter, maxIter,x1,(y1+y2)/2,(x1+x2)/2,y2);
      triangleEdge(constraintMap, iter, maxIter,(x1+x2)/2,(y1+y2)/2,x2,y2);
    }
//  }
}

float delta (float rt, float rs, int r, int n, float H)
{
  return (urand() + rt) * rs * exp2f(- (float)r * (float)n * H);
}

float deltaX (int iter)
{
//  return 0.0;//delta(0.2, 23.0, iter, 1, 0.770);
//  return delta(0.4, m, iter, 2, H);
//  return delta(0.4, 20.0, iter, 2, 0.33);
//  return delta(0.3, 36.0, iter, 2, 0.42);
  return delta(DELTA_RT, DELTA_RS, iter, DELTA_N, DELTA_H);
//  return delta(0.0, 44.0, iter, 1, 0.38);
//  return delta(0.0, 44.0, iter, 1, 0.73);
//  return delta(0.0, 12.0, iter, 1, 0.73);
}
