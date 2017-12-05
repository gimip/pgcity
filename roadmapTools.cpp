#include "roadmapTools.hpp"

#include "constants.hpp"
#include "utilities.hpp"
#include "Terrain.hpp"
#include "Population.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

float* findInitSeed(std::list<std::list<float*> > *hslList/*,std::list<float*> hyperstreamline*/, float dsep, float*** vectorField,int width,int height,float** popDensity)
{
  float* initSeed = new float [2];

  int x=0,y=0;
  float maxPop = popDensity[x][y];
  for (int i=0; i<height; i++)
  {
    for (int j=0; j<width; j++)
    {
      if (maxPop < popDensity[i][j]) // pridat rozdilnou podminku > 0 pro vodu (az bude)
      {
        maxPop = popDensity [i][j];
        x = i;
        y = j;
      }
    }
  }

  int xx = 0,yy = 0;
  float maxGrad = 0.0;
  for (int i=1; i<height-1; i++) // 1/-1 kvuli krajum
  {
    for (int j=1; j<width-1; j++) // 1/-1 kvuli krajum
    {
      if (/*popDensity[i][j] > maxPop/15.0 && */euclideanDistance((float)i,(float)j,(float)x,(float)y) < 30.0 /*&& popDensity[i][j+1] != 0.0 && popDensity[i+1][j] != 0.0 && popDensity[i-1][j] != 0.0 && popDensity[i][j-1] != 0.0*/) // pridat rozdilnou podminku > 0 pro vodu (az bude)
      {
        float tmp = euclideanDistance(0.0,0.0,vectorField[i][j][0],vectorField[i][j][1]);
        if (maxGrad < tmp)
        {
          maxGrad = tmp;
          xx = i;
          yy = j;
        }
      }
    }
  }


//  initSeed[0] = (float)x + (float)rand()/(float)RAND_MAX;
  initSeed[0] = (float)xx + (float)rand()/(float)RAND_MAX;
//  initSeed[1] = (float)y + (float)rand()/(float)RAND_MAX;
  initSeed[1] = (float)yy + (float)rand()/(float)RAND_MAX;
  return initSeed;
}

float* distanceFromSegment (float x,float y,float sx1,float sy1,float sx2,float sy2,float* distance)
{ //return 0-x,1-y
  float* nearestPoint = new float [2];

  float length = powf(sx2-sx1, 2.0f)+powf(sy2-sy1, 2.0f);
  if (length == 0.0) // segment is a point
  {
    nearestPoint[0] = sx1;
    nearestPoint[1] = sy1;
    *distance = euclideanDistance(x,y,sx1,sy1);
    return nearestPoint;
  }
  float sx1x = x - sx1;
  float sx1y = y - sy1;
  float sx2sx1 = sx2 - sx1;
  float sy2sy1 = sy2 - sy1;
  float t =  (sx1x * sx2sx1 + sx1y * sy2sy1) / length;
  if (t < 0.0f) // beyond s1 point
  {
    nearestPoint[0] = sx1;
    nearestPoint[1] = sy1;
    *distance = euclideanDistance(x,y,sx1,sy1);
    return nearestPoint;
  }
  if (t > 1.0f) // beyond s2 point
  {
    nearestPoint[0] = sx2;
    nearestPoint[1] = sy2;
    *distance = euclideanDistance(x,y,sx2,sy2);
    return nearestPoint;
  }

  nearestPoint[0] = sx1 + t * (sx2sx1);
  nearestPoint[1] = sy1 + t * (sy2sy1);
  *distance = euclideanDistance(x,y,nearestPoint[0],nearestPoint[1]);
  return nearestPoint;
}

float* checkPoint (float x,float y,float sx1,float sy1,float sx2,float sy2,float diff)
{
  float distance;
  float* neareastPoint = distanceFromSegment(x,y,sx1,sy1,sx2,sy2,&distance);
  if (distance <= diff)
  {
    return neareastPoint;
  }
  delete [] neareastPoint;
  return NULL;
}


