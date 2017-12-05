#include "Block.hpp"

#include "City.hpp"
#include "Land.hpp"
#include "Node.hpp"
#include "Population.hpp"
#include "Road.hpp"
#include "Roadmap.hpp"
#include "constants.hpp"
#include "utilities.hpp"
#include <math.h>

Block::Block()
{
  borders = new std::list<Road*>();
  lands = new std::list<Land*>();
  typeOfBlock = -1;
}
/** \brief return modified depth of land based on type of block and average population density in block
 *
 * \param type int type of block
 * \param averagePop float average population density in block
 * \return float modified depth of land
 *
 */
float depth(int type,float averagePop) //pridat nejakej random epsilon (nekdy nahodne prohodit depth a width?
{
  float depthOfLand = DEPTH_OF_LAND;
//  if (type == SMALL_BUILDING)
//  {
//    depthOfLand *= (2.5 - averagePop / 70.0);
//  }
//  else if (type == BLOCK_BUILDING)
//  {
//    depthOfLand *= (1.0 + averagePop / 350.0);
//  }
//  else if (type == FLATS_BUILDING)
//  {
//    depthOfLand *= (1.2 + averagePop / 350.0);
//  }
  return depthOfLand;
}

/** \brief return modified width of land based on type of block and average population density in block
 *
 * \param type int type of block
 * \param averagePop float average population density in block
 * \return float modified width of land
 *
 */
float width(int type,float averagePop) //pridat nejakej random epsilon (mozna zapojit i velikost pozemku?)
{
  float widthOfLand = WIDTH_OF_LAND;
//  if (type == SMALL_BUILDING)
//  {
//    widthOfLand *= (2.5 - averagePop / 70.0);
//  }
//  else if (type == BLOCK_BUILDING)
//  {
//    widthOfLand *= (1.0 + averagePop / 350.0);
//  }
//  else if (type == FLATS_BUILDING)
//  {
//    widthOfLand *= (2.1 + averagePop / 200.0);
//  }
  return widthOfLand;
}

/** \brief returns list of points that are visited during walk by distance - from here by road with curvature mainRoadCurvature.
 *         If first is -1 on first position in first pointer in return list then too long distance to be covered, in second position is remaining distance
 *
 * \param road Road* current road that i go on
 * \param from Node* start node of going
 * \param thisWay std::list<float*>::iterator* pointer to iterator of next point in the road curvature that i visit
 * \param here float* const current position
 * \param distance float distance to be covered
 * \param mainRoadCurvature std::list<float*>* curvature of current road
 * \return std::list<float*>* list of points that was visited during go
 *
 */
