#include "Land.hpp"

#include "Building.hpp"
#include "constants.hpp"
#include "utilities.hpp"
#include <stddef.h>

Land::Land()
{
  borderRoad = NULL;
  borders = new std::list<float*>();
  buildings = new std::list<Building*>();
}

Land::~Land()
{
  borderRoad = NULL;
  borders->clear();
  delete borders;
  buildings->clear();
  delete buildings;
}

/** \brief create building on land within its borders
 *
 * \param first float* start point
 * \param fromFirstToFourth std::list<float*>* last part of border of land
 * \param fourth float* end point
 * \param fromFirstToSecond std::list<float*>* first part of border of land
 * \param landBorders std::list<float*>* borders of land
 * \param depthOfLand float depth of land
 * \param widthOfLand float width of land
 * \param averagePopulation float average population density within block
 * \param blockType int type of block
 * \return Land* newly created building
 *
 */
 void Land::generateBuildings()
{
  //... doplnit algoritmus ...z LANDS??
  // Po vygenerovani kazdeho domu zavolat
  // Building()
  // building.setModel()
  // set bla bla apod.
  //
  // anebo Land vyhradi misto pro
  //
  //
  // blbost, pozemek je jiz urcenej, takze jen zavola generateModel() od Building
  // nicmene by tu mohl existovat nejaky algoritmus, ktery to rozdeli jeste na mensi kousky
  // napada me aby to melo barak a treba i kulnu, priklad
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
float* Land::findBuildingMainVector2(float* first,std::list<float*> twoFurthestPoints,float* fourth) // temporary - it is a duplicate - do something about it // in Block.cpp is the same one
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

void Land::createBuilding(float* first,std::list<float*>* fromFirstToFourth,float* fourth,std::list<float*>* fromFirstToSecond,std::list<float*>* landBorders,float depthOfLand,float widthOfLand,float averagePopulation,int blockType)
{
    Building* building = new Building();
//    building->borders = new std::list<float*> ();
    std::list<float*> twoFurthestPoints = twoFurthestLeftPointsFromLine(first, *fromFirstToFourth);
    float* buildingMainVector = findBuildingMainVector2(first, twoFurthestPoints, fourth);
    std::list<float*> fromSecondToFirst;
    float* second = *(--(fromFirstToSecond->end()));
    for(std::list<float*>::iterator fromFirstToSecondIt = (--fromFirstToSecond->end()); fromFirstToSecondIt!=fromFirstToSecond->begin(); fromFirstToSecondIt--)
    {
      fromSecondToFirst.push_back(*fromFirstToSecondIt);
    }
    fromSecondToFirst.push_back(*fromFirstToSecond->begin());
    fromSecondToFirst.push_back(first);
    float* furthestAnglePoint = furthestLeftPointFromLine(second,fromSecondToFirst);
    float* nfirst;
    float* nsecond = new float [2];
    float* nthird = new float [2];
    float* nfourth = new float [2];
    float tmp [2], tmp2 [2];
    float* zero = new float [2];
    zero[0] = 0.0;
    zero[1] = 0.0;
    float* buildingSideVector = getLeftNormalVector(zero,buildingMainVector);

    float constEpsilon = 0.1;
    float coef = constEpsilon/sizeOfVectorN(buildingSideVector,2);
    float coef2 = constEpsilon/sizeOfVectorN(buildingMainVector,2);

    if (furthestAnglePoint == NULL)
    {
      tmp2[0] = second[0] + coef2 * buildingMainVector[0];
      tmp2[1] = second[1] + coef2 * buildingMainVector[1];
    }
    else
    {
      tmp2[0] = furthestAnglePoint[0] + coef2 * buildingMainVector[0];
      tmp2[1] = furthestAnglePoint[1] + coef2 * buildingMainVector[1];
    }

    if ((*(twoFurthestPoints.begin())) == NULL)
    {
      tmp[0] = first[0] + coef * buildingSideVector[0];
      tmp[1] = first[1] + coef * buildingSideVector[1];
    }
    else
    {
      tmp[0] = (*(twoFurthestPoints.begin()))[0] + coef * buildingSideVector[0];
      tmp[1] = (*(twoFurthestPoints.begin()))[1] + coef * buildingSideVector[1];
    }

    nfirst = lineIntersection(
    tmp[0] + 10000.0 * buildingMainVector[0],
    tmp[1] + 10000.0 * buildingMainVector[1],
    tmp[0] - 10000.0 * buildingMainVector[0],
    tmp[1] - 10000.0 * buildingMainVector[1],
    tmp2[0] + 10000.0 * buildingSideVector[0],
    tmp2[1] + 10000.0 * buildingSideVector[1],
    tmp2[0] - 10000.0 * buildingSideVector[0],
    tmp2[1] - 10000.0 * buildingSideVector[1]);

    depthOfLand *= DEPTH_OF_BUILDING;
    float k = depthOfLand/sizeOfVectorN(buildingSideVector,2);
    nsecond[0] = nfirst[0] + k * buildingSideVector[0];
    nsecond[1] = nfirst[1] + k * buildingSideVector[1];

    widthOfLand *= WIDTH_OF_BUILDING;
    float q = widthOfLand/sizeOfVectorN(buildingMainVector,2);
    nthird[0] = nsecond[0] + q * buildingMainVector[0];
    nthird[1] = nsecond[1] + q * buildingMainVector[1];

    nfourth[0] = nfirst[0] + q * buildingMainVector[0];
    nfourth[1] = nfirst[1] + q * buildingMainVector[1];

    float a = nsecond[0];
    float aa = nsecond[1];

    building->borders->push_back(nfirst);
    building->borders->push_back(nsecond);
    building->borders->push_back(nthird);
    building->borders->push_back(nfourth);
    this->buildings->push_back(building);
}