bool hyperstreamline(float** heightMap,int width,int height,std::list<std::list<float*> > *hslList,float*** vectorField, float seedX, float seedY,float dsep, float** popDensity,float gauge)
{
  bool workAround = false;
  float* xy = new float[2];
  std::list<float*> hyperstreamline;
  std::list<float*> seedList;

  //tmp
  float conK = 0.05;
//  float conK = 0.35;

//  float conK = 0.03;
//  float dsep = 0.7;
//  float dsep = 0.5;
//  float dsep = 0.3;
  float diff;
  //tmp

//  float** asdf;
//  asdf = new float* [width];
//  for(int i = 0; i < width; i++)
//  {
//    asdf[i] = new float[width];
//  }
//  for (int i=0; i<width; i++)
//  {
//    for (int j=0; j<width; j++)
//    {
//      asdf[i][j] = 0;
//    }
//  }
    xy[0] = seedX;
    xy[1] = seedY;

    bool go = true;
    int i =0;
//           novinka
    bool mod2 = false;
    if (hslList->size() % 2 == 0)
    {
      mod2 = true;
    }
//

    float previousDistance;
    while (/*docasne odkoment*/seedX < height && seedX > 0 && seedY < width && seedY > 0.0 &&/*docasne odkoment*/ go /*&& bilinearInterpolation(heightMap,seedX,seedY,height,width) < 200.0f*/ && i < D_MAX)
    { // podminka na delku hyperstreamline?
      hyperstreamline.push_back(xy);
      xy = ark4(heightMap,vectorField,seedX,seedY,height,width);
      previousDistance = 0.0;
      while (euclideanDistance(seedX,seedY, xy[0],xy[1]) < RUNGEKUTTA_STEP && euclideanDistance(seedX,seedY, xy[0],xy[1]) > previousDistance)
      {
        previousDistance = euclideanDistance(seedX,seedY, xy[0],xy[1]);
        xy = ark4(heightMap,vectorField,xy[0],xy[1],height,width);
      }


      for (std::list<float*>::iterator it = hyperstreamline.begin(); it != hyperstreamline.end(); ++it)
//      int i = 0;
//      for (std::list<float*>::iterator it = hyperstreamline.begin(); i < hyperstreamline.size(); ++it,i++)
      {
        float* nuf = *it;
        if (nuf[0] > (xy [0]-conK) && nuf[0] < (xy [0]+conK) && nuf[1] > (xy [1]-conK) && nuf[1] < (xy [1]+conK))
        {
          go = false;
          break;
        }
      }

      if (!mod2) //check for back loop
      {
        std::list<std::list<float*> >::reverse_iterator hslListIt = hslList->rbegin();
        std::list<float*> lastHsl = *hslListIt;
        std::list<float*>::iterator it = lastHsl.begin();
        it++;
        if(it == lastHsl.end())
        {
          break;
        }
        for (;;)
        {
          float* nuf = *it;
          it++;
          if(it == lastHsl.end())
          {
            break;
          }
          it++;
          if(it == lastHsl.end())
          {
            break;
          }
          it--;
          float* puf = *it;
          if (puf == NULL)
          {
            break;
          }
          float* loop = checkPoint(xy[0],xy[1],nuf[0],nuf[1],puf[0],puf[1],0.5); // 0.4 vlozit do konstanty a mozna zmenit na 0.5
          if(loop != NULL)
          {
            go = false;
            hyperstreamline.push_back(loop);
            break;
          }
          delete[] loop;
        }
      }

      for (std::list<float*>::iterator it = hyperstreamline.begin(); it != hyperstreamline.end();/* ++it*/)
      {
        float* nuf = *it;
        it++;
        if(it == hyperstreamline.end())
        {
          break;
        }
        it++;
        if(it == hyperstreamline.end())
        {
          break;
        }
        it--;
        float* puf = *it;
//        float* loop = checkPoint(xy[0],xy[1],nuf[0],nuf[1],puf[0],puf[1],0.3); // 0.4 vlozit do konstanty a mozna zmenit na 0.5
        float* loop = checkPoint(xy[0],xy[1],nuf[0],nuf[1],puf[0],puf[1],0.5); // 0.4 vlozit do konstanty a mozna zmenit na 0.5
        if(loop != NULL)
        {
          go = false;
          std::list<float*>::iterator hIt = hyperstreamline.end();
          hIt--;
          hIt = hyperstreamline.erase(hIt);
          hIt--;
          hyperstreamline.erase(hIt);
          delete[] loop;
          break;
        }
        delete[] loop;
      }

//      if(!mod2)
//      {
//        std::list<float*> previousHsl = *hslList->rbegin();
//        int loops = 0;
//        for(std::list<float*>::iterator previousHslIt = previousHsl.begin(); previousHslIt!=previousHsl.end(); previousHslIt++)
//        {
//          float* nuf = *previousHslIt;
//          previousHslIt++;
//          if(previousHslIt == hyperstreamline.end())
//          {
//            break;
//          }
//          float* puf = *previousHslIt;
//          float* loop = checkPoint(xy[0],xy[1],nuf[0],nuf[1],puf[0],puf[1],0.4); // 0.4 vlozit do konstanty a mozna zmenit na 0.5
//          if(loop != NULL)
//          {
//            loops++;
//          }
//          delete[] loop;
//        }
//        if (loops > 2)
//        {
//          go=false;
//          break;
//        }
//      }

      if (!go)
      {
        break;
      }

      // novinka
//      bool mod2 = false;
//      if (hslList->size() % 2 == 0)
//      {
//        mod2 = true;
//      }
      //

// old
      for (std::list<std::list<float*> >::iterator hslListIt = hslList->begin(); hslListIt != hslList->end(); ++hslListIt)
      {
        std::list<float*> hsl = *hslListIt;
        // spoliham tu na push_back
        if (!mod2)
        {
          if ((++hslListIt) == hslList->end())
          {
            break;
          }
          --hslListIt;
        }
        // posud
        for (std::list<float*>::iterator iter = hsl.begin(); iter != hsl.end(); ++iter)
        {
          float* point1 = *iter;
          if ((++iter) == hsl.end())
          {
            break;
          }
          float* point2 = *iter;
          iter--;

          float* intersection = lineIntersection(seedX,seedY,xy[0],xy[1],point1[0],point1[1],point2[0],point2[1]);
          // protnou nebo pridat ze jsou velmi !VELMI! blizko pak taky skoncit
          // plus pridat jeste podminku na vyhledani! mozneho pripojeni po padesati bodech?
          if (intersection != NULL)
          {
            hyperstreamline.push_back(intersection);
            printf("was here!");
            go = false;
//            delete[] intersection;
            break;
          }
          delete[] intersection;

//          diff = dsep - (255.0 / bilinearInterpolation(popDensity,xy[0],xy[1],height,width)) * 1.5; // prevest na koncept dsep - (0,1)??
//          diff = 0.5; // idealne mezi 0.5-0.7 mozne posunout az na 0.35 (mozne min? asi ano ale uz je to moc blizko) pri dsep = 2.5
//          diff = 0.7; // idealne mezi 0.5-0.7 mozne posunout az na 0.35 (mozne min? asi ano ale uz je to moc blizko) pri dsep = 2.5
//          diff = 0.7; // prvni mesto


          diff = 1.1; // druhe mesto
          diff = dsep;


//         diff = 4.4; // treti mesto
//         diff = 0.7; // ctvrte mesto

//          diff = (dsep / 5.0 + ( 1.0 - bilinearInterpolation(popDensity,xy[0],xy[1],height,width) / 200.0)) * gauge; // 250.0 - something like max pop
//          diff = 0.3;

          // diff muze bejt i 1.1 neni to takovej problem ale je treba udelat
          float* tooNear = checkPoint(xy[0],xy[1],point1[0],point1[1],point2[0],point2[1],diff);

          if (tooNear != NULL)
          {
//            hyperstreamline.push_back(tooNear);
            if (hyperstreamline.size()>3)
            {
//              hyperstreamline.push_back(tooNear);
// Nevynechavat tooNear ale resit trosku jinak
            }
            else
            {
//              workAround = true;
int p = 9;
            }
            go = false;
//            delete[] tooNear;
            break;
          }

          delete[] tooNear;

        }

        if (!go)
        {
          break;
        }
      }
//old

//new
//      for (std::list<std::list<float*> >::iterator hslListIt = hslList->begin(); hslListIt != hslList->end(); ++hslListIt)
//      {
//        std::list<float*> hsl = *hslListIt;
//        std::list<float*> tooNearList;
//        // spoliham tu na push_back
//        if (!mod2)
//        {
//          if ((++hslListIt) == hslList->end())
//          {
//            break;
//          }
//          --hslListIt;
//        }
//        // posud
//        for (std::list<float*>::iterator iter = hsl.begin(); iter != hsl.end(); ++iter)
//        {
//          float* point1 = *iter;
//          if ((++iter) == hsl.end())
//          {
//            break;
//          }
//          float* point2 = *iter;
//          iter--;
//
////          float* intersection = lineIntersection(seedX,seedY,xy[0],xy[1],point1[0],point1[1],point2[0],point2[1]);
////          // protnou nebo pridat ze jsou velmi !VELMI! blizko pak taky skoncit
////          // plus pridat jeste podminku na vyhledani! mozneho pripojeni po padesati bodech?
////          if (intersection != NULL)
////          {
////            hyperstreamline.push_back(intersection);
////            printf("was here!");
////            go = false;
//////            delete[] intersection;
////            break;
////          }
////          delete[] intersection;
//
////          diff = dsep - (255.0 / bilinearInterpolation(popDensity,xy[0],xy[1],height,width)) * 1.5; // prevest na koncept dsep - (0,1)??
////          diff = 0.5; // idealne mezi 0.5-0.7 mozne posunout az na 0.35 (mozne min? asi ano ale uz je to moc blizko) pri dsep = 2.5
////          diff = 0.7; // idealne mezi 0.5-0.7 mozne posunout az na 0.35 (mozne min? asi ano ale uz je to moc blizko) pri dsep = 2.5
////          diff = 0.7; // prvni mesto
//
//
//          diff = 1.1; // druhe mesto
//          diff = dsep;
//
//
////         diff = 4.4; // treti mesto
////         diff = 0.7; // ctvrte mesto
//
////          diff = (dsep / 5.0 + ( 1.0 - bilinearInterpolation(popDensity,xy[0],xy[1],height,width) / 200.0)) * gauge; // 250.0 - something like max pop
////          diff = 0.3;
//
//          // diff muze bejt i 1.1 neni to takovej problem ale je treba udelat
//          float* tooNear = checkPoint(xy[0],xy[1],point1[0],point1[1],point2[0],point2[1],diff);
//
//          if (tooNear != NULL)
//          {
////            hyperstreamline.push_back(tooNear);
//            if (hyperstreamline.size()>3)
//            {
////              hyperstreamline.push_back(tooNear);
//              tooNearList.push_back(tooNear);
//// Nevynechavat tooNear ale resit trosku jinak
//            }
//            else
//            {
//              go = false;
//              break;
////              workAround = true;
//int p = 9;
//            }
////            go = false;
//////            delete[] tooNear;
////            break;
//          }
//
////          delete[] tooNear;
//
//        }
//
//        if (!tooNearList.empty())
//        {
//          float* nearestPoint;
//          float minimDistance = (float) (height * width);
//          for (std::list<float*>::iterator tooNearIt = tooNearList.begin(); tooNearIt != tooNearList.end(); tooNearIt++)
//          {
//            float* tooNear = *tooNearIt;
//            float distance = euclideanDistance(tooNear[0],tooNear[1],xy[0],xy[1]);
//            if (distance < minimDistance)
//            {
//              minimDistance = distance;
//              nearestPoint = tooNear;
//            }
//          }
//          hyperstreamline.push_back(nearestPoint);
//
//          for (std::list<float*>::iterator tooNearIt = tooNearList.begin(); tooNearIt != tooNearList.end(); tooNearIt++)
//          {
//            float* tooNear = *tooNearIt;
//            if (tooNear != nearestPoint)
//            {
//              delete[] tooNear;
//            }
//          }
//          tooNearList.clear();
//
//          go = false;
//          break;
//        }
//      }
////new
      if (bilinearInterpolation(popDensity,xy[0],xy[1],height,width) < 1.0)
      {
//            hyperstreamline.push_back(xy);
        go = false;
//            delete[] tooNear;
      }

      seedX = xy[0];
      seedY = xy[1];
      i++;
    }

  // mozny cyklus - vylepsit a hlavne PRIDAT podminku na cyklus pokud jedna hyperstreamline narazi do jiz existujici - tak ji ukoncit.
  //for (std::list<std::list<float*> >::iterator it = hslList.begin(); it != hslList.end(); ++it)
  //    {
  //      std::list<float*> hsl = *it;
  //      for (std::list<float*>::iterator iter = hsl.begin(); iter != hsl.end(); ++iter)
  //      {
  //        float* nuf = *iter;
  //        if (nuf[0] > (xy [0]-conK) && nuf[0] < (xy [0]+conK) && nuf[1] > (xy [1]-conK) && nuf[1] < (xy [1]+conK))
  //        {
  //          hyperstreamline.push_back(xy);
  //          go = false;
  //          break;
  //        }
  //      }
  //    }
int size = hyperstreamline.size();
float tmp = 0.0;
std::list<float*>::iterator hIt = hyperstreamline.begin();
float* p1 = *hIt;
hIt++;
for (; hIt != hyperstreamline.end(); hIt++)
{
  float* p2 = *hIt;
  tmp += euclideanDistance(p1[0],p1[1],p2[0],p2[1]);
  p1=p2;
}

if (!workAround && hyperstreamline.size() > 2)
{
    hslList->push_back(hyperstreamline);
}
else
{
  hyperstreamline.clear();
  hslList->push_back(hyperstreamline);
}
//  hyperstreamline.clear();

//  delete[] xy;
  return workAround;
}