std::list<float*>* goDistanceByRoad(Road* road, Node* from, std::list<float*>::iterator* thisWay,float* const here, float distance, std::list<float*>* mainRoadCurvature) // delete mainRoadCurvature
{
  float* nextPoint;
  std::list<float*>* pointsAlongRoad = new std::list<float*> ();
  float* tmpHere = here;
  float vectX,vectY,pointDistance;
//  std::list<float*> roadCurvature = road->curvature;

  if (road->from == from)
  {
    while (**thisWay != *(mainRoadCurvature->end()))
    {
      nextPoint = **thisWay;
      vectX = nextPoint[0] - tmpHere[0];
      vectY = nextPoint[1] - tmpHere[1];
//      float a = nextPoint[0];
//      float a1 = nextPoint[1];
//      float a11 = tmpHere[0];
//      float a112 = tmpHere[1];
      pointDistance = sizeOfVector(vectX,vectY);
      if (distance > pointDistance)
      {
        distance -= pointDistance;
        pointsAlongRoad->push_back(nextPoint);
        tmpHere = nextPoint;
        (*thisWay)++;
      }
      else
      {
        float k = distance/pointDistance;
        float* newHere = new float [2];
        newHere[0] = tmpHere[0] + k*vectX;
        newHere[1] = tmpHere[1] + k*vectY;
        pointsAlongRoad->push_back(newHere);
        return pointsAlongRoad;
      }
    }
    vectX = road->to->x - tmpHere[0];
    vectY = road->to->y - tmpHere[1];
    pointDistance = sizeOfVector(vectX,vectY);
    float* newHere = new float [2];
    if (distance > pointDistance)
    {
      newHere[0] = road->to->x;
      newHere[1] = road->to->y;
      pointsAlongRoad->push_back(newHere);
    }
    else
    {
      float k = distance/pointDistance;
      newHere[0] = tmpHere[0] + k*vectX;
      newHere[1] = tmpHere[1] + k*vectY;
      pointsAlongRoad->push_back(newHere);
      return pointsAlongRoad;
    }
  }
  else
  {
//if (abs(from->x - 297.764)< 0.01 && abs(from->y - 85.868)< 0.01)
//{
//int i = 111;
//}
    bool skip = false;
    if (**thisWay == *(mainRoadCurvature->begin()))
    {
      skip = true;
    }
    while (!skip)
    {
      int a = mainRoadCurvature->size();
      float* b = *(mainRoadCurvature->begin());
      float* c = *(++(mainRoadCurvature->begin()));
      float* d = *(mainRoadCurvature->rbegin());
      nextPoint = **thisWay;
      vectX = nextPoint[0] - tmpHere[0];
      vectY = nextPoint[1] - tmpHere[1];

      pointDistance = sizeOfVector(vectX,vectY);
      if (distance > pointDistance)
      {
        distance -= pointDistance;
        pointsAlongRoad->push_back(nextPoint);
        tmpHere = nextPoint;
        if (**thisWay == *mainRoadCurvature->begin())
        {
          break;
        }
        (*thisWay)--;
        float* q = **thisWay;
        int i = 6;
      }
      else
      {
        float k = distance/pointDistance;
        float* newHere = new float [2];
        newHere[0] = tmpHere[0] + k*vectX;
        newHere[1] = tmpHere[1] + k*vectY;
        pointsAlongRoad->push_back(newHere);
        return pointsAlongRoad;
      }
    }
    vectX = road->from->x - tmpHere[0];
    vectY = road->from->y - tmpHere[1];
    pointDistance = sizeOfVector(vectX,vectY);
    float* newHere = new float [2];
    if (distance > pointDistance)
    {
      newHere[0] = road->from->x;
      newHere[1] = road->from->y;
      pointsAlongRoad->push_back(newHere);
    }
    else
    {
      float k = distance/pointDistance;
      newHere[0] = tmpHere[0] + k*vectX;
      newHere[1] = tmpHere[1] + k*vectY;
      pointsAlongRoad->push_back(newHere);
      return pointsAlongRoad;
    }
  }

//  pointsAlongRoad->clear();
//  delete pointsAlongRoad;
//  return NULL; // too long
  float* newHere = new float [2];
  newHere[0] = -1.0;
  newHere[1] = distance;
  pointsAlongRoad->push_front(newHere);
  return pointsAlongRoad; //if first is -1 then too long, in second is remaining distance
}

/** \brief return pointer of size 2 with fourth point minus first point.
 *         Used to compute third point of land.
 *
 * \param first float* first point
 * \param twoFurthestPoints std::list<float*> not used
 * \param fourth float* fourth point
 * \return float* return fourth minus first
 *
 */
float* findBuildingMainVector(float* first,std::list<float*> twoFurthestPoints,float* fourth)
{
  float* pointTwo = *(--twoFurthestPoints.end());
  float* buildingMainVector = new float [2];
//  if (pointTwo == NULL)
//  {
    buildingMainVector[0] = fourth[0] - first[0];
    buildingMainVector[1] = fourth[1] - first[1];
//  }
//  else
//  {
//    float* pointOne = *(twoFurthestPoints.begin());
//    buildingMainVector[0] = pointTwo[0] - pointOne[0];
//    buildingMainVector[1] = pointTwo[1] - pointOne[1];
//  }
  return buildingMainVector;
}

/** \brief compute and return list of points from second to third of land
 *
 * \param buildingMainVector float* vector used for finding border created by connection of second and third point of land
 * \param threePoints std::list<float*> points connected from first to four
 * \param depthOfLand float depth of land
 * \return std::list<float*>* list of new points of land
 *
 */
