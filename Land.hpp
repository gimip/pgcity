#ifndef LAND_H_INCLUDED
#define LAND_H_INCLUDED

#include <list>

class Building;
class City;
class Road;
class Node;

class Land
{
private:
  float depth(int type,float averagePop);
  float width(int type,float averagePop);
  std::list<float*>* goDistanceByRoad(Road* road, Node* from, std::list<float*>::iterator* thisWay,float* const here, float distance, std::list<float*>* mainRoadCurvature);
  float* findBuildingMainVector2(float* first,std::list<float*> twoFurthestPoints,float* fourth);
  std::list<float*>* findThirdBorderPoint(float* buildingMainVector,std::list<float*> threePoints, float depthOfLand);
  void createBorderLands(std::list<float*>* newBorderLands,float* first,std::list<float*>* fromFirstToSecond,std::list<float*>* fromSecondToThird,std::list<float*>* fromFirstToFourth);
public:
  Land();
  ~Land();
  Road* borderRoad; //Needed? urcite!
  std::list<float*>* borders;
  std::list<Building*>* buildings;
  void generateLandsWithBuildings(City* city); // if blocks type is building
  void createBuilding(float* first,std::list<float*>* fromFirstToFourth,float* fourth,std::list<float*>* fromFirstToSecond,std::list<float*>* landBorders,float depthOfLand,float widthOfLand,float averagePopulation,int blockType);
  void generateBuildings();
};

#endif // LAND_H_INCLUDED