float* findPointOnSegment(float* from,float* to, float distance)
{
  float vectorX = to [0] - from [0];
  float vectorY = to [1] - from [1];
  float vectorsize = euclideanDistance(0.0,0.0,vectorX,vectorY);
  vectorX /= vectorsize;
  vectorY /= vectorsize;
  float* point = new float[2];
  point[0] = from[0] + vectorX * distance;
  point[1] = from[1] + vectorY * distance;
  return point;
}

void generateSeeds(std::list<float*>* seedQueue, std::list<float*>* hsl, float dsep, Population* population, float gauge)
{
  float** popDensity = population->getDensity();
  int width = population->width;
  int height = population->height;

  float pointsDistance, distance = 0.0;
  std::list<float*>::iterator hslIt = hsl->begin();
  float* point1 = *hslIt;
  float* point2;
  float diff = dsep * gauge;
  int counter = 0;
  hslIt++;
  for (hslIt; hslIt != hsl->end(); ++hslIt/*, counter++*/)
  {
    point2 = *hslIt;
    pointsDistance = euclideanDistance(point1[0],point1[1],point2[0],point2[1]);


//    diff = dsep * gauge * (2.0 - bilinearInterpolation(popDensity, point1[0], point1[1], width, height) / 250.0); // vytahnout z populace
//    diff = dsep * gauge * (4.0 - bilinearInterpolation(popDensity, point1[0], point1[1], width, height) / 64.0); // vytahnout z populace
//    diff = dsep * gauge * (2.0 - bilinearInterpolation(popDensity, point1[0], point1[1], width, height) / 128.0); // vytahnout z populace
//    diff = 1.1;
    diff = dsep;// / 1.4;
//    diff = 0.21;
//    diff = dsep;


    if (distance + pointsDistance > diff)
    {
      float* seed = findPointOnSegment(point1, point2, diff - distance);
//      seedQueue->push_back(seed);
      distance = distance + pointsDistance - diff;
      if (bilinearInterpolation(popDensity, seed[0], seed[1], width, height) < 1.0) // add width height, vytvorit zavislost lepsi na vysce
      {
        return;
        // chyba ci feature? kvuli tomu ze to je az za push_back to vytvari divny kusy silnice, jelikoz se vzdycky vytvori ten jeden bod
        // kterym to pokracuje a vznikaj schody
      }
        seedQueue->push_back(seed);
//        printf("seed0: %f, seedf1: %f\n",seed[0],seed[1]);
    }
    else
    {
      distance += pointsDistance;
    }

    point1 = point2;

  }
//  if (counter>1)
//  {
//    seedQueue->push_back(point2);
//    seedQueue->push_back(*hsl->begin()); //nefunguje?
//  }

}