std::list<float*>* findThirdBorderPoint(float* buildingMainVector,std::list<float*> threePoints, float depthOfLand) // pridat check s hranicema bloku
{
  std::list<float*>* fromSecondToThird = new std::list<float*> ();
  float* zero = new float [2];
  zero[0] = 0.0;
  zero[1] = 0.0;
  float* buildingSideVector = getLeftNormalVector(zero,buildingMainVector);
  float coef = depthOfLand/sizeOfVectorN(buildingSideVector,2);
  for (std::list<float*>::iterator threePointsIt = threePoints.begin(); threePointsIt != threePoints.end(); threePointsIt++)
  {
    float* point = *threePointsIt;
    if (point != NULL)
    {
      float* newPoint = new float [2];
      newPoint[0] = point[0] + coef * buildingSideVector[0];
      newPoint[1] = point[1] + coef * buildingSideVector[1];
      fromSecondToThird->push_back(newPoint);
    }
  }
  return fromSecondToThird;
}

/** \brief create borders in newBorderLands from other parameters.
 *         It take first point then list of fromFirstToSecond, then continue with fromSecondToThird and finalize with reverted list of fromFirstToFourth.
 *
 * \param newBorderLands std::list<float*>* newly created borders
 * \param first float* start point
 * \param fromFirstToSecond std::list<float*>* first part of border
 * \param fromSecondToThird std::list<float*>* second part of border
 * \param fromFirstToFourth std::list<float*>* last part of border
 * \return void
 *
 */
void createBorderLands(std::list<float*>* newBorderLands,float* first,std::list<float*>* fromFirstToSecond,std::list<float*>* fromSecondToThird,std::list<float*>* fromFirstToFourth)
{
//  int ppp = newBorderLands->size
  newBorderLands->push_back(first);
  if (fromFirstToSecond != NULL)
  {
    for(std::list<float*>::iterator borderPointsIt = fromFirstToSecond->begin(); borderPointsIt != fromFirstToSecond->end(); borderPointsIt++)
    {
      float* borderPoint = *borderPointsIt;
      float aa = borderPoint[0];
      float aab = borderPoint[1];
      newBorderLands->push_back(borderPoint);
    }
  }
  for(std::list<float*>::iterator borderPointsIt = fromSecondToThird->begin(); borderPointsIt != fromSecondToThird->end(); borderPointsIt++)
  {
    float* borderPoint = *borderPointsIt;
      float aa = borderPoint[0];
      float aab = borderPoint[1];
    newBorderLands->push_back(borderPoint);
  }
  for(std::list<float*>::reverse_iterator borderPointsIt = fromFirstToFourth->rbegin(); borderPointsIt != fromFirstToFourth->rend(); borderPointsIt++)
  {
    float* borderPoint = *borderPointsIt;
      float aa = borderPoint[0];
      float aab = borderPoint[1];
    newBorderLands->push_back(borderPoint);
  }
}

void Block::generateBlock(City* city)
// CHYBA UDELAT KONSTRUKTOR BEZ TOHO A PAK UDELAT GENERATE LANDS!!!!!!

//Lands::Lands(Blocks* blocksWrapper,std::list<Road*>* roads,Population* population)
{
    std::list<Road*>* roads = city->getRoadMap()->getRoads();
    Population* population = city->getPopulation();
//  std::list<Block*>* blocks = blocksWrapper->getBlocks();
//  this->buildingLands = new std::list<Land*> ();
//  this->vegetationLand = new std::list<Land*> ();
//  int qqpp = -10000;
//  for (std::list<Block*>::iterator blockIt = blocks->begin(); blockIt!= blocks->end(); blockIt++)
//  {
//    Block* block = *blockIt;
    float blockArea = computeBlockArea(this);
    float averagePop = sumUpPopulation(population);
//    Road* mainRoad = findLongestRoad(block);
//    float depthOfLand = 1.2;
//    float widthOfLand = 1.2;
    this->typeOfBlock = setTypeOfBlock(averagePop);
    float depthOfLand = depth(this->typeOfBlock,averagePop);
    float widthOfLand = width(this->typeOfBlock,averagePop);

//    bool end = false;

//    while (blockArea < MIN_SIZE || blockArea > MAX_SIZE || averagePop < MIN_DENSITY)
    if (blockArea < MIN_SIZE || blockArea > MAX_SIZE || averagePop < MIN_DENSITY)
      //blockArea < depthOfLand*widthOfLand*1.5
    {
      //add pozemek cely + pridat typ
      return;
//      blockIt++;
//      block = *blockIt;
//      if (blockIt == blocks->end())
//      {
//        end = true;
//        break; //uplne skonci -> dodelat
//      }
//
//      if (block != NULL && block->borders != NULL)//kdy???
//      {
//        blockArea = computeBlockArea(block);
//        averagePop = sumUpPopulation(block, population);
//        int blockType = typeOfBlock(block,averagePop);
//        block->typeOfBlock = blockType;
//        depthOfLand = depth(blockType,averagePop);
//        widthOfLand = width(blockType,averagePop);
//      }
    }
//    if (end)
//    {
//      break;
//    }
//    this->borders = new std::list<Road*>();
    this->lands = new std::list<Land*>();

    std::list<Road*>::iterator bordersIt = this->borders->begin();
    Road* mainRoad = *bordersIt;
    if (this->borders->empty())
    {
      return;
    }
    std::list<float*>* mainRoadCurvature = mainRoad->curvature;
    std::list<float*>::iterator mainRoadCurvatureIt = mainRoadCurvature->begin();
    Node* firstNode = mainRoad->from;
    bordersIt++;
    Road* sideRoad = *bordersIt;
    if (sideRoad->from == firstNode || sideRoad->to == firstNode)
    {
      firstNode = mainRoad->to;
      mainRoadCurvatureIt = mainRoadCurvature->end();
      mainRoadCurvatureIt--;
    }
    bordersIt--;
    //Road* findNextRoad(Node* here, Road* previousRoad, Block* block);
    std::list<Road*>::iterator lastRoadIt = this->borders->end();
    lastRoadIt--;
    Road* lastRoad = *lastRoadIt;
    std::list<float*>* lastRoadCurvature = lastRoad->curvature;
    std::list<float*>::iterator lastRoadCurvatureIt = lastRoadCurvature->begin();
    if (lastRoad->from == firstNode || lastRoad->to == firstNode)
    {
      lastRoadCurvatureIt = lastRoadCurvature->end();
      lastRoadCurvatureIt--;
    }

    float angle = leftAngleBetweenRoads(firstNode,lastRoad,mainRoad);
    float distanceOnRoad = 0.5;
    // first point
    float xx,yy;
    bool nooo = true;

    Land* land = new Land();
    land->borders = new std::list<float*> ();

    this->lands->push_back(land);//docasne
//    vegetationLand->push_back(land);//docasne

    float* first = new float [2];
    first[0] = firstNode->x;
    first[1] = firstNode->y;

    std::list<float*>* fromFirstToSecond = goDistanceByRoad(lastRoad,firstNode,&lastRoadCurvatureIt,first,depthOfLand,mainRoadCurvature);

//if (qqpp>38)
//{
//  float a = 6;
//  drawLands(10,roads);
//}
    float* second = *(fromFirstToSecond->begin());
    if (second[0] != -1.0)
    {
      std::list<float*> fromSecondToFirst;
      second = *(--(fromFirstToSecond->end()));
      for(std::list<float*>::iterator fromFirstToSecondIt = (--fromFirstToSecond->end()); fromFirstToSecondIt!=fromFirstToSecond->begin(); fromFirstToSecondIt--)
      {
        fromSecondToFirst.push_back(*fromFirstToSecondIt);
      }
      fromSecondToFirst.push_back(*fromFirstToSecond->begin());
      fromSecondToFirst.push_back(first);
      float* furthestAnglePoint = furthestLeftPointFromLine(second,fromSecondToFirst);
//      angle = leftAngleBetweenRoads(firstNode,lastRoad,mainRoad); //dodelat na spravny uhel, ne mezi ulicemi ale zakriveni
    }
    else
    {

    }

//    if (angle < 70)
//    {
//      distanceOnRoad = depthOfLand/(tan(angle * M_PI / 180.0));
//    }
//    else
//    {
//
//    }
    std::list<float*>* fromFirstToFourth = goDistanceByRoad(mainRoad,firstNode,&mainRoadCurvatureIt,first,widthOfLand,mainRoadCurvature);

    float* fourth = *(fromFirstToFourth->begin());
    std::list<float*>* fromSecondToThird;
    if (fourth[0] != -1.0)
    {
      fourth = *(--(fromFirstToFourth->end()));
      std::list<float*> twoFurthestPoints = twoFurthestLeftPointsFromLine(first,*fromFirstToFourth);
      float* buildingMainVector = findBuildingMainVector(first,twoFurthestPoints,fourth);
      twoFurthestPoints.push_back(fourth);
      fromSecondToThird = findThirdBorderPoint(buildingMainVector,twoFurthestPoints,depthOfLand); // pridat check s hranicema bloku
      delete [] buildingMainVector;

      createBorderLands(land->borders,first,fromFirstToSecond,fromSecondToThird,fromFirstToFourth);
  //    Land* buildingLand;
      if (this->typeOfBlock == BLOCK_BUILDING)
      {
  //      buildingLand = land;
      }
      else
      {
        land->createBuilding(first,fromFirstToFourth,fourth,fromFirstToSecond,land->borders,depthOfLand,widthOfLand,averagePop,this->typeOfBlock);
      }
//      buildingLands->push_back(buildingLand);//docasne

    }
    else  //docasne
    {
      fromFirstToFourth->pop_front();
      fourth = *(--(fromFirstToFourth->end()));
      std::list<float*> twoFurthestPoints = twoFurthestLeftPointsFromLine(first,*fromFirstToFourth);
      float* buildingMainVector = findBuildingMainVector(first,twoFurthestPoints,fourth);
      twoFurthestPoints.push_back(fourth);
      fromSecondToThird = findThirdBorderPoint(buildingMainVector,twoFurthestPoints,depthOfLand); // pridat check s hranicema bloku
      delete [] buildingMainVector;
    }

//    drawLands(10,roads);
    float* oldThird = *(--fromSecondToThird->end());

    bool fourthEnd = false;

    while (bordersIt != this->borders->end())
    {
      while (true)
      {
        Land* land2 = new Land();
        land2->borders = new std::list<float*> ();

        this->lands->push_back(land2);//docasne
//        vegetationLand->push_back(land2);//docasne
        float* newFourth;
//        if (fourthEnd == true)
//        {
//          fourthEnd = false;
//          newFourth =
//        }
//        else
//        {
          fromFirstToFourth = goDistanceByRoad(mainRoad,firstNode,&mainRoadCurvatureIt,fourth,widthOfLand,mainRoadCurvature);
          newFourth = *(fromFirstToFourth->begin());
//        }

        float aa = newFourth[0];
        float aab = newFourth[1];
        if (newFourth[0] != -1.0)
        {
          newFourth = *(--(fromFirstToFourth->end()));
          std::list<float*> twoFurthestPoints = twoFurthestLeftPointsFromLine(fourth,*fromFirstToFourth);
          float* buildingMainVector = findBuildingMainVector(fourth,twoFurthestPoints,newFourth);
          twoFurthestPoints.push_back(newFourth);
          fromSecondToThird = findThirdBorderPoint(buildingMainVector,twoFurthestPoints,depthOfLand); // pridat check s hranicema bloku
          delete [] buildingMainVector;
        }
        else
        {
          fourthEnd=true;
          break;
        }
        fromSecondToThird->push_front(oldThird);
        createBorderLands(land2->borders,fourth,NULL,fromSecondToThird,fromFirstToFourth);
        std::list<float*> fromFirstToSecondJustForNextFunction;
        fromFirstToSecondJustForNextFunction.push_back(oldThird);
//        Land* buildingLand2;
        if (this->typeOfBlock == BLOCK_BUILDING)
        {
//          buildingLand2 = land2;
        }
        else
        {
          land2->createBuilding(fourth,fromFirstToFourth,newFourth,&fromFirstToSecondJustForNextFunction,land2->borders,depthOfLand,widthOfLand,averagePop,this->typeOfBlock);
        }
//        buildingLands->push_back(buildingLand2);//docasne
        oldThird = *(--fromSecondToThird->end());
        fourth = newFourth;
//        if (qqpp>38)
//        {
//          float a = 6;
//          drawLands(10,roads);
//        }
      }
//      if (qqpp>38)
//      {
//        float a = 6;
//        drawLands(10,roads);
//      }
      bordersIt++;
      if (bordersIt == this->borders->end())
      {
        break;
      }
      firstNode = getOtherEndOfRoad(firstNode,mainRoad);
      mainRoad = *bordersIt;
      mainRoadCurvature = mainRoad->curvature;

      bordersIt++;
      if (bordersIt == this->borders->end())
      {
        sideRoad = *this->borders->begin();
      }
      else
      {
        sideRoad = *bordersIt;
      }
      bordersIt--;

      if (firstNode == mainRoad->from)
      {
        mainRoadCurvatureIt = mainRoadCurvature->begin();
      }
      else
      {
        mainRoadCurvatureIt = mainRoadCurvature->end();
        mainRoadCurvatureIt--;
      }

      float* oldF = fourth; //pouzivan? nebo smazat?
      fourth = new float[2];
      fourth[0] = firstNode->x;
      fourth[1] = firstNode->y;
      float ppp = firstNode->x;
      float pppa = firstNode->y;
      if (!fourthEnd || this->typeOfBlock != BLOCK_BUILDING)
      {
        fromFirstToFourth = goDistanceByRoad(mainRoad,firstNode,&mainRoadCurvatureIt,fourth,depthOfLand,mainRoadCurvature);
        fourth = *(fromFirstToFourth->begin());
        if (fourth[0] != -1.0) //vymyslet co tady
        {
          fourth = *(--(fromFirstToFourth->end()));
        }
        else //vylepsit a vymyslet co tády
        {
          fourth = *(--(fromFirstToFourth->end()));
        }
        float paaa=fourth[0];
        float paaq=fourth[1];
        float paaasffas=fourth[0];
      }
      else
      {
        Node* secondNode = getOtherEndOfRoad(firstNode,mainRoad);
        float leftAngle = leftAngleBetweenRoads(secondNode,mainRoad,sideRoad);
        if (leftAngle < 135.0 && this->typeOfBlock != BLOCK_BUILDING)
        {

        }
        else //if (blockType != BLOCK_BUILDING)
        {
          fromFirstToFourth->pop_front();
          std::list<float*>* fromFourthToNewFourth = goDistanceByRoad(mainRoad,firstNode,&mainRoadCurvatureIt,fourth,depthOfLand,mainRoadCurvature);
          for (std::list<float*>::iterator it = fromFourthToNewFourth->begin(); it != fromFourthToNewFourth->end(); it++)
          {
            if ((*it)[0] != -1.0 && (*it)[1] != -1.0)//zjistit odkud se to tu bere --> nejspis vyreseno, prozatim zapnuto, jeste zkontrolovat
            {
              fromFirstToFourth->push_back(*it);
            }
          }

          float* newFourth = *(--(fromFirstToFourth->end()));
          float aq = newFourth[0];
          float aqq = newFourth[1];
          std::list<float*> twoFurthestPoints = twoFurthestLeftPointsFromLine(fourth,*fromFirstToFourth);
          float* buildingMainVector = findBuildingMainVector(fourth,twoFurthestPoints,newFourth);
          twoFurthestPoints.push_back(newFourth);
          fromSecondToThird = findThirdBorderPoint(buildingMainVector,twoFurthestPoints,depthOfLand); // pridat check s hranicema bloku
          delete [] buildingMainVector;
          fromSecondToThird->push_front(oldThird);

          Land* land2 = *(this->lands->rbegin());
          createBorderLands(land2->borders,oldF,NULL,fromSecondToThird,fromFirstToFourth);

          std::list<float*> fromFirstToSecondJustForNextFunction;
          fromFirstToSecondJustForNextFunction.push_back(oldThird);
//          Land* buildingLand2;
          if (this->typeOfBlock == BLOCK_BUILDING)
          {
//            buildingLand2 = land2;
          }
          else
          {
            land2->createBuilding(oldF,fromFirstToFourth,newFourth,&fromFirstToSecondJustForNextFunction,land2->borders,depthOfLand,widthOfLand,averagePop,this->typeOfBlock);
          }
//          buildingLands->push_back(buildingLand2);//docasne
        }
//        else
//        {
//
//        }
//drawLands(10,roads);

fourth = *(fromFirstToFourth->rbegin()); // potrebuje zapnout pozdeji

      }
//      fourthEnd = false;
//      drawLands(10,roads);
    }
//    buildingLands->pop_back();
//if (qqpp>37)
//{
// float a =6;
//    drawLands(10,roads);
//}
//qqpp++;
//    break; // just for testing purposes
}