//bool deleteAll(float* theElement )
//{
//   delete[] theElement;
//   return true;
//}

void hyperstreamlines(Terrain* terrain, Population* population, std::list<std::list<float*> > *hslList, std::list<std::list<float*> > *majorHslList, std::list<std::list<float*> > *minorHslList)
{
  float** heightMap = terrain->getHeightMap();
  int width = terrain->getWidth();
  int height = terrain->getHeight();
  float*** gradF = gradientField(heightMap,height,width);
  float*** oGradF = gradientField(heightMap,height,width);
  orthogonalField(oGradF,width,height); // predelat orthogonalField na vraceci
  float*** minusGradF = gradientField(heightMap,height,width);
  minusField(minusGradF,height,width);// predelat orthogonalField na vraceci
  float*** minusOGradF = gradientField(heightMap,height,width);
  orthogonalField(minusOGradF,width,height);// predelat orthogonalField na vraceci
  minusField(minusOGradF,height,width);// predelat orthogonalField na vraceci

  float** popDensity = population->getDensity();

  float mapScale = 1.0; // == 1bunka = 1km --- az pujde skalovaci meritko vejs tak se musi zaradit i do gradientField
  // meritko nastavit na prevracenou hodnotu
  // repairGradF()
//  float dsep = 4.5 * gauge; // default bude mezi 2.5 - 5.0 bych to videl
//  float dsep = 4.5 * gauge; // prvni mesto


  float dsep = 3.7 * mapScale; // druhe mesto
  dsep = 2.8;
  dsep = 0.5;
  dsep = 1.2;


//  float dsep = 1.7 * gauge; // ctvrte mesto
//  float dsep = 3.75 * gauge; // default bude mezi 2.5 - 5.0 bych to videl
//  float dsep = 2.5 * gauge; // default bude mezi 2.5 - 5.0 bych to videl
  int konec = 1;
//
//  std::list<std::list<float*> > majorHslList;
//  std::list<std::list<float*> > minorHslList;
//
  std::list<float*> seedQueue;
  std::list<bool> majorList; // correspond to which vector field was used for generation hsl
  float* initSeed = findInitSeed(hslList,dsep,gradF,width,height,popDensity);
//dsep = D_REG - (1.7 * bilinearInterpolation(popDensity, initSeed[0], initSeed[1], width, height) / 250.0); // DEFAULT
dsep = 2.2 - (1.7 * bilinearInterpolation(popDensity, initSeed[0], initSeed[1], width, height) / 250.0); // DEFAULT
//dsep = 5.5;// - (1.7 * bilinearInterpolation(popDensity, initSeed[0], initSeed[1], width, height) / 250.0);
  bool workAround;
  hyperstreamline(heightMap,width,height,majorHslList,gradF,initSeed[0],initSeed[1],dsep,popDensity,mapScale);
  hyperstreamline(heightMap,width,height,majorHslList,minusGradF,initSeed[0],initSeed[1],dsep,popDensity,mapScale);
  hyperstreamline(heightMap,width,height,minorHslList,oGradF,initSeed[0],initSeed[1],dsep,popDensity,mapScale);
  hyperstreamline(heightMap,width,height,minorHslList,minusOGradF,initSeed[0],initSeed[1],dsep,popDensity,mapScale);
//  std::list<std::list<float*> >::iterator initHsl = majorHslList.begin();

  std::list<std::list<float*> >::iterator someIt = majorHslList->begin();
//  for(int i = 0; i < majorHslList->size()-2; i++)// pripraveno pro iteracni tvoreni ulic
//  {
//    ++someIt;//vylepsit s std::advance NAHRADIT PRO PUSH_FRONT?
//  }

//  hslList->push_back(*(majorHslList->begin()));
  hslList->push_back(*someIt);
  int bbb = (*someIt).size();
  majorList.push_back(true);
  ++someIt;
  hslList->push_back(*someIt);
  majorList.push_back(true);

  std::list<std::list<float*> >::iterator someOIt = minorHslList->begin();
//  for(int i = 0; i < minorHslList->size()-2; i++)// pripraveno pro iteracni tvoreni ulic
//  {
//    ++someOIt;//vylepsit s std::advance NAHRADIT PRO PUSH_FRONT?
//  }
  hslList->push_back(*someOIt);
  majorList.push_back(false);
  ++someOIt;
  hslList->push_back(*someOIt);
  majorList.push_back(false);

for (std::list<std::list<float*> >::iterator hslListIt = hslList->begin(); hslListIt != hslList->end(); ++hslListIt)
{
  int iii = (*hslListIt).size();
  int aa = 6;
}
  std::list<bool>::iterator majorIt = majorList.begin();
  std::list<std::list<float*> >::iterator hslListIt = hslList->begin(); // priprava na iterace
//  for(int i = 0; i < hslList->size()-4; i++) // pripraveno pro iteracni tvoreni ulic
//  {
//    ++hslListIt;//vylepsit s std::advance
//  }

FILE* file = fopen("debug/seedPoints.svg","w+");
int magnify = 10;
fprintf(file,"<svg width=\"%dpx\" height=\"%dpx\" xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" >\n", (width+1)*magnify, (height+1)*magnify);
  for (hslListIt; hslListIt != hslList->end(); ++hslListIt, ++majorIt)
  {
    std::list<float*> hsl = *hslListIt;
    bool major = *majorIt;
    int num = hslList->size();
    //    refreshDsep(&dsep); sem nebo az dovnitr generate?
    generateSeeds(&seedQueue,&hsl,dsep,population,mapScale);
    int blem = seedQueue.size();
    for (std::list<float*>::iterator seedQueueIt = seedQueue.begin(); seedQueueIt != seedQueue.end(); ++seedQueueIt)
    {
      float* seed = *seedQueueIt;
      refreshDsep(&dsep, seed, population, major);
      if (major)
      {
        //    refreshDsep(&dsep); sem nebo az dovnitr ?
        float ss = seed [0];
        float ss1 = seed [1];
//        std::list<std::list<float*> >::iterator lastHsl = (minorHslList.end()--); ne-e
        std::list<std::list<float*> >::iterator lastHsl = minorHslList->begin();

        workAround = hyperstreamline(heightMap,width,height,minorHslList,oGradF,seed[0],seed[1],dsep,popDensity,mapScale);

        if (!workAround)
        {
        for (std::list<std::list<float*> >::iterator it = ++minorHslList->begin(); it != minorHslList->end(); it++) // zkontrolovat jestli nevznika chyba kdyz vytvorim uz vraceci hyperstreamline true/false u ++
        {
          ++lastHsl;
// jit pozpatku !!! resp. udelat lastHsl = --end
        }
//        for(int i = 0; i < minorHslList->size()-1; i++)
//        {
//          ++lastHsl;//vylepsit s std::advance
//        }// vytvorit global  iteratory lastMinorHSl a major a vzdycky udelat jen ++ a push_back; premenit na push_front??? uvnitr hyperstreamline
        std::list<float*> lasths = *lastHsl;
        hslList->push_back(lasths);
        majorList.push_back(false);
        }

        workAround = hyperstreamline(heightMap,width,height,minorHslList,minusOGradF,seed[0],seed[1],dsep,popDensity,mapScale);
//        ++lastHsl;
//        lasths = *lastHsl;
//        hslList->push_back(lasths);
//        majorList.push_back(false);
lastHsl = minorHslList->begin();
        if (!workAround)
        {
        for (std::list<std::list<float*> >::iterator it = ++minorHslList->begin(); it != minorHslList->end(); it++) // zkontrolovat jestli nevznika chyba kdyz vytvorim uz vraceci hyperstreamline true/false u ++
        {
          ++lastHsl;
// jit pozpatku !!! resp. udelat lastHsl = --end
        }
//        for(int i = 0; i < minorHslList->size()-1; i++)
//        {
//          ++lastHsl;//vylepsit s std::advance
//        }// vytvorit global  iteratory lastMinorHSl a major a vzdycky udelat jen ++ a push_back; premenit na push_front??? uvnitr hyperstreamline
        std::list<float*> lasths = *lastHsl;
        hslList->push_back(lasths);
        majorList.push_back(false);
        }
      }
      else
      {
        std::list<std::list<float*> >::iterator lastHsl = majorHslList->begin();

        //    refreshDsep(&dsep); sem nebo az dovnitr ?
        workAround = hyperstreamline(heightMap,width,height,majorHslList,gradF,seed[0],seed[1],dsep,popDensity,mapScale);
        if (!workAround)
        {
        for (std::list<std::list<float*> >::iterator it = ++majorHslList->begin(); it != majorHslList->end(); it++) // zkontrolovat jestli nevznika chyba kdyz vytvorim uz vraceci hyperstreamline true/false u ++
        {
          ++lastHsl;
// jit pozpatku !!! resp. udelat lastHsl = --end
        }
//        for(int i = 0; i < majorHslList->size()-1; i++)
//        {
//          ++lastHsl;//vylepsit s std::advance
//        }// vytvorit global  iteratory lastMinorHSl a major a vzdycky udelat jen ++ a push_back;
        std::list<float*> lasths = *lastHsl;
        hslList->push_back(lasths);
        majorList.push_back(true);
        }
lastHsl = majorHslList->begin();
        workAround = hyperstreamline(heightMap,width,height,majorHslList,minusGradF,seed[0],seed[1],dsep,popDensity,mapScale);
                if (!workAround)
        {
        for (std::list<std::list<float*> >::iterator it = ++majorHslList->begin(); it != majorHslList->end(); it++) // zkontrolovat jestli nevznika chyba kdyz vytvorim uz vraceci hyperstreamline true/false u ++
        {
          ++lastHsl;
// jit pozpatku !!! resp. udelat lastHsl = --end
        }
//        for(int i = 0; i < majorHslList->size()-1; i++)
//        {
//          ++lastHsl;//vylepsit s std::advance
//        }// vytvorit global  iteratory lastMinorHSl a major a vzdycky udelat jen ++ a push_back;
        std::list<float*> lasths = *lastHsl;
        hslList->push_back(lasths);
        majorList.push_back(true);
        }
      }
    }

if (debug)
{
float multiply = (float) magnify;
for (std::list<float*>::iterator seedQueueIt = seedQueue.begin(); seedQueueIt != seedQueue.end(); seedQueueIt++)
{
float* seed = *seedQueueIt;
if (major)
{
fprintf(file,"\t<circle cx=\"%f\" cy=\"%f\" r=\"1\" style=\"fill:rgb(0,0,255)\"/>\n",seed[1]*multiply,seed[0]*multiply);
}
else
{
fprintf(file,"\t<circle cx=\"%f\" cy=\"%f\" r=\"1\" style=\"fill:rgb(255,0,0)\"/>\n",seed[1]*multiply,seed[0]*multiply);
}
}
}
    seedQueue.clear();
  }

fprintf(file,"</svg>");
fclose(file);
if (debug)
{
          FILE* filea = fopen("debug/major.svg","w+");
  int magnify = 10;
  float multiply = (float) magnify;
  fprintf(filea,"<svg width=\"%dpx\" height=\"%dpx\" xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" >\n", width*magnify, height*magnify);
  for (std::list<std::list<float*> >::iterator it = majorHslList->begin(); it != majorHslList->end(); ++it)
  {
    std::list<float*> hsl1 = *it;
    for (std::list<float*>::iterator iter = hsl1.begin(); iter != hsl1.end(); ++iter)
    {
      float* bit1 = *iter;
      if ((++iter) == hsl1.end())
      {
        break; // vylepsit na dva iteratory
      }
      float* bit2 = *iter;
        iter--;
      fprintf(filea,"\t<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" style=\"stroke:rgb(0,0,0);stroke-width:1\" />\n",bit1[1]*multiply,bit1[0]*multiply,bit2[1]*multiply,bit2[0]*multiply);
    }
  }
  fprintf(filea,"</svg>");
  fclose(filea);
}

if (debug)
{
          FILE* filea = fopen("debug/minor.svg","w+");
  int magnify = 10;
  float multiply = (float) magnify;
  fprintf(filea,"<svg width=\"%dpx\" height=\"%dpx\" xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" >\n", width*magnify, height*magnify);
  for (std::list<std::list<float*> >::iterator it = minorHslList->begin(); it != minorHslList->end(); ++it)
  {
    std::list<float*> hsl1 = *it;
    for (std::list<float*>::iterator iter = hsl1.begin(); iter != hsl1.end(); ++iter)
    {
      float* bit1 = *iter;
      if ((++iter) == hsl1.end())
      {
        break; // vylepsit na dva iteratory
      }
      float* bit2 = *iter;
        iter--;
      fprintf(filea,"\t<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" style=\"stroke:rgb(0,0,0);stroke-width:1\" />\n",bit1[1]*multiply,bit1[0]*multiply,bit2[1]*multiply,bit2[0]*multiply);
    }
  }
  fprintf(filea,"</svg>");
  fclose(filea);
}

  for (int i=0; i<width; i++)
  {
    for (int j=0; j<width; j++)
    {
      delete[] gradF[i][j];
    }
  }
  for(int i = 0; i < width; i++)
  {
    delete[] gradF[i];
  }
  delete[] gradF;

  for (int i=0; i<width; i++)
  {
    for (int j=0; j<width; j++)
    {
      delete[] oGradF[i][j];
    }
  }
  for(int i = 0; i < width; i++)
  {
    delete[] oGradF[i];
  }
  delete[] oGradF;
}

void mergeOppositeHyperstreamlines(std::list<std::list<float*> > *hslList,std::list<std::list<float*> > *mHslList)
{
  std::list<std::list<float*> >::iterator mit = mHslList->begin();
  std::list<std::list<float*> >::iterator mit2 = mHslList->begin();
  mit2++;
  for (;;mit++,mit2++)
  {
    std::list<float*> hsl = *mit;
    std::list<float*> hsl2 = *mit2;
    for (std::list<float*>::iterator pointIt = (++hsl2.begin()); pointIt != hsl2.end(); pointIt++)
    {
      float* point = *pointIt;
      hsl.push_front(point);
    }
    hslList->push_back(hsl);
    mit++;
    mit2++;
    if (mit2 == mHslList->end())
    {
      break;
    }
  }
}

void orthogonalField(float*** vectorField,int width,int height)
{
  float ttmmpp;
  for (int i=0; i < width; ++i)
  {
    for (int j=0; j < height; ++j)
    {
      ttmmpp = vectorField[i][j][0];
      vectorField[i][j][0] = vectorField[i][j][1];
      vectorField[i][j][1] = -ttmmpp;
    }
  }
}

void minusField(float*** vectorField,int width,int height)
{
  for (int i=0; i < width; ++i)
  {
    for (int j=0; j < height; ++j)
    {
      vectorField[i][j][0] = -vectorField[i][j][0];
      vectorField[i][j][1] = -vectorField[i][j][1];
    }
  }
}

void refreshDsep(float* dsep, float* seed, Population* population, bool major)
{
  *dsep = 4.5 - (1.5 * bilinearInterpolation(population->getDensity(), seed[0], seed[1], population->width, population->height) / 250.0);
//  *dsep = D_REG - (1.5 * bilinearInterpolation(population->getDensity(), seed[0], seed[1], population->width, population->height) / 250.0);
//  *dsep = 2.5;// - (2.0 * bilinearInterpolation(population->getDensity(), seed[0], seed[1], population->width, population->height) / 250.0);
  if (major)
  {
    *dsep /= 1.8;
  }
}