Block::~Block()
{
//  TODO: cleanup
//  std::list<Road*>* borders;
//  std::list<Land*>* lands;
//  int typeOfBlock;
}

void Block::generateLands()
{
  //vygeneruj pozemky
  // a pak na pozemcích budovy
  for (std::list<Land*>::iterator landsIt = lands->begin(); landsIt!= lands->end(); ++landsIt)
  {
//    (*landsIt)->generateLandsWithBuildings();
  }
}

/** \brief return average population within block counted from population density
 *
 * \param block Block* block to be counted
 * \param population Population* population with map of density
 * \return float average population
 *
 */
float Block::sumUpPopulation(Population* population)// az bude typeOfBlock presunuty, tohle presunout do utilities
{
  float averagePop = 0.0;
  int num = 0;
  float minX = 10000.0,maxX = 0.0;
  float minY = 10000.0,maxY = 0.0;
  float** density = population->getDensity();
  int height = population->height;
  int width = population->width;
  for (std::list<Road*>::iterator borderIt = this->borders->begin(); borderIt!=this->borders->end(); borderIt++)
  {
    Road* road = *borderIt;
    Node* node = road->from;
    if (node->x < minX)
    {
      minX = node->x;
    }
    if (node->x > maxX)
    {
      maxX = node->x;
    }
    if (node->y < minY)
    {
      minY = node->y;
    }
    if (node->y > maxY)
    {
      maxY = node->y;
    }
    averagePop += bilinearInterpolation(density,node->x,node->y,height,width);
    num++;

    node = road->to;
    if (node->x < minX)
    {
      minX = node->x;
    }
    if (node->x > maxX)
    {
      maxX = node->x;
    }
    if (node->y < minY)
    {
      minY = node->y;
    }
    if (node->y > maxY)
    {
      maxY = node->y;
    }
    averagePop += bilinearInterpolation(density,node->x,node->y,height,width);
    num++;
  }

  for (int x = (int) floor(minX); x != (int) floor(maxX); x++) //do while?
  {
    for (int y = (int) floor(minY); y != (int) floor(maxY); y++)
    {
      averagePop += density[x][y];//spravne? x/y ? ? y/x
      num++;
    }
  }

  return (averagePop/((float)num));
}

/** \brief choose type of block and assign it to block based on average population density in block
 *
 * \param block Block* choosed block
 * \param averagePop float average population density within block
 * \return int constant which interpret type of block
 *
 */
int Block::setTypeOfBlock(float averagePop)//presunout do konstruktoru bloku? zakomponovat do toho i rozmery a tvar pozemku
{
  int type = SMALL_BUILDING;
  if (averagePop < MIN_DENSITY)
  {
    type = VEGETATION;
  }
  this->typeOfBlock = type;
  return type;

  //
//  int type = VEGETATION;
  if (averagePop > 0.0 && averagePop < 70.0)
  {
    type = SMALL_BUILDING;
  }
  else if(averagePop > 90.0 && averagePop < 150.0)
  {
    type = FLATS_BUILDING;
  }
  else if(averagePop > 150.0)
  {
    type = BLOCK_BUILDING;
  }
  this->typeOfBlock = type;
  return type;
}

/** \brief find and return longest road in the block
 *
 * \param block Block* block to be searched through
 * \return Road* longest road in block
 *
 */
Road* Block::findLongestRoad()
{
  Road* longestRoad;
  float longest = 0.0,length;
  for (std::list<Road*>::iterator bordersIt = this->borders->begin(); bordersIt != this->borders->end(); bordersIt++)
  {
    Road* road = *bordersIt;
    length = lengthOfRoad(road);
    if (longest < length)
    {
      longest = length;
      longestRoad = road;
    }
  }
  return longestRoad;
}
